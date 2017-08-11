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
#include "MemPool.hpp"

#include <cassert>
#include <system_error>

#include <sys/mman.h>

using namespace std;

namespace swirly {
namespace detail {

MemRegion::MemRegion(size_t capacity) : capacity_{capacity}
{
    // The mmap system call is used instead of sbrk, because it allows us to free the memory.
    const int prot{PROT_READ | PROT_WRITE};
    const int flags{MAP_ANON | MAP_PRIVATE};
    addr_ = mmap(nullptr, capacity, prot, flags, -1, 0);
    if (addr_ == MAP_FAILED) {
        throw system_error{errno, system_category(), "mmap failed"};
    }
}

MemRegion::MemRegion() noexcept = default;

MemRegion::~MemRegion() noexcept
{
    if (addr_) {
        munmap(addr_, capacity_);
    }
}

// Move.
MemRegion::MemRegion(MemRegion&& rhs) noexcept
    : capacity_{rhs.capacity_}, addr_{rhs.addr_}, used_{rhs.used_}
{
    rhs.capacity_ = 0;
    rhs.addr_ = nullptr;
    rhs.used_ = 0;
}

MemRegion& MemRegion::operator=(MemRegion&& rhs) noexcept
{
    assert(this != &rhs);
    if (addr_) {
        munmap(addr_, capacity_);
    }
    capacity_ = rhs.capacity_;
    addr_ = rhs.addr_;
    used_ = rhs.used_;
    rhs.capacity_ = 0;
    rhs.addr_ = nullptr;
    rhs.used_ = 0;
    return *this;
}

} // detail

MemPool::MemPool(size_t capacity) : region_{capacity}
{
}

MemPool::MemPool() noexcept = default;
MemPool::~MemPool() noexcept = default;

// Move.
MemPool::MemPool(MemPool&&) noexcept = default;
MemPool& MemPool::operator=(MemPool&&) noexcept = default;

void* MemPool::alloc(size_t size)
{
    void* addr;
    switch (ceilCacheLine(size)) {
    case 1 << 6:
        addr = allocBlock(head1_);
        break;
    case 2 << 6:
        addr = allocBlock(head2_);
        break;
    case 3 << 6:
        addr = allocBlock(head3_);
        break;
    case 4 << 6:
        addr = allocBlock(head4_);
        break;
    case 5 << 6:
        addr = allocBlock(head5_);
        break;
    default:
        addr = malloc(size);
    }
    return addr;
}

void MemPool::dealloc(void* ptr, size_t size) noexcept
{
    switch (ceilCacheLine(size)) {
    case 1 << 6:
        deallocBlock(head1_, ptr);
        break;
    case 2 << 6:
        deallocBlock(head2_, ptr);
        break;
    case 3 << 6:
        deallocBlock(head3_, ptr);
        break;
    case 4 << 6:
        deallocBlock(head4_, ptr);
        break;
    case 5 << 6:
        deallocBlock(head5_, ptr);
        break;
    default:
        free(ptr);
    }
}

} // swirly
