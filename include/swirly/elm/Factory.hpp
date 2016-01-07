/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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
#ifndef SWIRLY_ELM_FACTORY_HPP
#define SWIRLY_ELM_FACTORY_HPP

#include <swirly/elm/Asset.hpp>
#include <swirly/elm/Contr.hpp>
#include <swirly/elm/Exec.hpp>
#include <swirly/elm/Market.hpp>
#include <swirly/elm/Order.hpp>
#include <swirly/elm/Posn.hpp>
#include <swirly/elm/Trader.hpp>

namespace swirly {

class Asset;
class Contr;
class Market;
class Trader;

/**
 * @addtogroup Entity
 * @{
 */

class SWIRLY_API Factory {
 protected:
    virtual std::unique_ptr<Asset> doNewAsset(const StringView& mnem, const StringView& display,
                                              AssetType type) const = 0;

    virtual std::unique_ptr<Contr> doNewContr(const StringView& mnem, const StringView& display,
                                              const StringView& asset, const StringView& ccy,
                                              int lotNumer, int lotDenom,
                                              int tickNumer, int tickDenom, int pipDp,
                                              Lots minLots, Lots maxLots) const = 0;

    virtual std::unique_ptr<Market> doNewMarket(const StringView& mnem, const StringView& display,
                                                const StringView& contr, Jd settlDay,
                                                Jd expiryDay, MarketState state, Lots lastLots,
                                                Ticks lastTicks, Millis lastTime, Iden maxOrderId,
                                                Iden maxExecId, Iden maxQuoteId) const = 0;

    virtual std::unique_ptr<Trader> doNewTrader(const StringView& mnem, const StringView& display,
                                                const StringView& email) const = 0;

    virtual OrderPtr doNewOrder(const StringView& trader, const StringView& market,
                                const StringView& contr, Jd settlDay, Iden id,
                                const StringView& ref, Iden quoteId, State state, Side side,
                                Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                                Lots lastLots, Ticks lastTicks, Lots minLots, bool pecan,
                                Millis created, Millis modified) const = 0;

    virtual ExecPtr doNewExec(const StringView& trader, const StringView& market,
                              const StringView& contr, Jd settlDay, Iden id,
                              const StringView& ref, Iden orderId, Iden quoteId, State state,
                              Side side, Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                              Lots lastLots, Ticks lastTicks, Lots minLots, Iden matchId,
                              Role role, const StringView& cpty, Millis created) const = 0;

    virtual PosnPtr doNewPosn(const StringView& trader, const StringView& contr, Jd settlDay,
                              Lots buyLots, Cost buyCost, Lots sellLots, Cost sellCost) const = 0;

 public:
    Factory() noexcept = default;
    virtual ~Factory() noexcept;

    // Copy.
    constexpr Factory(const Factory&) noexcept = default;
    Factory& operator =(const Factory&) noexcept = default;

    // Move.
    constexpr Factory(Factory&&) noexcept = default;
    Factory& operator =(Factory&&) noexcept = default;

    std::unique_ptr<Asset> newAsset(const StringView& mnem, const StringView& display,
                                    AssetType type) const
    {
        return doNewAsset(mnem, display, type);
    }
    std::unique_ptr<Contr> newContr(const StringView& mnem, const StringView& display,
                                    const StringView& asset, const StringView& ccy,
                                    int lotNumer, int lotDenom,
                                    int tickNumer, int tickDenom, int pipDp,
                                    Lots minLots, Lots maxLots) const
    {
        return doNewContr(mnem, display, asset, ccy, lotNumer, lotDenom, tickNumer, tickDenom,
                          pipDp, minLots, maxLots);
    }
    std::unique_ptr<Market> newMarket(const StringView& mnem, const StringView& display,
                                      const StringView& contr, Jd settlDay,
                                      Jd expiryDay, MarketState state, Lots lastLots,
                                      Ticks lastTicks, Millis lastTime, Iden maxOrderId,
                                      Iden maxExecId, Iden maxQuoteId) const
    {
        return doNewMarket(mnem, display, contr, settlDay, expiryDay, state, lastLots, lastTicks,
                           lastTime, maxOrderId, maxExecId, maxQuoteId);
    }
    std::unique_ptr<Market> newMarket(const StringView& mnem, const StringView& display,
                                      const StringView& contr, Jd settlDay,
                                      Jd expiryDay, MarketState state) const
    {
        return doNewMarket(mnem, display, contr, settlDay, expiryDay, state, 0, 0, 0, 0, 0, 0);
    }
    std::unique_ptr<Trader> newTrader(const StringView& mnem, const StringView& display,
                                      const StringView& email) const
    {
        return doNewTrader(mnem, display, email);
    }
    OrderPtr newOrder(const StringView& trader, const StringView& market,
                      const StringView& contr, Jd settlDay, Iden id,
                      const StringView& ref, Iden quoteId, State state, Side side,
                      Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                      Lots lastLots, Ticks lastTicks, Lots minLots, bool pecan,
                      Millis created, Millis modified) const
    {
        return doNewOrder(trader, market, contr, settlDay, id, ref, quoteId, state, side, lots,
                          ticks, resd, exec, cost, lastLots, lastTicks, minLots, pecan, created,
                          modified);
    }

