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
#include "Utility.hpp"

namespace swirly {
inline namespace util {
using namespace std;

int hex_digits(int64_t i) noexcept
{
    int n{0};
    if (i & 0xffffffff00000000) {
        n += 8;
        i >>= 32;
    }
    if (i & 0xffff0000) {
        n += 4;
        i >>= 16;
    }
    if (i & 0xff00) {
        n += 2;
        i >>= 8;
    }
    if (i & 0xf0) {
        n += 2;
    } else if (i & 0x0f) {
        ++n;
    }
    return n;
}

bool stob(string_view sv, bool dfl) noexcept
{
    bool val{dfl};
    switch (sv.size()) {
    case 1:
        switch (sv[0]) {
        case '0':
        case 'F':
        case 'N':
        case 'f':
        case 'n':
            val = false;
            break;
        case '1':
        case 'T':
        case 'Y':
        case 't':
        case 'y':
            val = true;
            break;
        }
        break;
    case 2:
        if ((sv[0] == 'N' || sv[0] == 'n') //
            && (sv[1] == 'O' || sv[1] == 'o')) {
            val = false;
        } else if ((sv[0] == 'O' || sv[0] == 'o') //
                   && (sv[1] == 'N' || sv[1] == 'n')) {
            val = true;
        }
        break;
    case 3:
        if ((sv[0] == 'O' || sv[0] == 'o')    //
            && (sv[1] == 'F' || sv[1] == 'f') //
            && (sv[2] == 'F' || sv[2] == 'f')) {
            val = false;
        } else if ((sv[0] == 'Y' || sv[0] == 'y')    //
                   && (sv[1] == 'E' || sv[1] == 'e') //
                   && (sv[2] == 'S' || sv[2] == 's')) {
            val = true;
        }
        break;
    case 4:
        if ((sv[0] == 'T' || sv[0] == 't')    //
            && (sv[1] == 'R' || sv[1] == 'r') //
            && (sv[2] == 'U' || sv[2] == 'u') //
            && (sv[3] == 'E' || sv[3] == 'e')) {
            val = true;
        }
        break;
    case 5:
        if ((sv[0] == 'F' || sv[0] == 'f')    //
            && (sv[1] == 'A' || sv[1] == 'a') //
            && (sv[2] == 'L' || sv[2] == 'l') //
            && (sv[3] == 'S' || sv[3] == 's') //
            && (sv[4] == 'E' || sv[4] == 'e')) {
            val = false;
        }
        break;
    }
    return val;
}

} // namespace util
} // namespace swirly
