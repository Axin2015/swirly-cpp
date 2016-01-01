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
#ifndef SWIRLY_FIG_EMAILSET_HPP
#define SWIRLY_FIG_EMAILSET_HPP

#include <swirly/elm/Trader.hpp>

namespace swirly {
namespace detail {

class EmailSet {
    static constexpr std::size_t BUCKETS = 101;
    struct ValueHash {
        std::size_t operator ()(const Trader& trader) const noexcept
        {
            return std::hash<StringView>()(trader.email());
        }
    };
    struct ValueEqual {
        bool operator()(const Trader& lhs, const Trader& rhs) const noexcept
        {
            return lhs.email() == rhs.email();
        }
    };
    using KeyHash = std::hash<StringView>;
    struct KeyValueEqual {
        bool operator()(const StringView& lhs, const Trader& rhs) const noexcept
        {
            return lhs == rhs.email();
        }
        bool operator()(const Trader& lhs, const StringView& rhs) const noexcept
        {
            return lhs.email() == rhs;
        }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using EqualOption = boost::intrusive::equal<ValueEqual>;
    using HashOption = boost::intrusive::hash<ValueHash>;
    using MemberHookOption = boost::intrusive::member_hook<Trader, decltype(Trader::email_hook_),
                                                           &Trader::email_hook_>;
    using Set = boost::intrusive::unordered_set<Trader,
                                                ConstantTimeSizeOption,
                                                EqualOption,
                                                HashOption,
                                                MemberHookOption>;
    using BucketType = Set::bucket_type;
    using BucketTraits = Set::bucket_traits;

    BucketType buckets_[BUCKETS];
    Set set_;
public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    EmailSet()
    :   set_{BucketTraits{buckets_, BUCKETS}}
    {
    }

    ~EmailSet() noexcept = default;

    // Copy.
    EmailSet(const EmailSet&) = delete;
    EmailSet& operator =(const EmailSet&) = delete;

    // Move.
    EmailSet(EmailSet&&) = default;
    EmailSet& operator =(EmailSet&&) = default;

    bool insert(Trader& trader) noexcept
    {
        return set_.insert(trader).second;
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
    Iterator find(const StringView& email) noexcept
    {
        return set_.find(email, KeyHash(), KeyValueEqual());
    }
    ConstIterator find(const StringView& email) const noexcept
    {
        return set_.find(email, KeyHash(), KeyValueEqual());
    }
};

} // detail
} // swirly

#endif // SWIRLY_FIG_EMAILSET_HPP