    OrderPtr newOrder(const StringView& trader, const StringView& market,
                      const StringView& contr, Jd settlDay, Iden id,
                      const StringView& ref, Iden quoteId, Side side,
                      Lots lots, Ticks ticks, Lots minLots, Millis created) const
    {
        return doNewOrder(trader, market, contr, settlDay, id, ref, quoteId, State::NEW, side, lots,
                          ticks, lots, 0, 0, 0, 0, minLots, false, created, created);
    }

    ExecPtr newExec(const StringView& trader, const StringView& market,
                    const StringView& contr, Jd settlDay, Iden id,
                    const StringView& ref, Iden orderId, Iden quoteId, State state,
                    Side side, Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                    Lots lastLots, Ticks lastTicks, Lots minLots, Iden matchId,
                    Role role, const StringView& cpty, Millis created) const
    {
        return doNewExec(trader, market, contr, settlDay, id, ref, orderId, quoteId, state, side,
                         lots, ticks, resd, exec, cost, lastLots, lastTicks, minLots, matchId,
                         role, cpty, created);
    }

    ExecPtr newExec(const Order& order, Iden id, Millis created) const
    {
        return doNewExec(order.trader(), order.market(), order.contr(), order.settlDay(),
                         id, order.ref(), order.id(), order.quoteId(), order.state(),
                         order.side(), order.lots(), order.ticks(), order.resd(),
                         order.exec(), order.cost(), order.lastLots(), order.lastTicks(),
                         order.minLots(), 0, Role::NONE, StringView{}, created);
    }

    PosnPtr newPosn(const StringView& trader, const StringView& contr, Jd settlDay,
                    Lots buyLots, Cost buyCost, Lots sellLots, Cost sellCost) const
    {
        return doNewPosn(trader, contr, settlDay, buyLots, buyCost, sellLots, sellCost);
    }

    PosnPtr newPosn(const StringView& trader, const StringView& contr, Jd settlDay) const
    {
        return doNewPosn(trader, contr, settlDay, 0, 0, 0, 0);
    }
};

class SWIRLY_API BasicFactory : public Factory {
 protected:
    std::unique_ptr<Asset> doNewAsset(const StringView& mnem, const StringView& display,
                                      AssetType type) const override;

    std::unique_ptr<Contr> doNewContr(const StringView& mnem, const StringView& display,
                                      const StringView& asset, const StringView& ccy,
                                      int lotNumer, int lotDenom,
                                      int tickNumer, int tickDenom, int pipDp,
                                      Lots minLots, Lots maxLots) const override;

    std::unique_ptr<Market> doNewMarket(const StringView& mnem, const StringView& display,
                                        const StringView& contr, Jd settlDay,
                                        Jd expiryDay, MarketState state, Lots lastLots,
                                        Ticks lastTicks, Millis lastTime, Iden maxOrderId,
                                        Iden maxExecId, Iden maxQuoteId) const override;

    std::unique_ptr<Trader> doNewTrader(const StringView& mnem, const StringView& display,
                                        const StringView& email) const override;

    OrderPtr doNewOrder(const StringView& trader, const StringView& market,
                        const StringView& contr, Jd settlDay, Iden id,
                        const StringView& ref, Iden quoteId, State state, Side side,
                        Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                        Lots lastLots, Ticks lastTicks, Lots minLots, bool pecan,
                        Millis created, Millis modified) const override;

    ExecPtr doNewExec(const StringView& trader, const StringView& market,
                      const StringView& contr, Jd settlDay, Iden id,
                      const StringView& ref, Iden orderId, Iden quoteId, State state,
                      Side side, Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                      Lots lastLots, Ticks lastTicks, Lots minLots, Iden matchId,
                      Role role, const StringView& cpty, Millis created) const override;

    PosnPtr doNewPosn(const StringView& trader, const StringView& contr, Jd settlDay,
                      Lots buyLots, Cost buyCost, Lots sellLots, Cost sellCost) const override;

 public:
    BasicFactory() noexcept = default;
    ~BasicFactory() noexcept override;

    // Copy.
    BasicFactory(const BasicFactory&) = default;
    BasicFactory& operator =(const BasicFactory&) = default;

    // Move.
    BasicFactory(BasicFactory&&) = default;
    BasicFactory& operator =(BasicFactory&&) = default;
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_FACTORY_HPP
