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

#include <swirly/fig/Accnt.hpp>
#include <swirly/fig/AsyncJourn.hpp>
#include <swirly/fig/Response.hpp>

#include <swirly/elm/Date.hpp>
#include <swirly/elm/Exception.hpp>
#include <swirly/elm/Journ.hpp>
#include <swirly/elm/MarketBook.hpp>
#include <swirly/elm/Model.hpp>

#include <swirly/ash/Date.hpp>
#include <swirly/ash/Finally.hpp>

#include "Match.hpp"

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

template <typename ValueT>
inline auto& constCast(const ValueT& ref)
{
  return const_cast<ValueT&>(ref);
}

} // anonymous

struct Serv::Impl {

  Impl(Journ& journ, size_t pipeCapacity, size_t maxExecs) noexcept
    : journ_{journ, pipeCapacity}, maxExecs_{maxExecs}
  {
  }

  void load(const Model& model, Millis now)
  {
    const auto busDay = busDay_(now);
    model.readAsset([& assets = assets_](auto ptr) { assets.insert(move(ptr)); });
    model.readContr([& contrs = contrs_](auto ptr) { contrs.insert(move(ptr)); });
    model.readMarket([& markets = markets_](MarketBookPtr ptr) { markets.insert(move(ptr)); });
    model.readOrder([this](auto ptr) {
      auto& accnt = this->accnt(ptr->accnt());
      accnt.insertOrder(ptr);
      bool success{false};
      auto finally = makeFinally([&]() {
        if (!success) {
          accnt.removeOrder(*ptr);
        }
      });
      auto it = this->markets_.find(ptr->market());
      assert(it != this->markets_.end());
      it->insertOrder(ptr);
      success = true;
    });
    model.readAccnt(now, [this, &model](auto mnem) {
      auto& accnt = this->accnt(mnem);
      model.readExec(mnem, accnt.execs().capacity(),
                     [&accnt](auto ptr) { accnt.pushExecBack(ptr); });
    });
    model.readTrade([this](auto ptr) {
      auto& accnt = this->accnt(ptr->accnt());
      accnt.insertTrade(ptr);
    });
    model.readPosn(busDay, [this](auto ptr) {
      auto& accnt = this->accnt(ptr->accnt());
      accnt.insertPosn(ptr);
    });
  }

  const AssetSet& assets() const noexcept { return assets_; }

  const ContrSet& contrs() const noexcept { return contrs_; }

  const MarketSet& markets() const noexcept { return markets_; }

  const MarketBook& market(Mnem mnem) const
  {
    auto it = markets_.find(mnem);
    if (it == markets_.end()) {
      throw MarketNotFoundException{errMsg() << "market '" << mnem << "' does not exist"};
    }
    return *it;
  }

  const Accnt& accnt(Mnem mnem) const
  {
    AccntSet::ConstIterator it;
    bool found;
    tie(it, found) = accnts_.findHint(mnem);
    if (!found) {
      it = accnts_.insertHint(it, Accnt::make(mnem, maxExecs_));
    }
    return *it;
  }

  Accnt& accnt(Mnem mnem)
  {
    AccntSet::Iterator it;
    bool found;
    tie(it, found) = accnts_.findHint(mnem);
    if (!found) {
      it = accnts_.insertHint(it, Accnt::make(mnem, maxExecs_));
    }
    return *it;
  }

