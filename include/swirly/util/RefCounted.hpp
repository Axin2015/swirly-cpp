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
#ifndef SWIRLY_UTIL_REFCOUNTED_HPP
#define SWIRLY_UTIL_REFCOUNTED_HPP

#include <swirly/util/Defs.hpp>

#include <boost/intrusive_ptr.hpp>

namespace swirly {

/**
 * Base class for referenced counted objects.
 */
template <typename DerivedT>
class RefCounted {
 public:
  constexpr RefCounted() noexcept = default;

  // Copy.
  constexpr RefCounted(const RefCounted&) noexcept = default;
  RefCounted& operator=(const RefCounted&) noexcept = default;

  // Move.
  constexpr RefCounted(RefCounted&&) noexcept = default;
  RefCounted& operator=(RefCounted&&) noexcept = default;

  void addRef() const noexcept { ++refs_; }
  void release() const noexcept
  {
    if (--refs_ <= 0) {
      delete static_cast<const DerivedT*>(this);
    }
  }
  int refs() const noexcept { return refs_; }

 protected:
  ~RefCounted() noexcept = default;

 private:
  mutable int refs_ = 1;
};

template <typename ValueT, typename... ArgsT>
boost::intrusive_ptr<ValueT> makeRefCounted(ArgsT&&... args)
{
  return {new ValueT{std::forward<ArgsT>(args)...}, false};
}

template <typename DerivedT>
void intrusive_ptr_add_ref(const RefCounted<DerivedT>* ptr) noexcept
{
  ptr->addRef();
}

template <typename DerivedT>
void intrusive_ptr_release(const RefCounted<DerivedT>* ptr) noexcept
{
  ptr->release();
}

} // swirly

#endif // SWIRLY_UTIL_REFCOUNTED_HPP
