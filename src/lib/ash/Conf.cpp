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
#include <swirly/ash/Conf.hpp>

using namespace std;

namespace swirly {

bool VarInterp::interp(string& s, const size_t i, size_t j, set<string>* outer) const
{
  // Position of last interpolation.
  size_t last{0};
  // Names interpolated at 'last' position.
  set<string> inner;

  int state{0};
  while (j < s.size()) {
    if (state == '\\') {
      state = 0;
      // Remove backslash.
      s.erase(j - 1, 1);
    } else {
      const auto ch = s[j];
      if (state == '$') {
        state = 0;
        if (ch == '{') {
          if (j > last) {
            // Position has advanced.
            last = j;
            inner.clear();
          }
          // Reverse to '$'.
          --j;
          // Descend: search for closing brace and interpolate.
          if (!interp(s, j, j + 2, &inner)) {
            return false;
          }
          continue;
        }
      }
      switch (ch) {
      case '$':
      case '\\':
        state = ch;
        break;
      case '}':
        // If outer is null then the closing brace was found at the top level. I.e. there is no
        // matching opening brace.
        if (outer) {
          // Substitute variable.
          const auto n = j - i;
          auto name = s.substr(i + 2, n - 2);
          if (outer->count(name) == 0) {
            s.replace(i, n + 1, fn_(name));
            outer->insert(move(name));
          } else {
            // Loop detected: this name has already been interpolated at this position.
            s.erase(i, n + 1);
          }
          // Ascend: matched closing brace.
          return true;
        }
        break;
      }
    }
    ++j;
  }
  // Ascend: no closing brace.
  return false;
}

void readConf(istream& is, boost::container::flat_map<string, string>& conf)
{
  VarInterp interp;
  parseConf(is, [&interp, &conf](const auto& key, string val) {
    interp(val);
    conf.emplace(key, move(val));
  });
}

} // swirly
