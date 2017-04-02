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
#ifndef SWIRLY_UTIL_SET_HPP
#define SWIRLY_UTIL_SET_HPP

#include <swirly/util/BasicTypes.hpp>
#include <swirly/util/Mnem.hpp>
#include <swirly/util/RefCounted.hpp>

#include <boost/intrusive/set.hpp>

namespace swirly {

template <typename ValueT>
struct IdTraits {
    using Id = Id64;
    static Id id(const ValueT& value) noexcept { return value.id(); }
};

/**
 * Set keyed by identifier.
 */
template <typename ValueT, typename TraitsT = IdTraits<ValueT>>
class IdSet {
    using Id = typename TraitsT::Id;
    struct ValueCompare {
        bool operator()(const ValueT& lhs, const ValueT& rhs) const noexcept
        {
            return TraitsT::id(lhs) < TraitsT::id(rhs);
        }
    };
    struct KeyValueCompare {
        bool operator()(Id lhs, const ValueT& rhs) const noexcept { return lhs < TraitsT::id(rhs); }
        bool operator()(const ValueT& lhs, Id rhs) const noexcept { return TraitsT::id(lhs) < rhs; }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<ValueCompare>;
    using MemberHookOption
        = boost::intrusive::member_hook<ValueT, decltype(ValueT::idHook_), &ValueT::idHook_>;
    using Set
        = boost::intrusive::set<ValueT, ConstantTimeSizeOption, CompareOption, MemberHookOption>;
    using ValuePtr = boost::intrusive_ptr<ValueT>;

  public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    IdSet() = default;
    ~IdSet() noexcept
    {
        set_.clear_and_dispose([](const ValueT* ptr) { ptr->release(); });
    }

    // Copy.
    IdSet(const IdSet&) = delete;
    IdSet& operator=(const IdSet&) = delete;

    // Move.
    IdSet(IdSet&&) = default;
    IdSet& operator=(IdSet&&) = default;

    // Begin.
    ConstIterator begin() const noexcept { return set_.begin(); }
    ConstIterator cbegin() const noexcept { return set_.cbegin(); }
    Iterator begin() noexcept { return set_.begin(); }

    // End.
    ConstIterator end() const noexcept { return set_.end(); }
    ConstIterator cend() const noexcept { return set_.cend(); }
    Iterator end() noexcept { return set_.end(); }

    // Find.
    ConstIterator find(Id id) const noexcept { return set_.find(id, KeyValueCompare()); }
    Iterator find(Id id) noexcept { return set_.find(id, KeyValueCompare()); }
    std::pair<ConstIterator, bool> findHint(Id id) const noexcept
    {
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(id, comp);
        return std::make_pair(it, it != set_.end() && !comp(id, *it));
    }
    std::pair<Iterator, bool> findHint(Id id) noexcept
    {
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(id, comp);
        return std::make_pair(it, it != set_.end() && !comp(id, *it));
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
        return insert(makeRefCounted<ValueT>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplaceHint(ConstIterator hint, ArgsT&&... args)
    {
        return insertHint(hint, makeRefCounted<ValueT>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplaceOrReplace(ArgsT&&... args)
    {
        return insertOrReplace(makeRefCounted<ValueT>(std::forward<ArgsT>(args)...));
    }
    ValuePtr remove(const ValueT& ref) noexcept
    {
        ValuePtr value;
        set_.erase_and_dispose(ref, [&value](ValueT* ptr) { value = ValuePtr{ptr, false}; });
        return value;
    }

  private:
    Set set_;
};

/**
 * Set keyed by mnemonic.
 */
template <typename ValueT>
class MnemSet {
    struct ValueCompare {
        bool operator()(const ValueT& lhs, const ValueT& rhs) const noexcept
        {
            return lhs.mnem() < rhs.mnem();
        }
    };
    struct KeyValueCompare {
        bool operator()(Mnem lhs, const ValueT& rhs) const noexcept { return lhs < rhs.mnem(); }
        bool operator()(const ValueT& lhs, Mnem rhs) const noexcept { return lhs.mnem() < rhs; }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<ValueCompare>;
    using MemberHookOption
        = boost::intrusive::member_hook<ValueT, decltype(ValueT::mnemHook_), &ValueT::mnemHook_>;
    using Set
        = boost::intrusive::set<ValueT, ConstantTimeSizeOption, CompareOption, MemberHookOption>;
    using ValuePtr = std::unique_ptr<ValueT>;

  public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    MnemSet() = default;
    ~MnemSet() noexcept
    {
        set_.clear_and_dispose([](ValueT* ptr) { delete ptr; });
    }

    // Copy.
    MnemSet(const MnemSet&) = delete;
    MnemSet& operator=(const MnemSet&) = delete;

    // Move.
    MnemSet(MnemSet&&) = default;
    MnemSet& operator=(MnemSet&&) = default;

    // Begin.
    ConstIterator begin() const noexcept { return set_.begin(); }
    ConstIterator cbegin() const noexcept { return set_.cbegin(); }
    Iterator begin() noexcept { return set_.begin(); }

    // End.
    ConstIterator end() const noexcept { return set_.end(); }
    ConstIterator cend() const noexcept { return set_.cend(); }
    Iterator end() noexcept { return set_.end(); }

    // Find.
    ConstIterator find(Mnem mnem) const noexcept { return set_.find(mnem, KeyValueCompare()); }
    Iterator find(Mnem mnem) noexcept { return set_.find(mnem, KeyValueCompare()); }
    std::pair<ConstIterator, bool> findHint(Mnem mnem) const noexcept
    {
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(mnem, comp);
        return std::make_pair(it, it != set_.end() && !comp(mnem, *it));
    }
    std::pair<Iterator, bool> findHint(Mnem mnem) noexcept
    {
        const auto comp = KeyValueCompare();
        auto it = set_.lower_bound(mnem, comp);
        return std::make_pair(it, it != set_.end() && !comp(mnem, *it));
    }
    Iterator insert(ValuePtr value) noexcept
    {
        Iterator it;
        bool inserted;
        std::tie(it, inserted) = set_.insert(*value);
        if (inserted) {
            // Take ownership if inserted.
            value.release();
        }
        return it;
    }
    Iterator insertHint(ConstIterator hint, ValuePtr value) noexcept
    {
        auto it = set_.insert(hint, *value);
        // Take ownership.
        value.release();
        return it;
    }
    Iterator insertOrReplace(ValuePtr value) noexcept
    {
        Iterator it;
        bool inserted;
        std::tie(it, inserted) = set_.insert(*value);
        if (!inserted) {
            // Replace if exists.
            ValuePtr prev{&*it};
            set_.replace_node(it, *value);
            it = Set::s_iterator_to(*value);
        }
        // Take ownership.
        value.release();
        return it;
    }
    template <typename... ArgsT>
    Iterator emplace(ArgsT&&... args)
    {
        return insert(std::make_unique<ValueT>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplaceHint(ConstIterator hint, ArgsT&&... args)
    {
        return insertHint(hint, std::make_unique<ValueT>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplaceOrReplace(ArgsT&&... args)
    {
        return insertOrReplace(std::make_unique<ValueT>(std::forward<ArgsT>(args)...));
    }

  private:
    Set set_;
};

} // swirly

#endif // SWIRLY_UTIL_SET_HPP
