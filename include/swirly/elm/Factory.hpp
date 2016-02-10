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

  std::unique_ptr<Asset> newAsset(const std::string_view& mnem, const std::string_view& display,
                                  AssetType type) const;

  std::unique_ptr<Contr> newContr(const std::string_view& mnem, const std::string_view& display,
                                  const std::string_view& asset, const std::string_view& ccy,
                                  int lotNumer, int lotDenom, int tickNumer, int tickDenom,
                                  int pipDp, Lots minLots, Lots maxLots) const;

  std::unique_ptr<Market> newMarket(const std::string_view& mnem, const std::string_view& display,
                                    const std::string_view& contr, Jday settlDay, Jday expiryDay,
                                    MarketState state, Lots lastLots, Ticks lastTicks,
                                    Millis lastTime, Iden maxOrderId, Iden maxExecId) const;

  std::unique_ptr<Market> newMarket(const std::string_view& mnem, const std::string_view& display,
                                    const std::string_view& contr, Jday settlDay, Jday expiryDay,
                                    MarketState state) const;

  std::unique_ptr<Trader> newTrader(const std::string_view& mnem, const std::string_view& display,
                                    const std::string_view& email) const;

  OrderPtr newOrder(const std::string_view& trader, const std::string_view& market,
                    const std::string_view& contr, Jday settlDay, Iden id,
                    const std::string_view& ref, State state, Side side, Lots lots, Ticks ticks,
                    Lots resd, Lots exec, Cost cost, Lots lastLots, Ticks lastTicks, Lots minLots,
                    Millis created, Millis modified) const;

  OrderPtr newOrder(const std::string_view& trader, const std::string_view& market,
                    const std::string_view& contr, Jday settlDay, Iden id,
                    const std::string_view& ref, Side side, Lots lots, Ticks ticks, Lots minLots,
                    Millis created) const;

  ExecPtr newExec(const std::string_view& trader, const std::string_view& market,
                  const std::string_view& contr, Jday settlDay, Iden id,
                  const std::string_view& ref, Iden orderId, State state, Side side, Lots lots,
                  Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots, Ticks lastTicks,
                  Lots minLots, Iden matchId, Role role, const std::string_view& cpty,
                  Millis created) const;

  ExecPtr newExec(const Order& order, Iden id, Millis created) const;

  PosnPtr newPosn(const std::string_view& trader, const std::string_view& contr, Jday settlDay,
                  Lots buyLots, Cost buyCost, Lots sellLots, Cost sellCost) const;

  PosnPtr newPosn(const std::string_view& trader, const std::string_view& contr,
                  Jday settlDay) const;

 protected:
  virtual std::unique_ptr<Asset> doNewAsset(const std::string_view& mnem,
                                            const std::string_view& display,
                                            AssetType type) const = 0;

  virtual std::unique_ptr<Contr>
  doNewContr(const std::string_view& mnem, const std::string_view& display,
             const std::string_view& asset, const std::string_view& ccy, int lotNumer, int lotDenom,
             int tickNumer, int tickDenom, int pipDp, Lots minLots, Lots maxLots) const = 0;

  virtual std::unique_ptr<Market> doNewMarket(const std::string_view& mnem,
                                              const std::string_view& display,
                                              const std::string_view& contr, Jday settlDay,
                                              Jday expiryDay, MarketState state, Lots lastLots,
                                              Ticks lastTicks, Millis lastTime, Iden maxOrderId,
                                              Iden maxExecId) const = 0;

  virtual std::unique_ptr<Trader> doNewTrader(const std::string_view& mnem,
                                              const std::string_view& display,
                                              const std::string_view& email) const = 0;

  virtual OrderPtr doNewOrder(const std::string_view& trader, const std::string_view& market,
                              const std::string_view& contr, Jday settlDay, Iden id,
                              const std::string_view& ref, State state, Side side, Lots lots,
                              Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots,
                              Ticks lastTicks, Lots minLots, Millis created,
                              Millis modified) const = 0;

  virtual ExecPtr doNewExec(const std::string_view& trader, const std::string_view& market,
                            const std::string_view& contr, Jday settlDay, Iden id,
                            const std::string_view& ref, Iden orderId, State state, Side side,
                            Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots,
                            Ticks lastTicks, Lots minLots, Iden matchId, Role role,
                            const std::string_view& cpty, Millis created) const = 0;

  virtual PosnPtr doNewPosn(const std::string_view& trader, const std::string_view& contr,
                            Jday settlDay, Lots buyLots, Cost buyCost, Lots sellLots,
                            Cost sellCost) const = 0;
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
  std::unique_ptr<Asset> doNewAsset(const std::string_view& mnem, const std::string_view& display,
                                    AssetType type) const override;

  std::unique_ptr<Contr> doNewContr(const std::string_view& mnem, const std::string_view& display,
                                    const std::string_view& asset, const std::string_view& ccy,
                                    int lotNumer, int lotDenom, int tickNumer, int tickDenom,
                                    int pipDp, Lots minLots, Lots maxLots) const override;

  std::unique_ptr<Market> doNewMarket(const std::string_view& mnem, const std::string_view& display,
                                      const std::string_view& contr, Jday settlDay, Jday expiryDay,
                                      MarketState state, Lots lastLots, Ticks lastTicks,
                                      Millis lastTime, Iden maxOrderId,
                                      Iden maxExecId) const override;

  std::unique_ptr<Trader> doNewTrader(const std::string_view& mnem, const std::string_view& display,
                                      const std::string_view& email) const override;

  OrderPtr doNewOrder(const std::string_view& trader, const std::string_view& market,
                      const std::string_view& contr, Jday settlDay, Iden id,
                      const std::string_view& ref, State state, Side side, Lots lots, Ticks ticks,
                      Lots resd, Lots exec, Cost cost, Lots lastLots, Ticks lastTicks, Lots minLots,
                      Millis created, Millis modified) const override;

  ExecPtr doNewExec(const std::string_view& trader, const std::string_view& market,
                    const std::string_view& contr, Jday settlDay, Iden id,
                    const std::string_view& ref, Iden orderId, State state, Side side, Lots lots,
                    Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots, Ticks lastTicks,
                    Lots minLots, Iden matchId, Role role, const std::string_view& cpty,
                    Millis created) const override;

  PosnPtr doNewPosn(const std::string_view& trader, const std::string_view& contr, Jday settlDay,
                    Lots buyLots, Cost buyCost, Lots sellLots, Cost sellCost) const override;
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_FACTORY_HPP
