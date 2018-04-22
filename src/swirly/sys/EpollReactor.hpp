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
#ifndef SWIRLY_SYS_EPOLLREACTOR_HPP
#define SWIRLY_SYS_EPOLLREACTOR_HPP

#include "Reactor.hpp"

#include <swirly/sys/Muxer.hpp>

namespace swirly {
inline namespace sys {

class SWIRLY_API EpollReactor : public Reactor {
  public:
    using Event = typename EpollMuxer::Event;

    explicit EpollReactor(std::size_t sizeHint = 1024);
    ~EpollReactor() override;

    // Copy.
    EpollReactor(const EpollReactor&) = delete;
    EpollReactor& operator=(const EpollReactor&) = delete;

    // Move.
    EpollReactor(EpollReactor&&) = delete;
    EpollReactor& operator=(EpollReactor&&) = delete;

  protected:
    int doInterrupted() const noexcept override;

    /**
     * Thread-safe.
     */
    void doInterrupt(int num) noexcept override;

    Handle doSubscribe(int fd, unsigned events, IoSlot slot) override;
    void doUnsubscribe(int fd, int sid) noexcept override;

    void doSetEvents(int fd, int sid, unsigned events, IoSlot slot) override;
    void doSetEvents(int fd, int sid, unsigned events) override;

    Timer doTimer(Time expiry, Duration interval, Priority priority, TimerSlot slot) override;
    Timer doTimer(Time expiry, Priority priority, TimerSlot slot) override;

    int doPoll(Time now, Millis timeout) override;

  private:
    int dispatch(Event* buf, int size, Time now);

    struct Data {
        int sid{};
        unsigned events{};
        IoSlot slot;
    };
    EpollMuxer mux_;
    std::vector<Data> data_;
    EventFd efd_{0, EFD_NONBLOCK};
    static_assert(static_cast<int>(Priority::High) == 0);
    static_assert(static_cast<int>(Priority::Low) == 1);
    std::array<TimerQueue, 2> tqs_;
    int interrupt_{0};
};

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_EPOLLREACTOR_HPP
