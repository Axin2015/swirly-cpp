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
#ifndef SWIRLY_HDR_RECORDER_HPP
#define SWIRLY_HDR_RECORDER_HPP

#include <swirly/Config.h>

#include <chrono>

namespace swirly {
inline namespace hdr {
class Histogram;
/**
 * Record time elapsed during object lifetime, i.e., between constructor and destructor calls. The
 * elapsed time is recorded in the Histogram object during destruction.
 */
class SWIRLY_API Recorder {
  public:
    explicit Recorder(Histogram& hist) noexcept
    : hist_(hist)
    , start_{std::chrono::high_resolution_clock::now()}
    {
    }
    ~Recorder();

    // Copy.
    Recorder(const Recorder& rhs) = delete;
    Recorder& operator=(const Recorder& rhs) = delete;

    // Move.
    Recorder(Recorder&&) = delete;
    Recorder& operator=(Recorder&&) = delete;

  private:
    Histogram& hist_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

} // namespace hdr
} // namespace swirly

#endif // SWIRLY_HDR_RECORDER_HPP
