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
#include "Signal.hpp"

#include <swirly/sys/Error.hpp>

namespace swirly {
inline namespace sys {

SigWait::SigWait(std::initializer_list<int> mask)
{
    sigemptyset(&new_mask_);
    for (auto sig : mask) {
        sigaddset(&new_mask_, sig);
    }

    const auto err = pthread_sigmask(SIG_BLOCK, &new_mask_, &old_mask_);
    if (err != 0) {
        throw std::system_error{os::make_error(err), "pthread_sigmask"};
    }
}

SigWait::~SigWait()
{
    // Restore original signal mask.
    pthread_sigmask(SIG_SETMASK, &old_mask_, nullptr);
}

int SigWait::operator()() const
{
    siginfo_t info;
    if (sigwaitinfo(&new_mask_, &info) < 0) {
        throw std::system_error{os::make_error(errno), "sigwaitinfo"};
    }
    return info.si_signo;
}

int SigWait::operator()(Millis timeout) const
{
    siginfo_t info;
    const auto ms = timeout.count();
    const timespec ts{ms / 1'000L, (ms % 1'000L) * 1'000'000L};
    if (sigtimedwait(&new_mask_, &info, &ts) < 0) {
        if (errno == EAGAIN) {
            // Timeout.
            return 0;
        }
        throw std::system_error{os::make_error(errno), "sigtimedwait"};
    }
    return info.si_signo;
}

void sig_block_all()
{
    sigset_t ss;
    sigemptyset(&ss);
    const auto err = pthread_sigmask(SIG_SETMASK, &ss, nullptr);
    if (err != 0) {
        throw std::system_error{os::make_error(err), "pthread_sigmask"};
    }
}

} // namespace sys
} // namespace swirly
