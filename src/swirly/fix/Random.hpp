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
#ifndef SWIRLY_FIX_RANDOM_HPP
#define SWIRLY_FIX_RANDOM_HPP

#include <swirly/fin/IntTypes.hpp>

#include <random>

namespace swirly {
inline namespace fix {

/**
 * Random best bid and offer based on "open" (or reference) price.
 */
class SWIRLY_API RandomBbo {
  public:
    explicit RandomBbo(std::random_device& rd)
    : rd_(rd)
    {
    }
    ~RandomBbo();

    // Copy.
    RandomBbo(const RandomBbo&) = delete;
    RandomBbo& operator=(const RandomBbo&) = delete;

    // Move.
    RandomBbo(RandomBbo&&) = delete;
    RandomBbo& operator=(RandomBbo&&) = delete;

    std::pair<Ticks, Ticks> operator()(Ticks open);

  private:
    std::random_device& rd_;
    std::mt19937 gen_{rd_()};
    std::binomial_distribution<std::int64_t> dist_{4, 0.5};
    std::int64_t offset_{0};
};

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_RANDOM_HPP
