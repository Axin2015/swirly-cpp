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
#ifndef SWIRLY_HDR_LOGREADER_HPP
#define SWIRLY_HDR_LOGREADER_HPP

#include <swirly/util/Time.hpp>

#include <hdr_histogram_log.h>

namespace swirly {
inline namespace hdr {

class Histogram;

class SWIRLY_API LogReader {
  public:
    /**
     * Initialise the log reader.
     *
     * @param stream The input stream.
     */
    explicit LogReader(std::FILE* stream);
    ~LogReader();

    // Copy.
    LogReader(const LogReader&) = delete;
    LogReader& operator=(const LogReader&) = delete;

    // Move.
    LogReader(LogReader&&) = delete;
    LogReader& operator=(LogReader&&) = delete;

    /**
     * Reads the the header information from the log. Will capure information such as version number
     * and start timestamp from the header.
     */
    void read_header();

    /**
     * Reads an entry from the log filling in the specified histogram, timestamp and interval
     * values. If the supplied pointer to the histogram for this method is NULL then a new histogram
     * will be allocated for the caller, however it will become the callers responsibility to free
     * it later. If the pointer is non-null the histogram read from the log will be merged with the
     * supplied histogram.
     *
     * @param hist Pointer to allocate a histogram to or merge into.
     *
     * @param timestamp The first timestamp from the CSV entry.
     *
     * @param interval The second timestamp from the CSV entry
     *
     * @return false indicates that there are no more histograms left to be read from 'file'.
     */
    bool read(Histogram& hist, WallTime& timestamp, WallTime& interval);

  private:
    std::FILE* stream_;
    hdr_log_reader reader_{};
};

} // namespace hdr
} // namespace swirly

#endif // SWIRLY_HDR_LOGREADER_HPP
