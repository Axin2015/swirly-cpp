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

#include <swirly/util/Date.hpp>
#include <swirly/util/RefCount.hpp>
#include <swirly/util/Symbol.hpp>

#include <boost/intrusive/set.hpp>

namespace swirly {
inline namespace fin {

class SWIRLY_API Request {
  public:
    Request(WallTime created, Symbol accnt, Id64 market_id, Id64 id, std::string_view ref,
            Side side, Lots lots) noexcept
    : created_{created}
    , accnt_{accnt}
    , market_id_{market_id}
    , id_{id}
    , ref_{ref}
    , side_{side}
    , lots_{lots}
    {
    }

    // Copy.
    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;

    // Move.
    Request(Request&&);
    Request& operator=(Request&&) = delete;

    auto created() const noexcept { return created_; }
    auto accnt() const noexcept { return accnt_; }
    auto market_id() const noexcept { return market_id_; }
    auto id() const noexcept { return id_; }
    auto ref() const noexcept { return +ref_; }
    auto side() const noexcept { return side_; }
    auto lots() const noexcept { return lots_; }

  protected:
    ~Request();

    const WallTime created_;
    /**
     * The executing accnt.
     */
    const Symbol accnt_;
    const Id64 market_id_;
    const Id64 id_;
    /**
     * Ref is optional.
     */
    const Ref ref_;
    const Side side_;
    Lots lots_;
};

/**
 * Request set keyed by market and id. Requests are identified by market and id only, so instances
 * should not be used as heterogeneous Request containers, where Requests of different types may
 * share the same identity.
 */
template <typename RequestT>
class RequestIdSet {
    using Key = std::tuple<Id64, Id64>;
    struct Compare {
        using is_transparent = void;
        int compare(const Request& lhs, const Request& rhs) const noexcept
        {
            int result{swirly::compare(lhs.market_id(), rhs.market_id())};
            if (result == 0) {
                result = swirly::compare(lhs.id(), rhs.id());
            }
            return result;
        }
        bool operator()(const Request& lhs, const Request& rhs) const noexcept
        {
            return compare(lhs, rhs) < 0;
        }
        bool operator()(const Key& lhs, const Request& rhs) const noexcept
        {
            int result{swirly::compare(std::get<0>(lhs), rhs.market_id())};
            if (result == 0) {
                result = swirly::compare(std::get<1>(lhs), rhs.id());
            }
            return result < 0;
        }
        bool operator()(const Request& lhs, const Key& rhs) const noexcept
        {
            int result{swirly::compare(lhs.market_id(), std::get<0>(rhs))};
            if (result == 0) {
                result = swirly::compare(lhs.id(), std::get<1>(rhs));
            }
            return result < 0;
        }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<Compare>;
    using MemberHookOption
        = boost::intrusive::member_hook<RequestT, decltype(RequestT::id_hook), &RequestT::id_hook>;
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
    ConstIterator find(Id64 market_id, Id64 id) const noexcept
    {
        return set_.find(std::make_tuple(market_id, id), Compare{});
    }
    Iterator find(Id64 market_id, Id64 id) noexcept
    {
        return set_.find(std::make_tuple(market_id, id), Compare{});
    }
    std::pair<ConstIterator, bool> find_hint(Id64 market_id, Id64 id) const noexcept
    {
        const auto key = std::make_tuple(market_id, id);
        const auto comp = Compare{};
        auto it = set_.lower_bound(key, comp);
        return std::make_pair(it, it != set_.end() && !comp(key, *it));
    }
    std::pair<Iterator, bool> find_hint(Id64 market_id, Id64 id) noexcept
    {
        const auto key = std::make_tuple(market_id, id);
        const auto comp = Compare{};
        auto it = set_.lower_bound(key, comp);
        return std::make_pair(it, it != set_.end() && !comp(key, *it));
    }
    Iterator insert(const ValuePtr& value) noexcept
    {
        auto [it, inserted] = set_.insert(*value);
        if (inserted) {
            // Take ownership if inserted.
            value->add_ref();
        }
        return it;
    }
    Iterator insert_hint(ConstIterator hint, const ValuePtr& value) noexcept
    {
        auto it = set_.insert(hint, *value);
        // Take ownership.
        value->add_ref();
        return it;
    }
    Iterator insert_or_replace(const ValuePtr& value) noexcept
    {
        auto [it, inserted] = set_.insert(*value);
        if (!inserted) {
            // Replace if exists.
            ValuePtr prev{&*it, false};
            set_.replace_node(it, *value);
            it = Set::s_iterator_to(*value);
        }
        // Take ownership.
        value->add_ref();
        return it;
    }
    template <typename... ArgsT>
    Iterator emplace(ArgsT&&... args)
    {
        return insert(make_intrusive<RequestT>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplace_hint(ConstIterator hint, ArgsT&&... args)
    {
        return insert_hint(hint, make_intrusive<RequestT>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplace_or_replace(ArgsT&&... args)
    {
        return insert_or_replace(make_intrusive<RequestT>(std::forward<ArgsT>(args)...));
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
