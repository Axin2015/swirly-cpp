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
#ifndef SWIRLY_FIN_REQUEST_HPP
#define SWIRLY_FIN_REQUEST_HPP

#include <swirly/fin/Types.hpp>

#include <swirly/util/BasicTypes.hpp>
#include <swirly/util/Date.hpp>
#include <swirly/util/Symbol.hpp>

#include <swirly/sys/RefCount.hpp>

#include <boost/intrusive/set.hpp>

namespace swirly {
inline namespace fin {

class SWIRLY_API Request {
  public:
    Request(Symbol accnt, Id64 marketId, Symbol instr, JDay settlDay, Id64 id, std::string_view ref,
            Side side, Lots lots, Time created) noexcept
    : accnt_{accnt}
    , marketId_{marketId}
    , instr_{instr}
    , settlDay_{settlDay}
    , id_{id}
    , ref_{ref}
    , side_{side}
    , lots_{lots}
    , created_{created}
    {
    }

    // Copy.
    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;

    // Move.
    Request(Request&&);
    Request& operator=(Request&&) = delete;

    auto accnt() const noexcept { return accnt_; }
    auto marketId() const noexcept { return marketId_; }
    auto instr() const noexcept { return instr_; }
    auto settlDay() const noexcept { return settlDay_; }
    auto id() const noexcept { return id_; }
    auto ref() const noexcept { return +ref_; }
    auto side() const noexcept { return side_; }
    auto lots() const noexcept { return lots_; }
    auto created() const noexcept { return created_; }

  protected:
    ~Request();

    /**
     * The executing accnt.
     */
    const Symbol accnt_;
    const Id64 marketId_;
    const Symbol instr_;
    const JDay settlDay_;
    const Id64 id_;
    /**
     * Ref is optional.
     */
    const Ref ref_;
    const Side side_;
    Lots lots_;
    const Time created_;
};

/**
 * Request set keyed by market and id. Requests are identified by market and id only, so instances
 * should not be used as heterogeneous Request containers, where Requests of different types may
 * share the same identity.
 */
template <typename RequestT>
class RequestIdSet {
    using Key = std::tuple<Id64, Id64>;
    struct ValueCompare {
        int compare(const Request& lhs, const Request& rhs) const noexcept
        {
            int result{swirly::compare(lhs.marketId(), rhs.marketId())};
            if (result == 0) {
                result = swirly::compare(lhs.id(), rhs.id());
            }
            return result;
        }
        bool operator()(const Request& lhs, const Request& rhs) const noexcept
        {
            return compare(lhs, rhs) < 0;
        }
    };
    struct KeyValueCompare {
        bool operator()(const Key& lhs, const Request& rhs) const noexcept
        {
            int result{swirly::compare(std::get<0>(lhs), rhs.marketId())};
            if (result == 0) {
                result = swirly::compare(std::get<1>(lhs), rhs.id());
            }
            return result < 0;
        }
        bool operator()(const Request& lhs, const Key& rhs) const noexcept
        {
            int result{swirly::compare(lhs.marketId(), std::get<0>(rhs))};
            if (result == 0) {
                result = swirly::compare(lhs.id(), std::get<1>(rhs));
            }
            return result < 0;
        }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<ValueCompare>;
    using MemberHookOption
        = boost::intrusive::member_hook<RequestT, decltype(RequestT::idHook), &RequestT::idHook>;
    using Set
        = boost::intrusive::set<RequestT, ConstantTimeSizeOption, CompareOption, MemberHookOption>;
    using ValuePtr = boost::intrusive_ptr<RequestT>;

  public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    RequestIdSet() = default;

    ~RequestIdSet()
    {
        set_.clear_and_dispose([](const RequestT* ptr) { ptr->release(); });
    }

    // Copy.
    RequestIdSet(const RequestIdSet&) = delete;
    RequestIdSet& operator=(const RequestIdSet&) = delete;

    // Move.
    RequestIdSet(RequestIdSet&&) = default;
    RequestIdSet& operator=(RequestIdSet&&) = default;

    // Begin.
    ConstIterator begin() const noexcept { return set_.begin(); }
    ConstIterator cbegin() const noexcept { return set_.cbegin(); }
    Iterator begin() noexcept { return set_.begin(); }

    // End.
    ConstIterator end() const noexcept { return set_.end(); }
    ConstIterator cend() const noexcept { return set_.cend(); }
    Iterator end() noexcept { return set_.end(); }

    // Find.
    ConstIterator find(Id64 marketId, Id64 id) const noexcept
    {
        return set_.find(std::make_tuple(marketId, id), KeyValueCompare());
    }
    Iterator find(Id64 marketId, Id64 id) noexcept
    {
        return set_.find(std::make_tuple(marketId, id), KeyValueCompare());
    }
    std::pair<ConstIterator, bool> findHint(Id64 marketId, Id64 id) const noexcept
    {
        const auto key = std::make_tuple(marketId, id);
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(key, comp);
        return std::make_pair(it, it != set_.end() && !comp(key, *it));
    }
    std::pair<Iterator, bool> findHint(Id64 marketId, Id64 id) noexcept
    {
        const auto key = std::make_tuple(marketId, id);
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(key, comp);
        return std::make_pair(it, it != set_.end() && !comp(key, *it));
    }
    Iterator insert(const ValuePtr& value) noexcept
    {
        Iterator it;
        bool inserted;
        std::tie(it, inserted) = set_.insert(*value);
        if (inserted) {
            // Take ownership if inserted.
            value->addRef();
        }
        return it;
    }
    Iterator insertHint(ConstIterator hint, const ValuePtr& value) noexcept
    {
        auto it = set_.insert(hint, *value);
        // Take ownership.
        value->addRef();
        return it;
    }
    Iterator insertOrReplace(const ValuePtr& value) noexcept
    {
        Iterator it;
        bool inserted;
        std::tie(it, inserted) = set_.insert(*value);
        if (!inserted) {
            // Replace if exists.
            ValuePtr prev{&*it, false};
            set_.replace_node(it, *value);
            it = Set::s_iterator_to(*value);
        }
        // Take ownership.
        value->addRef();
        return it;
    }
    template <typename... ArgsT>
    Iterator emplace(ArgsT&&... args)
    {
        return insert(makeIntrusive<RequestT>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplaceHint(ConstIterator hint, ArgsT&&... args)
    {
        return insertHint(hint, makeIntrusive<RequestT>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplaceOrReplace(ArgsT&&... args)
    {
        return insertOrReplace(makeIntrusive<RequestT>(std::forward<ArgsT>(args)...));
    }
    ValuePtr remove(const RequestT& ref) noexcept
    {
        ValuePtr value;
        set_.erase_and_dispose(ref, [&value](RequestT* ptr) { value = ValuePtr{ptr, false}; });
        return value;
    }

  private:
    Set set_;
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_REQUEST_HPP
