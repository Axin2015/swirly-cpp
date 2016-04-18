/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#ifndef SWIRLY_ELM_DATE_HPP
#define SWIRLY_ELM_DATE_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Types.hpp>

namespace swirly {

/**
 * @addtogroup Date
 * @{
 */

/**
 * Get the business day from a transaction time.
 *
 * Business day rolls at 5pm New York.
 *
 * @param ms
 *            The milliseconds since epoch.
 * @return the business day.
 */
SWIRLY_API Jday getBusDay(Millis ms);

/** @} */

} // swirly

#endif // SWIRLY_ELM_DATE_HPP
