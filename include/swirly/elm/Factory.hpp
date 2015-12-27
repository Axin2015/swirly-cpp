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
#ifndef SWIRLY_ELM_FACTORY_HPP
#define SWIRLY_ELM_FACTORY_HPP

#include <swirly/elm/Asset.hpp>
#include <swirly/elm/Order.hpp>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

class SWIRLY_API Factory {
protected:
    virtual std::unique_ptr<Asset> doNewAsset(const StringView& mnem, const StringView& display,
                                              AssetType type) const = 0;

    virtual OrderPtr doNewOrder(const StringView& trader) const = 0;
public:
    Factory() noexcept = default;
    virtual ~Factory() noexcept;

    // Copy.
    constexpr Factory(const Factory&) noexcept = default;
    Factory& operator =(const Factory&) noexcept = default;

    // Move.
    constexpr Factory(Factory&&) noexcept = default;
    Factory& operator =(Factory&&) noexcept = default;

    std::unique_ptr<Asset> newAsset(const StringView& mnem, const StringView& display,
                                    AssetType type) const
    {
        return doNewAsset(mnem, display, type);
    }
    OrderPtr newOrder(const StringView& trader) const
    {
        return doNewOrder(trader);
    }
};

class SWIRLY_API BasicFactory : public Factory {
protected:
    std::unique_ptr<Asset> doNewAsset(const StringView& mnem, const StringView& display,
                                      AssetType type) const override;

    OrderPtr doNewOrder(const StringView& trader) const override;

public:
    BasicFactory() noexcept = default;
    ~BasicFactory() noexcept override;

    // Copy.
    BasicFactory(const BasicFactory&) = default;
    BasicFactory& operator =(const BasicFactory&) = default;

    // Move.
    BasicFactory(BasicFactory&&) = default;
    BasicFactory& operator =(BasicFactory&&) = default;
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_FACTORY_HPP
