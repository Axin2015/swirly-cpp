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
#include "HdrHistogram.hpp"

#include "File.hpp"
#include "HdrLogWriter.hpp"

#include <cassert>
#include <system_error>

namespace swirly {
inline namespace prof {
using namespace std;
namespace {
inline error_code make_error(int err)
{
    return error_code{err, system_category()};
}
} // namespace

HdrHistogram::HdrHistogram(int64_t lowest_trackable_value, int64_t highest_trackable_value,
                           int significant_figures)
{
    const auto err
        = hdr_init(lowest_trackable_value, highest_trackable_value, significant_figures, &hist_);
    if (err != 0) {
        throw system_error{make_error(err), "hdr_init"};
    }
    assert(hist_);
}

HdrHistogram::~HdrHistogram()
{
    if (hist_) {
        hdr_close(hist_);
    }
}

void HdrHistogram::print(FILE* stream, int32_t ticks_per_half_distance, double value_scale,
                         format_type format) const
{
    const auto err
        = hdr_percentiles_print(hist_, stream, ticks_per_half_distance, value_scale, format);
    if (err != 0) {
        throw system_error{make_error(err), "hdr_percentiles_print"};
    }
}

void HdrHistogram::print(const char* path, int32_t ticks_per_half_distance, double value_scale,
                         format_type format) const
{
    auto fp = open_file(path, "w");
    print(fp.get(), ticks_per_half_distance, value_scale, format);
}

void HdrHistogram::write(Time start_time, Time end_time, HdrLogWriter& writer)
{
    writer.write(start_time, end_time, *hist_);
}

} // namespace prof
} // namespace swirly
