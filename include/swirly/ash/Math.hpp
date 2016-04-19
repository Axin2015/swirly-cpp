/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#ifndef SWIRLY_ASH_MATH_HPP
#define SWIRLY_ASH_MATH_HPP

#include <swirly/ash/Defs.hpp>

#include <algorithm>
#include <cmath>

namespace swirly {

/**
 * @addtogroup Util
 * @{
 */

class SWIRLY_API VarAccum {
 public:
  VarAccum() noexcept = default;
  ~VarAccum() noexcept = default;

  // Copy.
  constexpr VarAccum(const VarAccum& rhs) noexcept = default;
  VarAccum& operator=(const VarAccum& rhs) noexcept = default;

  // Move.
  constexpr VarAccum(VarAccum&&) noexcept = default;
  VarAccum& operator=(VarAccum&&) noexcept = default;

  bool empty() const noexcept { return size_ == 0; }
  std::size_t size() const noexcept { return size_; }
  double mean() const noexcept { return mean_; }
  double sum2() const noexcept { return sum2_; }
  double min() const noexcept { return min_; }
  double max() const noexcept { return max_; }

  void append(double val) noexcept
  {
    ++size_;
    double delta{val - mean_};
    mean_ += delta / size_;
    sum2_ += delta * (val - mean_);
    min_ = std::min(min_, val);
    max_ = std::max(max_, val);
  }
  template <typename... ArgsT>
  void append(double first, ArgsT... args) noexcept
  {
    append(first);
    // Recursively apply to tail.
    append(args...);
  }

 private:
  std::size_t size_{0};
  double mean_{0.0};
  double sum2_{0.0};
  double min_{std::numeric_limits<double>::max()};
  double max_{std::numeric_limits<double>::min()};
};

inline double var(const VarAccum& v) noexcept
{
  return v.size() > 1 ? v.sum2() / (v.size() - 1) : std::numeric_limits<double>::quiet_NaN();
}

inline double varp(const VarAccum& v) noexcept
{
  return !v.empty() ? v.sum2() / v.size() : std::numeric_limits<double>::quiet_NaN();
}

inline double stdev(const VarAccum& v) noexcept
{
  return std::sqrt(var(v));
}

inline double stdevp(const VarAccum& v) noexcept
{
  return std::sqrt(varp(v));
}

inline double zscore(double mean, double sd, double val) noexcept
{
  return (val - mean) / sd;
}

inline double pctile95(double mean, double sd) noexcept
{
  // NORMSINV(0.95) = 1.6448536
  return mean + 1.6448536 * sd;
}

inline double pctile99(double mean, double sd) noexcept
{
  // NORMSINV(0.99) = 2.3263479
  return mean + 2.3263479 * sd;
}

inline double pctile999(double mean, double sd) noexcept
{
  // NORMSINV(0.999) = 3.0902323
  return mean + 3.0902323 * sd;
}

/** @} */

} // swirly

#endif // SWIRLY_ASH_MATH_HPP
