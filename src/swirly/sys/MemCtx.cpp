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

#include "File.hpp"
#include "MMap.hpp"
#include "MemPool.hpp"

#include <fcntl.h>

using namespace std;

namespace swirly {
namespace {

File reserveFile(const char* path, size_t size)
{
    File file{sys::open(path, O_RDWR | O_CREAT, 0644)};
    sys::ftruncate(file.get(), size);
    return file;
}

} // namespace

struct MemCtx::Impl {
    explicit Impl(size_t maxSize)
      : maxSize{maxSize}
      , memMap{sys::mmap(nullptr, PageSize + maxSize, PROT_READ | PROT_WRITE,
                         MAP_ANON | MAP_PRIVATE, -1, 0)}
      , pool(*static_cast<MemPool*>(memMap.get().data()))
    {
    }
    Impl(const char* path, size_t maxSize)
      : maxSize{maxSize}
      , file{reserveFile(path, PageSize + maxSize)}
      , memMap{sys::mmap(nullptr, PageSize + maxSize, PROT_READ | PROT_WRITE, MAP_SHARED,
                         file.get(), 0)}
      , pool(*static_cast<MemPool*>(memMap.get().data()))
    {
    }
    void* alloc(size_t size)
    {
        void* addr;
        enum { Max = (1 << CacheLineBits) - 1 };
        const auto cacheLines = (size + Max) >> CacheLineBits;
        switch (nextPow2(cacheLines)) {
        case 1: // 64
            addr = allocBlock(pool, pool.free1, maxSize);
            break;
        case 2: // 128
            addr = allocBlock(pool, pool.free2, maxSize);
            break;
        case 4:
            if (cacheLines == 3) {
                // 192
                addr = allocBlock(pool, pool.free3, maxSize);
            } else {
                // 256
                addr = allocBlock(pool, pool.free4, maxSize);
            }
            break;
        case 8: // 512
            addr = allocBlock(pool, pool.free8, maxSize);
            break;
        case 16: // 1024
            addr = allocBlock(pool, pool.free16, maxSize);
            break;
        case 32: // 2048
            addr = allocBlock(pool, pool.free32, maxSize);
            break;
        default:
            throw bad_alloc{};
        }
        return addr;
    }
    void dealloc(void* addr, size_t size) noexcept
    {
        const auto cacheLines = size >> CacheLineBits;
        switch (nextPow2(cacheLines)) {
        case 1: // 64
            deallocBlock(pool, pool.free1, addr);
            break;
        case 2: // 128
            deallocBlock(pool, pool.free2, addr);
            break;
        case 4:
            if (cacheLines == 3) {
                // 192
                deallocBlock(pool, pool.free3, addr);
            } else {
                // 256
                deallocBlock(pool, pool.free4, addr);
            }
            break;
        case 8: // 512
            deallocBlock(pool, pool.free8, addr);
            break;
        case 16: // 1024
            deallocBlock(pool, pool.free16, addr);
            break;
        case 32: // 2048
            deallocBlock(pool, pool.free32, addr);
            break;
        default:
            abort();
        }
    }
    const size_t maxSize;
    File file;
    MMap memMap;
    MemPool& pool;
};

MemCtx::MemCtx(size_t maxSize)
  : impl_{make_unique<Impl>(maxSize)}
{
}

MemCtx::MemCtx(const char* path, size_t maxSize)
  : impl_{make_unique<Impl>(path, maxSize)}
{
}

MemCtx::MemCtx() = default;

MemCtx::~MemCtx() noexcept = default;

// Move.
MemCtx::MemCtx(MemCtx&&) noexcept = default;
MemCtx& MemCtx::operator=(MemCtx&&) noexcept = default;

size_t MemCtx::maxSize() noexcept
{
    assert(impl_);
    return impl_->maxSize;
}

void* MemCtx::alloc(size_t size)
{
    assert(impl_);
    return impl_->alloc(size);
}

#if __GNUC__ >= 7
void* MemCtx::alloc(std::size_t size, align_val_t al)
{
    assert(impl_);
    assert(static_cast<size_t>(al) <= CacheLineSize);
    return impl_->alloc(size);
}
#endif

void MemCtx::dealloc(void* addr, size_t size) noexcept
{
    assert(impl_);
    impl_->dealloc(addr, size);
}

} // namespace swirly