  const MarketBook& createMarket(Mnem mnem, string_view display, Mnem contr, JDay settlDay,
                                 JDay expiryDay, MarketState state, Millis now)
  {
    if (contrs_.find(contr) == contrs_.end()) {
      throw NotFoundException{errMsg() << "contr '" << contr << "' does not exist"};
    }
    if (settlDay != 0_jd) {
      // busDay <= expiryDay <= settlDay.
      const auto busDay = busDay_(now);
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
    tie(it, found) = markets_.findHint(mnem);
    if (found) {
      throw AlreadyExistsException{errMsg() << "market '" << mnem << "' already exists"};
    }
    {
      auto market = newMarket(mnem, display, contr, settlDay, expiryDay, state);
      journ_.createMarket(mnem, display, contr, settlDay, expiryDay, state);
      it = markets_.insertHint(it, move(market));
    }
    return *it;
  }

  const MarketBook& updateMarket(Mnem mnem, optional<string_view> display,
                                 optional<MarketState> state, Millis now)
  {
    auto it = markets_.find(mnem);
    if (it == markets_.end()) {
      throw MarketNotFoundException{errMsg() << "market '" << mnem << "' does not exist"};
    }
    journ_.updateMarket(mnem, display ? *display : it->display(), state ? *state : it->state());
    if (display) {
      it->setDisplay(*display);
    }
    if (state) {
      it->setState(*state);
    }
    return *it;
  }

  void createOrder(Accnt& accnt, MarketBook& book, string_view ref, Side side, Lots lots,
                   Ticks ticks, Lots minLots, Millis now, Response& resp)
  {
    // N.B. we only check for duplicates in the refIdx; no unique constraint exists in the database,
    // and order-refs can be reused so long as only one order is live in the system at any given time.
    if (!ref.empty() && accnt.exists(ref)) {
      throw RefAlreadyExistsException{errMsg() << "order '" << ref << "' already exists"};
    }

    const auto busDay = busDay_(now);
    if (book.expiryDay() != 0_jd && book.expiryDay() < busDay) {
      throw MarketClosedException{errMsg() << "market for '" << book.contr() << "' in '"
                                           << maybeJdToIso(book.settlDay()) << "' has expired"};
    }
    if (lots == 0_lts || lots < minLots) {
      throw InvalidLotsException{errMsg() << "invalid lots '" << lots << '\''};
    }
    const auto id = book.allocId();
    auto order = Order::make(accnt.mnem(), book.mnem(), book.contr(), book.settlDay(), id, ref,
                             side, lots, ticks, minLots, now);
    auto exec = newExec(*order, id, now);

    resp.insertOrder(order);
    resp.insertExec(exec);
    // Order fields are updated on match.
    matchOrders(accnt, book, *order, now, resp);
    // Ensure that matches are cleared when scope exits.
    auto finally = makeFinally([& matches = matches_]() { matches.clear(); });

    resp.setBook(book);

    // Avoid allocating position when there are no matches.
    PosnPtr posn;
    if (!matches_.empty()) {
      // Avoid allocating position when there are no matches.
      // N.B. before commit phase, because this may fail.
      posn = accnt.posn(book.contr(), book.settlDay());
      resp.setPosn(posn);
    }

    // Place incomplete order in market.
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

      journ_.createExec(resp.execs());
      success = true;
    }

    // Commit phase.

    if (!order->done()) {
      accnt.insertOrder(order);
    }
    accnt.pushExecFront(exec);

    // Commit matches.
    if (!matches_.empty()) {
      assert(posn);
      commitMatches(accnt, book, now);
      posn->addTrade(order->side(), order->exec(), order->cost());
    }
  }

  void reviseOrder(Accnt& accnt, MarketBook& book, Order& order, Lots lots, Millis now,
                   Response& resp)
  {
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    doReviseOrder(accnt, book, order, lots, now, resp);
  }

  void reviseOrder(Accnt& accnt, MarketBook& book, Id64 id, Lots lots, Millis now, Response& resp)
  {
    auto& order = accnt.order(book.mnem(), id);
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    doReviseOrder(accnt, book, order, lots, now, resp);
  }

  void reviseOrder(Accnt& accnt, MarketBook& book, string_view ref, Lots lots, Millis now,
                   Response& resp)
  {
    auto& order = accnt.order(ref);
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    doReviseOrder(accnt, book, order, lots, now, resp);
  }

  void reviseOrder(Accnt& accnt, MarketBook& book, ArrayView<Id64> ids, Lots lots, Millis now,
                   Response& resp)
  {
    resp.setBook(book);
    for (const auto id : ids) {

      auto& order = accnt.order(book.mnem(), id);
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
      auto exec = newExec(order, book.allocId(), now);
      exec->revise(lots);

      resp.insertOrder(&order);
      resp.insertExec(exec);
    }

    journ_.createExec(resp.execs());

    // Commit phase.

    for (const auto& exec : resp.execs()) {
      auto it = accnt.orders().find(book.mnem(), exec->orderId());
      assert(it != accnt.orders().end());
      book.reviseOrder(*it, lots, now);
      accnt.pushExecFront(exec);
    }
  }

