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
#ifndef SWIRLY_SYS_MEMPOOL_HPP
#define SWIRLY_SYS_MEMPOOL_HPP

#include <swirly/sys/Math.hpp>

#include <swirly/Config.h>

#include <cassert>
#include <cstddef>
#include <new>
#include <type_traits>

namespace swirly {

// Assumptions:
// sysconf(_SC_LEVEL1_DCACHE_LINESIZE) == 64
// sysconf(_SC_PAGESIZE) == 4096

enum : std::size_t {
    CacheLineBits = 6,
    CacheLineSize = 1 << CacheLineBits,
    PageBits = 12,
    PageSize = 1 << PageBits
};

constexpr std::size_t ceilCacheLine(std::size_t size) noexcept
{
    return ceilPow2<CacheLineBits>(size);
}

constexpr std::size_t ceilPage(std::size_t size) noexcept
{
    return ceilPow2<PageBits>(size);
}

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

constexpr MemSize linkToOffset(MemLink link) noexcept
{
    return link & 0xffffffff;
}

constexpr MemLink makeLink(MemTag tag, MemSize offset) noexcept
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
static_assert(std::is_pod<MemNode<1>>::value);
static_assert(sizeof(MemNode<256>) == 256);

template <std::size_t SizeN>
struct alignas(CacheLineSize) MemStack {
    struct SWIRLY_PACKED {
        MemLink head;
        MemTag tag;
    };
};
static_assert(std::is_pod<MemStack<1>>::value);
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
static_assert(std::is_pod<MemPool>::value);
static_assert(offsetof(MemPool, storage) == PageSize);

constexpr void* offsetToPtr(const MemPool& pool, MemSize offset) noexcept
{
    return const_cast<char*>(&pool.storage[offset]);
}

template <typename PtrT>
constexpr PtrT offsetToPtr(const MemPool& pool, MemSize offset) noexcept
{
    return static_cast<PtrT>(offsetToPtr(pool, offset));
}

inline MemSize ptrToOffset(const MemPool& pool, void* ptr) noexcept
{
    assert(ptr >= &pool.storage[0]);
    return static_cast<char*>(ptr) - &pool.storage[0];
}

template <std::size_t SizeN>
inline void push(const MemPool& pool, MemStack<SizeN>& stack, MemNode<SizeN>* node)
{
    // Add then fetch.
    const auto tag = __atomic_add_fetch(&stack.tag, 1, __ATOMIC_RELAXED);
    const auto newHead = makeLink(tag, ptrToOffset(pool, node));

    decltype(stack.head) oldHead;
    __atomic_load(&stack.head, &oldHead, __ATOMIC_RELAXED);
    do {
        node->next = oldHead;
    } while (!__atomic_compare_exchange_n(&stack.head,
                                          &oldHead, // Expected.
                                          newHead, // Desired.
                                          1, // Weak.
                                          __ATOMIC_RELEASE, // Success.
                                          __ATOMIC_RELAXED // Failure.
                                          ));
}

template <std::size_t SizeN>
inline MemNode<SizeN>* pop(const MemPool& pool, MemStack<SizeN>& stack)
{
    MemNode<SizeN>* node;

    decltype(stack.head) oldHead;
    __atomic_load(&stack.head, &oldHead, __ATOMIC_ACQUIRE);
    do {
        if (oldHead == 0) {
            return nullptr;
        }
        node = offsetToPtr<decltype(node)>(pool, linkToOffset(oldHead));
    } while (!__atomic_compare_exchange_n(&stack.head,
                                          &oldHead, // Expected.
                                          node->next, // Desired.
                                          1, // Weak.
                                          __ATOMIC_ACQUIRE, // Success.
                                          __ATOMIC_ACQUIRE // Failure.
                                          ));
    return node;
}

inline MemSize reserve(MemPool& pool, MemSize size, MemSize maxSize)
{
    MemSize newOffset, oldOffset;
    __atomic_load(&pool.offset, &oldOffset, __ATOMIC_RELAXED);
    do {
        newOffset = oldOffset + size;
        if (newOffset > maxSize) {
            throw std::bad_alloc{};
        }
    } while (!__atomic_compare_exchange_n(&pool.offset,
                                          &oldOffset, // Expected.
                                          newOffset, // Desired.
                                          1, // Weak.
                                          __ATOMIC_RELAXED, // Success.
                                          __ATOMIC_RELAXED // Failure.
                                          ));
    return oldOffset;
}

template <std::size_t SizeN>
inline void* allocBlock(MemPool& pool, MemStack<SizeN>& stack, MemSize maxSize)
{
    void* addr = pop(pool, stack);
    if (!addr) {
        const auto offset = reserve(pool, SizeN, maxSize);
        addr = offsetToPtr(pool, offset);
    }
    return addr;
}

template <std::size_t SizeN>
inline void deallocBlock(MemPool& pool, MemStack<SizeN>& stack, void* addr)
{
    push(pool, stack, static_cast<MemNode<SizeN>*>(addr));
}

} // namespace swirly

#endif // SWIRLY_SYS_MEMPOOL_HPP
