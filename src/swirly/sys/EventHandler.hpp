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
#ifndef SWIRLY_SYS_EVENTHANDLER_HPP
#define SWIRLY_SYS_EVENTHANDLER_HPP

#include <swirly/sys/Event.hpp>
#include <swirly/sys/RefCount.hpp>
#include <swirly/sys/Time.hpp>
#include <swirly/sys/Types.hpp>

namespace swirly {

class Reactor;
class Timer;

class SWIRLY_API EventHandler : public RefCount<EventHandler, ThreadUnsafePolicy> {
  public:
    explicit EventHandler(Reactor& reactor) noexcept
      : reactor_(reactor)
    {
    }
    virtual ~EventHandler() noexcept;

    // Copy.
    EventHandler(const EventHandler&) noexcept = delete;
    EventHandler& operator=(const EventHandler&) noexcept = delete;

    // Move.
    EventHandler(EventHandler&&) noexcept = delete;
    EventHandler& operator=(EventHandler&&) noexcept = delete;

    void close() { doClose(); }

    void onEvent(const Event& event, Time now) { doEvent(event, now); }
    void onReady(int fd, FileEvents mask, Time now) { doReady(fd, mask, now); }
    void onSignal(int sig, Time now) { doSignal(sig, now); }
    void onTimer(const Timer& tmr, Time now) { doTimer(tmr, now); }

  protected:
    virtual void doClose() = 0;
    virtual void doEvent(const Event& event, Time now);
    virtual void doReady(int fd, FileEvents mask, Time now);
    /**
     * The default doSignal() implementation calls doClose() on SIGINT or SIGTERM.
     */
    virtual void doSignal(int sig, Time now);
    virtual void doTimer(const Timer& tmr, Time now);

    const Reactor& reactor() const noexcept { return reactor_; }
    boost::intrusive_ptr<EventHandler> self() noexcept { return {this, true}; }

    Reactor& reactor() noexcept { return reactor_; }

  private:
    Reactor& reactor_;
};

using EventHandlerPtr = boost::intrusive_ptr<EventHandler>;

} // namespace swirly

#endif // SWIRLY_SYS_EVENTHANDLER_HPP
