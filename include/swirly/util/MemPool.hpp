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
#ifndef SWIRLY_UTIL_MEMPOOL_HPP
#define SWIRLY_UTIL_MEMPOOL_HPP

#include <swirly/util/Defs.hpp>

#include <cstddef> // size_t
#include <new>

namespace swirly {

namespace detail {

/**
 * Memory region. Used to reserve large block of virtual memory for pool allocator.
 */
class SWIRLY_API MemRegion {
 public:
  explicit MemRegion(std::size_t capacity);
  MemRegion() noexcept;
  ~MemRegion() noexcept;

  // Copy.
  MemRegion(const MemRegion& rhs) noexcept = delete;
  MemRegion& operator=(const MemRegion& rhs) noexcept = delete;

  // Move.
  MemRegion(MemRegion&& rhs) noexcept;
  MemRegion& operator=(MemRegion&& rhs) noexcept;

  auto capacity() const noexcept { return capacity_; }
  auto used() const noexcept { return used_; }
  void* alloc(std::size_t size)
  {
    const auto newUsed = used_ + size;
    if (newUsed > capacity_) {
      throw std::bad_alloc{};
    }
    // Allocate block from virtual memory.
    void* const addr{static_cast<char*>(addr_) + used_};
    used_ = newUsed;
    return addr;
  }

 private:
  std::size_t capacity_{0};
  void* addr_{nullptr};
  std::size_t used_{0};
};

} // detail

class SWIRLY_API MemPool {
  template <std::size_t SizeN>
  struct MemBlock {
    union {
      MemBlock* next;
      char storage[SizeN];
    };
  };

 public:
  explicit MemPool(std::size_t capacity);
  MemPool() noexcept;
  ~MemPool() noexcept;

  // Copy.
  MemPool(const MemPool& rhs) noexcept = delete;
  MemPool& operator=(const MemPool& rhs) noexcept = delete;

  MemPool(MemPool&&) noexcept;
  MemPool& operator=(MemPool&&) noexcept;

  auto capacity() const noexcept { return region_.capacity(); }

  void reserve(std::size_t capacity)
  {
    region_ = detail::MemRegion{capacity};
    head1_ = nullptr;
    head2_ = nullptr;
    head3_ = nullptr;
    head4_ = nullptr;
    head5_ = nullptr;
  }
  void* alloc(std::size_t size);

  void dealloc(void* ptr, std::size_t size) noexcept;

 private:
  template <std::size_t SizeN>
  void* allocBlock(MemBlock<SizeN>*& head)
  {
    using Block = MemBlock<SizeN>;
    Block* block;
    if (head) {
      // Pop from free-list.
      block = head;
      head = block->next;
    } else {
      block = static_cast<Block*>(region_.alloc(sizeof(Block)));
    }
    return block;
  }
  template <std::size_t SizeN>
  void deallocBlock(MemBlock<SizeN>*& head, void* ptr) noexcept
  {
    using Block = MemBlock<SizeN>;
    Block* const block{static_cast<Block*>(ptr)};
    if (block) {
      // Push onto free-list.
      block->next = head;
      head = block;
    }
  }

  detail::MemRegion region_;
  MemBlock<(1 << 6)>* head1_{nullptr};
  MemBlock<(2 << 6)>* head2_{nullptr};
  MemBlock<(3 << 6)>* head3_{nullptr};
  MemBlock<(4 << 6)>* head4_{nullptr};
  MemBlock<(5 << 6)>* head5_{nullptr};
};

constexpr std::size_t roundCacheLine(std::size_t size) noexcept
{
  return ((size + 63) >> 6) << 6;
}

} // swirly

#endif // SWIRLY_UTIL_MEMPOOL_HPP
