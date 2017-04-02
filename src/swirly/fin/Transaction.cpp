/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#include <swirly/fin/Transaction.hpp>

#include <swirly/util/Log.hpp>

using namespace std;

namespace swirly {

Transactional::~Transactional() noexcept = default;

Transaction::~Transaction() noexcept
{
    if (more_ == More::No) {
        try {
            target_.reset();
        } catch (const exception& e) {
            SWIRLY_ERROR(logMsg() << "failed to reset transaction: " << e.what());
        }
    } else if (!done_) {
        try {
            target_.tryRollback();
        } catch (const exception& e) {
            SWIRLY_ERROR(logMsg() << "failed to rollback transaction: " << e.what());
        }
    }
}

} // swirly
