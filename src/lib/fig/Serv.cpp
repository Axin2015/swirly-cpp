/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include <swirly/fig/Serv.hpp>

#include <swirly/fig/AsyncJourn.hpp>
#include <swirly/fig/Response.hpp>
#include <swirly/fig/TraderSess.hpp>

#include <swirly/elm/Date.hpp>
#include <swirly/elm/Exception.hpp>
#include <swirly/elm/Journ.hpp>
#include <swirly/elm/MarketBook.hpp>
#include <swirly/elm/Model.hpp>

#include <swirly/ash/Finally.hpp>
#include <swirly/ash/JulianDay.hpp>

#include "Match.hpp"
#include "ServFactory.hpp"
#include "TraderSessSet.hpp"

#include <regex>

using namespace std;

namespace swirly {
namespace {

const regex MnemPattern{R"(^[0-9A-Za-z-._]{3,16}$)"};

Ticks spread(const Order& takerOrder, const Order& makerOrder, Direct direct) noexcept
{
  using namespace enumops;
  return direct == Direct::Paid
    // Paid when the taker lifts the offer.
    ? makerOrder.ticks() - takerOrder.ticks()
    // Given when the taker hits the bid.
    : takerOrder.ticks() - makerOrder.ticks();
}

} // anonymous

struct Serv::Impl {

