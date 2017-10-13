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
#include "Muxer.hpp"

#include <algorithm>
#include <cassert>

namespace swirly {

void PollPolicy::insert(Impl* md, int fd, Event event)
{
    auto& pfds = md->pfds;

    const pollfd pfd{fd, static_cast<short>(event.events), 0};
    const auto it = std::lower_bound(pfds.begin(), pfds.end(), pfd, Cmp{});
    if (it != pfds.end() && it->fd == fd) {
        throw std::system_error{sys::error(EEXIST), "fd already registered"};
    }
    auto& data = md->data;
    const auto jt = data.begin() + distance(pfds.begin(), it);

    pfds.insert(it, pfd);
    data.insert(jt, event.data);
}

void PollPolicy::update(Impl* md, int fd, Event event)
{
    auto& pfds = md->pfds;

    const pollfd pfd{fd, 0, 0};
    const auto it = std::lower_bound(pfds.begin(), pfds.end(), pfd, Cmp{});
    if (it == pfds.end() || it->fd != fd) {
        throw std::system_error{sys::error(ENOENT), "fd is not registered"};
    }
    auto& data = md->data;
    const auto jt = data.begin() + distance(pfds.begin(), it);

    it->events = event.events;
    *jt = event.data;
}

void PollPolicy::erase(Impl* md, int fd)
{
    auto& pfds = md->pfds;

    const pollfd pfd{fd, 0, 0};
    const auto it = std::lower_bound(pfds.begin(), pfds.end(), pfd, Cmp{});
    if (it == pfds.end() || it->fd != fd) {
        throw std::system_error{sys::error(ENOENT), "fd is not registered"};
    }
    auto& data = md->data;
    const auto jt = data.begin() + distance(pfds.begin(), it);

    pfds.erase(it);
    data.erase(jt);
}

int PollPolicy::wait(Impl* md, Event* buf, std::size_t size, int timeout)
{
    assert(buf && size > 0);
    auto& pfds = md->pfds;

    int n{sys::poll(&pfds[0], pfds.size(), timeout)};
    n = std::min<int>(n, size);

    const auto* end = buf + n;
    for (std::size_t i{0}; buf != end && i < pfds.size(); ++i) {
        const auto& pfd = pfds[i];
        if (pfd.revents) {
            buf->events = pfd.revents;
            buf->data = md->data[i];
            ++buf;
        }
    }
    return n;
}

} // namespace swirly
