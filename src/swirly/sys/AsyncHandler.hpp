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
#ifndef SWIRLY_SYS_ASYNCHANDLER_HPP
#define SWIRLY_SYS_ASYNCHANDLER_HPP

#include <swirly/sys/RefCount.hpp>
#include <swirly/sys/Time.hpp>
#include <swirly/sys/Types.hpp>

namespace swirly {

class Reactor;
class Timer;

class SWIRLY_API AsyncHandler : public RefCount<AsyncHandler, ThreadUnsafePolicy> {
  public:
    explicit AsyncHandler(Reactor& reactor) noexcept : reactor_(reactor) {}
    virtual ~AsyncHandler() noexcept;

    // Copy.
    AsyncHandler(const AsyncHandler&) noexcept = delete;
    AsyncHandler& operator=(const AsyncHandler&) noexcept = delete;

    // Move.
    AsyncHandler(AsyncHandler&&) noexcept = delete;
    AsyncHandler& operator=(AsyncHandler&&) noexcept = delete;

    void event(int fd, EventMask events, Time now) { onEvent(fd, events, now); }
    void timer(const Timer& tmr, Time now) { onTimer(tmr, now); }

  protected:
    virtual void onEvent(int fd, EventMask events, Time now) = 0;
    virtual void onTimer(const Timer& tmr, Time now) = 0;

    const Reactor& reactor() const noexcept { return reactor_; }

    Reactor& reactor() noexcept { return reactor_; }

  private:
    Reactor& reactor_;
};

using AsyncHandlerPtr = boost::intrusive_ptr<AsyncHandler>;

} // namespace swirly

#endif // SWIRLY_SYS_ASYNCHANDLER_HPP
