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
#include <swirly/util/Profile.hpp>

#include <swirly/util/Log.hpp>

using namespace std;

namespace swirly {

Profile::~Profile() noexcept
{
  report();
}

void Profile::report() const noexcept
{
  const auto sd = stdev(var_);
  if (!std::isnan(sd)) {
    SWIRLY_INFO(logMsg() << '<' << name_ //
                         << "> {\"size\":" << var_.size() //
                         << ",\"mean\":" << var_.mean() //
                         << ",\"stdevp\":" << sd //
                         << ",\"pctile95\":" << pctile95(var_.mean(), sd) //
                         << ",\"pctile99\":" << pctile99(var_.mean(), sd) //
                         << ",\"pctile999\":" << pctile999(var_.mean(), sd) //
                         << ",\"min\":" << var_.min() //
                         << ",\"max\":" << var_.max() //
                         << '}');
  }
}

void Profile::record(double val) noexcept
{
  var_.append(val);
  const auto sd = stdev(var_);
  if (!std::isnan(sd)) {
    const auto z = zscore(var_.mean(), sd, val);
    // NORMSINV(0.999) = 3.0902323
    if (z > 3.0902323) {
      SWIRLY_WARNING(logMsg() << '<' << name_ << "> outlier " << val << " with z-score " << z);
    }
  }
}

TimeRecorder::~TimeRecorder() noexcept
{
  const auto end = chrono::high_resolution_clock::now();
  const chrono::duration<double, micro> diff{end - start_};
  const auto usec = diff.count();
  profile_.record(usec);
}

} // swirly
