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
#ifndef SWIRLY_FIG_SERV_HPP
#define SWIRLY_FIG_SERV_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/String.hpp>
#include <swirly/ash/Types.hpp>

#include <memory> // unique_ptr<>

namespace swirly {

class Journ;
class MarketBook;
class Model;
class RecSet;
class TraderSess;

/**
 * @addtogroup App
 * @{
 */

class SWIRLY_API Serv {
    struct Impl;
    std::unique_ptr<Impl> impl_;
public:

    Serv(const Model& model, Journ& journ, Millis now);

    ~Serv() noexcept;

    // Copy.
    Serv(const Serv&) = delete;
    Serv& operator =(const Serv&) = delete;

    // Move.
    Serv(Serv&&);
    Serv& operator =(Serv&&);

    const TraderSess& createTrader(const StringView& mnem, const StringView& display,
                                   const StringView& email);

    const TraderSess& updateTrader(const StringView& mnem, const StringView& display);

    const RecSet& assets() const noexcept;

    const RecSet& contrs() const noexcept;

    const RecSet& markets() const noexcept;

    const RecSet& traders() const noexcept;

    const MarketBook& market(const StringView& mnem) const;

    const TraderSess& trader(const StringView& mnem) const;

    const TraderSess* findTraderByEmail(const StringView& email) const;

    const MarketBook& createMarket(const StringView& mnem, const StringView& display,
                                   const StringView& contr, Jd settlDay, Jd expiryDay,
                                   MarketState state, Millis now);
};

/** @} */

} // swirly

#endif // SWIRLY_FIG_SERV_HPP
