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
#ifndef SWIRLY_ASH_CONF_HPP
#define SWIRLY_ASH_CONF_HPP

#include <swirly/ash/String.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/container/flat_map.hpp>
#pragma GCC diagnostic pop

#include <string>
#include <tuple>

namespace swirly {

/**
 * @addtogroup Util
 * @{
 */

template <typename FnT>
void parsePairs(std::istream& is, FnT fn)
{
  std::string line;
  while (std::getline(is, line)) {

    trim(line);

    // Ignore empty lines and comments.

    if (line.empty() || line[0] == '#') {
      continue;
    }

    std::string key, val;
    std::tie(key, val) = splitPair(line, '=');
    rtrim(key);
    ltrim(val);

    fn(key, val);
  }
}

/**
 * Simple conf reader with environment variable substitution.
 */
SWIRLY_API void readConf(std::istream& is,
                         boost::container::flat_map<std::string, std::string>& conf);

/** @} */

} // swirly

#endif // SWIRLY_ASH_CONF_HPP
