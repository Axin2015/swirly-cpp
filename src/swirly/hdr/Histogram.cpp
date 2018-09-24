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
#include "Histogram.hpp"

#include "File.hpp"
#include "LogWriter.hpp"

#include <cassert>

extern "C" const char* hdr_strerror(int errnum);

namespace swirly {
inline namespace hdr {
using namespace std;
namespace {
} // namespace

Histogram::Histogram(int64_t lowest_trackable_value, int64_t highest_trackable_value,
                     int significant_figures)
{
    hdr_histogram* hist{nullptr};
    const auto err
        = hdr_init(lowest_trackable_value, highest_trackable_value, significant_figures, &hist);
    if (err != 0) {
        throw runtime_error{"hdr_init: "s + hdr_strerror(err)};
    }
    assert(hist);
    ptr_.reset(hist);
}

Histogram::~Histogram() = default;

void Histogram::percentiles_print(FILE* stream, int32_t ticks_per_half_distance, double value_scale,
                                  format_type format) const
{
    assert(ptr_);
    const auto err
        = hdr_percentiles_print(ptr_.get(), stream, ticks_per_half_distance, value_scale, format);
    if (err != 0) {
        throw runtime_error{"hdr_percentiles_print: "s + hdr_strerror(err)};
    }
}

void Histogram::percentiles_print(const char* path, int32_t ticks_per_half_distance,
                                  double value_scale, format_type format) const
{
    auto fp = open_file(path, "w");
    percentiles_print(fp.get(), ticks_per_half_distance, value_scale, format);
}

void Histogram::write(WallTime start_time, WallTime end_time, LogWriter& writer)
{
    assert(ptr_);
    writer.write(start_time, end_time, *ptr_);
}

} // namespace hdr
} // namespace swirly
