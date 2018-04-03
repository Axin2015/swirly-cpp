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
#ifndef SWIRLY_SYS_HANDLE_HPP
#define SWIRLY_SYS_HANDLE_HPP

#include <cstddef> // nullptr_t
#include <utility> // swap<>

#include <unistd.h> // close()

namespace swirly {
inline namespace sys {

template <typename PolicyT>
class BasicHandle {
  public:
    using Id = typename PolicyT::Id;

    static constexpr Id invalid() noexcept { return PolicyT::invalid(); }

    constexpr BasicHandle(std::nullptr_t = nullptr) noexcept {}
    constexpr BasicHandle(Id id) noexcept
    : id_{id}
    {
    }
    ~BasicHandle()
    {
        if (id_ != invalid()) {
            PolicyT::close(id_);
        }
    }

    // Copy.
    BasicHandle(const BasicHandle&) = delete;
    BasicHandle& operator=(const BasicHandle&) = delete;

    // Move.
    constexpr BasicHandle(BasicHandle&& rhs)
    : id_{rhs.id_}
    {
        rhs.id_ = invalid();
    }
    BasicHandle& operator=(BasicHandle&& rhs)
    {
        close();
        std::swap(id_, rhs.id_);
        return *this;
    }

    constexpr bool empty() const noexcept { return id_ == invalid(); }
    constexpr explicit operator bool() const noexcept { return id_ != invalid(); }

    constexpr Id get() const noexcept { return id_; }
    constexpr Id operator*() const noexcept { return get(); }

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

    void swap(BasicHandle& rhs) noexcept { std::swap(id_, rhs.id_); }

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
constexpr bool operator==(const BasicHandle<PolicyT>& lhs, const BasicHandle<PolicyT>& rhs)
{
    return lhs.get() == rhs.get();
}

template <typename PolicyT>
constexpr bool operator!=(const BasicHandle<PolicyT>& lhs, const BasicHandle<PolicyT>& rhs)
{
    return !(lhs == rhs);
}

struct FilePolicy {
    using Id = int;
    static constexpr int invalid() noexcept { return -1; }
    static void close(int d) noexcept { ::close(d); }
};

using FileHandle = BasicHandle<FilePolicy>;

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_HANDLE_HPP
