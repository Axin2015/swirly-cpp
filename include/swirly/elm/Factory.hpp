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
#ifndef SWIRLY_ELM_FACTORY_HPP
#define SWIRLY_ELM_FACTORY_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Mnem.hpp>
#include <swirly/ash/Types.hpp>

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

  AssetPtr newAsset(Mnem mnem, std::string_view display, AssetType type) const;

  ContrPtr newContr(Mnem mnem, std::string_view display, Mnem asset, Mnem ccy, int lotNumer,
                    int lotDenom, int tickNumer, int tickDenom, int pipDp, Lots minLots,
                    Lots maxLots) const;

  MarketPtr newMarket(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay,
                      Jday expiryDay, MarketState state, Lots lastLots, Ticks lastTicks,
                      Millis lastTime, Iden maxOrderId, Iden maxExecId) const;

  MarketPtr newMarket(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay,
                      Jday expiryDay, MarketState state) const;

  TraderPtr newTrader(Mnem mnem, std::string_view display, std::string_view email) const;

  OrderPtr newOrder(Mnem trader, Mnem market, Mnem contr, Jday settlDay, Iden id,
                    std::string_view ref, State state, Side side, Lots lots, Ticks ticks, Lots resd,
                    Lots exec, Cost cost, Lots lastLots, Ticks lastTicks, Lots minLots,
                    Millis created, Millis modified) const;

  OrderPtr newOrder(Mnem trader, Mnem market, Mnem contr, Jday settlDay, Iden id,
                    std::string_view ref, Side side, Lots lots, Ticks ticks, Lots minLots,
                    Millis created) const;

  ExecPtr newExec(Mnem trader, Mnem market, Mnem contr, Jday settlDay, Iden id,
                  std::string_view ref, Iden orderId, State state, Side side, Lots lots,
                  Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots, Ticks lastTicks,
                  Lots minLots, Iden matchId, Role role, Mnem cpty, Millis created) const;

  ExecPtr newExec(const Order& order, Iden id, Millis created) const;

  PosnPtr newPosn(Mnem trader, Mnem contr, Jday settlDay, Lots buyLots, Cost buyCost, Lots sellLots,
                  Cost sellCost) const;

  PosnPtr newPosn(Mnem trader, Mnem contr, Jday settlDay) const;

 protected:
  virtual AssetPtr doNewAsset(Mnem mnem, std::string_view display, AssetType type) const = 0;

  virtual ContrPtr doNewContr(Mnem mnem, std::string_view display, Mnem asset, Mnem ccy,
                              int lotNumer, int lotDenom, int tickNumer, int tickDenom, int pipDp,
                              Lots minLots, Lots maxLots) const = 0;

  virtual MarketPtr doNewMarket(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay,
                                Jday expiryDay, MarketState state, Lots lastLots, Ticks lastTicks,
                                Millis lastTime, Iden maxOrderId, Iden maxExecId) const = 0;

  virtual TraderPtr doNewTrader(Mnem mnem, std::string_view display,
                                std::string_view email) const = 0;

  virtual OrderPtr doNewOrder(Mnem trader, Mnem market, Mnem contr, Jday settlDay, Iden id,
                              std::string_view ref, State state, Side side, Lots lots, Ticks ticks,
                              Lots resd, Lots exec, Cost cost, Lots lastLots, Ticks lastTicks,
                              Lots minLots, Millis created, Millis modified) const = 0;

  virtual ExecPtr doNewExec(Mnem trader, Mnem market, Mnem contr, Jday settlDay, Iden id,
                            std::string_view ref, Iden orderId, State state, Side side, Lots lots,
                            Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots,
                            Ticks lastTicks, Lots minLots, Iden matchId, Role role, Mnem cpty,
                            Millis created) const = 0;

  virtual PosnPtr doNewPosn(Mnem trader, Mnem contr, Jday settlDay, Lots buyLots, Cost buyCost,
                            Lots sellLots, Cost sellCost) const = 0;
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
  AssetPtr doNewAsset(Mnem mnem, std::string_view display, AssetType type) const override;

  ContrPtr doNewContr(Mnem mnem, std::string_view display, Mnem asset, Mnem ccy, int lotNumer,
                      int lotDenom, int tickNumer, int tickDenom, int pipDp, Lots minLots,
                      Lots maxLots) const override;

  MarketPtr doNewMarket(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay,
                        Jday expiryDay, MarketState state, Lots lastLots, Ticks lastTicks,
                        Millis lastTime, Iden maxOrderId, Iden maxExecId) const override;

  TraderPtr doNewTrader(Mnem mnem, std::string_view display, std::string_view email) const override;

  OrderPtr doNewOrder(Mnem trader, Mnem market, Mnem contr, Jday settlDay, Iden id,
                      std::string_view ref, State state, Side side, Lots lots, Ticks ticks,
                      Lots resd, Lots exec, Cost cost, Lots lastLots, Ticks lastTicks, Lots minLots,
                      Millis created, Millis modified) const override;

  ExecPtr doNewExec(Mnem trader, Mnem market, Mnem contr, Jday settlDay, Iden id,
                    std::string_view ref, Iden orderId, State state, Side side, Lots lots,
                    Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots, Ticks lastTicks,
                    Lots minLots, Iden matchId, Role role, Mnem cpty,
                    Millis created) const override;

  PosnPtr doNewPosn(Mnem trader, Mnem contr, Jday settlDay, Lots buyLots, Cost buyCost,
                    Lots sellLots, Cost sellCost) const override;
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_FACTORY_HPP
