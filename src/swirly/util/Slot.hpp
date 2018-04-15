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
#ifndef SWIRLY_UTIL_SLOT_HPP
#define SWIRLY_UTIL_SLOT_HPP

#include <swirly/util/Traits.hpp>

namespace swirly {
inline namespace util {

template <typename... ArgsT>
class BasicSlot {
  public:
    friend constexpr bool operator==(BasicSlot lhs, BasicSlot rhs) noexcept
    {
        return lhs.obj_ == rhs.obj_ && lhs.fn_ == rhs.fn_;
    }
    friend constexpr bool operator!=(BasicSlot lhs, BasicSlot rhs) noexcept
    {
        return !(lhs == rhs);
    }
    constexpr BasicSlot() noexcept = default;
    ~BasicSlot() = default;

    // Copy.
    constexpr BasicSlot(const BasicSlot&) noexcept = default;
    constexpr BasicSlot& operator=(const BasicSlot&) noexcept = default;

    // Move.
    constexpr BasicSlot(BasicSlot&&) noexcept = default;
    constexpr BasicSlot& operator=(BasicSlot&&) noexcept = default;

    void invoke(ArgsT... args) const { fn_(obj_, args...); }
    void operator()(ArgsT... args) const { fn_(obj_, args...); }
    constexpr bool empty() const noexcept { return fn_ == nullptr; }
    constexpr explicit operator bool() const noexcept { return fn_ != nullptr; }

    // Free function.
    template <void (*FnT)(ArgsT...)>
    constexpr auto& bind() noexcept
    {
        obj_ = nullptr;
        fn_ = [](void* obj, ArgsT... args) { FnT(args...); };
        return *this;
    }
    // Lambda function.
    template <typename ClassT>
    constexpr auto& bind(ClassT* obj) noexcept
    {
        obj_ = obj;
        fn_ = [](void* obj, ArgsT... args) { (*static_cast<ClassT*>(obj))(args...); };
        return *this;
    }
    // Member function.
    template <auto MemFnT, typename ClassT = typename FunctionTraits<decltype(MemFnT)>::ClassType>
    constexpr auto& bind(ClassT* obj) noexcept
    {
        obj_ = obj;
        fn_ = [](void* obj, ArgsT... args) { (static_cast<ClassT*>(obj)->*MemFnT)(args...); };
        return *this;
    }
    void reset() noexcept
    {
        obj_ = nullptr;
        fn_ = nullptr;
    }

  private:
    void* obj_{nullptr};
    void (*fn_)(void*, ArgsT...){nullptr};
};

template <auto FnT>
constexpr auto bind() noexcept
{
    using Traits = FunctionTraits<decltype(FnT)>;
    using Slot = typename Traits::template Pack<BasicSlot>;
    return Slot{}.template bind<FnT>();
}

template <typename ClassT>
constexpr auto bind(ClassT* obj) noexcept
{
    using Traits = FunctionTraits<decltype(&ClassT::operator())>;
    using Slot = typename Traits::template Pack<BasicSlot>;
    return Slot{}.bind(obj);
}

template <auto MemFnT, typename ClassT = typename FunctionTraits<decltype(MemFnT)>::ClassType>
constexpr auto bind(ClassT* obj) noexcept
{
    using Traits = FunctionTraits<decltype(MemFnT)>;
    using Slot = typename Traits::template Pack<BasicSlot>;
    return Slot{}.template bind<MemFnT>(obj);
}

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_SLOT_HPP
