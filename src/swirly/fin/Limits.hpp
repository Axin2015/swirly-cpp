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
#ifndef SWIRLY_FIN_LIMITS_HPP
#define SWIRLY_FIN_LIMITS_HPP

#include <swirly/Config.h>

#include <cstddef> // size_t

namespace swirly {

/**
 * Maximum display characters.
 */
constexpr std::size_t MaxDisplay{64};

/**
 * Maximum number of price levels.
 */
constexpr std::size_t MaxLevels{SWIRLY_MAX_LEVELS};

/**
 * Maximum reference characters.
 */
constexpr std::size_t MaxRef{64};

} // swirly

#endif // SWIRLY_FIN_LIMITS_HPP
