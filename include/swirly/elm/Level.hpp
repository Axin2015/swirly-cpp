/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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
#ifndef SWIRLY_ELM_LEVEL_HPP
#define SWIRLY_ELM_LEVEL_HPP

#include <swirly/elm/Types.hpp>

#include <boost/intrusive/set.hpp>

#include <memory>

namespace swirly {

/**
 * @addtogroup Book
 * @{
 */

using LevelKey = std::underlying_type_t<Ticks>;

namespace detail {

/**
 * Synthetic level key.
 */
constexpr LevelKey composeKey(Side side, Ticks ticks) noexcept
{
    using namespace enumops;
    return unbox(side == Side::BUY ? -ticks : ticks);
}

} // detail

/**
 * Price level.
 *
 * A price level is an aggregation of orders by price. I.e. the sum of all orders in the book at the
 * same price.
 */
class SWIRLY_API Level : public Comparable<Level> {
    const Order* firstOrder_;
    const LevelKey key_;
    const Ticks ticks_;
    /**
     * Must be greater than zero.
     */
    Lots resd_;
    Lots quotd_;
    /**
     * Must be greater than zero.
     */
    int count_;
 public:
    boost::intrusive::set_member_hook<> keyHook_;

    explicit Level(const Order& firstOrder) noexcept;

    ~Level() noexcept;

    // Copy.
    Level(const Level&) = delete;
    Level& operator =(const Level&) = delete;

    // Move.
    Level(Level&&);
    Level& operator =(Level&&) = delete;

    int compare(const Level& rhs) const noexcept
    {
        return swirly::compare(key_, rhs.key_);
    }
    const Order& firstOrder() const noexcept
    {
        return *firstOrder_;
    }
    LevelKey key() const noexcept
    {
        return key_;
    }
    Ticks ticks() const noexcept
    {
        return ticks_;
    }
    Lots resd() const noexcept
    {
        return resd_;
    }
    Lots quotd() const noexcept
    {
        return quotd_;
    }
    int count() const noexcept
    {
        return count_;
    }
};

class SWIRLY_API LevelSet {
    struct ValueCompare {
        bool operator()(const Level& lhs, const Level& rhs) const noexcept
        {
            return lhs.key() < rhs.key();
        }
    };
    struct KeyValueCompare {
        bool operator()(LevelKey lhs, const Level& rhs) const noexcept
        {
            return lhs < rhs.key();
        }
        bool operator()(const Level& lhs, LevelKey rhs) const noexcept
        {
            return lhs.key() < rhs;
        }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<ValueCompare>;
    using MemberHookOption = boost::intrusive::member_hook<Level, decltype(Level::keyHook_),
                                                           &Level::keyHook_>;
    using Set = boost::intrusive::set<Level,
                                      ConstantTimeSizeOption,
                                      CompareOption,
                                      MemberHookOption
                                      >;
    using ValuePtr = std::unique_ptr<Level>;

    Set set_;
 public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    LevelSet() = default;
    ~LevelSet() noexcept;

    // Copy.
    LevelSet(const LevelSet&) = delete;
    LevelSet& operator =(const LevelSet&) = delete;

    // Move.
    LevelSet(LevelSet&&);
    LevelSet& operator =(LevelSet&&);

    Level& insert(ValuePtr rec) noexcept;

    Level& insertOrReplace(ValuePtr rec) noexcept;

    template <typename... ArgsT>
    Level& emplace(ArgsT&&... args)
    {
        return insert(std::make_unique<Level>(std::forward<ArgsT>(args)...));
    }

    template <typename... ArgsT>
    Level& emplaceOrReplace(ArgsT&&... args)
    {
        return insertOrReplace(std::make_unique<Level>(std::forward<ArgsT>(args)...));
    }

    // Begin.
    Iterator begin() noexcept
    {
        return set_.begin();
    }
    ConstIterator begin() const noexcept
    {
        return set_.begin();
    }
    ConstIterator cbegin() const noexcept
    {
        return set_.cbegin();
    }

    // End.
    Iterator end() noexcept
    {
        return set_.end();
    }
    ConstIterator end() const noexcept
    {
        return set_.end();
    }
    ConstIterator cend() const noexcept
    {
        return set_.cend();
    }

    // Find.
    Iterator find(Side side, Ticks ticks) noexcept
    {
        return set_.find(detail::composeKey(side, ticks), KeyValueCompare());
    }
    ConstIterator find(Side side, Ticks ticks) const noexcept
    {
        return set_.find(detail::composeKey(side, ticks), KeyValueCompare());
    }
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_LEVEL_HPP
