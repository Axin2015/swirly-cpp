/*
 * Swirly Order-Book and Matching-Engine.
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
#ifndef SWIRLY_ELM_FACTORY_HPP
#define SWIRLY_ELM_FACTORY_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Types.hpp>

#include <memory>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

class SWIRLY_API Factory {
 public:
    Factory() noexcept = default;
    virtual ~Factory() noexcept;

    // Copy.
    Factory(const Factory&) noexcept = default;
    Factory& operator=(const Factory&) noexcept = default;

    // Move.
    Factory(Factory&&) noexcept = default;
    Factory& operator=(Factory&&) noexcept = default;

    std::unique_ptr<Asset> newAsset(const StringView& mnem, const StringView& display,
                                    AssetType type) const;

    std::unique_ptr<Contr> newContr(const StringView& mnem, const StringView& display,
                                    const StringView& asset, const StringView& ccy, int lotNumer,
                                    int lotDenom, int tickNumer, int tickDenom, int pipDp,
                                    Lots minLots, Lots maxLots) const;

    std::unique_ptr<Market> newMarket(const StringView& mnem, const StringView& display,
                                      const StringView& contr, Jday settlDay, Jday expiryDay,
                                      MarketState state, Lots lastLots, Ticks lastTicks,
                                      Millis lastTime, Iden maxOrderId, Iden maxExecId) const;

    std::unique_ptr<Market> newMarket(const StringView& mnem, const StringView& display,
                                      const StringView& contr, Jday settlDay, Jday expiryDay,
                                      MarketState state) const;

    std::unique_ptr<Trader> newTrader(const StringView& mnem, const StringView& display,
                                      const StringView& email) const;

    OrderPtr newOrder(const StringView& trader, const StringView& market, const StringView& contr,
                      Jday settlDay, Iden id, const StringView& ref, State state, Side side,
                      Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots,
                      Ticks lastTicks, Lots minLots, Millis created, Millis modified) const;

    OrderPtr newOrder(const StringView& trader, const StringView& market, const StringView& contr,
                      Jday settlDay, Iden id, const StringView& ref, Side side, Lots lots,
                      Ticks ticks, Lots minLots, Millis created) const;

    ExecPtr newExec(const StringView& trader, const StringView& market, const StringView& contr,
                    Jday settlDay, Iden id, const StringView& ref, Iden orderId, State state,
                    Side side, Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                    Lots lastLots, Ticks lastTicks, Lots minLots, Iden matchId, Role role,
                    const StringView& cpty, Millis created) const;

    ExecPtr newExec(const Order& order, Iden id, Millis created) const;

    PosnPtr newPosn(const StringView& trader, const StringView& contr, Jday settlDay, Lots buyLots,
                    Cost buyCost, Lots sellLots, Cost sellCost) const;

    PosnPtr newPosn(const StringView& trader, const StringView& contr, Jday settlDay) const;

 protected:
    virtual std::unique_ptr<Asset> doNewAsset(const StringView& mnem, const StringView& display,
                                              AssetType type) const = 0;

    virtual std::unique_ptr<Contr> doNewContr(const StringView& mnem, const StringView& display,
                                              const StringView& asset, const StringView& ccy,
                                              int lotNumer, int lotDenom, int tickNumer,
                                              int tickDenom, int pipDp, Lots minLots,
                                              Lots maxLots) const = 0;

    virtual std::unique_ptr<Market> doNewMarket(const StringView& mnem, const StringView& display,
                                                const StringView& contr, Jday settlDay,
                                                Jday expiryDay, MarketState state, Lots lastLots,
                                                Ticks lastTicks, Millis lastTime, Iden maxOrderId,
                                                Iden maxExecId) const = 0;

    virtual std::unique_ptr<Trader> doNewTrader(const StringView& mnem, const StringView& display,
                                                const StringView& email) const = 0;

    virtual OrderPtr doNewOrder(const StringView& trader, const StringView& market,
                                const StringView& contr, Jday settlDay, Iden id,
                                const StringView& ref, State state, Side side, Lots lots,
                                Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots,
                                Ticks lastTicks, Lots minLots, Millis created,
                                Millis modified) const = 0;

    virtual ExecPtr doNewExec(const StringView& trader, const StringView& market,
                              const StringView& contr, Jday settlDay, Iden id,
                              const StringView& ref, Iden orderId, State state, Side side,
                              Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                              Lots lastLots, Ticks lastTicks, Lots minLots, Iden matchId, Role role,
                              const StringView& cpty, Millis created) const = 0;

    virtual PosnPtr doNewPosn(const StringView& trader, const StringView& contr, Jday settlDay,
                              Lots buyLots, Cost buyCost, Lots sellLots, Cost sellCost) const = 0;
};

class SWIRLY_API BasicFactory : public Factory {
 public:
    BasicFactory() noexcept = default;
    ~BasicFactory() noexcept override;

    // Copy.
    BasicFactory(const BasicFactory&) noexcept = default;
    BasicFactory& operator=(const BasicFactory&) noexcept = default;

    // Move.
    BasicFactory(BasicFactory&&) noexcept = default;
    BasicFactory& operator=(BasicFactory&&) noexcept = default;

 protected:
    std::unique_ptr<Asset> doNewAsset(const StringView& mnem, const StringView& display,
                                      AssetType type) const override;

    std::unique_ptr<Contr> doNewContr(const StringView& mnem, const StringView& display,
                                      const StringView& asset, const StringView& ccy, int lotNumer,
                                      int lotDenom, int tickNumer, int tickDenom, int pipDp,
                                      Lots minLots, Lots maxLots) const override;

    std::unique_ptr<Market> doNewMarket(const StringView& mnem, const StringView& display,
                                        const StringView& contr, Jday settlDay, Jday expiryDay,
                                        MarketState state, Lots lastLots, Ticks lastTicks,
                                        Millis lastTime, Iden maxOrderId,
                                        Iden maxExecId) const override;

    std::unique_ptr<Trader> doNewTrader(const StringView& mnem, const StringView& display,
                                        const StringView& email) const override;

    OrderPtr doNewOrder(const StringView& trader, const StringView& market, const StringView& contr,
                        Jday settlDay, Iden id, const StringView& ref, State state, Side side,
                        Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots,
                        Ticks lastTicks, Lots minLots, Millis created,
                        Millis modified) const override;

    ExecPtr doNewExec(const StringView& trader, const StringView& market, const StringView& contr,
                      Jday settlDay, Iden id, const StringView& ref, Iden orderId, State state,
                      Side side, Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                      Lots lastLots, Ticks lastTicks, Lots minLots, Iden matchId, Role role,
                      const StringView& cpty, Millis created) const override;

    PosnPtr doNewPosn(const StringView& trader, const StringView& contr, Jday settlDay,
                      Lots buyLots, Cost buyCost, Lots sellLots, Cost sellCost) const override;
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_FACTORY_HPP
