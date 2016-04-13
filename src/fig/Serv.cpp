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
const regex mnemPattern{R"(^[0-9A-Za-z-._]{3,16}$)"};

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

  using MarketBookPtr = unique_ptr<MarketBook, default_delete<Market>>;
  using TraderSessPtr = unique_ptr<TraderSess, default_delete<Trader>>;

  Impl(const Model& model, Journ& journ, Millis now) noexcept : journ{journ} {}
  MarketBookPtr newMarket(string_view mnem, string_view display, string_view contr, Jday settlDay,
                          Jday expiryDay, MarketState state) const
  {
    if (!regex_match(mnem.begin(), mnem.end(), mnemPattern)) {
      throw InvalidException{errMsg() << "invalid mnem '" << mnem << '\''};
    }
    auto up = factory.newMarket(mnem, display, contr, settlDay, expiryDay, state);
    return {static_cast<MarketBook*>(up.release()), move(up.get_deleter())};
  }
  TraderSessPtr newTrader(string_view mnem, string_view display, string_view email) const
  {
    if (!regex_match(mnem.begin(), mnem.end(), mnemPattern)) {
      throw InvalidException{errMsg() << "invalid mnem '" << mnem << '\''};
    }
    auto up = factory.newTrader(mnem, display, email);
    return {static_cast<TraderSess*>(up.release()), move(up.get_deleter())};
  }
  ExecPtr newExec(MarketBook& book, const Order& order, Millis now) const
  {
    return factory.newExec(order, book.allocExecId(), now);
  }
  Match newMatch(MarketBook& book, const Order& takerOrder, const OrderPtr& makerOrder, Lots lots,
                 Lots sumLots, Cost sumCost, Millis now)
  {
    const auto makerId = book.allocExecId();
    const auto takerId = book.allocExecId();

    auto it = traders.find(makerOrder->trader());
    assert(it != traders.end());
    auto& makerSess = static_cast<TraderSess&>(*it);
    auto makerPosn = makerSess.lazyPosn(book.contr(), book.settlDay());

    const auto ticks = makerOrder->ticks();

    auto makerTrade = factory.newExec(*makerOrder, makerId, now);
    makerTrade->trade(lots, ticks, takerId, Role::Maker, takerOrder.trader());

    auto takerTrade = factory.newExec(takerOrder, takerId, now);
    takerTrade->trade(sumLots, sumCost, lots, ticks, makerId, Role::Taker, makerOrder->trader());

    return {lots, makerOrder, makerTrade, makerPosn, takerTrade};
  }
  void matchOrders(const TraderSess& takerSess, MarketBook& book, Order& takerOrder, BookSide& side,
                   Direct direct, Millis now)
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
      matches.push_back(move(match));
    }

    // FIXME: can this be postponed until commit phase?
    if (!matches.empty()) {
      takerOrder.trade(sumLots, sumCost, lastLots, lastTicks, now);
    }
  }
  void matchOrders(const TraderSess& takerSess, MarketBook& book, Order& takerOrder, Millis now)
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
    matchOrders(takerSess, book, takerOrder, *bookSide, direct, now);
  }
  // Assumes that maker lots have not been reduced since matching took place. N.B. this function is
  // responsible for committing a transaction, so it is particularly important that it does not
  // throw. This function also assumes that sufficient capacity has been reserved in the response
  // object, so that no memory allocations take place.
  void commitMatches(TraderSess& takerSess, Millis now, Response& resp) noexcept
  {
    for (const auto& match : matches) {
      const auto makerOrder = match.makerOrder;
      assert(makerOrder);
      // Reduce maker.
      resp.takeOrder(*makerOrder, match.lots, now);
      // Must succeed because maker order exists.
      auto it = traders.find(makerOrder->trader());
      assert(it != traders.end());
      auto& makerSess = static_cast<TraderSess&>(*it);
      // Maker updated first because this is consistent with last-look semantics.
      // Update maker.
      const auto makerTrade = match.makerTrade;
      assert(makerTrade);
      makerSess.insertTrade(makerTrade);
      match.makerPosn->addTrade(*makerTrade);
      // Update taker.
      const auto takerTrade = match.takerTrade;
      assert(takerTrade);
      takerSess.insertTrade(takerTrade);
      resp.addTrade(*takerTrade);

      // Insert order if trade crossed with self.
      if (makerOrder->trader() == takerSess.mnem()) {
        resp.insertOrder(makerOrder);
        // Maker updated first because this is consistent with last-look semantics.
        // N.B. the reference count is not incremented here.
        resp.insertExec(match.makerTrade);
      }
      resp.insertExec(takerTrade);
    }
  }

  Journ& journ;
  ServFactory factory;
  AssetSet assets;
  ContrSet contrs;
  MarketSet markets;
  TraderSet traders;
  TraderSessSet emailIdx;
  vector<Match> matches;
};

