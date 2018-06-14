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
#ifndef SWIRLY_DB_EXCEPTION_HPP
#define SWIRLY_DB_EXCEPTION_HPP

#include <swirly/util/Exception.hpp>

namespace swirly {
inline namespace db {

class DbException : public Exception {
  public:
    explicit DbException(std::string_view what) noexcept
    : Exception{what}
    {
    }
    ~DbException() override;

    // Copy.
    DbException(const DbException&) noexcept = default;
    DbException& operator=(const DbException&) noexcept = default;

    // Move.
    DbException(DbException&&) noexcept = default;
    DbException& operator=(DbException&&) noexcept = default;
};

} // namespace db
} // namespace swirly

#endif // SWIRLY_DB_EXCEPTION_HPP
