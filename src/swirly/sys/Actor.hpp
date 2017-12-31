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
#ifndef SWIRLY_SYS_ACTOR_HPP
#define SWIRLY_SYS_ACTOR_HPP

#include <swirly/sys/Event.hpp>
#include <swirly/sys/RefCount.hpp>
#include <swirly/sys/Time.hpp>
#include <swirly/sys/Types.hpp>

namespace swirly {

class Reactor;
class Timer;

class SWIRLY_API Actor : public RefCount<Actor, ThreadUnsafePolicy> {
  public:
    explicit Actor(Reactor& reactor) noexcept
      : reactor_(reactor)
    {
    }
    virtual ~Actor() noexcept;

    // Copy.
    Actor(const Actor&) noexcept = delete;
    Actor& operator=(const Actor&) noexcept = delete;

    // Move.
    Actor(Actor&&) noexcept = delete;
    Actor& operator=(Actor&&) noexcept = delete;

    void onEvent(const Event& event) { doEvent(event); }
    void onReady(int fd, FileEvents mask, Time now) { doReady(fd, mask, now); }
    void onSignal(int sig) { doSignal(sig); }
    void onTimer(const Timer& tmr, Time now) { doTimer(tmr, now); }

  protected:
    virtual void doEvent(const Event& event);
    virtual void doReady(int fd, FileEvents mask, Time now);
    virtual void doSignal(int sig);
    virtual void doTimer(const Timer& tmr, Time now);

    const Reactor& reactor() const noexcept { return reactor_; }

    Reactor& reactor() noexcept { return reactor_; }

  private:
    Reactor& reactor_;
};

using ActorPtr = boost::intrusive_ptr<Actor>;

} // namespace swirly

#endif // SWIRLY_SYS_ACTOR_HPP
