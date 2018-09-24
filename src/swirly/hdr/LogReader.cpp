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
#include "LogReader.hpp"

#include "Histogram.hpp"

#include <cassert>

namespace swirly {
inline namespace hdr {
using namespace std;
namespace {
} // namespace

LogReader::LogReader(std::FILE* stream)
: stream_{stream}
{
    assert(stream);
    const auto err = hdr_log_reader_init(&reader_);
    if (err != 0) {
        throw runtime_error{"hdr_log_reader_init: "s + hdr_strerror(err)};
    }
}

LogReader::~LogReader() = default;

void LogReader::read_header()
{
    const auto err = hdr_log_read_header(&reader_, stream_);
    if (err != 0) {
        throw runtime_error{"hdr_log_read_header: "s + hdr_strerror(err)};
    }
}

bool LogReader::read(Histogram& hist, WallTime& timestamp, WallTime& interval)
{
    // Merge if histogram is non-null.
    hdr_histogram* h{hist.get()};
    hdr_timespec timestamp_ts, interval_ts;
    const auto ret = hdr_log_read(&reader_, stream_, &h, &timestamp_ts, &interval_ts);
    if (ret == EOF) {
        return false;
    } else if (ret != 0) {
        throw runtime_error{"hdr_log_read: "s + hdr_strerror(ret)};
    }
    assert(h);
    // If not merge, then reset pointer to new histogram.
    if (!hist) {
        hist.reset(h);
    }
    timestamp = to_time<WallClock>(timestamp_ts);
    interval = to_time<WallClock>(interval_ts);
    return true;
}

} // namespace hdr
} // namespace swirly
