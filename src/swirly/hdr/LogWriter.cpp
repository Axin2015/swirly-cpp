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
#include "LogWriter.hpp"

#include <cassert>

namespace swirly {
inline namespace hdr {
using namespace std;
namespace {
} // namespace

LogWriter::LogWriter(std::FILE* stream)
: stream_{stream}
{
    assert(stream);
    const auto err = hdr_log_writer_init(&writer_);
    if (err != 0) {
        throw runtime_error{"hdr_log_writer_init: "s + hdr_strerror(err)};
    }
}

LogWriter::~LogWriter() = default;

void LogWriter::write_header(Time now, const char* user_prefix)
{
    auto ts = to_timespec(now);
    const auto err = hdr_log_write_header(&writer_, stream_, user_prefix, &ts);
    if (err != 0) {
        throw runtime_error{"hdr_log_write_header: "s + hdr_strerror(err)};
    }
}

void LogWriter::write(Time start_time, Time end_time, hdr_histogram& hist)
{
    auto start_ts = to_timespec(start_time);
    auto end_ts = to_timespec(end_time);
    const auto err = hdr_log_write(&writer_, stream_, &start_ts, &end_ts, &hist);
    if (err != 0) {
        throw runtime_error{"hdr_log_write: "s + hdr_strerror(err)};
    }
}

} // namespace hdr
} // namespace swirly
