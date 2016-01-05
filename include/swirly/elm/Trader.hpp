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
#ifndef SWIRLY_ELM_TRADER_HPP
#define SWIRLY_ELM_TRADER_HPP

#include <swirly/elm/Rec.hpp>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

class SWIRLY_API Trader : public Rec {
    const Email email_;

 public:
    boost::intrusive::set_member_hook<> mnemHook_;

    Trader(const StringView& mnem, const StringView& display, const StringView& email) noexcept
    :   Rec{RecType::MARKET, mnem, display},
        email_{email}
    {
    }

    ~Trader() noexcept override;

    // Copy.
    Trader(const Trader&) = default;
    Trader& operator =(const Trader&) = default;

    // Move.
    Trader(Trader&&) = default;
    Trader& operator =(Trader&&) = default;

    StringView email() const noexcept
    {
        return email_.view();
    }
};

using TraderSet = RecSet<Trader>;

/** @} */

} // swirly

#endif // SWIRLY_ELM_TRADER_HPP
