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
#include "HdrLogWriter.hpp"

#include <system_error>

namespace swirly {
inline namespace prof {
using namespace std;
namespace {
inline error_code make_error(int err)
{
    return error_code{err, system_category()};
}

inline hdr_timespec to_time_spec(Time t) noexcept
{
    const auto ns = ns_since_epoch(t);
    hdr_timespec ts;
    ts.tv_sec = ns / 1'000'000'000L;
    ts.tv_nsec = ns % 1'000'000'000L;
    return ts;
}
} // namespace

HdrLogWriter::HdrLogWriter(const char* path, const char* user_prefix)
: file_{open_file(path, "a")}
{
    auto err = hdr_log_writer_init(&writer_);
    if (err != 0) {
        throw system_error{make_error(err), "hdr_log_writer_init"};
    }
    if (ftell(file_.get()) > 0) {
        return;
    }
    auto ts = to_time_spec(UnixClock::now());
    err = hdr_log_write_header(&writer_, file_.get(), user_prefix, &ts);
    if (err != 0) {
        throw runtime_error{"hdr_log_write_header failed:"s + hdr_strerror(err)};
    }
}

HdrLogWriter::~HdrLogWriter() = default;

void HdrLogWriter::write(Time start_time, Time end_time, hdr_histogram& hist)
{
    auto start_ts = to_time_spec(start_time);
    auto end_ts = to_time_spec(end_time);
    const auto err = hdr_log_write(&writer_, file_.get(), &start_ts, &end_ts, &hist);
    if (err != 0) {
        throw runtime_error{"hdr_log_write failed:"s + hdr_strerror(err)};
    }
}

} // namespace prof
} // namespace swirly
