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

  MarketPtr newMarket(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay,
                      Jday expiryDay, MarketState state, Lots lastLots = 0_lts,
                      Ticks lastTicks = 0_tks, Millis lastTime = 0_ms, Iden maxOrderId = 0_id,
                      Iden maxExecId = 0_id) const;

 protected:
  virtual MarketPtr doNewMarket(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay,
                                Jday expiryDay, MarketState state, Lots lastLots, Ticks lastTicks,
                                Millis lastTime, Iden maxOrderId, Iden maxExecId) const = 0;
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
  MarketPtr doNewMarket(Mnem mnem, std::string_view display, Mnem contr, Jday settlDay,
                        Jday expiryDay, MarketState state, Lots lastLots, Ticks lastTicks,
                        Millis lastTime, Iden maxOrderId, Iden maxExecId) const override;
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_FACTORY_HPP