  void cancelOrder(Accnt& accnt, MarketBook& book, Order& order, Millis now, Response& resp)
  {
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    doCancelOrder(accnt, book, order, now, resp);
  }

  void cancelOrder(Accnt& accnt, MarketBook& book, Id64 id, Millis now, Response& resp)
  {
    auto& order = accnt.order(book.mnem(), id);
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    doCancelOrder(accnt, book, order, now, resp);
  }

  void cancelOrder(Accnt& accnt, MarketBook& book, string_view ref, Millis now, Response& resp)
  {
    auto& order = accnt.order(ref);
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    doCancelOrder(accnt, book, order, now, resp);
  }

  void cancelOrder(Accnt& accnt, MarketBook& book, ArrayView<Id64> ids, Millis now, Response& resp)
  {
    resp.setBook(book);
    for (const auto id : ids) {

      auto& order = accnt.order(book.mnem(), id);
      if (order.done()) {
        throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
      }
      auto exec = newExec(order, book.allocId(), now);
      exec->cancel();

      resp.insertOrder(&order);
      resp.insertExec(exec);
    }

    journ_.createExec(resp.execs());

    // Commit phase.

    for (const auto& exec : resp.execs()) {
      auto it = accnt.orders().find(book.mnem(), exec->orderId());
      assert(it != accnt.orders().end());
      book.cancelOrder(*it, now);
      accnt.removeOrder(*it);
      accnt.pushExecFront(exec);
    }
  }

  void cancelOrder(Accnt& accnt, Millis now)
  {
    // FIXME: Not implemented.
  }

  void cancelOrder(MarketBook& book, Millis now)
  {
    // FIXME: Not implemented.
  }

  TradePair createTrade(Accnt& accnt, MarketBook& book, string_view ref, Side side, Lots lots,
                        Ticks ticks, LiqInd liqInd, Mnem cpty, Millis created)
  {
    auto posn = accnt.posn(book.contr(), book.settlDay());
    auto trade = newManual(accnt.mnem(), book, ref, side, lots, ticks, liqInd, cpty, created);
    decltype(trade) cptyTrade;

    if (!cpty.empty()) {

      // Create back-to-back trade if counter-party is specified.
      auto& cptyAccnt = this->accnt(cpty);
      auto cptyPosn = cptyAccnt.posn(book.contr(), book.settlDay());
      cptyTrade = trade->inverse(book.allocId());

      ConstExecPtr trades[] = {trade, cptyTrade};
      journ_.createExec(trades);

      // Commit phase.

      cptyAccnt.pushExecFront(cptyTrade);
      cptyAccnt.insertTrade(cptyTrade);
      cptyPosn->addTrade(cptyTrade->side(), cptyTrade->lastLots(), cptyTrade->lastTicks());

    } else {

      journ_.createExec(*trade);

      // Commit phase.
    }
    accnt.pushExecFront(trade);
    accnt.insertTrade(trade);
    posn->addTrade(trade->side(), trade->lastLots(), trade->lastTicks());

    return {trade, cptyTrade};
  }

  void archiveTrade(Accnt& accnt, const Exec& trade, Millis now)
  {
    if (trade.state() != State::Trade) {
      throw InvalidException{errMsg() << "exec '" << trade.id() << "' is not a trade"};
    }
    doArchiveTrade(accnt, trade, now);
  }

  void archiveTrade(Accnt& accnt, Mnem market, Id64 id, Millis now)
  {
    auto& trade = accnt.trade(market, id);
    doArchiveTrade(accnt, trade, now);
  }

