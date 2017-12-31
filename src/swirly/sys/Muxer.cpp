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
#include "Muxer.hpp"

#include <algorithm>
#include <cassert>

namespace swirly {

using namespace std;

void PollPolicy::subscribe(Impl* md, int sid, int fd, FileEvents mask)
{
    auto& pfds = md->pfds;

    const pollfd pfd{fd, static_cast<short>(mask), 0};
    const auto it = lower_bound(pfds.begin(), pfds.end(), pfd, Cmp{});
    if (it != pfds.end() && it->fd == fd) {
        throw system_error{sys::makeError(EEXIST)};
    }
    auto& sids = md->sids;
    const auto jt = sids.begin() + distance(pfds.begin(), it);

    pfds.insert(it, pfd);
    sids.insert(jt, sid);
}

void PollPolicy::unsubscribe(Impl* md, int fd) noexcept
{
    auto& pfds = md->pfds;

    const pollfd pfd{fd, 0, 0};
    const auto it = lower_bound(pfds.begin(), pfds.end(), pfd, Cmp{});
    if (it != pfds.end() && it->fd == fd) {

        auto& sids = md->sids;
        const auto jt = sids.begin() + distance(pfds.begin(), it);

        pfds.erase(it);
        sids.erase(jt);
    }
}

void PollPolicy::setMask(Impl* md, int fd, FileEvents mask)
{
    auto& pfds = md->pfds;

    const pollfd pfd{fd, 0, 0};
    const auto it = lower_bound(pfds.begin(), pfds.end(), pfd, Cmp{});
    if (it == pfds.end() || it->fd != fd) {
        throw system_error{sys::makeError(ENOENT)};
    }
    it->events = mask;
}

int PollPolicy::wait(Impl* md, FileEvent* buf, size_t size, int timeout, error_code& ec)
{
    assert(buf && size > 0);
    auto& pfds = md->pfds;
    auto& sids = md->sids;

    int ret{sys::poll(&pfds[0], pfds.size(), timeout, ec)};
    if (ret > 0) {
        ret = min<int>(ret, size);
        const auto* end = buf + ret;
        for (size_t i{0}; buf != end && i < pfds.size(); ++i) {
            const auto& pfd = pfds[i];
            const auto sid = sids[i];
            if (pfd.revents) {
                buf->events = pfd.revents;
                buf->data.u64 = static_cast<uint64_t>(sid) << 32 | pfd.fd;
                ++buf;
            }
        }
    }
    return ret;
}

} // namespace swirly
