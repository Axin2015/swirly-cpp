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
#include "IntervalRecorder.hpp"

extern "C" const char* hdr_strerror(int errnum);

namespace swirly {
inline namespace hdr {
using namespace std;
namespace {
} // namespace

IntervalRecorder::IntervalRecorder(int64_t lowest_trackable_value, int64_t highest_trackable_value,
                                   int significant_figures)
{
    const auto err = hdr_interval_recorder_init_all(&recorder_, lowest_trackable_value,
                                                    highest_trackable_value, significant_figures);
    if (err != 0) {
        throw runtime_error{"hdr_interval_recorder_init_all: "s + hdr_strerror(err)};
    }
}

IntervalRecorder::IntervalRecorder()
{
    const auto err = hdr_interval_recorder_init(&recorder_);
    if (err != 0) {
        throw runtime_error{"hdr_interval_recorder_init: "s + hdr_strerror(err)};
    }
}

IntervalRecorder::~IntervalRecorder()
{
    hdr_interval_recorder_destroy(&recorder_);
}

} // namespace hdr
} // namespace swirly
