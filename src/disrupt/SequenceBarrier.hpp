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
#ifndef SWIRLY_DISRUPT_SEQUENCEBARRIER_HPP
#define SWIRLY_DISRUPT_SEQUENCEBARRIER_HPP

#include <swirly/Config.h>

namespace swirly {

class Sequence;

class SWIRLY_API SequenceBarrier {
  public:
    SequenceBarrier() noexcept = default;
    virtual ~SequenceBarrier() noexcept;

    // Copy.
    SequenceBarrier(const SequenceBarrier&) noexcept = default;
    SequenceBarrier& operator=(const SequenceBarrier&) noexcept = default;

    // Move.
    SequenceBarrier(SequenceBarrier&&) noexcept = default;
    SequenceBarrier& operator=(SequenceBarrier&&) noexcept = default;

    virtual long waitFor(long sequence) = 0;
};

} // namespace swirly

#endif // SWIRLY_DISRUPT_SEQUENCEBARRIER_HPP
