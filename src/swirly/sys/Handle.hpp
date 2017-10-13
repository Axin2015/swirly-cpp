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
    using Descriptor = typename PolicyT::Descriptor;

    static constexpr Descriptor invalid() noexcept { return PolicyT::invalid(); }

    Handle(std::nullptr_t = nullptr) noexcept {}
    Handle(Descriptor d) noexcept : d_{d} {}
    ~Handle() noexcept
    {
        if (d_ != invalid()) {
            PolicyT::close(d_);
        }
    }

    // Copy.
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

    // Move.
    Handle(Handle&& rhs) : d_{rhs.d_} { rhs.d_ = invalid(); }
    Handle& operator=(Handle&& rhs)
    {
        close();
        std::swap(d_, rhs.d_);
        return *this;
    }

    Descriptor operator*() const noexcept { return get(); }

    Descriptor get() const noexcept { return d_; }

    explicit operator bool() const noexcept { return d_ != invalid(); }

    Descriptor release() noexcept
    {
        const auto d = d_;
        d_ = invalid();
        return d;
    }

    void reset(Descriptor d = invalid()) noexcept
    {
        std::swap(d_, d);
        if (d != invalid()) {
            PolicyT::close(d);
        }
    }

    void swap(Handle& rhs) noexcept { std::swap(d_, rhs.d_); }

  private:
    void close() noexcept
    {
        if (d_ != invalid()) {
            PolicyT::close(d_);
            d_ = invalid();
        }
    }
    Descriptor d_{invalid()};
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
