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

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Types.hpp>

namespace swirly {

class Conf;
class Factory;

/**
 * @addtogroup IO
 * @{
 */

template <typename ValuePtrT>
using ModelCallback = std::function<void(ValuePtrT&&)>;

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

  void readAsset(const Factory& factory, const ModelCallback<AssetPtr>& cb) const
  {
    doReadAsset(factory, cb);
  }
  void readContr(const Factory& factory, const ModelCallback<ContrPtr>& cb) const
  {
    doReadContr(factory, cb);
  }
  void readMarket(const Factory& factory, const ModelCallback<MarketPtr>& cb) const
  {
    doReadMarket(factory, cb);
  }
  void readTrader(const Factory& factory, const ModelCallback<TraderPtr>& cb) const
  {
    doReadTrader(factory, cb);
  }
  void readOrder(const Factory& factory, const ModelCallback<OrderPtr>& cb) const
  {
    doReadOrder(factory, cb);
  }
  void readTrade(const Factory& factory, const ModelCallback<ExecPtr>& cb) const
  {
    doReadTrade(factory, cb);
  }
  void readPosn(Jday busDay, const Factory& factory, const ModelCallback<PosnPtr>& cb) const
  {
    doReadPosn(busDay, factory, cb);
  }

 protected:
  virtual void doReadAsset(const Factory& factory, const ModelCallback<AssetPtr>& cb) const = 0;

  virtual void doReadContr(const Factory& factory, const ModelCallback<ContrPtr>& cb) const = 0;

  virtual void doReadMarket(const Factory& factory, const ModelCallback<MarketPtr>& cb) const = 0;

  virtual void doReadTrader(const Factory& factory, const ModelCallback<TraderPtr>& cb) const = 0;

  virtual void doReadOrder(const Factory& factory, const ModelCallback<OrderPtr>& cb) const = 0;

  virtual void doReadTrade(const Factory& factory, const ModelCallback<ExecPtr>& cb) const = 0;

  virtual void doReadPosn(Jday busDay, const Factory& factory,
                          const ModelCallback<PosnPtr>& cb) const = 0;
};

/**
 * Make Model. Forward declaration for Model backend.
 *
 * @param conf Configuration.
 */
SWIRLY_API std::unique_ptr<Model> makeModel(const Conf& conf);

/** @} */

} // swirly

#endif // SWIRLY_ELM_MODEL_HPP