Serv::Serv(const Model& model, Journ& journ, Millis now)
  : impl_{make_unique<Impl>(model, journ, now)}
{
  impl_->assets = model.readAsset(impl_->factory);
  impl_->contrs = model.readContr(impl_->factory);
  impl_->markets = model.readMarket(impl_->factory);
  impl_->traders = model.readTrader(impl_->factory);

  for (auto& rec : impl_->traders) {
    auto& trader = static_cast<TraderSess&>(rec);
    impl_->emailIdx.insert(trader);
  }
}

Serv::~Serv() noexcept = default;

Serv::Serv(Serv&&) = default;

Serv& Serv::operator=(Serv&&) = default;

const AssetSet& Serv::assets() const noexcept
{
  return impl_->assets;
}

const ContrSet& Serv::contrs() const noexcept
{
  return impl_->contrs;
}

const MarketSet& Serv::markets() const noexcept
{
  return impl_->markets;
}

const TraderSet& Serv::traders() const noexcept
{
  return impl_->traders;
}

MarketBook& Serv::market(string_view mnem) const
{
  auto it = impl_->markets.find(mnem);
  if (it == impl_->markets.end()) {
    throw MarketNotFoundException{errMsg() << "market '" << mnem << "' does not exist"};
  }
  auto& market = static_cast<MarketBook&>(*it);
  return market;
}

TraderSess& Serv::trader(string_view mnem) const
{
  auto it = impl_->traders.find(mnem);
  if (it == impl_->traders.end()) {
    throw TraderNotFoundException{errMsg() << "trader '" << mnem << "' does not exist"};
  }
  auto& trader = static_cast<TraderSess&>(*it);
  return trader;
}

TraderSess* Serv::findTraderByEmail(string_view email) const
{
  TraderSess* trader{nullptr};
  auto it = impl_->emailIdx.find(email);
  if (it != impl_->emailIdx.end()) {
    trader = &*it;
  }
  return trader;
}

const MarketBook& Serv::createMarket(string_view mnem, string_view display, string_view contr,
                                     Jday settlDay, Jday expiryDay, MarketState state, Millis now)
{
  if (impl_->contrs.find(contr) == impl_->contrs.end()) {
    throw NotFoundException{errMsg() << "contr '" << contr << "' does not exist"};
  }
  if (settlDay != 0_jd) {
    // busDay <= expiryDay <= settlDay.
    const auto busDay = getBusDay(now);
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
  it = impl_->markets.insertHint(
    it, impl_->newMarket(mnem, display, contr, settlDay, expiryDay, state));
  const auto& market = static_cast<const MarketBook&>(*it);
  return market;
}

const MarketBook& Serv::updateMarket(string_view mnem, optional<string_view> display,
                                     optional<MarketState> state, Millis now)
{
  auto it = impl_->markets.find(mnem);
  if (it == impl_->markets.end()) {
    throw MarketNotFoundException{errMsg() << "market '" << mnem << "' does not exist"};
  }
  auto& market = static_cast<MarketBook&>(*it);
  if (display) {
    market.setDisplay(*display);
  }
  if (state) {
    market.setState(*state);
  }
  return market;
}

const TraderSess& Serv::createTrader(string_view mnem, string_view display, string_view email,
                                     Millis now)
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
  it = impl_->traders.insertHint(it, impl_->newTrader(mnem, display, email));
  auto& trader = static_cast<TraderSess&>(*it);
  impl_->emailIdx.insert(trader);
  return trader;
}

const TraderSess& Serv::updateTrader(string_view mnem, string_view display, Millis now)
{
  auto it = impl_->traders.find(mnem);
  if (it == impl_->traders.end()) {
    throw TraderNotFoundException{errMsg() << "trader '" << mnem << "' does not exist"};
  }
  auto& trader = static_cast<TraderSess&>(*it);
  trader.setDisplay(display);
  return trader;
}

