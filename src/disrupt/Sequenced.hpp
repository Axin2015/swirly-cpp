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
#ifndef SWIRLY_DISRUPT_SEQUENCED_HPP
#define SWIRLY_DISRUPT_SEQUENCED_HPP

#include <swirly/Config.h>

namespace swirly {

class Sequence;

class SWIRLY_API Sequenced {
  public:
    Sequenced() noexcept = default;
    virtual ~Sequenced() noexcept;

    // Copy.
    Sequenced(const Sequenced&) noexcept = default;
    Sequenced& operator=(const Sequenced&) noexcept = default;

    // Move.
    Sequenced(Sequenced&&) noexcept = default;
    Sequenced& operator=(Sequenced&&) noexcept = default;

    virtual int getBufferSize() = 0;
    virtual bool hasAvailableCapacity(int requiredCapacity) = 0;
    virtual long remainingCapacity() = 0;
    virtual long next() = 0;
    virtual long next(int n) = 0;
    virtual long tryNext() = 0;
    virtual long tryNext(int n) = 0;
    virtual void publish(long sequence) = 0;
    virtual void publish(long lo, long hi) = 0;
};

} // namespace swirly

#endif // SWIRLY_DISRUPT_SEQUENCED_HPP
