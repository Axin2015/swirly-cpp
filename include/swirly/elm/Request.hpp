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
#ifndef SWIRLY_ELM_REQUEST_HPP
#define SWIRLY_ELM_REQUEST_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/RefCounted.hpp>
#include <swirly/ash/Types.hpp>

#include <boost/intrusive/set.hpp>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

class SWIRLY_API Request : public RefCounted {
 protected:
    /**
     * The executing trader.
     */
    const Mnem trader_;
    const Mnem market_;
    const Mnem contr_;
    const Jd settlDay_;
    const Iden id_;
    /**
     * Ref is optional.
     */
    const Ref ref_;
    const Side side_;
    Lots lots_;
    const Millis created_;

 public:
    Request(const StringView& trader, const StringView& market, const StringView& contr,
            Jd settlDay, Iden id, const StringView& ref, Side side, Lots lots,
            Millis created) noexcept
    :   trader_{trader},
        market_{market},
        contr_{contr},
        settlDay_{settlDay},
        id_{id},
        ref_{ref},
        side_{side},
        lots_{lots},
        created_{created}
    {
    }

    ~Request() noexcept override;

    // Copy.
    Request(const Request&) = default;
    Request& operator =(const Request&) = default;

    // Move.
    Request(Request&&) = default;
    Request& operator =(Request&&) = default;

    StringView trader() const noexcept
    {
        return trader_.view();
    }
    StringView market() const noexcept
    {
        return market_.view();
    }
    StringView contr() const noexcept
    {
        return contr_.view();
    }
    Jd settlDay() const noexcept
    {
        return settlDay_;
    }
    Iden id() const noexcept
    {
        return id_;
    }
    StringView ref() const noexcept
    {
        return ref_.view();
    }
    Side side() const noexcept
    {
        return side_;
    }
    Lots lots() const noexcept
    {
        return lots_;
    }
    Millis created() const noexcept
    {
        return created_;
    }
};

using RequestPtr = boost::intrusive_ptr<Request>;

/**
 * Request set keyed by market and id. Requests are identified by market and id only, so instances
 * should not be used as heterogeneous Request containers, where Requests of different types may
 * share the same identity.
 */
template <typename RequestT>
class RequestIdSet {
    using Key = std::tuple<StringView, Iden>;
    struct ValueCompare {
        int compare(const Request& lhs, const Request& rhs) const noexcept
        {
            int result{lhs.market().compare(rhs.market())};
            if (result == 0)
                result = swirly::compare(lhs.id(), rhs.id());
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
            int result{std::get<0>(lhs).compare(rhs.market())};
            if (result == 0)
                result = swirly::compare(std::get<1>(lhs), rhs.id());
            return result < 0;
        }
        bool operator()(const Request& lhs, const Key& rhs) const noexcept
        {
            int result{lhs.market().compare(std::get<0>(rhs))};
            if (result == 0)
                result = swirly::compare(lhs.id(), std::get<1>(rhs));
            return result < 0;
        }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<ValueCompare>;
    using MemberHookOption = boost::intrusive::member_hook<RequestT, decltype(RequestT::refHook_),
                                                           &RequestT::refHook_>;
    using Set = boost::intrusive::set<RequestT,
                                      ConstantTimeSizeOption,
                                      CompareOption,
                                      MemberHookOption
                                      >;
    using ValuePtr = boost::intrusive_ptr<RequestT>;

    Set set_;
 public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    RequestIdSet() = default;

    ~RequestIdSet() noexcept
    {
        set_.clear_and_dispose([](Request* ptr) { ptr->release(); });
    }

    // Copy.
    RequestIdSet(const RequestIdSet&) = delete;
    RequestIdSet& operator =(const RequestIdSet&) = delete;

    // Move.
    RequestIdSet(RequestIdSet&&) = default;
    RequestIdSet& operator =(RequestIdSet&&) = default;

    ValuePtr insert(const ValuePtr& request) noexcept
    {
        auto result = set_.insert(*request);
        if (result.second) {
            // Take ownership if inserted.
            request->addRef();
        }
        return &*result.first;
    }

    ValuePtr insertOrReplace(const ValuePtr& request) noexcept
    {
        auto result = set_.insert(*request);
        if (!result.second) {
            // Replace if exists.
            auto& prev = *result.first;
            set_.replace_node(result.first, *request);
            prev.release();
        }
        // Take ownership.
        request->addRef();
        return request;
    }

    template <typename... ArgsT>
    ValuePtr emplace(ArgsT&&... args)
    {
        return insert(makeRefCounted<RequestT>(std::forward<ArgsT>(args)...));
    }

    template <typename... ArgsT>
    ValuePtr emplaceOrReplace(ArgsT&&... args)
    {
        return insertOrReplace(makeRefCounted<RequestT>(std::forward<ArgsT>(args)...));
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
    Iterator find(const StringView& market, Iden id) noexcept
    {
        return set_.find(std::make_tuple(market, id), KeyValueCompare());
    }
    ConstIterator find(const StringView& market, Iden id) const noexcept
    {
        return set_.find(std::make_tuple(market, id), KeyValueCompare());
    }
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_REQUEST_HPP