  Impl(Journ& journ, size_t capacity) noexcept : journ{journ, capacity} {}
  MarketBookPtr newMarket(Mnem mnem, string_view display, Mnem contr, Jday settlDay, Jday expiryDay,
                          MarketState state) const
  {
    if (!regex_match(mnem.begin(), mnem.end(), MnemPattern)) {
      throw InvalidException{errMsg() << "invalid mnem '" << mnem << '\''};
    }
    return MarketBook::make(mnem, display, contr, settlDay, expiryDay, state);
  }
  TraderSessPtr newTrader(Mnem mnem, string_view display, string_view email) const
  {
    if (!regex_match(mnem.begin(), mnem.end(), MnemPattern)) {
      throw InvalidException{errMsg() << "invalid mnem '" << mnem << '\''};
    }
    return TraderSess::make(mnem, display, email);
  }
  ExecPtr newExec(const Order& order, Iden id, Millis created) const
  {
    return Exec::make(order.trader(), order.market(), order.contr(), order.settlDay(), id,
                      order.ref(), order.id(), order.state(), order.side(), order.lots(),
                      order.ticks(), order.resd(), order.exec(), order.cost(), order.lastLots(),
                      order.lastTicks(), order.minLots(), 0_id, Role::None, Mnem{}, created);
  }
  ExecPtr newExec(MarketBook& book, const Order& order, Millis created) const
  {
    return newExec(order, book.allocExecId(), created);
  }
  /**
   * Special factory method for manual trades.
   */
  ExecPtr newManual(Mnem trader, Mnem market, Mnem contr, Jday settlDay, Iden id, string_view ref,
                    Side side, Lots lots, Ticks ticks, Role role, Mnem cpty, Millis created) const
  {
    const auto orderId = 0_id;
    const auto state = State::Trade;
    const auto resd = 0_lts;
    const auto exec = lots;
    const auto cost = swirly::cost(lots, ticks);
    const auto lastLots = lots;
    const auto lastTicks = ticks;
    const auto minLots = 1_lts;
    const auto matchId = 0_id;
    return Exec::make(trader, market, contr, settlDay, id, ref, orderId, state, side, lots, ticks,
                      resd, exec, cost, lastLots, lastTicks, minLots, matchId, role, cpty, created);
  }
  ExecPtr newManual(Mnem trader, MarketBook& book, string_view ref, Side side, Lots lots,
                    Ticks ticks, Role role, Mnem cpty, Millis created) const
  {
    return newManual(trader, book.mnem(), book.contr(), book.settlDay(), book.allocExecId(), ref,
                     side, lots, ticks, role, cpty, created);
  }
  Match newMatch(MarketBook& book, const Order& takerOrder, const OrderPtr& makerOrder, Lots lots,
                 Lots sumLots, Cost sumCost, Millis created)
  {
    const auto makerId = book.allocExecId();
    const auto takerId = book.allocExecId();

    auto it = traders.find(makerOrder->trader());
    assert(it != traders.end());
    auto& makerSess = static_cast<TraderSess&>(*it);
    auto makerPosn = makerSess.lazyPosn(book.contr(), book.settlDay());

    const auto ticks = makerOrder->ticks();

    auto makerTrade = newExec(*makerOrder, makerId, created);
    makerTrade->trade(lots, ticks, takerId, Role::Maker, takerOrder.trader());

    auto takerTrade = newExec(takerOrder, takerId, created);
    takerTrade->trade(sumLots, sumCost, lots, ticks, makerId, Role::Taker, makerOrder->trader());

    return {lots, makerOrder, makerTrade, makerPosn, takerTrade};
  }
  void matchOrders(const TraderSess& takerSess, MarketBook& book, Order& takerOrder, BookSide& side,
                   Direct direct, Millis now, Response& resp)
  {
    using namespace enumops;

    auto sumLots = 0_lts;
    auto sumCost = 0_cst;
    auto lastLots = 0_lts;
    auto lastTicks = 0_tks;

    for (auto& makerOrder : side.orders()) {
      // Break if order is fully filled.
      if (sumLots == takerOrder.resd()) {
        break;
      }
      // Only consider orders while prices cross.
      if (spread(takerOrder, makerOrder, direct) > 0_tks) {
        break;
      }

      const auto lots = min(takerOrder.resd() - sumLots, makerOrder.resd());
      const auto ticks = makerOrder.ticks();

      sumLots += lots;
      sumCost += cost(lots, ticks);
      lastLots = lots;
      lastTicks = ticks;

      auto match = newMatch(book, takerOrder, &makerOrder, lots, sumLots, sumCost, now);

      // Insert order if trade crossed with self.
      if (makerOrder.trader() == takerSess.mnem()) {
        resp.insertOrder(&makerOrder);
        // Maker updated first because this is consistent with last-look semantics.
        // N.B. the reference count is not incremented here.
        resp.insertExec(match.makerTrade);
      }
      resp.insertExec(match.takerTrade);

      matches.push_back(move(match));
    }

    if (!matches.empty()) {
      takerOrder.trade(sumLots, sumCost, lastLots, lastTicks, now);
    }
  }
  void matchOrders(const TraderSess& takerSess, MarketBook& book, Order& takerOrder, Millis now,
                   Response& resp)
  {
    BookSide* bookSide;
    Direct direct;
    if (takerOrder.side() == Side::Buy) {
      // Paid when the taker lifts the offer.
      bookSide = &book.offerSide();
      direct = Direct::Paid;
    } else {
      assert(takerOrder.side() == Side::Sell);
      // Given when the taker hits the bid.
      bookSide = &book.bidSide();
      direct = Direct::Given;
    }
    matchOrders(takerSess, book, takerOrder, *bookSide, direct, now, resp);
  }
  // Assumes that maker lots have not been reduced since matching took place. N.B. this function is
  // responsible for committing a transaction, so it is particularly important that it does not
  // throw.
  void commitMatches(TraderSess& takerSess, MarketBook& book, Millis now) noexcept
  {
    for (const auto& match : matches) {
      const auto makerOrder = match.makerOrder;
      assert(makerOrder);
      // Reduce maker.
      book.takeOrder(*makerOrder, match.lots, now);
      // Must succeed because maker order exists.
      auto it = traders.find(makerOrder->trader());
      assert(it != traders.end());
      auto& makerSess = static_cast<TraderSess&>(*it);
      // Maker updated first because this is consistent with last-look semantics.
      // Update maker.
      const auto makerTrade = match.makerTrade;
      assert(makerTrade);
      makerSess.insertTrade(makerTrade);
      match.makerPosn->addTrade(makerTrade->side(), makerTrade->lastLots(),
                                makerTrade->lastTicks());
      // Update taker.
      const auto takerTrade = match.takerTrade;
      assert(takerTrade);
      takerSess.insertTrade(takerTrade);
    }
  }

