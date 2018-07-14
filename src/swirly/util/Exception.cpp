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
#include "Exception.hpp"

namespace swirly {
inline namespace util {
using namespace std;
namespace {
thread_local ErrMsg err_msg_;
} // namespace

Exception::Exception(error_code ec)
: runtime_error{ec.message()}
, ec_{ec}
{
}
Exception::Exception(int err, const error_category& ecat)
: Exception{error_code{err, ecat}}
{
}
Exception::Exception(error_code ec, string_view what)
: runtime_error{string{what} + ": " + ec.message()}
, ec_{ec}
{
}
Exception::Exception(int err, const error_category& ecat, string_view what)
: Exception(error_code{err, ecat}, what)
{
}

Exception::~Exception() = default;

void Exception::to_json(ostream& os, int status, const char* reason, const char* detail)
{
    os << "{\"status\":" << status     //
       << ",\"reason\":\"" << reason   //
       << "\",\"detail\":\"" << detail //
       << "\"}";
}

ErrMsg& err_msg() noexcept
{
    err_msg_.reset();
    return err_msg_;
}

} // namespace util
} // namespace swirly
