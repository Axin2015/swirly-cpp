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
#ifndef SWIRLY_ASH_MEMALLOC_HPP
#define SWIRLY_ASH_MEMALLOC_HPP

#include <swirly/ash/Defs.hpp>

#include <new>

namespace swirly {

SWIRLY_API void* alloc(std::size_t size);
SWIRLY_API void dealloc(void* ptr, std::size_t size) noexcept;

struct MemAlloc {
  static void* operator new(std::size_t size) { return alloc(size); }
  static void operator delete(void* ptr, std::size_t size) noexcept { return dealloc(ptr, size); }
};

} // swirly

#endif // SWIRLY_ASH_MEMALLOC_HPP