  void archiveTrade(Accnt& accnt, Mnem market, ArrayView<Id64> ids, Millis now)
  {
    for (const auto id : ids) {
      accnt.trade(market, id);
    }

    journ_.archiveTrade(market, ids, now);

    // Commit phase.

    for (const auto id : ids) {

      auto it = accnt.trades().find(market, id);
      assert(it != accnt.trades().end());
      accnt.removeTrade(*it);
    }
  }

  void expireEndOfDay(Millis now)
  {
    // FIXME: Not implemented.
  }

  void settlEndOfDay(Millis now)
  {
    // FIXME: Not implemented.
  }

 private:
  MarketBookPtr newMarket(Mnem mnem, string_view display, Mnem contr, JDay settlDay, JDay expiryDay,
                          MarketState state) const
  {
    if (!regex_match(mnem.begin(), mnem.end(), MnemPattern)) {
      throw InvalidException{errMsg() << "invalid mnem '" << mnem << '\''};
    }
    return MarketBook::make(mnem, display, contr, settlDay, expiryDay, state);
  }

  ExecPtr newExec(const Order& order, Id64 id, Millis created) const
  {
    return Exec::make(order.accnt(), order.market(), order.contr(), order.settlDay(), id,
                      order.ref(), order.id(), order.state(), order.side(), order.lots(),
                      order.ticks(), order.resd(), order.exec(), order.cost(), order.lastLots(),
                      order.lastTicks(), order.minLots(), 0_id64, LiqInd::None, Mnem{}, created);
  }

  /**
   * Special factory method for manual trades.
   */
  ExecPtr newManual(Mnem accnt, Mnem market, Mnem contr, JDay settlDay, Id64 id, string_view ref,
                    Side side, Lots lots, Ticks ticks, LiqInd liqInd, Mnem cpty,
                    Millis created) const
  {
    const auto orderId = 0_id64;
    const auto state = State::Trade;
    const auto resd = 0_lts;
    const auto exec = lots;
    const auto cost = swirly::cost(lots, ticks);
    const auto lastLots = lots;
    const auto lastTicks = ticks;
    const auto minLots = 1_lts;
    const auto matchId = 0_id64;
    return Exec::make(accnt, market, contr, settlDay, id, ref, orderId, state, side, lots, ticks,
                      resd, exec, cost, lastLots, lastTicks, minLots, matchId, liqInd, cpty,
                      created);
  }

  ExecPtr newManual(Mnem accnt, MarketBook& book, string_view ref, Side side, Lots lots,
                    Ticks ticks, LiqInd liqInd, Mnem cpty, Millis created) const
  {
    return newManual(accnt, book.mnem(), book.contr(), book.settlDay(), book.allocId(), ref, side,
                     lots, ticks, liqInd, cpty, created);
  }

  Match newMatch(MarketBook& book, const Order& takerOrder, const OrderPtr& makerOrder, Lots lots,
                 Lots sumLots, Cost sumCost, Millis created)
  {
    const auto makerId = book.allocId();
    const auto takerId = book.allocId();

    auto it = accnts_.find(makerOrder->accnt());
    assert(it != accnts_.end());
    auto& makerAccnt = *it;
    auto makerPosn = makerAccnt.posn(book.contr(), book.settlDay());

    const auto ticks = makerOrder->ticks();

    auto makerTrade = newExec(*makerOrder, makerId, created);
    makerTrade->trade(lots, ticks, takerId, LiqInd::Maker, takerOrder.accnt());

    auto takerTrade = newExec(takerOrder, takerId, created);
    takerTrade->trade(sumLots, sumCost, lots, ticks, makerId, LiqInd::Taker, makerOrder->accnt());

    return {lots, makerOrder, makerTrade, makerPosn, takerTrade};
  }

  void matchOrders(const Accnt& takerAccnt, MarketBook& book, Order& takerOrder, BookSide& side,
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
      if (makerOrder.accnt() == takerAccnt.mnem()) {
        // Maker updated first because this is consistent with last-look semantics.
        // N.B. the reference count is not incremented here.
        resp.insertOrder(&makerOrder);
        resp.insertExec(match.makerTrade);
      }
      resp.insertExec(match.takerTrade);

      matches_.push_back(move(match));
    }

