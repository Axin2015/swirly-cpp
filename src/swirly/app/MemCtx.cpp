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
#include "MemCtx.hpp"

#include "MemPool.hpp"

#include <swirly/sys/File.hpp>
#include <swirly/sys/MMap.hpp>

#include <fcntl.h>

namespace swirly {
inline namespace app {
using namespace std;
namespace {

FileHandle reserve_file(const char* path, size_t size)
{
    FileHandle fh{os::open(path, O_RDWR | O_CREAT, 0644)};
    os::ftruncate(fh.get(), size);
    return fh;
}

} // namespace

struct MemCtx::Impl {
    explicit Impl(size_t max_size)
    : max_size{max_size}
    , mem_map{os::mmap(nullptr, PageSize + max_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE,
                       -1, 0)}
    , pool(*static_cast<MemPool*>(mem_map.get().data()))
    {
    }
    Impl(const char* path, size_t max_size)
    : max_size{max_size}
    , fh{reserve_file(path, PageSize + max_size)}
    , mem_map{os::mmap(nullptr, PageSize + max_size, PROT_READ | PROT_WRITE, MAP_SHARED, fh.get(),
                       0)}
    , pool(*static_cast<MemPool*>(mem_map.get().data()))
    {
    }
    void* alloc(size_t size)
    {
        void* addr;
        enum { Max = (1 << CacheLineBits) - 1 };
        const auto cache_lines = (size + Max) >> CacheLineBits;
        switch (next_pow2(cache_lines)) {
        case 1: // 64
            addr = alloc_block(pool, pool.free1, max_size);
            break;
        case 2: // 128
            addr = alloc_block(pool, pool.free2, max_size);
            break;
        case 4:
            if (cache_lines == 3) {
                // 192
                addr = alloc_block(pool, pool.free3, max_size);
            } else {
                // 256
                addr = alloc_block(pool, pool.free4, max_size);
            }
            break;
        case 8: // 512
            addr = alloc_block(pool, pool.free8, max_size);
            break;
        case 16: // 1024
            addr = alloc_block(pool, pool.free16, max_size);
            break;
        case 32: // 2048
            addr = alloc_block(pool, pool.free32, max_size);
            break;
        default:
            throw bad_alloc{};
        }
        return addr;
    }
    void dealloc(void* addr, size_t size) noexcept
    {
        const auto cache_lines = size >> CacheLineBits;
        switch (next_pow2(cache_lines)) {
        case 1: // 64
            dealloc_block(pool, pool.free1, addr);
            break;
        case 2: // 128
            dealloc_block(pool, pool.free2, addr);
            break;
        case 4:
            if (cache_lines == 3) {
                // 192
                dealloc_block(pool, pool.free3, addr);
            } else {
                // 256
                dealloc_block(pool, pool.free4, addr);
            }
            break;
        case 8: // 512
            dealloc_block(pool, pool.free8, addr);
            break;
        case 16: // 1024
            dealloc_block(pool, pool.free16, addr);
            break;
        case 32: // 2048
            dealloc_block(pool, pool.free32, addr);
            break;
        default:
            abort();
        }
    }
    const size_t max_size;
    FileHandle fh;
    MMap mem_map;
    MemPool& pool;
};

MemCtx::MemCtx(size_t max_size)
: impl_{make_unique<Impl>(max_size)}
{
}

MemCtx::MemCtx(const char* path, size_t max_size)
: impl_{make_unique<Impl>(path, max_size)}
{
}

MemCtx::MemCtx() = default;

MemCtx::~MemCtx() = default;

// Move.
MemCtx::MemCtx(MemCtx&&) noexcept = default;
MemCtx& MemCtx::operator=(MemCtx&&) noexcept = default;

size_t MemCtx::max_size() noexcept
{
    assert(impl_);
    return impl_->max_size;
}

void* MemCtx::alloc(size_t size)
{
    assert(impl_);
    return impl_->alloc(size);
}

void* MemCtx::alloc(std::size_t size, align_val_t al)
{
    assert(impl_);
    assert(static_cast<size_t>(al) <= CacheLineSize);
    return impl_->alloc(size);
}

void MemCtx::dealloc(void* addr, size_t size) noexcept
{
    assert(impl_);
    impl_->dealloc(addr, size);
}

void MemCtx::dealloc(void* addr, size_t size, align_val_t al) noexcept
{
    assert(impl_);
    assert(static_cast<size_t>(al) <= CacheLineSize);
    impl_->dealloc(addr, size);
}

} // namespace app
} // namespace swirly
