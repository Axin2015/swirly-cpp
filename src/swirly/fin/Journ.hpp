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
#ifndef SWIRLY_FIN_JOURN_HPP
#define SWIRLY_FIN_JOURN_HPP

#include <swirly/Config.h>

namespace swirly {
inline namespace fin {
struct Msg;

class SWIRLY_API Journ {
  public:
    Journ() noexcept = default;
    virtual ~Journ();

    // Copy.
    constexpr Journ(const Journ&) noexcept = default;
    Journ& operator=(const Journ&) noexcept = default;

    // Move.
    constexpr Journ(Journ&&) noexcept = default;
    Journ& operator=(Journ&&) noexcept = default;

    void write(const Msg& msg) { do_write(msg); }

  protected:
    virtual void do_write(const Msg& msg) = 0;
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_JOURN_HPP
