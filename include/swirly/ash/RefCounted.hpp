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
#ifndef SWIRLY_ASH_REFCOUNTED_HPP
#define SWIRLY_ASH_REFCOUNTED_HPP

#include <swirly/ash/Defs.hpp>

#include <boost/intrusive_ptr.hpp>

namespace swirly {

/**
 * @addtogroup Util
 * @{
 */

/**
 * Base class for referenced counted objects.
 */
class SWIRLY_API RefCounted {
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
      delete this;
    }
  }
  int refs() const noexcept { return refs_; }

 protected:
  virtual ~RefCounted() noexcept;

 private:
  mutable int refs_ = 1;
};

using RefCountedPtr = boost::intrusive_ptr<RefCounted>;

template <typename ValueT, typename... ArgsT>
boost::intrusive_ptr<ValueT> makeRefCounted(ArgsT&&... args)
{
  return {new ValueT{std::forward<ArgsT>(args)...}, false};
}

inline void intrusive_ptr_add_ref(const RefCounted* ptr) noexcept
{
  ptr->addRef();
}

inline void intrusive_ptr_release(const RefCounted* ptr) noexcept
{
  ptr->release();
}

/** @} */

} // swirly

#endif // SWIRLY_ASH_REFCOUNTED_HPP
