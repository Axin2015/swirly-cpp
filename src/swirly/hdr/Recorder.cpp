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
#include "Recorder.hpp"

#include "Histogram.hpp"

namespace swirly {
inline namespace hdr {
using namespace std;

Recorder::Recorder(Histogram& hist, int count) noexcept
: hist_(hist)
, count_{count}
, start_{std::chrono::high_resolution_clock::now()}
{
    assert(count >= 1);
}

Recorder::~Recorder()
{
    const auto end = chrono::high_resolution_clock::now();
    const auto diff = chrono::duration_cast<chrono::nanoseconds>(end - start_);
    if (count_ > 1) {
        hist_.record_values(diff.count() / count_, count_);
    } else {
        hist_.record_value(diff.count());
    }
}
} // namespace hdr
} // namespace swirly
