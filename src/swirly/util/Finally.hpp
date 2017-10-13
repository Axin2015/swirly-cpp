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
#ifndef SWIRLY_UTIL_FINALLY_HPP
#define SWIRLY_UTIL_FINALLY_HPP

#include <utility>

namespace swirly {

template <typename FnT>
class Finally {
  public:
    explicit Finally(FnT fn) noexcept : fn_{std::move(fn)} {}
    ~Finally() noexcept { fn_(); }
    // Copy.
    Finally(const Finally&) = delete;
    Finally& operator=(const Finally&) = delete;

    // Move.
    Finally(Finally&&) = default;
    Finally& operator=(Finally&&) = delete;

  private:
    FnT fn_;
};

template <typename FnT>
auto makeFinally(FnT fn) noexcept
{
    return Finally<FnT>{std::move(fn)};
}

} // namespace swirly

#endif // SWIRLY_UTIL_FINALLY_HPP
