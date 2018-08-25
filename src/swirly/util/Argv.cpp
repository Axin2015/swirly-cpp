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
#include "Argv.hpp"

namespace swirly {
inline namespace util {
using namespace std;

ArgvLexer::~ArgvLexer() = default;

void ArgvLexer::pop_switch()
{
    ++it_;
    if (val_) {
        throw runtime_error{"unexpected value for option: " + string{opt_}};
    }
    next();
}

string_view ArgvLexer::pop_value()
{
    ++it_;
    string_view val;
    if (val_) {
        val = *val_;
    } else {
        if (empty()) {
            throw runtime_error{"missing value for option: " + string{opt_}};
        }
        val = *it_++;
    }
    next();
    return val;
}

void ArgvLexer::next() noexcept
{
    if (empty()) {
        opt_ = {};
        val_.reset();
        return;
    }
    const string_view arg{*it_};
    if (!(arg.size() > 1 && arg[0] == '-')) {
        // Positional argument.
        opt_ = {};
        val_ = arg;
        return;
    }

    if (arg[1] == '-') {
        if (arg.size() == 2) {
            // Treat double dash as positional argument.
            opt_ = {};
            val_ = arg;
            return;
        }
        // Long option.
        const auto pos = arg.find_first_of('=', 2);
        opt_ = arg.substr(2, pos - 2);
        if (pos != string_view::npos) {
            val_ = arg.substr(pos + 1);
        } else {
            val_.reset();
        }
    } else {
        // Short option.
        opt_ = arg.substr(1, 1);
        if (arg.size() > 2) {
            val_ = arg.substr(2);
        } else {
            val_.reset();
        }
    }
}

} // namespace util
} // namespace swirly
