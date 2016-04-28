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
#ifndef SWIRLY_SQLITE_MODEL_HPP
#define SWIRLY_SQLITE_MODEL_HPP

#include "Sqlite.hpp"

#include <swirly/elm/Model.hpp>

namespace swirly {
namespace sqlite {

/**
 * @addtogroup IO
 * @{
 */

class Model : public swirly::Model {
 public:
  explicit Model(const char* connString);
  ~Model() noexcept;

  // Copy.
  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;

  // Move.
  Model(Model&&);
  Model& operator=(Model&&);

 protected:
  AssetSet doReadAsset(const Factory& factory) const override;

  ContrSet doReadContr(const Factory& factory) const override;

  MarketSet doReadMarket(const Factory& factory) const override;

  TraderSet doReadTrader(const Factory& factory) const override;

 private:
  DbPtr db_;
};

/** @} */

} // sqlite
} // swirly

#endif // SWIRLY_SQLITE_MODEL_HPP
