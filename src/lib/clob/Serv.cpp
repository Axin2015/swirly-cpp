/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#include <swirly/clob/Serv.hpp>

#include <swirly/clob/Accnt.hpp>
#include <swirly/clob/AsyncJourn.hpp>
#include <swirly/clob/Response.hpp>

#include <swirly/fin/Date.hpp>
#include <swirly/fin/Exception.hpp>
#include <swirly/fin/Journ.hpp>
#include <swirly/fin/Model.hpp>

#include <swirly/util/Date.hpp>
#include <swirly/util/Finally.hpp>

#include "Match.hpp"

#include <regex>

using namespace std;

namespace swirly {
namespace {

const regex MnemPattern{R"(^[0-9A-Za-z-._]{3,16}$)"};

Ticks spread(const Order& takerOrder, const Order& makerOrder, Direct direct) noexcept
{
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
    matches_.reserve(8);
    execs_.reserve(1 + 16);
  }

  void load(const Model& model, Time now)
  {
    const auto busDay = busDay_(now);
    model.readAsset([& assets = assets_](auto ptr) { assets.insert(move(ptr)); });
    model.readContr([& contrs = contrs_](auto ptr) { contrs.insert(move(ptr)); });
    model.readAccnt(now, [this, &model](auto mnem) {
      auto& accnt = this->accnt(mnem);
      model.readExec(mnem, accnt.execs().capacity(),
                     [&accnt](auto ptr) { accnt.pushExecBack(ptr); });
    });
    model.readMarket([& markets = markets_](MarketPtr ptr) { markets.insert(ptr); });
    model.readOrder([this](auto ptr) {
      auto& accnt = this->accnt(ptr->accnt());
      accnt.insertOrder(ptr);
      bool success{false};
      auto finally = makeFinally([&]() {
        if (!success) {
          accnt.removeOrder(*ptr);
        }
      });
      auto it = this->markets_.find(ptr->marketId());
      assert(it != this->markets_.end());
      it->insertOrder(ptr);
      success = true;
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

  const Contr& contr(Mnem mnem) const
  {
    auto it = contrs_.find(mnem);
    if (it == contrs_.end()) {
      throw MarketNotFoundException{errMsg() << "contract '" << mnem << "' does not exist"};
    }
    return *it;
  }

  const ContrSet& contrs() const noexcept { return contrs_; }

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

  const Market& market(Id64 id) const
  {
    auto it = markets_.find(id);
    if (it == markets_.end()) {
      throw MarketNotFoundException{errMsg() << "market '" << id << "' does not exist"};
    }
    return *it;
  }

  const MarketSet& markets() const noexcept { return markets_; }

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

  const Market& createMarket(const Contr& contr, JDay settlDay, MarketState state, Time now)
  {
    if (settlDay != 0_jd) {
      // busDay <= settlDay.
      const auto busDay = busDay_(now);
      if (settlDay < busDay) {
        throw InvalidException{"settl-day before bus-day"_sv};
      }
    }
    const auto id = toMarketId(contr.id(), settlDay);

    MarketSet::Iterator it;
    bool found;
    tie(it, found) = markets_.findHint(id);
    if (found) {
      throw AlreadyExistsException{errMsg() << "market for '" << contr.mnem() << "' on "
                                            << jdToIso(settlDay) << " already exists"};
    }
    {
      auto market = Market::make(id, contr.mnem(), settlDay, state);
      journ_.createMarket(id, contr.mnem(), settlDay, state);
      it = markets_.insertHint(it, market);
    }
    return *it;
  }

  void updateMarket(Market& market, MarketState state, Time now)
  {
    journ_.updateMarket(market.id(), state);
    market.setState(state);
  }

  void createOrder(Accnt& accnt, Market& market, string_view ref, Side side, Lots lots, Ticks ticks,
                   Lots minLots, Time now, Response& resp)
  {
    // N.B. we only check for duplicates in the refIdx; no unique constraint exists in the database,
    // and order-refs can be reused so long as only one order is live in the system at any given
    // time.
    if (!ref.empty() && accnt.exists(ref)) {
      throw RefAlreadyExistsException{errMsg() << "order '" << ref << "' already exists"};
    }

    const auto busDay = busDay_(now);
    if (market.settlDay() != 0_jd && market.settlDay() < busDay) {
      throw MarketClosedException{errMsg() << "market for '" << market.contr() << "' on "
                                           << jdToIso(market.settlDay()) << " has closed"};
    }
    if (lots == 0_lts || lots < minLots) {
      throw InvalidLotsException{errMsg() << "invalid lots '" << lots << '\''};
    }
    const auto id = market.allocId();
    auto order = Order::make(accnt.mnem(), market.id(), market.contr(), market.settlDay(), id, ref,
                             side, lots, ticks, minLots, now);
    auto exec = newExec(*order, id, now);

    resp.insertOrder(order);
    resp.insertExec(exec);

    // Ensure that matches are cleared when scope exits.
    auto finally = makeFinally([this]() {
      this->matches_.clear();
      this->execs_.clear();
    });
    execs_.push_back(exec);
    // Order fields are updated on match.
    matchOrders(accnt, market, *order, now, resp);

    resp.setMarket(&market);

    // Avoid allocating position when there are no matches.
    PosnPtr posn;
    if (!matches_.empty()) {
      // Avoid allocating position when there are no matches.
      // N.B. before commit phase, because this may fail.
      posn = accnt.posn(market.id(), market.contr(), market.settlDay());
      resp.setPosn(posn);
    }

    // Place incomplete order in market.
    if (!order->done()) {
      // This may fail if level cannot be allocated.
      market.insertOrder(order);
    }
    {
      // TODO: IOC orders would need an additional revision for the unsolicited cancellation of any
      // unfilled quantity.
      bool success{false};
      auto finally = makeFinally([&market, &order, &success]() {
        if (!success && !order->done()) {
          // Undo market insertion.
          market.removeOrder(*order);
        }
      });

      journ_.createExec(execs_);
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
      commitMatches(accnt, market, now);
      posn->addTrade(order->side(), order->exec(), order->cost());
    }
  }

  void reviseOrder(Accnt& accnt, Market& market, Order& order, Lots lots, Time now, Response& resp)
  {
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    doReviseOrder(accnt, market, order, lots, now, resp);
  }

  void reviseOrder(Accnt& accnt, Market& market, Id64 id, Lots lots, Time now, Response& resp)
  {
    auto& order = accnt.order(market.id(), id);
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    doReviseOrder(accnt, market, order, lots, now, resp);
  }

  void reviseOrder(Accnt& accnt, Market& market, string_view ref, Lots lots, Time now,
                   Response& resp)
  {
    auto& order = accnt.order(ref);
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    doReviseOrder(accnt, market, order, lots, now, resp);
  }

  void reviseOrder(Accnt& accnt, Market& market, ArrayView<Id64> ids, Lots lots, Time now,
                   Response& resp)
  {
    resp.setMarket(&market);
    for (const auto id : ids) {

      auto& order = accnt.order(market.id(), id);
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
      auto exec = newExec(order, market.allocId(), now);
      exec->revise(lots);

      resp.insertOrder(&order);
      resp.insertExec(exec);
    }

    journ_.createExec(resp.execs());

    // Commit phase.

    for (const auto& exec : resp.execs()) {
      auto it = accnt.orders().find(market.id(), exec->orderId());
      assert(it != accnt.orders().end());
      market.reviseOrder(*it, lots, now);
      accnt.pushExecFront(exec);
    }
  }

  void cancelOrder(Accnt& accnt, Market& market, Order& order, Time now, Response& resp)
  {
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    doCancelOrder(accnt, market, order, now, resp);
  }

  void cancelOrder(Accnt& accnt, Market& market, Id64 id, Time now, Response& resp)
  {
    auto& order = accnt.order(market.id(), id);
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    doCancelOrder(accnt, market, order, now, resp);
  }

  void cancelOrder(Accnt& accnt, Market& market, string_view ref, Time now, Response& resp)
  {
    auto& order = accnt.order(ref);
    if (order.done()) {
      throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
    }
    doCancelOrder(accnt, market, order, now, resp);
  }

  void cancelOrder(Accnt& accnt, Market& market, ArrayView<Id64> ids, Time now, Response& resp)
  {
    resp.setMarket(&market);
    for (const auto id : ids) {

      auto& order = accnt.order(market.id(), id);
      if (order.done()) {
        throw TooLateException{errMsg() << "order '" << order.id() << "' is done"};
      }
      auto exec = newExec(order, market.allocId(), now);
      exec->cancel();

      resp.insertOrder(&order);
      resp.insertExec(exec);
    }

    journ_.createExec(resp.execs());

    // Commit phase.

    for (const auto& exec : resp.execs()) {
      auto it = accnt.orders().find(market.id(), exec->orderId());
      assert(it != accnt.orders().end());
      market.cancelOrder(*it, now);
      accnt.removeOrder(*it);
      accnt.pushExecFront(exec);
    }
  }

  void cancelOrder(Accnt& accnt, Time now)
  {
    // FIXME: Not implemented.
  }

  void cancelOrder(Market& market, Time now)
  {
    // FIXME: Not implemented.
  }

  TradePair createTrade(Accnt& accnt, Market& market, string_view ref, Side side, Lots lots,
                        Ticks ticks, LiqInd liqInd, Mnem cpty, Time created)
  {
    auto posn = accnt.posn(market.id(), market.contr(), market.settlDay());
    auto trade = newManual(accnt.mnem(), market, ref, side, lots, ticks, liqInd, cpty, created);
    decltype(trade) cptyTrade;

    if (!cpty.empty()) {

      // Create back-to-back trade if counter-party is specified.
      auto& cptyAccnt = this->accnt(cpty);
      auto cptyPosn = cptyAccnt.posn(market.id(), market.contr(), market.settlDay());
      cptyTrade = trade->inverse(market.allocId());

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

  void archiveTrade(Accnt& accnt, const Exec& trade, Time now)
  {
    if (trade.state() != State::Trade) {
      throw InvalidException{errMsg() << "exec '" << trade.id() << "' is not a trade"};
    }
    doArchiveTrade(accnt, trade, now);
  }

  void archiveTrade(Accnt& accnt, Id64 marketId, Id64 id, Time now)
  {
    auto& trade = accnt.trade(marketId, id);
    doArchiveTrade(accnt, trade, now);
  }

  void archiveTrade(Accnt& accnt, Id64 marketId, ArrayView<Id64> ids, Time now)
  {
    for (const auto id : ids) {
      accnt.trade(marketId, id);
    }

    journ_.archiveTrade(marketId, ids, now);

    // Commit phase.

    for (const auto id : ids) {

      auto it = accnt.trades().find(marketId, id);
      assert(it != accnt.trades().end());
      accnt.removeTrade(*it);
    }
  }

  void expireEndOfDay(Time now)
  {
    // FIXME: Not implemented.
  }

  void settlEndOfDay(Time now)
  {
    // FIXME: Not implemented.
  }

 private:
  ExecPtr newExec(const Order& order, Id64 id, Time created) const
  {
    return Exec::make(order.accnt(), order.marketId(), order.contr(), order.settlDay(), id,
                      order.id(), order.ref(), order.state(), order.side(), order.lots(),
                      order.ticks(), order.resd(), order.exec(), order.cost(), order.lastLots(),
                      order.lastTicks(), order.minLots(), 0_id64, LiqInd::None, Mnem{}, created);
  }

  /**
   * Special factory method for manual trades.
   */
  ExecPtr newManual(Id64 marketId, Mnem contr, JDay settlDay, Id64 id, Mnem accnt, string_view ref,
                    Side side, Lots lots, Ticks ticks, LiqInd liqInd, Mnem cpty, Time created) const
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
    return Exec::make(accnt, marketId, contr, settlDay, id, orderId, ref, state, side, lots, ticks,
                      resd, exec, cost, lastLots, lastTicks, minLots, matchId, liqInd, cpty,
                      created);
  }

  ExecPtr newManual(Mnem accnt, Market& market, string_view ref, Side side, Lots lots, Ticks ticks,
                    LiqInd liqInd, Mnem cpty, Time created) const
  {
    return newManual(market.id(), market.contr(), market.settlDay(), market.allocId(), accnt, ref,
                     side, lots, ticks, liqInd, cpty, created);
  }

  Match newMatch(Market& market, const Order& takerOrder, const OrderPtr& makerOrder, Lots lots,
                 Lots sumLots, Cost sumCost, Time created)
  {
    const auto makerId = market.allocId();
    const auto takerId = market.allocId();

    auto it = accnts_.find(makerOrder->accnt());
    assert(it != accnts_.end());
    auto& makerAccnt = *it;
    auto makerPosn = makerAccnt.posn(market.id(), market.contr(), market.settlDay());

    const auto ticks = makerOrder->ticks();

    auto makerTrade = newExec(*makerOrder, makerId, created);
    makerTrade->trade(lots, ticks, takerId, LiqInd::Maker, takerOrder.accnt());

    auto takerTrade = newExec(takerOrder, takerId, created);
    takerTrade->trade(sumLots, sumCost, lots, ticks, makerId, LiqInd::Taker, makerOrder->accnt());

    return {lots, makerOrder, makerTrade, makerPosn, takerTrade};
  }

  void matchOrders(const Accnt& takerAccnt, Market& market, Order& takerOrder, MarketSide& side,
                   Direct direct, Time now, Response& resp)
  {
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

      auto match = newMatch(market, takerOrder, &makerOrder, lots, sumLots, sumCost, now);

      // Insert order if trade crossed with self.
      if (makerOrder.accnt() == takerAccnt.mnem()) {
        // Maker updated first because this is consistent with last-look semantics.
        // N.B. the reference count is not incremented here.
        resp.insertOrder(&makerOrder);
        resp.insertExec(match.makerTrade);
      }
      resp.insertExec(match.takerTrade);

      matches_.push_back(move(match));
      execs_.push_back(match.makerTrade);
      execs_.push_back(match.takerTrade);
    }

    if (!matches_.empty()) {
      takerOrder.trade(sumLots, sumCost, lastLots, lastTicks, now);
    }
  }

  void matchOrders(const Accnt& takerAccnt, Market& market, Order& takerOrder, Time now,
                   Response& resp)
  {
    MarketSide* marketSide;
    Direct direct;
    if (takerOrder.side() == Side::Buy) {
      // Paid when the taker lifts the offer.
      marketSide = &market.offerSide();
      direct = Direct::Paid;
    } else {
      assert(takerOrder.side() == Side::Sell);
      // Given when the taker hits the bid.
      marketSide = &market.bidSide();
      direct = Direct::Given;
    }
    matchOrders(takerAccnt, market, takerOrder, *marketSide, direct, now, resp);
  }

  // Assumes that maker lots have not been reduced since matching took place. N.B. this function is
  // responsible for committing a transaction, so it is particularly important that it does not
  // throw.
  void commitMatches(Accnt& takerAccnt, Market& market, Time now) noexcept
  {
    for (const auto& match : matches_) {
      const auto makerOrder = match.makerOrder;
      assert(makerOrder);
      // Reduce maker.
      market.takeOrder(*makerOrder, match.lots, now);
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

  void doReviseOrder(Accnt& accnt, Market& market, Order& order, Lots lots, Time now,
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
    auto exec = newExec(order, market.allocId(), now);
    exec->revise(lots);

    resp.setMarket(&market);
    resp.insertOrder(&order);
    resp.insertExec(exec);

    journ_.createExec(*exec);

    // Commit phase.

    market.reviseOrder(order, lots, now);
    accnt.pushExecFront(exec);
  }
  void doCancelOrder(Accnt& accnt, Market& market, Order& order, Time now, Response& resp)
  {
    auto exec = newExec(order, market.allocId(), now);
    exec->cancel();

    resp.setMarket(&market);
    resp.insertOrder(&order);
    resp.insertExec(exec);

    journ_.createExec(*exec);

    // Commit phase.

    market.cancelOrder(order, now);
    accnt.removeOrder(order);
    accnt.pushExecFront(exec);
  }

  void doArchiveTrade(Accnt& accnt, const Exec& trade, Time now)
  {
    journ_.archiveTrade(trade.marketId(), trade.id(), now);

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
  vector<ConstExecPtr> execs_;
};

Serv::Serv(Journ& journ, size_t pipeCapacity, size_t maxExecs)
  : impl_{make_unique<Impl>(journ, pipeCapacity, maxExecs)}
{
}

Serv::~Serv() noexcept = default;

Serv::Serv(Serv&&) = default;

Serv& Serv::operator=(Serv&&) = default;

void Serv::load(const Model& model, Time now)
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

const Contr& Serv::contr(Mnem mnem) const
{
  return impl_->contr(mnem);
}

const Market& Serv::market(Id64 id) const
{
  return impl_->market(id);
}

const Accnt& Serv::accnt(Mnem mnem) const
{
  return impl_->accnt(mnem);
}

const Market& Serv::createMarket(const Contr& contr, JDay settlDay, MarketState state, Time now)
{
  return impl_->createMarket(contr, settlDay, state, now);
}

void Serv::updateMarket(const Market& market, MarketState state, Time now)
{
  return impl_->updateMarket(constCast(market), state, now);
}

void Serv::createOrder(const Accnt& accnt, const Market& market, string_view ref, Side side,
                       Lots lots, Ticks ticks, Lots minLots, Time now, Response& resp)
{
  impl_->createOrder(constCast(accnt), constCast(market), ref, side, lots, ticks, minLots, now,
                     resp);
}

void Serv::reviseOrder(const Accnt& accnt, const Market& market, const Order& order, Lots lots,
                       Time now, Response& resp)
{
  impl_->reviseOrder(constCast(accnt), constCast(market), constCast(order), lots, now, resp);
}

void Serv::reviseOrder(const Accnt& accnt, const Market& market, Id64 id, Lots lots, Time now,
                       Response& resp)
{
  impl_->reviseOrder(constCast(accnt), constCast(market), id, lots, now, resp);
}

void Serv::reviseOrder(const Accnt& accnt, const Market& market, string_view ref, Lots lots,
                       Time now, Response& resp)
{
  impl_->reviseOrder(constCast(accnt), constCast(market), ref, lots, now, resp);
}

void Serv::reviseOrder(const Accnt& accnt, const Market& market, ArrayView<Id64> ids, Lots lots,
                       Time now, Response& resp)
{
  impl_->reviseOrder(constCast(accnt), constCast(market), ids, lots, now, resp);
}

void Serv::cancelOrder(const Accnt& accnt, const Market& market, const Order& order, Time now,
                       Response& resp)
{
  impl_->cancelOrder(constCast(accnt), constCast(market), constCast(order), now, resp);
}

void Serv::cancelOrder(const Accnt& accnt, const Market& market, Id64 id, Time now, Response& resp)
{
  impl_->cancelOrder(constCast(accnt), constCast(market), id, now, resp);
}

void Serv::cancelOrder(const Accnt& accnt, const Market& market, string_view ref, Time now,
                       Response& resp)
{
  impl_->cancelOrder(constCast(accnt), constCast(market), ref, now, resp);
}

void Serv::cancelOrder(const Accnt& accnt, const Market& market, ArrayView<Id64> ids, Time now,
                       Response& resp)
{
  impl_->cancelOrder(constCast(accnt), constCast(market), ids, now, resp);
}

void Serv::cancelOrder(const Accnt& accnt, Time now)
{
  impl_->cancelOrder(constCast(accnt), now);
}

void Serv::cancelOrder(const Market& market, Time now)
{
  impl_->cancelOrder(constCast(market), now);
}

TradePair Serv::createTrade(const Accnt& accnt, const Market& market, string_view ref, Side side,
                            Lots lots, Ticks ticks, LiqInd liqInd, Mnem cpty, Time created)
{
  return impl_->createTrade(constCast(accnt), constCast(market), ref, side, lots, ticks, liqInd,
                            cpty, created);
}

void Serv::archiveTrade(const Accnt& accnt, const Exec& trade, Time now)
{
  impl_->archiveTrade(constCast(accnt), trade, now);
}

void Serv::archiveTrade(const Accnt& accnt, Id64 marketId, Id64 id, Time now)
{
  impl_->archiveTrade(constCast(accnt), marketId, id, now);
}

void Serv::archiveTrade(const Accnt& accnt, Id64 marketId, ArrayView<Id64> ids, Time now)
{
  impl_->archiveTrade(constCast(accnt), marketId, ids, now);
}

void Serv::expireEndOfDay(Time now)
{
  impl_->expireEndOfDay(now);
}

void Serv::settlEndOfDay(Time now)
{
  impl_->settlEndOfDay(now);
}

} // swirly
