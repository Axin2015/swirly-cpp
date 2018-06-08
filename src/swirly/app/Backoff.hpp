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
#ifndef SWIRLY_APP_BACKOFF_HPP
#define SWIRLY_APP_BACKOFF_HPP

#include <swirly/Config.h>

namespace swirly {
inline namespace app {

struct NoBackoff {
    void idle() noexcept {}
    void reset() noexcept {}
};

class SWIRLY_API PhasedBackoff {
  public:
    void idle() noexcept;
    void reset() noexcept { i_ = 0; }

  private:
    int i_{0};
};

struct SWIRLY_API YieldBackoff {
    void idle() noexcept;
    void reset() noexcept {}
};

inline void cpu_relax() noexcept
{
#if defined(__x86_64__) || defined(__i386__)
    asm volatile("pause" ::: "memory");
#elif defined(__arm__) || defined(__arm64__)
    asm volatile("yield" ::: "memory");
#endif
}

} // namespace app
} // namespace swirly

#endif // SWIRLY_APP_BACKOFF_HPP
