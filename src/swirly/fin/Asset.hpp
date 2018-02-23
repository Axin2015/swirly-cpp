/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#ifndef SWIRLY_FIN_ASSET_HPP
#define SWIRLY_FIN_ASSET_HPP

#include <swirly/fin/Types.hpp>

#include <swirly/util/Set.hpp>

namespace swirly {

/**
 * An item of value.
 */
class SWIRLY_API Asset : public Comparable<Asset> {
  public:
    Asset(Id32 id, Symbol symbol, std::string_view display, AssetType type) noexcept
      : id_{id}
      , symbol_{symbol}
      , display_{display}
      , type_{type}
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

    void toDsv(std::ostream& os, char sep = ',') const;
    void toJson(std::ostream& os) const;

    int compare(const Asset& rhs) const noexcept { return symbol_.compare(rhs.symbol_); }
    auto id() const noexcept { return id_; }
    auto symbol() const noexcept { return symbol_; }
    auto display() const noexcept { return +display_; }
    auto type() const noexcept { return type_; }
    boost::intrusive::set_member_hook<> symbolHook_;

  private:
    const Id32 id_;
    const Symbol symbol_;
    const Display display_;
    const AssetType type_;
};

inline std::ostream& operator<<(std::ostream& os, const Asset& asset)
{
    asset.toJson(os);
    return os;
}

using AssetSet = SymbolSet<Asset>;

} // namespace swirly

#endif // SWIRLY_FIN_ASSET_HPP
