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
#ifndef SWIRLY_SYS_EVENT_HPP
#define SWIRLY_SYS_EVENT_HPP

#include <swirly/sys/MemAlloc.hpp>

#include <boost/intrusive_ptr.hpp>

namespace swirly {

class Event {
    struct Impl {
        mutable int refs;
        int type, size;
        char data[];
    };
    static_assert(sizeof(Impl) == 12);
    friend inline void intrusive_ptr_add_ref(const Impl* impl) noexcept
    {
        __atomic_fetch_add(&impl->refs, 1, __ATOMIC_RELAXED);
    }
    friend inline void intrusive_ptr_release(const Impl* impl) noexcept
    {
        if (__atomic_fetch_sub(&impl->refs, 1, __ATOMIC_RELEASE) == 1) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            dealloc(const_cast<Impl*>(impl), sizeof(Impl) + impl->size);
        }
    }
    explicit Event(boost::intrusive_ptr<Impl>&& impl)
      : impl_{std::move(impl)}
    {
    }

  public:
    Event() = default;
    ~Event() noexcept = default;

    // Copy.
    Event(const Event&) = default;
    Event& operator=(const Event&) = default;

    // Move.
    Event(Event&&) = default;
    Event& operator=(Event&&) = default;

    static Event alloc(int type, std::size_t size)
    {
        auto* const impl = static_cast<Impl*>(swirly::alloc(sizeof(Impl) + size));
        impl->refs = 1;
        impl->type = type;
        impl->size = size;
        return Event{boost::intrusive_ptr<Impl>{impl, false}};
    }
    explicit operator bool() const noexcept { return static_cast<bool>(impl_); }

    void reset() noexcept { impl_.reset(); }

    void swap(Event& rhs) noexcept { std::swap(impl_, rhs.impl_); }

    template <typename DataT>
    inline const DataT& data() const noexcept
    {
        return *reinterpret_cast<const DataT*>(impl_->data);
    }
    template <typename DataT>
    inline DataT& data() noexcept
    {
        return *reinterpret_cast<DataT*>(impl_->data);
    }

  private:
    boost::intrusive_ptr<Impl> impl_;
};

} // namespace swirly

#endif // SWIRLY_SYS_EVENT_HPP
