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
#ifndef SWIRLY_UTIL_MEMCTX_HPP
#define SWIRLY_UTIL_MEMCTX_HPP

#include <swirly/util/Defs.hpp>

#include <cstddef>
#include <memory>

namespace swirly {

class SWIRLY_API MemCtx {
  public:
    /**
     * This constructor uses a anonymous, private memory-map.
     */
    explicit MemCtx(std::size_t maxSize);

    /**
     * This constructor uses a shared memory-map.
     */
    MemCtx(const char* path, std::size_t maxSize);

    MemCtx();
    ~MemCtx() noexcept;

    // Copy.
    MemCtx(const MemCtx& rhs) noexcept = delete;
    MemCtx& operator=(const MemCtx& rhs) noexcept = delete;

    MemCtx(MemCtx&&) noexcept;
    MemCtx& operator=(MemCtx&&) noexcept;

    std::size_t maxSize() noexcept;

    void* alloc(std::size_t size);

#if __GNUC__ >= 7
    // Requested alignment must not be greater than the size of a cache-line.
    void* alloc(std::size_t size, std::align_val_t al);
#endif

    void dealloc(void* addr, std::size_t size) noexcept;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // swirly

#endif // SWIRLY_UTIL_MEMCTX_HPP
