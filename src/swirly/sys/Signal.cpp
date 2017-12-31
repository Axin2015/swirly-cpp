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

SigWait::SigWait()
{
    sigemptyset(&newMask_);
    sigaddset(&newMask_, SIGHUP);
    sigaddset(&newMask_, SIGINT);
    sigaddset(&newMask_, SIGUSR1);
    sigaddset(&newMask_, SIGUSR2);
    sigaddset(&newMask_, SIGTERM);

    const auto err = pthread_sigmask(SIG_BLOCK, &newMask_, &oldMask_);
    if (err != 0) {
        throw std::system_error{sys::makeError(err), "pthread_sigmask"};
    }
}

SigWait::~SigWait() noexcept
{
    // Restore original signal mask.
    pthread_sigmask(SIG_SETMASK, &oldMask_, nullptr);
}

int SigWait::operator()() const
{
    int sig;
    const auto err = sigwait(&newMask_, &sig);
    if (err != 0) {
        throw std::system_error{sys::makeError(err), "sigwait"};
    }
    return sig;
}

} // namespace swirly
