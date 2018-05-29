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
#ifndef SWIRLY_PROF_HDRRECORDER_HPP
#define SWIRLY_PROF_HDRRECORDER_HPP

#include <swirly/Config.h>

#include <hdr_histogram.h>

#include <system_error>

#include <chrono>
#include <cstdio>

namespace swirly {
inline namespace prof {
class HdrHistogram;
/**
 * Record time elapsed during object lifetime, i.e., between constructor and destructor calls. The
 * elapsed time is recorded in the HdrHistogram object during destruction.
 */
class SWIRLY_API HdrRecorder {
  public:
    explicit HdrRecorder(HdrHistogram& hdrHist) noexcept
    : hdrHist_(hdrHist)
    , start_{std::chrono::high_resolution_clock::now()}
    {
    }
    ~HdrRecorder();

    // Copy.
    HdrRecorder(const HdrRecorder& rhs) = delete;
    HdrRecorder& operator=(const HdrRecorder& rhs) = delete;

    // Move.
    HdrRecorder(HdrRecorder&&) = delete;
    HdrRecorder& operator=(HdrRecorder&&) = delete;

  private:
    HdrHistogram& hdrHist_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

} // namespace prof
} // namespace swirly

#endif // SWIRLY_PROF_HDRRECORDER_HPP
