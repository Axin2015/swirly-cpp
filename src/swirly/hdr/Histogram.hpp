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
#ifndef SWIRLY_HDR_HISTOGRAM_HPP
#define SWIRLY_HDR_HISTOGRAM_HPP

#include <swirly/util/Time.hpp>

#include <hdr_histogram.h>

#include <cassert>
#include <cstdio>

namespace swirly {
inline namespace hdr {
class LogWriter;

using HistogramPtr = std::unique_ptr<hdr_histogram, void (*)(hdr_histogram*)>;

inline void clear(hdr_histogram& hist) noexcept
{
    hdr_reset(&hist);
}

class SWIRLY_API Histogram {
  public:
    /**
     * Construct the histogram.
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
    explicit Histogram(hdr_histogram* hist = nullptr)
    : ptr_{hist, hdr_close}
    {
    }
    Histogram(std::int64_t lowest_trackable_value, std::int64_t highest_trackable_value,
              int significant_figures);
    ~Histogram();

    // Copy.
    Histogram(const Histogram& rhs) = delete;
    Histogram& operator=(const Histogram& rhs) = delete;

    // Move.
    Histogram(Histogram&&) = default;
    Histogram& operator=(Histogram&&) = default;

    explicit operator bool() const noexcept { return bool{ptr_}; }
    hdr_histogram* get() const noexcept { return ptr_.get(); }
    void reset(hdr_histogram* hist = nullptr) noexcept { ptr_.reset(hist); }
    void swap(Histogram& rhs) noexcept { ptr_.swap(rhs.ptr_); }

    /**
     * Get minimum value from the histogram. Will return 2^63-1 if the histogram is empty.
     */
    std::int64_t min() const noexcept
    {
        assert(ptr_);
        return hdr_min(ptr_.get());
    }

    /**
     * Get maximum value from the histogram. Will return 0 if the histogram is empty.
     */
    std::int64_t max() const noexcept
    {
        assert(ptr_);
        return hdr_max(ptr_.get());
    }

    /**
     * Get the value at a specific percentile.
     */
    std::int64_t value_at_percentile(double percentile) const noexcept
    {
        assert(ptr_);
        return hdr_value_at_percentile(ptr_.get(), percentile);
    }

    /**
     * Gets the standard deviation for the values in the histogram.
     */
    double stddev() const noexcept
    {
        assert(ptr_);
        return hdr_stddev(ptr_.get());
    }

    /**
     * Gets the mean for the values in the histogram.
     */
    double mean() const noexcept
    {
        assert(ptr_);
        return hdr_mean(ptr_.get());
    }

    /**
     * Get the count of recorded values at a specific value (to within the histogram resolution at
     * the value level).
     *
     * @param value The value for which to provide the recorded count.
     *
     * @return The total count of values recorded in the histogram within the value range that is
     * lowestEquivalentValue <= value <= highestEquivalentValue.
     */
    std::int64_t count_at_value(std::int64_t value) const noexcept
    {
        assert(ptr_);
        return hdr_count_at_value(ptr_.get(), value);
    }

    /**
     * Print out a percentile based histogram to the supplied stream. Note that this call will not
     * flush the FILE, this is left up to the user.
     *
     * @param stream The FILE to write the output to.
     *
     * @param ticks_per_half_distance The number of iteration steps per half-distance to 100%.
     *
     * @param value_scale Scale the output values by this amount.
     *
     * @param format CLASSIC or CSV.
     */
    void percentiles_print(std::FILE* stream, std::int32_t ticks_per_half_distance,
                           double value_scale, format_type format = CLASSIC) const;

    /**
     * Print out a percentile based histogram to the supplied stream.
     *
     * @param path The output file path.
     *
     * @param ticks_per_half_distance The number of iteration steps per half-distance to 100%.
     *
     * @param value_scale Scale the output values by this amount.
     *
     * @param format CLASSIC or CSV.
     */
    void percentiles_print(const char* path, std::int32_t ticks_per_half_distance,
                           double value_scale, format_type format = CLASSIC) const;

    /**
     * Clear histogram.
     */
    void clear() noexcept
    {
        if (ptr_) {
            hdr_reset(ptr_.get());
        }
    }

    /**
     * Records a value in the histogram, will round this value of to a precision at or better than
     * the significant_figure specified at construction time.
     *
     * @param value Value to add to the histogram.
     *
     * @return false if the value is larger than the highest_trackable_value and can't be recorded,
     * true otherwise.
     */
    bool record_value(std::int64_t value) noexcept
    {
        assert(ptr_);
        return hdr_record_value(ptr_.get(), value);
    }

    /**
     * Records count values in the histogram, will round this value of to a precision at or better
     * than the significant_figure specified at construction time.
     *
     * @param value Value to add to the histogram.
     *
     * @param count Number of 'value's to add to the histogram.
     *
     * @return false if any value is larger than the highest_trackable_value and can't be recorded,
     * true otherwise.
     */
    bool record_values(std::int64_t value, std::int64_t count) noexcept
    {
        assert(ptr_);
        return hdr_record_values(ptr_.get(), value, count);
    }

    void write(WallTime start_time, WallTime end_time, LogWriter& writer);

  private:
    HistogramPtr ptr_{nullptr, hdr_close};
};

} // namespace hdr
} // namespace swirly

#endif // SWIRLY_HDR_HISTOGRAM_HPP
