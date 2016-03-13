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
#ifndef SWIRLY_ELM_MODEL_HPP
#define SWIRLY_ELM_MODEL_HPP

#include <swirly/elm/Asset.hpp>
#include <swirly/elm/Contr.hpp>
#include <swirly/elm/Market.hpp>
#include <swirly/elm/Trader.hpp>

/**
 * @addtogroup IO
 * @{
 */

namespace swirly {

class Factory;

class SWIRLY_API Model {
 public:
  Model() noexcept = default;
  virtual ~Model() noexcept;

  // Copy.
  constexpr Model(const Model&) noexcept = default;
  Model& operator=(const Model&) noexcept = default;

  // Move.
  constexpr Model(Model&&) noexcept = default;
  Model& operator=(Model&&) noexcept = default;

  AssetSet readAsset(const Factory& factory) const { return doReadAsset(factory); }
  ContrSet readContr(const Factory& factory) const { return doReadContr(factory); }
  MarketSet readMarket(const Factory& factory) const { return doReadMarket(factory); }
  TraderSet readTrader(const Factory& factory) const { return doReadTrader(factory); }

 protected:
  virtual AssetSet doReadAsset(const Factory& factory) const = 0;

  virtual ContrSet doReadContr(const Factory& factory) const = 0;

  virtual MarketSet doReadMarket(const Factory& factory) const = 0;

  virtual TraderSet doReadTrader(const Factory& factory) const = 0;
};

} // swirly

/** @} */

#endif // SWIRLY_ELM_MODEL_HPP
