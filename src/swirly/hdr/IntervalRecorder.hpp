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
#ifndef SWIRLY_HDR_INTERVALRECORDER_HPP
#define SWIRLY_HDR_INTERVALRECORDER_HPP

#include <swirly/util/Time.hpp>

#include <hdr_interval_recorder.h>

namespace swirly {
inline namespace hdr {

class SWIRLY_API IntervalRecorder {
  public:
    /**
     * Construct the interval recorder.
     *
     * @param lowest_trackable_value The smallest possible value to be put into the histogram.
     *
     * @param highest_trackable_value The largest possible value to be put into the histogram.
     *
     * @param significant_figures The level of precision for this histogram, i.e. the number of
     * figures in a decimal number that will be maintained. For example, a value of 3 will mean the
     * results from the histogram will be accurate up to the first three digits. Must be a value
     * between 1 and 5 (inclusive).
     */
    IntervalRecorder(std::int64_t lowest_trackable_value, std::int64_t highest_trackable_value,
                     int significant_figures);
    IntervalRecorder();
    ~IntervalRecorder();

    // Copy.
    IntervalRecorder(const IntervalRecorder& rhs) = delete;
    IntervalRecorder& operator=(const IntervalRecorder& rhs) = delete;

    // Move.
    IntervalRecorder(IntervalRecorder&&) = delete;
    IntervalRecorder& operator=(IntervalRecorder&&) = delete;

    /**
     * Records a value in the histogram, will round this value of to a precision at or better than
     * the "significant figures" specified at construction time.
     *
     * @param value Value to add to the histogram.
     *
     * @return false if the value is larger than the "highest trackable value" and can't be
     * recorded.
     */
    bool record_value(std::int64_t value) noexcept
    {
        return hdr_interval_recorder_record_value(&recorder_, value) != 0;
    }

    hdr_histogram* sample_and_recycle(hdr_histogram* inactive_histogram) noexcept
    {
        return hdr_interval_recorder_sample_and_recycle(&recorder_, inactive_histogram);
    }

  private:
    hdr_interval_recorder recorder_{};
};

} // namespace hdr
} // namespace swirly

#endif // SWIRLY_HDR_INTERVALRECORDER_HPP
