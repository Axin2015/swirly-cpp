/*
 * Swirly Order-Book and Matching-Engine.
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
#ifndef SWIRLY_ELM_POSN_HPP
#define SWIRLY_ELM_POSN_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/RefCounted.hpp>
#include <swirly/ash/Types.hpp>

#include <boost/intrusive/set.hpp>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

class SWIRLY_API Posn : public RefCounted {

    const Mnem trader_;
    const Mnem contr_;
    Jday settlDay_;
    Lots buyLots_;
    Cost buyCost_;
    Lots sellLots_;
    Cost sellCost_;

 public:
    boost::intrusive::set_member_hook<> traderHook_;

    Posn(const StringView& trader, const StringView& contr, Jday settlDay, Lots buyLots,
         Cost buyCost, Lots sellLots, Cost sellCost) noexcept : trader_{trader},
                                                                contr_{contr},
                                                                settlDay_{settlDay},
                                                                buyLots_{buyLots},
                                                                buyCost_{buyCost},
                                                                sellLots_{sellLots},
                                                                sellCost_{sellCost}
    {
    }

    ~Posn() noexcept override;

    // Copy.
    Posn(const Posn&) = delete;
    Posn& operator=(const Posn&) = delete;

    // Move.
    Posn(Posn&&);
    Posn& operator=(Posn&&) = delete;

    StringView trader() const noexcept
    {
        return trader_.view();
    }
    StringView contr() const noexcept
    {
        return contr_.view();
    }
    Jday settlDay() const noexcept
    {
        return settlDay_;
    }
    Lots buyLots() const noexcept
    {
        return buyLots_;
    }
    Cost buyCost() const noexcept
    {
        return buyCost_;
    }
    Lots sellLots() const noexcept
    {
        return sellLots_;
    }
    Cost sellCost() const noexcept
    {
        return sellCost_;
    }
};

using PosnPtr = boost::intrusive_ptr<Posn>;

class SWIRLY_API TraderPosnSet {
    using Key = std::tuple<StringView, Jday>;
    struct ValueCompare {
        int compare(const Posn& lhs, const Posn& rhs) const noexcept
        {
            int result{lhs.contr().compare(rhs.contr())};
            if (result == 0)
                result = swirly::compare(lhs.settlDay(), rhs.settlDay());
            return result;
        }
        bool operator()(const Posn& lhs, const Posn& rhs) const noexcept
        {
            return compare(lhs, rhs) < 0;
        }
    };
    struct KeyValueCompare {
        bool operator()(const Key& lhs, const Posn& rhs) const noexcept
        {
            int result{std::get<0>(lhs).compare(rhs.contr())};
            if (result == 0)
                result = swirly::compare(std::get<1>(lhs), rhs.settlDay());
            return result < 0;
        }
        bool operator()(const Posn& lhs, const Key& rhs) const noexcept
        {
            int result{lhs.contr().compare(std::get<0>(rhs))};
            if (result == 0)
                result = swirly::compare(lhs.settlDay(), std::get<1>(rhs));
            return result < 0;
        }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<ValueCompare>;
    using MemberHookOption
        = boost::intrusive::member_hook<Posn, decltype(Posn::traderHook_), &Posn::traderHook_>;
    using Set
        = boost::intrusive::set<Posn, ConstantTimeSizeOption, CompareOption, MemberHookOption>;
    using ValuePtr = boost::intrusive_ptr<Posn>;

    Set set_;

 public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    TraderPosnSet() = default;

    ~TraderPosnSet() noexcept;

    // Copy.
    TraderPosnSet(const TraderPosnSet&) = delete;
    TraderPosnSet& operator=(const TraderPosnSet&) = delete;

    // Move.
    TraderPosnSet(TraderPosnSet&&);
    TraderPosnSet& operator=(TraderPosnSet&&);

    Iterator insert(const ValuePtr& value) noexcept;

    Iterator insertHint(ConstIterator hint, const ValuePtr& value) noexcept;

    Iterator insertOrReplace(const ValuePtr& value) noexcept;

    template <typename... ArgsT>
    Iterator emplace(ArgsT&&... args)
    {
        return insert(makeRefCounted<Posn>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplaceHint(ConstIterator hint, ArgsT&&... args)
    {
        return insertHint(hint, makeRefCounted<Posn>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplaceOrReplace(ArgsT&&... args)
    {
        return insertOrReplace(makeRefCounted<Posn>(std::forward<ArgsT>(args)...));
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
    Iterator find(const StringView& contr, Jday settlDay) noexcept
    {
        return set_.find(std::make_tuple(contr, settlDay), KeyValueCompare());
    }
    ConstIterator find(const StringView& contr, Jday settlDay) const noexcept
    {
        return set_.find(std::make_tuple(contr, settlDay), KeyValueCompare());
    }
    std::pair<Iterator, bool> findHint(const StringView& contr, Jday settlDay) noexcept
    {
        const auto key = std::make_tuple(contr, settlDay);
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(key, comp);
        return std::make_pair(it, it != set_.end() && !comp(key, *it));
    }
    std::pair<ConstIterator, bool> findHint(const StringView& contr, Jday settlDay) const noexcept
    {
        const auto key = std::make_tuple(contr, settlDay);
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(key, comp);
        return std::make_pair(it, it != set_.end() && !comp(key, *it));
    }
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_POSN_HPP
