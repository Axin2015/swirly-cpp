/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#include <swirly/fig/MemPool.hpp>

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
  if (size == sizeof(Exec)) {
    addr = allocBlock(exec_);
  } else if (size == sizeof(Order)) {
    addr = allocBlock(order_);
  } else if (size == sizeof(Level)) {
    addr = allocBlock(level_);
  } else {
    addr = malloc(size);
  }
  return addr;
}

void MemPool::dealloc(void* ptr, size_t size) noexcept
{
  if (size == sizeof(Exec)) {
    deallocBlock(exec_, ptr);
  } else if (size == sizeof(Order)) {
    deallocBlock(order_, ptr);
  } else if (size == sizeof(Level)) {
    deallocBlock(level_, ptr);
  } else {
    free(ptr);
  }
}

} // swirly
