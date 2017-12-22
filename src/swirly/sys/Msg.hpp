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
#ifndef SWIRLY_SYS_MSG_HPP
#define SWIRLY_SYS_MSG_HPP

#include <boost/intrusive_ptr.hpp>

#include <type_traits>

#define SWIRLY_MSG                                                                                 \
    static void* operator new(std::size_t size) { return alloc(size); }                            \
    static void operator delete(void* ptr, std::size_t size) noexcept                              \
    {                                                                                              \
        return dealloc(ptr, size);                                                                 \
    }                                                                                              \
    operator MsgTag() { return {}; }                                                               \
    mutable int refs;

namespace swirly {

struct MsgTag {
};

template <typename MsgT, typename = std::enable_if_t<std::is_convertible<MsgT, MsgTag>::value>>
inline int refCount(const MsgT& msg) noexcept
{
    return __atomic_load_n(&msg.refs, __ATOMIC_RELAXED);
}

template <typename MsgT, typename = std::enable_if_t<std::is_convertible<MsgT, MsgTag>::value>>
inline void intrusive_ptr_add_ref(const MsgT* msg) noexcept
{
    __atomic_fetch_add(&msg->refs, 1, __ATOMIC_RELAXED);
}

template <typename MsgT, typename = std::enable_if_t<std::is_convertible<MsgT, MsgTag>::value>>
inline void intrusive_ptr_release(const MsgT* msg) noexcept
{
    if (__atomic_fetch_sub(&msg->refs, 1, __ATOMIC_RELEASE) == 1) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        delete msg;
    }
}

template <typename MsgT, typename = std::enable_if_t<std::is_convertible<MsgT, MsgTag>::value>>
boost::intrusive_ptr<MsgT> makeMsg()
{
    auto* const msg = new MsgT;
    msg->refs = 1;
    return {msg, false};
}

} // namespace swirly

#endif // SWIRLY_SYS_MSG_HPP
