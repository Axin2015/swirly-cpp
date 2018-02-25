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
#ifndef SWIRLY_UTIL_PROFILE_HPP
#define SWIRLY_UTIL_PROFILE_HPP

#include <swirly/util/Math.hpp>
#include <swirly/util/StringBuf.hpp>

#include <chrono>

namespace swirly {

/**
 * A statistical profile of observations.
 */
class SWIRLY_API Profile {
  public:
    Profile(std::string_view name) noexcept
      : name_{name}
    {
    }
    ~Profile() noexcept;

    // Copy.
    Profile(const Profile& rhs) noexcept = default;
    Profile& operator=(const Profile& rhs) noexcept = default;

    // Move.
    Profile(Profile&&) noexcept = default;
    Profile& operator=(Profile&&) noexcept = default;

    /**
     * Write report to log if sufficient observations exists.
     */
    void report() const noexcept;
    /**
     * @return true if there are no observations.
     */
    bool empty() const noexcept { return var_.empty(); }
    /**
     * @return the number of observations.
     */
    std::size_t size() const noexcept { return var_.size(); }
    /**
     * Clear profile.
     */
    void clear() noexcept { var_.clear(); }
    /**
     * Add observation to profile.
     *
     * @param val The observation.
     */
    void record(double val) noexcept;

  private:
    StringBuf<16> name_;
    VarAccum var_;
};

/**
 * Record time elapsed during object lifetime, i.e., between constructor and destructor calls. The
 * elapsed time is recorded in the Profile object during destruction.
 */
class SWIRLY_API TimeRecorder {
  public:
    explicit TimeRecorder(Profile& profile) noexcept
      : profile_(profile)
      , start_{std::chrono::high_resolution_clock::now()}
    {
    }
    ~TimeRecorder() noexcept;

    // Copy.
    TimeRecorder(const TimeRecorder& rhs) = delete;
    TimeRecorder& operator=(const TimeRecorder& rhs) = delete;

    // Move.
    TimeRecorder(TimeRecorder&&) = delete;
    TimeRecorder& operator=(TimeRecorder&&) = delete;

  private:
    Profile& profile_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

} // namespace swirly

#endif // SWIRLY_UTIL_PROFILE_HPP
