/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#ifndef SWIRLY_FIN_DATE_HPP
#define SWIRLY_FIN_DATE_HPP

#include <swirly/util/Date.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/date_time/local_time/local_time_types.hpp>
#pragma GCC diagnostic pop

namespace swirly {

/**
 * America/New_York.
 * See http://www.di-mgt.com.au/wclock/tz.html
 */
constexpr char NewYork[]{"EST-5EDT,M3.2.0/2,M11.1.0/2"};

/**
 * Custom TimeZone representing "Market Time".
 * Where is Market Time is NewYork + 7 hours.
 */
constexpr char MarketZone[]{"MKST+2MKDT,M3.2.0/2,M11.1.0/2"};

/**
 * Business day functor. Date calculations on the critical path can become a performance bottleneck.
 * This class improves performance for the most common use-case by caching the most recent results.
 */
class SWIRLY_API BusinessDay {
  public:
    explicit BusinessDay(const char* timeZone);
    ~BusinessDay() noexcept;

    // Copy.
    BusinessDay(const BusinessDay& rhs) noexcept;
    BusinessDay& operator=(const BusinessDay& rhs);

    // Move.
    BusinessDay(BusinessDay&&) noexcept;
    BusinessDay& operator=(BusinessDay&&) noexcept;

    /**
     * Get the business day from a transaction time.
     *
     * @param time Unix time.
     *
     * @return the business day.
     */
    JDay operator()(Time time) const;

  private:
    boost::local_time::time_zone_ptr timeZone_;
    /**
     * Cache entries for odd and even time_t values.
     */
    mutable std::pair<int64_t, JDay> cache_[2]{};
};

} // namespace swirly

#endif // SWIRLY_FIN_DATE_HPP