void Serv::createOrder(TraderSess& sess, MarketBook& book, string_view ref, Side side, Lots lots,
                       Ticks ticks, Lots minLots, Millis now, Response& resp)
{
  const auto busDay = getBusDay(now);
  if (book.expiryDay() != 0_jd && book.expiryDay() < busDay) {
    throw MarketClosedException{errMsg() << "market for '" << book.contr() << "' in '"
                                         << maybeJdToIso(book.settlDay()) << "' has expired"};
  }
  if (lots == 0_lts || lots < minLots) {
    throw InvalidLotsException{errMsg() << "invalid lots '" << lots << '\''};
  }
  const auto orderId = book.allocOrderId();
  auto order = impl_->factory.newOrder(sess.mnem(), book.mnem(), book.contr(), book.settlDay(),
                                       orderId, ref, side, lots, ticks, minLots, now);
  auto exec = impl_->newExec(book, *order, now);

  // Order fields are updated on match.
  impl_->matchOrders(sess, book, *order, now);
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

    // New execution plus 2 trade executions for each match assuming crossed with self.
    const size_t len{1 + 2 * matches.size()};
    Exec* execs[len];

    execs[0] = exec.get();
    int i{1};
    for (const auto& match : matches) {
      execs[i] = match.makerTrade.get();
      execs[i + 1] = match.takerTrade.get();
      i += 2;
    }
    impl_->journ.createExec(book.mnem(), makeArrayView(execs, len));
    success = true;
  }

  resp.setBook(book);

  // Avoid allocating position when there are no matches.
  if (!matches.empty()) {
    // Avoid allocating position when there are no matches.
    // N.B. before commit phase, because this may fail.
    resp.setPosn(sess.lazyPosn(book.contr(), book.settlDay()));
  }

  // New order plus updated order for each match assuming crossed with self.
  resp.reserveOrders(1 + matches.size());
  // New execution plus 2 trade executions for each match assuming crossed with self.
  resp.reserveExecs(1 + 2 * matches.size());

  // Commit phase.

  sess.insertOrder(order);

  // Commit matches.

  // These insert calls are effectively noexcept, because sufficient capacity has been reserved
  // above.
  resp.insertOrder(order);
  resp.insertExec(exec);

  // Commit matches.
  if (!matches.empty()) {
    assert(resp.posn());
    impl_->commitMatches(sess, now, resp);
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
}

void Serv::cancelOrder(TraderSess& sess, MarketBook& book, Order& order, Millis now, Response& resp)
{
}

void Serv::cancelOrder(TraderSess& sess, MarketBook& book, Iden id, Millis now, Response& resp)
{
}

void Serv::cancelOrder(TraderSess& sess, MarketBook& book, string_view ref, Millis now,
                       Response& resp)
{
}

void Serv::cancelOrder(TraderSess& sess, MarketBook& book, ArrayView<Iden> ids, Millis now,
                       Response& resp)
{
}

void Serv::cancelOrder(TraderSess& sess, Millis now)
{
}

void Serv::cancelOrder(MarketBook& book, Millis now)
{
}

void Serv::archiveOrder(TraderSess& sess, Order& order, Millis now)
{
}

void Serv::archiveOrder(TraderSess& sess, string_view market, Iden id, Millis now)
{
}

void Serv::archiveOrder(TraderSess& sess, Millis now)
{
}

void Serv::archiveOrder(TraderSess& sess, string_view market, ArrayView<Iden> ids, Millis now)
{
}

ConstExecPtr Serv::createTrade(TraderSess& sess, MarketBook& book, string_view ref, Side side,
                               Lots lots, Ticks ticks, Role role, string_view cpty, Millis created)
{
  return {};
}

void Serv::archiveTrade(TraderSess& sess, Exec& trade, Millis now)
{
}

void Serv::archiveTrade(TraderSess& sess, string_view market, Iden id, Millis now)
{
}

void Serv::archiveTrade(TraderSess& sess, Millis now)
{
}

void Serv::archiveTrade(TraderSess& sess, string_view market, ArrayView<Iden> ids, Millis now)
{
}

void Serv::expireEndOfDay(Millis now)
{
}

void Serv::settlEndOfDay(Millis now)
{
}

void Serv::poll(Millis now)
{
}

Millis Serv::getTimeout() const noexcept
{
  return 0_ms;
}

} // swirly
