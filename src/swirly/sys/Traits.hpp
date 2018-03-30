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
#ifndef SWIRLY_SYS_TRAITS_HPP
#define SWIRLY_SYS_TRAITS_HPP

#include <tuple>

namespace swirly {
inline namespace sys {

/**
 * Default case for functors and lambdas.
 */
template <typename TypeT>
struct FunctionTraits : FunctionTraits<decltype(&TypeT::operator())> {
};

/**
 * Specialisation for free functions.
 */
template <typename ReturnT, typename... ArgsT>
struct FunctionTraits<ReturnT (*)(ArgsT...)> {

    using FunctionType = ReturnT (*)(ArgsT...);
    using ClassType = void;
    using ReturnType = ReturnT;

    enum { Arity = sizeof...(ArgsT) };

    template <std::size_t i>
    using ArgType = std::tuple_element_t<i, std::tuple<ArgsT...>>;

    // Apply parameter pack to template.
    template <template <typename...> typename TemplT>
    using Pack = TemplT<ArgsT...>;
};

/**
 * Specialisation for member functions.
 */
template <typename ClassT, typename ReturnT, typename... ArgsT>
struct FunctionTraits<ReturnT (ClassT::*)(ArgsT...)> : FunctionTraits<ReturnT (*)(ArgsT...)> {
    using FunctionType = ReturnT (ClassT::*)(ArgsT...);
    using ClassType = ClassT;
};

/**
 * Specialisation for const member functions.
 */
template <typename ClassT, typename ReturnT, typename... ArgsT>
struct FunctionTraits<ReturnT (ClassT::*)(ArgsT...) const> : FunctionTraits<ReturnT (*)(ArgsT...)> {
    using FunctionType = ReturnT (ClassT::*)(ArgsT...) const;
    using ClassType = ClassT;
};

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_TRAITS_HPP
