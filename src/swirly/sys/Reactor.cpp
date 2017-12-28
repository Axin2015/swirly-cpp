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
#include "Reactor.hpp"

#include "EventFile.hpp"
#include "EventQueue.hpp"
#include "Log.hpp"

namespace swirly {
using namespace std;
namespace {
constexpr size_t MaxEvents{16};
} // namespace

struct Reactor::Impl {

    explicit Impl(size_t sizeHint)
      : mux{sizeHint}
    {
        const auto fd = ef.waitfd();
        data.resize(max<size_t>(fd + 1, sizeHint));

        auto& ref = data[fd];
        mux.attach(0, fd, In);
        ref.mask = In;
        ref.actor = {};
    }

    Muxer mux;
    EventFile ef;
    EventQueue eq;
    TimerQueue tq;
    vector<Data> data;
    bool quit{false};
};

Reactor::Reactor(size_t sizeHint)
  : impl_{make_unique<Impl>(sizeHint)}
{
}

Reactor::~Reactor() noexcept = default;

// Move.
Reactor::Reactor(Reactor&&) = default;
Reactor& Reactor::operator=(Reactor&&) = default;

bool Reactor::quit() const noexcept
{
    return impl_->quit;
}

Token Reactor::attach(int fd, FileEvents mask, const ActorPtr& actor)
{
    assert(fd >= 0);
    assert(actor);
    if (fd >= static_cast<int>(impl_->data.size())) {
        impl_->data.resize(fd + 1);
    }
    auto& ref = impl_->data[fd];
    impl_->mux.attach(++ref.sid, fd, mask);
    ref.mask = mask;
    ref.actor = actor;
    return Token{{this, fd}};
}

void Reactor::mask(int fd, FileEvents mask)
{
    if (impl_->data[fd].mask != mask) {
        impl_->mux.setMask(fd, mask);
        impl_->data[fd].mask = mask;
    }
}

void Reactor::detach(int fd) noexcept
{
    impl_->mux.detach(fd);
    auto& ref = impl_->data[fd];
    ref.mask = 0;
    ref.actor.reset();
}

Timer Reactor::timer(Time expiry, Duration interval, const ActorPtr& actor)
{
    return impl_->tq.insert(expiry, interval, actor);
}

Timer Reactor::timer(Time expiry, const ActorPtr& actor)
{
    return impl_->tq.insert(expiry, actor);
}

int Reactor::poll(chrono::milliseconds timeout)
{
    using namespace chrono;

    if (!impl_->tq.empty()) {
        // Millis until next expiry.
        const auto expiry
            = duration_cast<milliseconds>(impl_->tq.front().expiry() - UnixClock::now());
        if (expiry < timeout) {
            timeout = max(expiry, 0ms);
        }
    }
    FileEvent buf[MaxEvents];
    error_code ec;
    const auto ret = impl_->mux.wait(buf, MaxEvents, timeout, ec);
    if (ret < 0) {
        if (ec.value() != EINTR) {
            throw system_error{ec};
        }
        return 0;
    }
    const auto now = UnixClock::now();
    return impl_->tq.dispatch(now) + dispatch(buf, ret, now);
}

void Reactor::post(const Event& ev)
{
    if (impl_->eq.push(ev) == 1) {
        impl_->ef.notify();
    }
}

void Reactor::post(Event&& ev)
{
    if (impl_->eq.push(move(ev)) == 1) {
        impl_->ef.notify();
    }
}

int Reactor::dispatch(FileEvent* buf, int size, Time now)
{
    int n{};
    for (int i{0}; i < size; ++i) {

        auto& ev = buf[i];
        const auto fd = static_cast<int>(ev.data.u64 & 0xffffffff);
        if (fd == impl_->ef.waitfd()) {
            while (auto event = impl_->eq.pop()) {
                // TODO: dispatch events to Actors.
                impl_->quit = true;
                ++n;
            }
            continue;
        }

        const auto sid = static_cast<int>(ev.data.u64 >> 32);

        const auto& ref = impl_->data[fd];
        // Skip this socket if it was modified after the call to wait().
        if (ref.sid > sid) {
            continue;
        }
        // Apply the interest mask to filter-out any events that the user may have removed from the
        // mask since the call to wait() was made. This would typically happen via a reentrant call
        // into the reactor from an actor.
        ev.events &= ref.mask;
        if (!(ev.events)) {
            continue;
        }

        ActorPtr actor{ref.actor};
        try {
            actor->ready(fd, ev.events, now);
        } catch (const std::exception& e) {
            using namespace string_literals;
            SWIRLY_ERROR("error handling io event: "s + e.what());
        }
        ++n;
    }
    return n;
}

} // namespace swirly
