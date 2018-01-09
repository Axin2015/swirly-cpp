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
#ifndef SWIRLY_DISRUPT_DISRUPTOR_HPP
#define SWIRLY_DISRUPT_DISRUPTOR_HPP

#include "RingBuffer.hpp"
#include "Types.hpp"

namespace swirly {

class Executor;

class SWIRLY_API Disruptor {
  public:
    Disruptor(int ringBufferSize, Executor& executor) noexcept
      : ringBuffer_{}
      , executor_(executor)
    {
    }
    virtual ~Disruptor() noexcept;

    // Copy.
    Disruptor(const Disruptor&) noexcept = default;
    Disruptor& operator=(const Disruptor&) noexcept = default;

    // Move.
    Disruptor(Disruptor&&) noexcept = default;
    Disruptor& operator=(Disruptor&&) noexcept = default;

    RingBuffer& ringBuffer() noexcept { return ringBuffer_; }

    void handleEventsWith(const EventHandlerPtr& handler)
    {
        // FIXME.
    }

    void start()
    {
        // FIXME.
    }

  private:
    RingBuffer ringBuffer_;
    Executor& executor_;
};

} // namespace swirly

#endif // SWIRLY_DISRUPT_DISRUPTOR_HPP
