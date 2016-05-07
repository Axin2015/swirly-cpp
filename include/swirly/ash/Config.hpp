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
#ifndef SWIRLY_ASH_CONFIG_HPP
#define SWIRLY_ASH_CONFIG_HPP

#include <swirly/ash/String.hpp>

#include <functional>
#include <set>
#include <string>
#include <tuple>

namespace swirly {

/**
 * @addtogroup Util
 * @{
 */

class SWIRLY_API VarInterp {
 public:
  explicit VarInterp(std::function<std::string(const std::string&)> fn) : fn_{std::move(fn)} {}
  ~VarInterp() noexcept = default;

  // Copy.
  VarInterp(const VarInterp&) = default;
  VarInterp& operator=(const VarInterp&) = default;

  // Move.
  VarInterp(VarInterp&&) = default;
  VarInterp& operator=(VarInterp&&) = default;

  void operator()(std::string& s) const { interp(s, std::string::npos, 0); }

 private:
  /**
   * Interpolate variables.
   *
   * @param s The string to interpolate.
   *
   * @param i Position of opening brace or std::string::npos if top-level.
   *
   * @param j Starting position of search. The search space is [j, size).
   *
   * @param outer Set of names seen at the same position in outer level.
   *
   * @return true if closing brace was found for brace at position i.
   */
  bool interp(std::string& s, const std::size_t i, std::size_t j,
              std::set<std::string>* outer = nullptr) const;

  std::function<std::string(const std::string&)> fn_;
};

template <typename FnT>
void parseConfig(std::istream& is, FnT fn)
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

/** @} */

} // swirly

#endif // SWIRLY_ASH_CONFIG_HPP
