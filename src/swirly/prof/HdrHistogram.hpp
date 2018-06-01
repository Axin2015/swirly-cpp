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
#ifndef SWIRLY_PROF_HDRHISTOGRAM_HPP
#define SWIRLY_PROF_HDRHISTOGRAM_HPP

#include <swirly/util/Time.hpp>

#include <hdr_histogram.h>

#include <cstdio>

namespace swirly {
inline namespace prof {
class HdrLogWriter;

class SWIRLY_API HdrHistogram {
  public:
    /**
     * Construct the histogram.
     *
     * @param lowestTrackableValue The smallest possible value to be put into the histogram.
     *
     * @param highestTrackableValue The largest possible value to be put into the histogram.
     *
     * @param significantFigures The level of precision for this histogram, i.e. the number of
     * figures in a decimal number that will be maintained. For example, a value of 3 will mean the
     * results from the histogram will be accurate up to the first three digits. Must be a value
     * between 1 and 5 (inclusive).
     *
     * @param result Output parameter to capture allocated histogram.
     */
    HdrHistogram(std::int64_t lowestTrackableValue, std::int64_t highestTrackableValue,
                 int significantFigures);
    ~HdrHistogram();

    // Copy.
    HdrHistogram(const HdrHistogram& rhs) = delete;
    HdrHistogram& operator=(const HdrHistogram& rhs) = delete;

    // Move.
    HdrHistogram(HdrHistogram&&) noexcept = delete;
    HdrHistogram& operator=(HdrHistogram&&) noexcept = delete;

    /**
     * Print out a percentile based histogram to the supplied stream. Note that this call will not
     * flush the FILE, this is left up to the user.
     *
     * @param stream The FILE to write the output to.
     *
     * @param ticksPerHalfDistance The number of iteration steps per half-distance to 100%.
     *
     * @param valueScale Scale the output values by this amount.
     *
     * @param format CLASSIC or CSV.
     */
    void print(std::FILE* stream, std::int32_t ticksPerHalfDistance, double valueScale,
               format_type format = CLASSIC) const;

    /**
     * Print out a percentile based histogram to the supplied stream.
     *
     * @param path The output file path.
     *
     * @param ticksPerHalfDistance The number of iteration steps per half-distance to 100%.
     *
     * @param valueScale Scale the output values by this amount.
     *
     * @param format CLASSIC or CSV.
     */
    void print(const char* path, int32_t ticksPerHalfDistance, double valueScale,
               format_type format = CLASSIC) const;

    /**
     * Reset histogram.
     */
    void reset() noexcept { hdr_reset(hist_); }

    /**
     * Records a value in the histogram, will round this value of to a precision at or better than
     * the "significant figures" specified at construction time.
     *
     * @param value Value to add to the histogram.
     *
     * @return false if the value is larger than the "highest trackable value" and can't be
     * recorded.
     */
    bool record(std::int64_t value) noexcept { return hdr_record_value(hist_, value); }

    void write(Time startTime, Time endTime, HdrLogWriter& writer);

  private:
    hdr_histogram* hist_{nullptr};
};

} // namespace prof
} // namespace swirly

#endif // SWIRLY_PROF_HDRHISTOGRAM_HPP
