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
#include "Backoff.hpp"

#include <chrono>
#include <thread>

namespace swirly {
inline namespace app {

void PhasedBackoff::idle() noexcept
{
    using namespace std::literals::chrono_literals;
    if (i_ < 1000) {
        cpuRelax();
    } else if (i_ < 2000) {
        sched_yield();
    } else if (i_ < 4000) {
        std::this_thread::sleep_for(1ms);
    } else {
        // Deep sleep.
        std::this_thread::sleep_for(250ms);
    }
    ++i_;
}

void YieldBackoff::idle() noexcept
{
    sched_yield();
}

} // namespace app
} // namespace swirly
