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
#ifndef SWIRLY_SYS_SIGNAL_HPP
#define SWIRLY_SYS_SIGNAL_HPP

#include <swirly/Config.h>

#include <signal.h>

namespace swirly {

class SWIRLY_API SigWait {
  public:
    SigWait();
    ~SigWait() noexcept;

    // Copy.
    SigWait(const SigWait&) = delete;
    SigWait& operator=(const SigWait&) = delete;

    // Move.
    SigWait(SigWait&&) = delete;
    SigWait& operator=(SigWait&&) = delete;

    int operator()() const;

  private:
    sigset_t newMask_, oldMask_;
};

} // namespace swirly

#endif // SWIRLY_SYS_SIGNAL_HPP
