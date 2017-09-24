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
#ifndef SWIRLY_WS_ENTITYSET_HPP
#define SWIRLY_WS_ENTITYSET_HPP

#include <swirly/Defs.hpp>

#include <experimental/string_view>

namespace std {
using experimental::string_view;
}

namespace swirly {

class SWIRLY_API EntitySet {
  public:
    enum : int {
        Asset = 1 << 0,
        Instr = 1 << 1,
        Market = 1 << 2,
        Order = 1 << 3,
        Exec = 1 << 4,
        Trade = 1 << 5,
        Posn = 1 << 6,
        RecMask = Asset | Instr,
        AccntMask = Market | Order | Exec | Trade | Posn
    };

    constexpr EntitySet(int bs = 0) noexcept : bs_{bs} {}
    ~EntitySet() noexcept;

    // Copy.
    EntitySet(const EntitySet& rhs) noexcept = default;
    EntitySet& operator=(const EntitySet& rhs) noexcept = default;

    // Move.
    EntitySet(EntitySet&&) noexcept = default;
    EntitySet& operator=(EntitySet&&) noexcept = default;

    static EntitySet parse(std::string_view sv);

    int top() const noexcept
    {
        // Find First Set.
        return bs_ & -bs_;
    }
    bool empty() const noexcept { return bs_ == 0; }
    int get() const noexcept { return bs_; }
    bool many() const noexcept { return (bs_ & (bs_ - 1)) != 0; }
    bool asset() const noexcept { return (bs_ & Asset) != 0; }
    bool instr() const noexcept { return (bs_ & Instr) != 0; }
    bool market() const noexcept { return (bs_ & Market) != 0; }
    bool order() const noexcept { return (bs_ & Order) != 0; }
    bool exec() const noexcept { return (bs_ & Exec) != 0; }
    bool trade() const noexcept { return (bs_ & Trade) != 0; }
    bool posn() const noexcept { return (bs_ & Posn) != 0; }
    bool onlyRec() const noexcept { return (bs_ & ~RecMask) == 0; }
    bool onlyAccnt() const noexcept { return (bs_ & ~AccntMask) == 0; }
    int pop() noexcept
    {
        const auto i = top();
        bs_ &= ~i;
        return i;
    }

  private:
    int bs_;
};

} // swirly

#endif // SWIRLY_WS_ENTITYSET_HPP
