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
#include <swirly/ash/VarSub.hpp>

#include <cstdlib> // getenv()

using namespace std;

namespace swirly {

string getEnv(const string& name)
{
  const char* const val{getenv(name.c_str())};
  return val ? string{val} : string{};
}

VarSub::~VarSub() noexcept = default;

// Copy.
VarSub::VarSub(const VarSub&) = default;
VarSub& VarSub::operator=(const VarSub&) = default;

// Move.
VarSub::VarSub(VarSub&&) = default;
VarSub& VarSub::operator=(VarSub&&) = default;

bool VarSub::substitute(string& s, const size_t i, size_t j, set<string>* outer) const
{
  // Position of last substitution.
  size_t last{0};
  // Names substituted at 'last' position.
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
          // Descend: search for closing brace and substitute.
          if (!substitute(s, j, j + 2, &inner)) {
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
            // Loop detected: this name has already been substituted at this position.
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

} // swirly
