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
#ifndef SWIRLY_FIX_GROUP_HPP
#define SWIRLY_FIX_GROUP_HPP

#include <swirly/fix/Field.hpp>

namespace swirly {
inline namespace fix {
class FixLexer;

struct MdEntry {
    MdEntryType type;
    MdEntryPx px;
    MdEntrySize size;
};

template <typename StreamT>
StreamT& operator<<(StreamT& os, const MdEntry& grp)
{
    os << grp.type << grp.px << grp.size;
    return os;
}

SWIRLY_API void parse_group(FixLexer& lex, MdEntry& grp);

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_GROUP_HPP