  AsyncJourn journ;
  const BusinessDay busDay{RollHour, NewYork};
  const ServFactory factory{};
  AssetSet assets;
  ContrSet contrs;
  MarketSet markets;
  TraderSet traders;
  TraderSessSet emailIdx;
  vector<Match> matches;
};

Serv::Serv(Journ& journ, size_t capacity) : impl_{make_unique<Impl>(journ, capacity)}
{
}

Serv::~Serv() noexcept = default;

Serv::Serv(Serv&&) = default;

Serv& Serv::operator=(Serv&&) = default;

void Serv::load(const Model& model, Millis now)
{
  const auto busDay = impl_->busDay(now);
  model.readAsset([& assets = impl_->assets](auto&& ptr) { assets.insert(move(ptr)); });
  model.readContr([& contrs = impl_->contrs](auto&& ptr) { contrs.insert(move(ptr)); });
  model.readMarket(impl_->factory,
                   [& markets = impl_->markets](auto&& ptr) { markets.insert(move(ptr)); });
  model.readTrader(impl_->factory,
                   [& traders = impl_->traders, &emailIdx = impl_->emailIdx ](auto&& ptr) {
                     emailIdx.insert(static_cast<TraderSess&>(*ptr));
                     traders.insert(move(ptr));
                   });
  model.readOrder([& markets = impl_->markets, &traders = impl_->traders ](auto&& ptr) {
    auto it = traders.find(ptr->trader());
    assert(it != traders.end());
    static_cast<TraderSess&>(*it).insertOrder(ptr);
    bool success{false};
    auto finally = makeFinally([&]() {
      if (!success) {
        static_cast<TraderSess&>(*it).removeOrder(*ptr);
      }
    });
    auto jt = markets.find(ptr->market());
    assert(jt != markets.end());
    static_cast<MarketBook&>(*jt).insertOrder(ptr);
    success = true;
  });
  model.readTrade([& traders = impl_->traders](auto&& ptr) {
    auto it = traders.find(ptr->trader());
    assert(it != traders.end());
    static_cast<TraderSess&>(*it).insertTrade(ptr);
  });
  model.readPosn(busDay, [& traders = impl_->traders](auto&& ptr) {
    auto it = traders.find(ptr->trader());
    assert(it != traders.end());
    static_cast<TraderSess&>(*it).insertPosn(ptr);
  });
}

AssetSet& Serv::assets() const noexcept
{
  return impl_->assets;
}

ContrSet& Serv::contrs() const noexcept
{
  return impl_->contrs;
}

MarketSet& Serv::markets() const noexcept
{
  return impl_->markets;
}

TraderSet& Serv::traders() const noexcept
{
  return impl_->traders;
}

MarketBook& Serv::market(Mnem mnem) const
{
  auto it = impl_->markets.find(mnem);
  if (it == impl_->markets.end()) {
    throw MarketNotFoundException{errMsg() << "market '" << mnem << "' does not exist"};
  }
  auto& market = static_cast<MarketBook&>(*it);
  return market;
}

TraderSess& Serv::trader(Mnem mnem) const
{
  auto it = impl_->traders.find(mnem);
  if (it == impl_->traders.end()) {
    throw TraderNotFoundException{errMsg() << "trader '" << mnem << "' does not exist"};
  }
  auto& trader = static_cast<TraderSess&>(*it);
  return trader;
}

TraderSess& Serv::traderFromEmail(string_view email) const
{
  auto it = impl_->emailIdx.find(email);
  if (it == impl_->emailIdx.end()) {
    throw TraderNotFoundException{errMsg() << "trader '" << email << "' does not exist"};
  }
  return *it;
}

MarketBook& Serv::createMarket(Mnem mnem, string_view display, Mnem contr, Jday settlDay,
                               Jday expiryDay, MarketState state, Millis now)
{
  if (impl_->contrs.find(contr) == impl_->contrs.end()) {
    throw NotFoundException{errMsg() << "contr '" << contr << "' does not exist"};
  }
  if (settlDay != 0_jd) {
    // busDay <= expiryDay <= settlDay.
    const auto busDay = impl_->busDay(now);
    if (settlDay < expiryDay) {
      throw InvalidException{"settl-day before expiry-day"_sv};
    }
    if (expiryDay < busDay) {
      throw InvalidException{"expiry-day before bus-day"_sv};
    }
  } else {
    if (expiryDay != 0_jd) {
      throw InvalidException{"expiry-day without settl-day"_sv};
    }
  }
  MarketSet::Iterator it;
  bool found;
  tie(it, found) = impl_->markets.findHint(mnem);
  if (found) {
    throw AlreadyExistsException{errMsg() << "market '" << mnem << "' already exists"};
  }
  {
    auto market = impl_->newMarket(mnem, display, contr, settlDay, expiryDay, state);
    impl_->journ.createMarket(mnem, display, contr, settlDay, expiryDay, state);
    it = impl_->markets.insertHint(it, move(market));
  }
  auto& market = static_cast<MarketBook&>(*it);
  return market;
}

MarketBook& Serv::updateMarket(Mnem mnem, optional<string_view> display,
                               optional<MarketState> state, Millis now)
{
  auto it = impl_->markets.find(mnem);
  if (it == impl_->markets.end()) {
    throw MarketNotFoundException{errMsg() << "market '" << mnem << "' does not exist"};
  }
  auto& market = static_cast<MarketBook&>(*it);
  impl_->journ.updateMarket(mnem, display ? *display : market.display(),
                            state ? *state : market.state());
  if (display) {
    market.setDisplay(*display);
  }
  if (state) {
    market.setState(*state);
  }
  return market;
}

TraderSess& Serv::createTrader(Mnem mnem, string_view display, string_view email, Millis now)
{
  TraderSet::Iterator it;
  bool found;
  tie(it, found) = impl_->traders.findHint(mnem);
  if (found) {
    throw AlreadyExistsException{errMsg() << "trader '" << mnem << "' already exists"};
  }
  if (impl_->emailIdx.find(email) != impl_->emailIdx.end()) {
    throw AlreadyExistsException{errMsg() << "email '" << email << "' is already in use"};
  }
  {
    auto trader = impl_->newTrader(mnem, display, email);
    impl_->journ.createTrader(mnem, display, email);
    it = impl_->traders.insertHint(it, move(trader));
  }
  auto& trader = static_cast<TraderSess&>(*it);
  impl_->emailIdx.insert(trader);
  return trader;
}

TraderSess& Serv::updateTrader(Mnem mnem, string_view display, Millis now)
{
  auto it = impl_->traders.find(mnem);
  if (it == impl_->traders.end()) {
    throw TraderNotFoundException{errMsg() << "trader '" << mnem << "' does not exist"};
  }
  auto& trader = static_cast<TraderSess&>(*it);
  impl_->journ.updateTrader(mnem, display);
  trader.setDisplay(display);
  return trader;
}

void Serv::createOrder(TraderSess& sess, MarketBook& book, string_view ref, Side side, Lots lots,
                       Ticks ticks, Lots minLots, Millis now, Response& resp)
{
  if (!ref.empty() && sess.refIdx().find(ref) != sess.refIdx().end()) {
    throw RefAlreadyExistsException{errMsg() << "order '" << ref << "' already exists"};
  }

  const auto busDay = impl_->busDay(now);
  if (book.expiryDay() != 0_jd && book.expiryDay() < busDay) {
    throw MarketClosedException{errMsg() << "market for '" << book.contr() << "' in '"
                                         << maybeJdToIso(book.settlDay()) << "' has expired"};
  }
  if (lots == 0_lts || lots < minLots) {
    throw InvalidLotsException{errMsg() << "invalid lots '" << lots << '\''};
  }
  const auto orderId = book.allocOrderId();
  auto order = Order::make(sess.mnem(), book.mnem(), book.contr(), book.settlDay(), orderId, ref,
                           side, lots, ticks, minLots, now);
  auto exec = impl_->newExec(book, *order, now);

  resp.insertOrder(order);
  resp.insertExec(exec);
  // Order fields are updated on match.
  impl_->matchOrders(sess, book, *order, now, resp);
  // Ensure that matches are cleared when scope exits.
  auto& matches = impl_->matches;
  auto finally = makeFinally([&matches]() { matches.clear(); });

  // Place incomplete order in market. N.B. done() is sufficient here because the order cannot be
  // pending cancellation.
  if (!order->done()) {
    // This may fail if level cannot be allocated.
    book.insertOrder(order);
  }
  {
    // TODO: IOC orders would need an additional revision for the unsolicited cancellation of any
    // unfilled quantity.
    bool success{false};
    auto finally = makeFinally([&book, &order, &success]() {
      if (!success && !order->done()) {
        // Undo market insertion.
        book.removeOrder(*order);
      }
    });

    impl_->journ.createExec(resp.execs());
    success = true;
  }

  resp.setBook(book);

  // Avoid allocating position when there are no matches.
  PosnPtr posn;
  if (!matches.empty()) {
    // Avoid allocating position when there are no matches.
    // N.B. before commit phase, because this may fail.
    posn = sess.lazyPosn(book.contr(), book.settlDay());
    resp.setPosn(posn);
  }

  // Commit phase.

  sess.insertOrder(order);

  // Commit matches.
  if (!matches.empty()) {
    assert(posn);
    posn->addTrade(order->side(), order->lastLots(), order->lastTicks());
    impl_->commitMatches(sess, book, now);
  }
}

void Serv::reviseOrder(TraderSess& sess, MarketBook& book, Order& order, Lots lots, Millis now,
                       Response& resp)
{
  if (order.done()) {
    throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
  }
  // Revised lots must not be:
  // 1. greater than original lots;
  // 2. less than executed lots;
  // 3. less than min lots.
  if (lots == 0_lts //
      || lots > order.lots() //
      || lots < order.exec() //
      || lots < order.minLots()) {
    throw new InvalidLotsException{errMsg() << "invalid lots '" << lots << '\''};
  }
  auto exec = impl_->newExec(book, order, now);
  exec->revise(lots);

  resp.setBook(book);
  resp.insertOrder(&order);
  resp.insertExec(exec);

  impl_->journ.createExec(*exec);

  // Commit phase.

  book.reviseOrder(order, lots, now);
}

void Serv::reviseOrder(TraderSess& sess, MarketBook& book, Iden id, Lots lots, Millis now,
                       Response& resp)
{
  auto& order = sess.order(book.mnem(), id);
  reviseOrder(sess, book, order, lots, now, resp);
}

void Serv::reviseOrder(TraderSess& sess, MarketBook& book, string_view ref, Lots lots, Millis now,
                       Response& resp)
{
  auto& order = sess.order(ref);
  reviseOrder(sess, book, order, lots, now, resp);
}

void Serv::reviseOrder(TraderSess& sess, MarketBook& book, ArrayView<Iden> ids, Lots lots,
                       Millis now, Response& resp)
{
  resp.setBook(book);
  for (const auto id : ids) {

    auto& order = sess.order(book.mnem(), id);
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    // Revised lots must not be:
    // 1. greater than original lots;
    // 2. less than executed lots;
    // 3. less than min lots.
    if (lots == 0_lts //
        || lots > order.lots() //
        || lots < order.exec() //
        || lots < order.minLots()) {
      throw new InvalidLotsException{errMsg() << "invalid lots '" << lots << '\''};
    }
    auto exec = impl_->newExec(book, order, now);
    exec->revise(lots);

    resp.insertOrder(&order);
    resp.insertExec(exec);
  }

  impl_->journ.createExec(resp.execs());

  // Commit phase.

  for (const auto id : ids) {
    auto it = sess.orders().find(book.mnem(), id);
    assert(it != sess.orders().end());
    book.reviseOrder(*it, lots, now);
  }
}

void Serv::cancelOrder(TraderSess& sess, MarketBook& book, Order& order, Millis now, Response& resp)
{
  if (order.done()) {
    throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
  }
  auto exec = impl_->newExec(book, order, now);
  exec->cancel();

  resp.setBook(book);
  resp.insertOrder(&order);
  resp.insertExec(exec);

  impl_->journ.createExec(*exec);

  // Commit phase.

  book.cancelOrder(order, now);
}

void Serv::cancelOrder(TraderSess& sess, MarketBook& book, Iden id, Millis now, Response& resp)
{
  auto& order = sess.order(book.mnem(), id);
  cancelOrder(sess, book, order, now, resp);
}

void Serv::cancelOrder(TraderSess& sess, MarketBook& book, string_view ref, Millis now,
                       Response& resp)
{
  auto& order = sess.order(ref);
  cancelOrder(sess, book, order, now, resp);
}

void Serv::cancelOrder(TraderSess& sess, MarketBook& book, ArrayView<Iden> ids, Millis now,
                       Response& resp)
{
  resp.setBook(book);
  for (const auto id : ids) {

    auto& order = sess.order(book.mnem(), id);
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    auto exec = impl_->newExec(book, order, now);
    exec->cancel();

    resp.insertOrder(&order);
    resp.insertExec(exec);
  }

  impl_->journ.createExec(resp.execs());

  // Commit phase.

  for (const auto id : ids) {
    auto it = sess.orders().find(book.mnem(), id);
    assert(it != sess.orders().end());
    book.cancelOrder(*it, now);
  }
}

void Serv::cancelOrder(TraderSess& sess, Millis now)
{
  // FIXME: Not implemented.
}

void Serv::cancelOrder(MarketBook& book, Millis now)
{
  // FIXME: Not implemented.
}

void Serv::archiveOrder(TraderSess& sess, const Order& order, Millis now)
{
  if (!order.done()) {
    throw InvalidException{errMsg() << "order '" << order.id() << "' is not done"};
  }

  impl_->journ.archiveOrder(order.market(), order.id(), now);

  // Commit phase.

  sess.removeOrder(order);
}

void Serv::archiveOrder(TraderSess& sess, Mnem market, Iden id, Millis now)
{
  auto& order = sess.order(market, id);
  archiveOrder(sess, order, now);
}

void Serv::archiveOrder(TraderSess& sess, Mnem market, ArrayView<Iden> ids, Millis now)
{
  for (const auto id : ids) {

    auto& order = sess.order(market, id);
    if (!order.done()) {
      throw InvalidException{errMsg() << "order '" << order.id() << "' is not done"};
    }
  }

  impl_->journ.archiveOrder(market, ids, now);

  // Commit phase.

  for (const auto id : ids) {

    auto it = sess.orders().find(market, id);
    assert(it != sess.orders().end());
    sess.removeOrder(*it);
  }
}

TradePair Serv::createTrade(TraderSess& sess, MarketBook& book, string_view ref, Side side,
                            Lots lots, Ticks ticks, Role role, Mnem cpty, Millis created)
{
  auto posn = sess.lazyPosn(book.contr(), book.settlDay());
  auto trade = impl_->newManual(sess.mnem(), book, ref, side, lots, ticks, role, cpty, created);
  decltype(trade) cptyTrade;

  if (!cpty.empty()) {

    // Create back-to-back trade if counter-party is specified.
    auto& cptySess = trader(cpty);
    auto cptyPosn = cptySess.lazyPosn(book.contr(), book.settlDay());
    cptyTrade = trade->inverse(book.allocExecId());

    ConstExecPtr trades[] = {trade, cptyTrade};
    impl_->journ.createExec(trades);

    // Commit phase.

    cptySess.insertTrade(cptyTrade);
    cptyPosn->addTrade(cptyTrade->side(), cptyTrade->lastLots(), cptyTrade->lastTicks());

  } else {

    impl_->journ.createExec(*trade);

    // Commit phase.
  }
  sess.insertTrade(trade);
  posn->addTrade(trade->side(), trade->lastLots(), trade->lastTicks());

  return {trade, cptyTrade};
}

void Serv::archiveTrade(TraderSess& sess, const Exec& trade, Millis now)
{
  if (trade.state() != State::Trade) {
    throw InvalidException{errMsg() << "exec '" << trade.id() << "' is not a trade"};
  }

  impl_->journ.archiveTrade(trade.market(), trade.id(), now);

  // Commit phase.

  sess.removeTrade(trade);
}

void Serv::archiveTrade(TraderSess& sess, Mnem market, Iden id, Millis now)
{
  auto& trade = sess.trade(market, id);
  archiveTrade(sess, trade, now);
}

void Serv::archiveTrade(TraderSess& sess, Mnem market, ArrayView<Iden> ids, Millis now)
{
  for (const auto id : ids) {

    auto& trade = sess.trade(market, id);
    if (trade.state() != State::Trade) {
      throw InvalidException{errMsg() << "exec '" << trade.id() << "' is not a trade"};
    }
  }

  impl_->journ.archiveTrade(market, ids, now);

  // Commit phase.

  for (const auto id : ids) {

    auto it = sess.trades().find(market, id);
    assert(it != sess.trades().end());
    sess.removeTrade(*it);
  }
}

void Serv::expireEndOfDay(Millis now)
{
  // FIXME: Not implemented.
}

void Serv::settlEndOfDay(Millis now)
{
  // FIXME: Not implemented.
}

} // swirly
