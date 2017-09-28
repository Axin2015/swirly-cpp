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
#ifndef SWIRLY_WS_PAGE_HPP
#define SWIRLY_WS_PAGE_HPP

#include <swirly/Config.h>

#include <experimental/optional>
#include <experimental/string_view>

namespace std {
template <typename T>
using optional = experimental::optional<T>;
using experimental::nullopt;
using experimental::string_view;
}

namespace swirly {

struct Page {
    std::size_t offset{0};
    std::optional<std::size_t> limit;
};

// Parse Page arguments from URL Query String. Note that special characters and percent encodings
// are not supported for simplicity.
SWIRLY_API Page parseQuery(std::string_view query) noexcept;

} // swirly

#endif // SWIRLY_WS_PAGE_HPP
