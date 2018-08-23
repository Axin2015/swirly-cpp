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
#ifndef SWIRLY_UTIL_REFCOUNT_HPP
#define SWIRLY_UTIL_REFCOUNT_HPP

#include <swirly/Config.h>

#include <boost/intrusive_ptr.hpp>

#include <atomic>

namespace swirly {
inline namespace util {

struct ThreadSafePolicy {
    using Type = std::atomic<int>;
    static void acquire() noexcept { std::atomic_thread_fence(std::memory_order_acquire); }
    static int fetch_add(Type& ref_count) noexcept
    {
        return ref_count.fetch_add(1, std::memory_order_relaxed);
    }
    static int fetch_sub(Type& ref_count) noexcept
    {
        return ref_count.fetch_sub(1, std::memory_order_release);
    }
    static int load(const Type& ref_count) noexcept
    {
        return ref_count.load(std::memory_order_relaxed);
    }
};

struct ThreadUnsafePolicy {
    using Type = int;
    static void acquire() noexcept {}
    static int fetch_add(Type& ref_count) noexcept { return ref_count++; }
    static int fetch_sub(Type& ref_count) noexcept { return ref_count--; }
    static int load(Type ref_count) noexcept { return ref_count; }
};

/**
 * Base class for atomic referenced counted objects.
 */
template <typename DerivedT, typename PolicyT>
class RefCount {
  public:
    constexpr RefCount() noexcept = default;
    ~RefCount() = default;

    // Copy.
    constexpr RefCount(const RefCount&) noexcept = default;
    RefCount& operator=(const RefCount&) noexcept = default;

    // Move.
    constexpr RefCount(RefCount&&) noexcept = default;
    RefCount& operator=(RefCount&&) noexcept = default;

    void add_ref() const noexcept { PolicyT::fetch_add(refs_); }
    void release() const noexcept
    {
        if (PolicyT::fetch_sub(refs_) == 1) {
            PolicyT::acquire();
            delete static_cast<const DerivedT*>(this);
        }
    }
    int ref_count() const noexcept { return PolicyT::load(refs_); }

  private:
    mutable typename PolicyT::Type refs_{1};
};

template <typename DerivedT, typename PolicyT>
void intrusive_ptr_add_ref(const RefCount<DerivedT, PolicyT>* ptr) noexcept
{
    ptr->add_ref();
}

template <typename DerivedT, typename PolicyT>
void intrusive_ptr_release(const RefCount<DerivedT, PolicyT>* ptr) noexcept
{
    ptr->release();
}

template <typename ValueT, typename... ArgsT>
boost::intrusive_ptr<ValueT> make_intrusive(ArgsT&&... args)
{
    return {new ValueT{std::forward<ArgsT>(args)...}, false};
}

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_REFCOUNT_HPP
