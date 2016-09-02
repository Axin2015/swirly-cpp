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

#include "Utility.hpp"

#include <swirly/elm/Model.hpp>

namespace swirly {
namespace sqlite {

class Model : public swirly::Model {
 public:
  explicit Model(const Conf& conf);
  ~Model() noexcept;

  // Copy.
  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;

  // Move.
  Model(Model&&);
  Model& operator=(Model&&);

 protected:
  void doReadAsset(const ModelCallback<AssetPtr>& cb) const override;

  void doReadContr(const ModelCallback<ContrPtr>& cb) const override;

  void doReadMarket(const ModelCallback<MarketPtr>& cb) const override;

  void doReadAccnt(Millis now, const ModelCallback<std::string_view>& cb) const override;

  void doReadOrder(const ModelCallback<OrderPtr>& cb) const override;

  void doReadExec(std::string_view accnt, std::size_t limit,
                  const ModelCallback<ExecPtr>& cb) const override;

  void doReadTrade(const ModelCallback<ExecPtr>& cb) const override;

  void doReadPosn(JDay busDay, const ModelCallback<PosnPtr>& cb) const override;

 private:
  DbPtr db_;
};

} // sqlite
} // swirly

#endif // SWIRLY_SQLITE_MODEL_HPP
