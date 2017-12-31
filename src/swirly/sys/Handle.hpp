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
#ifndef SWIRLY_SYS_HANDLE_HPP
#define SWIRLY_SYS_HANDLE_HPP

#include <cstddef> // nullptr_t
#include <utility> // swap<>

namespace swirly {

template <typename PolicyT>
class Handle {
  public:
    using Id = typename PolicyT::Id;

    static constexpr Id invalid() noexcept { return PolicyT::invalid(); }

    Handle(std::nullptr_t = nullptr) noexcept {}
    Handle(Id id) noexcept
      : id_{id}
    {
    }
    ~Handle() noexcept
    {
        if (id_ != invalid()) {
            PolicyT::close(id_);
        }
    }

    // Copy.
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

    // Move.
    Handle(Handle&& rhs)
      : id_{rhs.id_}
    {
        rhs.id_ = invalid();
    }
    Handle& operator=(Handle&& rhs)
    {
        close();
        std::swap(id_, rhs.id_);
        return *this;
    }

    Id operator*() const noexcept { return get(); }

    Id get() const noexcept { return id_; }

    explicit operator bool() const noexcept { return id_ != invalid(); }

    Id release() noexcept
    {
        const auto id = id_;
        id_ = invalid();
        return id;
    }

    void reset(Id id = invalid()) noexcept
    {
        std::swap(id_, id);
        if (id != invalid()) {
            PolicyT::close(id);
        }
    }

    void swap(Handle& rhs) noexcept { std::swap(id_, rhs.id_); }

  private:
    void close() noexcept
    {
        if (id_ != invalid()) {
            PolicyT::close(id_);
            id_ = invalid();
        }
    }
    Id id_{invalid()};
};

template <typename PolicyT>
inline bool operator==(const Handle<PolicyT>& lhs, const Handle<PolicyT>& rhs)
{
    return lhs.get() == rhs.get();
}

template <typename PolicyT>
inline bool operator!=(const Handle<PolicyT>& lhs, const Handle<PolicyT>& rhs)
{
    return !(lhs == rhs);
}

} // namespace swirly

#endif // SWIRLY_SYS_HANDLE_HPP
