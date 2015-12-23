/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

/**
 * An item of value.
 */
class SWIRLY_API Asset : public Rec {
protected:
    const AssetType type_;
public:
    Asset(const char* mnem, const char* display, AssetType type) noexcept
    :   Rec{RecType::ASSET, mnem, display},
        type_{type}
    {
    }

    ~Asset() noexcept override;

    // Copy.
    Asset(const Asset&) = default;
    Asset& operator =(const Asset&) = default;

    // Move.
    Asset(Asset&&) = default;
    Asset& operator =(Asset&&) = default;

    AssetType assetType() const noexcept
    {
        return type_;
    }
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_ASSET_HPP
