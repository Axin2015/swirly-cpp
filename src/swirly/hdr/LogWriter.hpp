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
#ifndef SWIRLY_HDR_LOGWRITER_HPP
#define SWIRLY_HDR_LOGWRITER_HPP

#include <swirly/util/Time.hpp>

#include <hdr_histogram_log.h>

namespace swirly {
inline namespace hdr {

class SWIRLY_API LogWriter {
  public:
    /**
     * Initialise the log writer.
     *
     * @param stream The output stream.
     */
    explicit LogWriter(std::FILE* stream);
    ~LogWriter();

    // Copy.
    LogWriter(const LogWriter&) = delete;
    LogWriter& operator=(const LogWriter&) = delete;

    // Move.
    LogWriter(LogWriter&&) = delete;
    LogWriter& operator=(LogWriter&&) = delete;

    /**
     * Write the header to the log, this will constist of a user defined string, the current
     * timestamp, version information and the CSV header.
     *
     * @param now The start time that the histogram started recording from.
     *
     * @param user_prefix User defined string to include in the header.
     */
    void write_header(WallTime now, const char* user_prefix);

    /**
     * Write an hdr_histogram entry to the log. It will be encoded in a similar fashion to the
     * approach used by the Java version of the HdrHistogram. It will be a CSV line consisting of
     * <start timestamp>,<end timestamp>,<max>,<histogram> where <histogram> is the binary histogram
     * gzip compressed and base64 encoded.
     *
     * Timestamp is a bit of misnomer for the start_time and end_time values these could be offsets,
     * e.g. start_timestamp could be offset from process start time and end_timestamp could actually
     * be the length of the recorded interval.
     *
     * @param start_time The start timestamp to include in the logged entry.
     *
     * @param end_time The end timestamp to include in the logged entry.
     *
     * @param hist The histogram to encode and log.
     */
    void write(WallTime start_time, WallTime end_time, hdr_histogram& hist);

    void flush() noexcept { fflush(stream_); }

  private:
    std::FILE* stream_;
    hdr_log_writer writer_{};
};

} // namespace hdr
} // namespace swirly

#endif // SWIRLY_HDR_LOGWRITER_HPP
