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

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Set.hpp>

namespace swirly {

/**
 * An item of value.
 */
class SWIRLY_API Asset : public Comparable<Asset> {
 public:
  Asset(Mnem mnem, std::string_view display, AssetType type) noexcept
    : mnem_{mnem}, display_{display}, type_{type}
  {
  }

  ~Asset() noexcept;

  // Copy.
  Asset(const Asset&);
  Asset& operator=(const Asset&) = delete;

  // Move.
  Asset(Asset&&);
  Asset& operator=(Asset&&) = delete;

  template <typename... ArgsT>
  static AssetPtr make(ArgsT&&... args)
  {
    return std::make_unique<Asset>(std::forward<ArgsT>(args)...);
  }

  void toJson(std::ostream& os) const;

  int compare(const Asset& rhs) const noexcept { return mnem_.compare(rhs.mnem_); }
  auto mnem() const noexcept { return mnem_; }
  auto display() const noexcept { return +display_; }
  auto type() const noexcept { return type_; }
  boost::intrusive::set_member_hook<> mnemHook_;

 private:
  const Mnem mnem_;
  const Display display_;
  const AssetType type_;
};

inline std::ostream& operator<<(std::ostream& os, const Asset& asset)
{
  asset.toJson(os);
  return os;
}

using AssetSet = MnemSet<Asset>;

} // swirly

#endif // SWIRLY_ELM_ASSET_HPP
