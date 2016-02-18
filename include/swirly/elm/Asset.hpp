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
#ifndef SWIRLY_ELM_ASSET_HPP
#define SWIRLY_ELM_ASSET_HPP

#include <swirly/elm/Rec.hpp>

#include <swirly/ash/Types.hpp>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

/**
 * An item of value.
 */
class SWIRLY_API Asset : public Rec {
 public:
  Asset(const std::string_view& mnem, const std::string_view& display, AssetType type) noexcept
    : Rec{RecType::Asset, mnem, display},
      type_{type}
  {
  }

  ~Asset() noexcept override;

  // Copy.
  Asset(const Asset&);
  Asset& operator=(const Asset&) = delete;

  // Move.
  Asset(Asset&&);
  Asset& operator=(Asset&&) = delete;

  void toJson(std::ostream& os) const override;

  AssetType assetType() const noexcept { return type_; }
  boost::intrusive::set_member_hook<> mnemHook_;

 private:
  const AssetType type_;
};

using AssetSet = RecSet<Asset>;

/** @} */

} // swirly

#endif // SWIRLY_ELM_ASSET_HPP
