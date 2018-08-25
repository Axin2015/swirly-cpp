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
#ifndef SWIRLY_UTIL_ARGV_HPP
#define SWIRLY_UTIL_ARGV_HPP

#include <swirly/Config.h>

#include <optional>
#include <string_view>

namespace swirly {
inline namespace util {

/**
 * Utility class that allows an argv array to be treated as a sequence.
 */
class ArgvSequence {
  public:
    using ConstIterator = const char* const*;
    constexpr ArgvSequence(int argc, const char* const argv[])
    : begin_{argv}
    , end_{argv + argc}
    {
    }
    ~ArgvSequence() = default;

    // Copy.
    constexpr ArgvSequence(const ArgvSequence& rhs) noexcept = default;
    constexpr ArgvSequence& operator=(const ArgvSequence& rhs) noexcept = default;

    // Move.
    constexpr ArgvSequence(ArgvSequence&&) noexcept = default;
    constexpr ArgvSequence& operator=(ArgvSequence&&) noexcept = default;

    constexpr ConstIterator begin() const noexcept { return begin_; }
    constexpr ConstIterator end() const noexcept { return end_; }

  private:
    ConstIterator begin_, end_;
};

class SWIRLY_API ArgvLexer {
  public:
    using ConstIterator = ArgvSequence::ConstIterator;
    ArgvLexer(ConstIterator begin, ConstIterator end) noexcept
    : it_{begin}
    , end_{end}
    {
        next();
    }
    explicit ArgvLexer(ArgvSequence argv) noexcept
    : ArgvLexer{argv.begin(), argv.end()}
    {
    }
    ArgvLexer(int argc, const char* const argv[]) noexcept
    : ArgvLexer{ArgvSequence{argc, argv}}
    {
    }
    ~ArgvLexer();

    // Copy.
    ArgvLexer(const ArgvLexer& rhs) noexcept = default;
    ArgvLexer& operator=(const ArgvLexer& rhs) noexcept = default;

    // Move.
    ArgvLexer(ArgvLexer&&) noexcept = default;
    ArgvLexer& operator=(ArgvLexer&&) noexcept = default;

    bool empty() const noexcept { return it_ == end_; }

    std::string_view arg() const noexcept { return *it_; }
    std::string_view opt() const noexcept { return opt_; }

    void pop() noexcept
    {
        ++it_;
        next();
    }
    /**
     * Use this pop overload when the current option is a switch.
     * Throw std::runtime_error if the switch had an attached value.
     */
    void pop_switch();

    /**
     * Use this pop overload when the current option expects a value.
     * Throw std::runtime_error if no value exists.
     */
    std::string_view pop_value();

  private:
    void next() noexcept;

    ConstIterator it_, end_;
    std::string_view opt_;
    std::optional<std::string_view> val_;
};

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_ARGV_HPP
