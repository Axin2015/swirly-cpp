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
inline error_code makeError(int err)
{
    return error_code{err, system_category()};
}
} // namespace

HdrHistogram::HdrHistogram(int64_t lowestTrackableValue, int64_t highestTrackableValue,
                           int significantFigures)
{
    const auto err
        = hdr_init(lowestTrackableValue, highestTrackableValue, significantFigures, &hist_);
    if (err != 0) {
        throw system_error{makeError(err), "hdr_init"};
    }
    assert(hist_);
}

HdrHistogram::~HdrHistogram()
{
    if (hist_) {
        hdr_close(hist_);
    }
}

void HdrHistogram::print(FILE* stream, int32_t ticksPerHalfDistance, double valueScale,
                         format_type format) const
{
    const auto err = hdr_percentiles_print(hist_, stream, ticksPerHalfDistance, valueScale, format);
    if (err != 0) {
        throw system_error{makeError(err), "hdr_percentiles_print"};
    }
}

void HdrHistogram::print(const char* path, int32_t ticksPerHalfDistance, double valueScale,
                         format_type format) const
{
    auto fp = openFile(path, "w");
    print(fp.get(), ticksPerHalfDistance, valueScale, format);
}

void HdrHistogram::write(Time startTime, Time endTime, HdrLogWriter& writer)
{
    writer.write(startTime, endTime, *hist_);
}

} // namespace prof
} // namespace swirly