    if (!matches_.empty()) {
      takerOrder.trade(sumLots, sumCost, lastLots, lastTicks, now);
    }
  }

  void matchOrders(const Accnt& takerAccnt, MarketBook& book, Order& takerOrder, Millis now,
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
    matchOrders(takerAccnt, book, takerOrder, *bookSide, direct, now, resp);
  }

  // Assumes that maker lots have not been reduced since matching took place. N.B. this function is
  // responsible for committing a transaction, so it is particularly important that it does not
  // throw.
  void commitMatches(Accnt& takerAccnt, MarketBook& book, Millis now) noexcept
  {
    for (const auto& match : matches_) {
      const auto makerOrder = match.makerOrder;
      assert(makerOrder);
      // Reduce maker.
      book.takeOrder(*makerOrder, match.lots, now);
      // Must succeed because maker order exists.
      auto it = accnts_.find(makerOrder->accnt());
      assert(it != accnts_.end());
      auto& makerAccnt = *it;
      // Maker updated first because this is consistent with last-look semantics.
      // Update maker.
      const auto makerTrade = match.makerTrade;
      assert(makerTrade);
      if (makerOrder->done()) {
        makerAccnt.removeOrder(*makerOrder);
      }
      makerAccnt.pushExecFront(makerTrade);
      makerAccnt.insertTrade(makerTrade);
      match.makerPosn->addTrade(makerTrade->side(), makerTrade->lastLots(),
                                makerTrade->lastTicks());
      // Update taker.
      const auto takerTrade = match.takerTrade;
      assert(takerTrade);
      takerAccnt.pushExecFront(takerTrade);
      takerAccnt.insertTrade(takerTrade);
    }
  }

  void doReviseOrder(Accnt& accnt, MarketBook& book, Order& order, Lots lots, Millis now,
                     Response& resp)
  {
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
    auto exec = newExec(order, book.allocId(), now);
    exec->revise(lots);

    resp.setBook(book);
    resp.insertOrder(&order);
    resp.insertExec(exec);

    journ_.createExec(*exec);

    // Commit phase.

    book.reviseOrder(order, lots, now);
    accnt.pushExecFront(exec);
  }
  void doCancelOrder(Accnt& accnt, MarketBook& book, Order& order, Millis now, Response& resp)
  {
    auto exec = newExec(order, book.allocId(), now);
    exec->cancel();

    resp.setBook(book);
    resp.insertOrder(&order);
    resp.insertExec(exec);

    journ_.createExec(*exec);

    // Commit phase.

    book.cancelOrder(order, now);
    accnt.removeOrder(order);
    accnt.pushExecFront(exec);
  }

  void doArchiveTrade(Accnt& accnt, const Exec& trade, Millis now)
  {
    journ_.archiveTrade(trade.market(), trade.id(), now);

    // Commit phase.

    accnt.removeTrade(trade);
  }

  AsyncJourn journ_;
  const BusinessDay busDay_{RollHour, NewYork};
  const size_t maxExecs_;
  AssetSet assets_;
  ContrSet contrs_;
  MarketSet markets_;
  mutable AccntSet accnts_;
  vector<Match> matches_;
};

Serv::Serv(Journ& journ, size_t pipeCapacity, size_t maxExecs)
  : impl_{make_unique<Impl>(journ, pipeCapacity, maxExecs)}
{
}

Serv::~Serv() noexcept = default;

Serv::Serv(Serv&&) = default;

Serv& Serv::operator=(Serv&&) = default;

void Serv::load(const Model& model, Millis now)
{
  impl_->load(model, now);
}

const AssetSet& Serv::assets() const noexcept
{
  return impl_->assets();
}

const ContrSet& Serv::contrs() const noexcept
{
  return impl_->contrs();
}

const MarketSet& Serv::markets() const noexcept
{
  return impl_->markets();
}

const MarketBook& Serv::market(Mnem mnem) const
{
  return impl_->market(mnem);
}

const Accnt& Serv::accnt(Mnem mnem) const
{
  return impl_->accnt(mnem);
}

