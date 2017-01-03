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
#ifndef SWIRLY_FIN_CONTR_HPP
#define SWIRLY_FIN_CONTR_HPP

#include <swirly/fin/Types.hpp>

#include <swirly/util/Set.hpp>

namespace swirly {

/**
 * A specification that stipulates the terms and conditions of sale.
 */
class SWIRLY_API Contr : public Comparable<Contr> {
 public:
  Contr(Id32 id, Mnem mnem, std::string_view display, Mnem asset, Mnem ccy, int lotNumer,
        int lotDenom, int tickNumer, int tickDenom, int pipDp, Lots minLots, Lots maxLots) noexcept;

  ~Contr() noexcept;

  // Copy.
  Contr(const Contr&);
  Contr& operator=(const Contr&) = delete;

  // Move.
  Contr(Contr&&);
  Contr& operator=(Contr&&) = delete;

  template <typename... ArgsT>
  static ContrPtr make(ArgsT&&... args)
  {
    return std::make_unique<Contr>(std::forward<ArgsT>(args)...);
  }

  void toJson(std::ostream& os) const;

  int compare(const Contr& rhs) const noexcept { return mnem_.compare(rhs.mnem_); }
  auto id() const noexcept { return id_; }
  auto mnem() const noexcept { return mnem_; }
  auto display() const noexcept { return +display_; }
  auto asset() const noexcept { return asset_; }
  auto ccy() const noexcept { return ccy_; }
  auto lotNumer() const noexcept { return lotNumer_; }
  auto lotDenom() const noexcept { return lotDenom_; }
  auto qtyInc() const noexcept { return qtyInc_; }
  auto tickNumer() const noexcept { return tickNumer_; }
  auto tickDenom() const noexcept { return tickDenom_; }
  auto priceInc() const noexcept { return priceInc_; }
  auto pipDp() const noexcept { return pipDp_; }
  auto qtyDp() const noexcept { return qtyDp_; }
  auto priceDp() const noexcept { return priceDp_; }
  auto minLots() const noexcept { return minLots_; }
  auto maxLots() const noexcept { return maxLots_; }
  boost::intrusive::set_member_hook<> mnemHook_;

 private:
  const Id32 id_;
  const Mnem mnem_;
  const Display display_;
  const Mnem asset_;
  const Mnem ccy_;
  const int lotNumer_;
  const int lotDenom_;
  // Transient.
  const double qtyInc_;
  const int tickNumer_;
  const int tickDenom_;
  // Transient.
  const double priceInc_;
  const int pipDp_;
  // Transient.
  const int qtyDp_;
  // Transient.
  const int priceDp_;
  const Lots minLots_;
  const Lots maxLots_;
};

inline std::ostream& operator<<(std::ostream& os, const Contr& contr)
{
  contr.toJson(os);
  return os;
}

using ContrSet = MnemSet<Contr>;

} // swirly

#endif // SWIRLY_FIN_CONTR_HPP
