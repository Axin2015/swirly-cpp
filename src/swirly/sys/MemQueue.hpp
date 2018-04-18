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
#ifndef SWIRLY_SYS_MEMQUEUE_HPP
#define SWIRLY_SYS_MEMQUEUE_HPP

#include <swirly/sys/File.hpp>
#include <swirly/sys/MMap.hpp>
#include <swirly/sys/Memory.hpp>

#include <atomic>
#include <cassert>
#include <memory>

namespace swirly {
inline namespace sys {
namespace detail {
inline std::size_t fileSize(int fd)
{
    struct stat st;
    os::fstat(fd, st);
    return st.st_size;
}
} // namespace detail

// Thanks to Dmitry Vyukov for this algorithm.
template <typename ValueT>
class MemQueue {
    static_assert(std::is_trivially_copyable_v<ValueT>);

  public:
    struct alignas(CacheLineSize) Elem {
        std::int64_t seq;
        ValueT val;
    };
    static_assert(std::is_trivially_copyable_v<Elem>);
    struct alignas(CacheLineSize) Impl {
        // Ensure that read and write positions are in different cache-lines.
        std::int64_t rpos;
        alignas(CacheLineSize) std::int64_t wpos;
        alignas(CacheLineSize) Elem elems[];
    };
    static_assert(std::is_trivially_copyable_v<Impl>);
    static_assert(sizeof(Impl) == 2 * CacheLineSize);
    static_assert(offsetof(Impl, rpos) == 0 * CacheLineSize);
    static_assert(offsetof(Impl, wpos) == 1 * CacheLineSize);
    static_assert(offsetof(Impl, elems) == 2 * CacheLineSize);

    constexpr MemQueue(std::nullptr_t = nullptr) noexcept {}
    explicit MemQueue(std::size_t capacity)
    : capacity_{nextPow2(capacity)}
    , mask_{capacity_ - 1}
    , memMap_{os::mmap(nullptr, size(capacity_), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1,
                       0)}
    , impl_{static_cast<Impl*>(memMap_.get().data())}
    {
        assert(capacity >= 2);

        memset(impl_, 0, size(capacity_));
        // Initialise sequence numbers.
        for (std::int64_t i{0}; i < static_cast<std::int64_t>(capacity); ++i) {
            __atomic_store_n(&impl_->elems[i].seq, i, __ATOMIC_RELAXED);
        }
    }
    explicit MemQueue(const char* path)
    : fh_{os::open(path, O_RDWR)}
    , capacity_{capacity(detail::fileSize(fh_.get()))}
    , mask_{capacity_ - 1}
    , memMap_{os::mmap(nullptr, size(capacity_), PROT_READ | PROT_WRITE, MAP_SHARED, fh_.get(), 0)}
    , impl_{static_cast<Impl*>(memMap_.get().data())}
    {
        if (!isPow2(capacity_)) {
            throw std::runtime_error{"capacity not a power of two"};
        }
    }
    ~MemQueue() = default;

    // Copy.
    MemQueue(const MemQueue& rhs) = delete;
    MemQueue& operator=(const MemQueue& rhs) = delete;

    // Move.
    MemQueue(MemQueue&& rhs) noexcept
    : fh_{rhs.fh}
    , capacity_{rhs.capacity_}
    , mask_{rhs.mask_}
    , memMap_{std::move(rhs.memMap_)}
    , impl_{rhs.impl_}
    {
        capacity_ = 0;
        mask_ = 0;
        impl_ = nullptr;
    }
    MemQueue& operator=(MemQueue&& rhs) noexcept
    {
        reset();
        swap(rhs);
        return *this;
    }

    /**
     * Returns true if the queue is empty.
     */
    bool empty() const noexcept
    {
        // Acquire prevents reordering of these loads.
        const auto rpos = __atomic_load_n(&impl_->rpos, __ATOMIC_ACQUIRE);
        const auto wpos = __atomic_load_n(&impl_->wpos, __ATOMIC_RELAXED);
        return rpos == wpos;
    }
    /**
     * Returns the number of unused elements.
     */
    std::size_t reserve() const noexcept { return capacity_ - size(); }
    /**
     * Returns true if the number of elements in the queue.
     */
    std::size_t size() const noexcept
    {
        // Acquire prevents reordering of these loads.
        const auto rpos = __atomic_load_n(&impl_->rpos, __ATOMIC_ACQUIRE);
        const auto wpos = __atomic_load_n(&impl_->wpos, __ATOMIC_RELAXED);
        return wpos - rpos;
    }
    void reset(std::nullptr_t = nullptr) noexcept
    {
        // Reverse order.
        impl_ = nullptr;
        memMap_.reset(nullptr);
        mask_ = 0;
        capacity_ = 0;
        fh_.reset(nullptr);
    }
    void swap(MemQueue& rhs) noexcept
    {
        fh_.swap(rhs.fh_);
        std::swap(capacity_, rhs.capacity_);
        std::swap(mask_, rhs.mask_);
        memMap_.swap(rhs.memMap_);
        std::swap(impl_, rhs.impl_);
    }
    /**
     * Returns false if queue is empty.
     */
    template <typename FnT>
    bool fetch(FnT fn) noexcept
    {
        static_assert(std::is_nothrow_invocable_v<FnT, ValueT&&>);
        auto rpos = __atomic_load_n(&impl_->rpos, __ATOMIC_RELAXED);
        for (;;) {
            auto& elem = impl_->elems[rpos & mask_];
            const auto seq = __atomic_load_n(&elem.seq, __ATOMIC_ACQUIRE);
            const auto diff = seq - (rpos + 1);
            if (diff == 0) {
                // The compare_exchange_weak function re-reads wpos on failure.
                if (__atomic_compare_exchange_n(&impl_->rpos, &rpos, rpos + 1, true,
                                                __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
                    fn(std::move(elem.val));
                    // Commit.
                    __atomic_store_n(&elem.seq, rpos + capacity_, __ATOMIC_RELEASE);
                    break;
                }
            } else if (diff < 0) {
                return false;
            } else {
                rpos = __atomic_load_n(&impl_->rpos, __ATOMIC_RELAXED);
            }
        }
        return true;
    }
    /**
     * Returns false if capacity is exceeded.
     */
    template <typename FnT>
    bool post(FnT fn) noexcept
    {
        static_assert(std::is_nothrow_invocable_v<FnT, ValueT&>);
        auto wpos = __atomic_load_n(&impl_->wpos, __ATOMIC_RELAXED);
        for (;;) {
            auto& elem = impl_->elems[wpos & mask_];
            const auto seq = __atomic_load_n(&elem.seq, __ATOMIC_ACQUIRE);
            const auto diff = seq - wpos;
            if (diff == 0) {
                // The compare_exchange_weak function re-reads wpos on failure.
                if (__atomic_compare_exchange_n(&impl_->wpos, &wpos, wpos + 1, true,
                                                __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
                    fn(elem.val);
                    // Commit.
                    __atomic_store_n(&elem.seq, wpos + 1, __ATOMIC_RELEASE);
                    break;
                }
            } else if (diff < 0) {
                return false;
            } else {
                wpos = __atomic_load_n(&impl_->wpos, __ATOMIC_RELAXED);
            }
        }
        return true;
    }
    /**
     * Returns false if queue is empty.
     */
    bool pop(ValueT& val) noexcept
    {
        static_assert(std::is_nothrow_move_assignable_v<ValueT>);
        return fetch([&val](ValueT && ref) noexcept { val = std::move(ref); });
    }
    /**
     * Returns false if capacity is exceeded.
     */
    bool push(const ValueT& val) noexcept
    {
        static_assert(std::is_nothrow_copy_assignable_v<ValueT>);
        return post([&val](ValueT & ref) noexcept { ref = val; });
    }
    /**
     * Returns false if capacity is exceeded.
     */
    bool push(ValueT&& val) noexcept
    {
        static_assert(std::is_nothrow_move_assignable_v<ValueT>);
        return post([&val](ValueT & ref) noexcept { ref = std::move(val); });
    }

  private:
    static constexpr std::size_t capacity(std::size_t size) noexcept
    {
        return (size - sizeof(Impl)) / sizeof(Elem);
    }
    static constexpr std::size_t size(std::size_t capacity) noexcept
    {
        return sizeof(Impl) + capacity * sizeof(Elem);
    }

    FileHandle fh_{nullptr};
    std::uint64_t capacity_{}, mask_{};
    MMap memMap_{nullptr};
    Impl* impl_{nullptr};
};

/**
 * Initialise file-based MemQueue.
 */
template <typename ValueT>
void createMemQueue(const char* path, std::size_t capacity, mode_t mode)
{
    using Elem = typename MemQueue<ValueT>::Elem;
    using Impl = typename MemQueue<ValueT>::Impl;

    assert(capacity >= 2);

    capacity = nextPow2(capacity);
    const auto size = sizeof(Impl) + capacity * sizeof(Elem);

    FileHandle fh{os::open(path, O_RDWR | O_CREAT | O_EXCL, mode)};
    os::ftruncate(fh.get(), size);
    MMap memMap{os::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fh.get(), 0)};
    auto* const impl = static_cast<Impl*>(memMap.get().data());

    memset(impl, 0, size);
    // Initialise sequence numbers.
    for (std::int64_t i{0}; i < static_cast<std::int64_t>(capacity); ++i) {
        __atomic_store_n(impl->elems[i].seq, i, __ATOMIC_RELAXED);
    }
}

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_MEMQUEUE_HPP