const MarketBook& Serv::createMarket(Mnem mnem, string_view display, Mnem contr, JDay settlDay,
                                     JDay expiryDay, MarketState state, Millis now)
{
  return impl_->createMarket(mnem, display, contr, settlDay, expiryDay, state, now);
}

const MarketBook& Serv::updateMarket(Mnem mnem, optional<string_view> display,
                                     optional<MarketState> state, Millis now)
{
  return impl_->updateMarket(mnem, display, state, now);
}

void Serv::createOrder(const Accnt& accnt, const MarketBook& book, string_view ref, Side side,
                       Lots lots, Ticks ticks, Lots minLots, Millis now, Response& resp)
{
  impl_->createOrder(constCast(accnt), constCast(book), ref, side, lots, ticks, minLots, now, resp);
}

void Serv::reviseOrder(const Accnt& accnt, const MarketBook& book, const Order& order, Lots lots,
                       Millis now, Response& resp)
{
  impl_->reviseOrder(constCast(accnt), constCast(book), constCast(order), lots, now, resp);
}

void Serv::reviseOrder(const Accnt& accnt, const MarketBook& book, Id64 id, Lots lots, Millis now,
                       Response& resp)
{
  impl_->reviseOrder(constCast(accnt), constCast(book), id, lots, now, resp);
}

void Serv::reviseOrder(const Accnt& accnt, const MarketBook& book, string_view ref, Lots lots,
                       Millis now, Response& resp)
{
  impl_->reviseOrder(constCast(accnt), constCast(book), ref, lots, now, resp);
}

void Serv::reviseOrder(const Accnt& accnt, const MarketBook& book, ArrayView<Id64> ids, Lots lots,
                       Millis now, Response& resp)
{
  impl_->reviseOrder(constCast(accnt), constCast(book), ids, lots, now, resp);
}

void Serv::cancelOrder(const Accnt& accnt, const MarketBook& book, const Order& order, Millis now,
                       Response& resp)
{
  impl_->cancelOrder(constCast(accnt), constCast(book), constCast(order), now, resp);
}

void Serv::cancelOrder(const Accnt& accnt, const MarketBook& book, Id64 id, Millis now,
                       Response& resp)
{
  impl_->cancelOrder(constCast(accnt), constCast(book), id, now, resp);
}

void Serv::cancelOrder(const Accnt& accnt, const MarketBook& book, string_view ref, Millis now,
                       Response& resp)
{
  impl_->cancelOrder(constCast(accnt), constCast(book), ref, now, resp);
}

void Serv::cancelOrder(const Accnt& accnt, const MarketBook& book, ArrayView<Id64> ids, Millis now,
                       Response& resp)
{
  impl_->cancelOrder(constCast(accnt), constCast(book), ids, now, resp);
}

void Serv::cancelOrder(const Accnt& accnt, Millis now)
{
  impl_->cancelOrder(constCast(accnt), now);
}

void Serv::cancelOrder(const MarketBook& book, Millis now)
{
  impl_->cancelOrder(constCast(book), now);
}

TradePair Serv::createTrade(const Accnt& accnt, const MarketBook& book, string_view ref, Side side,
                            Lots lots, Ticks ticks, LiqInd liqInd, Mnem cpty, Millis created)
{
  return impl_->createTrade(constCast(accnt), constCast(book), ref, side, lots, ticks, liqInd, cpty,
                            created);
}

void Serv::archiveTrade(const Accnt& accnt, const Exec& trade, Millis now)
{
  impl_->archiveTrade(constCast(accnt), trade, now);
}

void Serv::archiveTrade(const Accnt& accnt, Mnem market, Id64 id, Millis now)
{
  impl_->archiveTrade(constCast(accnt), market, id, now);
}

void Serv::archiveTrade(const Accnt& accnt, Mnem market, ArrayView<Id64> ids, Millis now)
{
  impl_->archiveTrade(constCast(accnt), market, ids, now);
}

void Serv::expireEndOfDay(Millis now)
{
  impl_->expireEndOfDay(now);
}

void Serv::settlEndOfDay(Millis now)
{
  impl_->settlEndOfDay(now);
}

} // swirly
