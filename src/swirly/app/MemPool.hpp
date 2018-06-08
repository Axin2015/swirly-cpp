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
#ifndef SWIRLY_APP_MEMPOOL_HPP
#define SWIRLY_APP_MEMPOOL_HPP

#include <swirly/sys/Memory.hpp>

#include <swirly/Config.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <new>
#include <type_traits>

namespace swirly {
inline namespace app {

/**
 * Size type used for 32bit index or offset into shared memory segment.
 */
using MemSize = std::uint32_t;

/**
 * Tag type used as work-around for ABA problem.
 */
using MemTag = std::uint32_t;

/**
 * Link type comprising a tag in the upper 32bits and an index or offset in the lower 32 bits. Links
 * are used to represent "offset pointers"
 */
using MemLink = std::uint64_t;

constexpr MemSize link_to_offset(MemLink link) noexcept
{
    return link & 0xffffffff;
}

constexpr MemLink make_link(MemTag tag, MemSize offset) noexcept
{
    return (static_cast<MemLink>(tag) << 32) | offset;
}

template <std::size_t SizeN>
struct SWIRLY_PACKED MemNode {
    union {
        MemLink next;
        char storage[SizeN];
    };
};
static_assert(std::is_pod_v<MemNode<1>>);
static_assert(sizeof(MemNode<256>) == 256);

template <std::size_t SizeN>
struct alignas(CacheLineSize) MemStack {
    struct SWIRLY_PACKED {
        MemLink head;
        MemTag tag;
    };
};
static_assert(std::is_pod_v<MemStack<1>>);
static_assert(sizeof(MemStack<1>) == CacheLineSize);

struct MemPool {
    union {
        char pad[PageSize];
        struct {
            MemStack<(1 << 6)> free1;
            MemStack<(2 << 6)> free2;
            MemStack<(3 << 6)> free3;
            MemStack<(4 << 6)> free4;
            MemStack<(8 << 6)> free8;
            MemStack<(16 << 6)> free16;
            MemStack<(32 << 6)> free32;
            MemSize offset;
        };
    };
    char storage[];
};
static_assert(std::is_pod_v<MemPool>);
static_assert(offsetof(MemPool, storage) == PageSize);

constexpr void* offset_to_ptr(const MemPool& pool, MemSize offset) noexcept
{
    return const_cast<char*>(&pool.storage[offset]);
}

template <typename PtrT>
constexpr PtrT offset_to_ptr(const MemPool& pool, MemSize offset) noexcept
{
    return static_cast<PtrT>(offset_to_ptr(pool, offset));
}

inline MemSize ptr_to_offset(const MemPool& pool, void* ptr) noexcept
{
    assert(ptr >= &pool.storage[0]);
    return static_cast<char*>(ptr) - &pool.storage[0];
}

template <std::size_t SizeN>
inline void push(const MemPool& pool, MemStack<SizeN>& stack, MemNode<SizeN>* node)
{
    // Add then fetch.
    const auto tag = __atomic_add_fetch(&stack.tag, 1, __ATOMIC_RELAXED);
    const auto new_head = make_link(tag, ptr_to_offset(pool, node));

    decltype(stack.head) old_head;
    __atomic_load(&stack.head, &old_head, __ATOMIC_RELAXED);
    do {
        node->next = old_head;
    } while (!__atomic_compare_exchange_n(&stack.head,
                                          &old_head,        // Expected.
                                          new_head,         // Desired.
                                          1,                // Weak.
                                          __ATOMIC_RELEASE, // Success.
                                          __ATOMIC_RELAXED  // Failure.
                                          ));
}

template <std::size_t SizeN>
inline MemNode<SizeN>* pop(const MemPool& pool, MemStack<SizeN>& stack)
{
    MemNode<SizeN>* node;

    decltype(stack.head) old_head;
    __atomic_load(&stack.head, &old_head, __ATOMIC_ACQUIRE);
    do {
        if (old_head == 0) {
            return nullptr;
        }
        node = offset_to_ptr<decltype(node)>(pool, link_to_offset(old_head));
    } while (!__atomic_compare_exchange_n(&stack.head,
                                          &old_head,        // Expected.
                                          node->next,       // Desired.
                                          1,                // Weak.
                                          __ATOMIC_ACQUIRE, // Success.
                                          __ATOMIC_ACQUIRE  // Failure.
                                          ));
    return node;
}

inline MemSize reserve(MemPool& pool, MemSize size, MemSize max_size)
{
    MemSize new_offset, old_offset;
    __atomic_load(&pool.offset, &old_offset, __ATOMIC_RELAXED);
    do {
        new_offset = old_offset + size;
        if (new_offset > max_size) {
            throw std::bad_alloc{};
        }
    } while (!__atomic_compare_exchange_n(&pool.offset,
                                          &old_offset,      // Expected.
                                          new_offset,       // Desired.
                                          1,                // Weak.
                                          __ATOMIC_RELAXED, // Success.
                                          __ATOMIC_RELAXED  // Failure.
                                          ));
    return old_offset;
}

template <std::size_t SizeN>
inline void* alloc_block(MemPool& pool, MemStack<SizeN>& stack, MemSize max_size)
{
    void* addr = pop(pool, stack);
    if (!addr) {
        const auto offset = reserve(pool, SizeN, max_size);
        addr = offset_to_ptr(pool, offset);
    }
    return addr;
}

template <std::size_t SizeN>
inline void dealloc_block(MemPool& pool, MemStack<SizeN>& stack, void* addr)
{
    push(pool, stack, static_cast<MemNode<SizeN>*>(addr));
}

} // namespace app
} // namespace swirly

#endif // SWIRLY_APP_MEMPOOL_HPP
