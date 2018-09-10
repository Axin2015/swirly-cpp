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
#ifndef SWIRLY_UTIL_SET_HPP
#define SWIRLY_UTIL_SET_HPP

#include <swirly/util/IntTypes.hpp>
#include <swirly/util/RefCount.hpp>
#include <swirly/util/Symbol.hpp>

#include <boost/intrusive/set.hpp>

namespace swirly {
inline namespace util {

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
    using Key = Id;
    struct Compare {
        using is_transparent = void;
        bool operator()(const ValueT& lhs, const ValueT& rhs) const noexcept
        {
            return TraitsT::id(lhs) < TraitsT::id(rhs);
        }
        bool operator()(Id lhs, const ValueT& rhs) const noexcept { return lhs < TraitsT::id(rhs); }
        bool operator()(const ValueT& lhs, Id rhs) const noexcept { return TraitsT::id(lhs) < rhs; }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<Compare>;
    using MemberHookOption
        = boost::intrusive::member_hook<ValueT, decltype(ValueT::id_hook), &ValueT::id_hook>;
    using Set
        = boost::intrusive::set<ValueT, ConstantTimeSizeOption, CompareOption, MemberHookOption>;
    using ValuePtr = boost::intrusive_ptr<ValueT>;

  public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    IdSet() = default;
    ~IdSet()
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
    ConstIterator find(Id id) const noexcept { return set_.find(id, Compare{}); }
    Iterator find(Id id) noexcept { return set_.find(id, Compare{}); }
    std::pair<ConstIterator, bool> find_hint(Id id) const noexcept
    {
        const auto comp = Compare{};
        auto it = set_.lower_bound(id, comp);
        return std::make_pair(it, it != set_.end() && !comp(id, *it));
    }
    std::pair<Iterator, bool> find_hint(Id id) noexcept
    {
        const auto comp = Compare{};
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
        value->add_ref();
        return it;
    }
    template <typename... ArgsT>
    Iterator emplace(ArgsT&&... args)
    {
        return insert(make_intrusive<ValueT>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplace_hint(ConstIterator hint, ArgsT&&... args)
    {
        return insert_hint(hint, make_intrusive<ValueT>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplace_or_replace(ArgsT&&... args)
    {
        return insert_or_replace(make_intrusive<ValueT>(std::forward<ArgsT>(args)...));
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

template <typename ValueT>
struct SymbolTraits {
    static Symbol symbol(const ValueT& value) noexcept { return value.symbol(); }
};

/**
 * Set keyed by symbol.
 */
template <typename ValueT, typename TraitsT = SymbolTraits<ValueT>>
class SymbolSet {
    using Key = Symbol;
    struct Compare {
        using is_transparent = void;
        bool operator()(const ValueT& lhs, const ValueT& rhs) const noexcept
        {
            return TraitsT::symbol(lhs) < TraitsT::symbol(rhs);
        }
        bool operator()(Symbol lhs, const ValueT& rhs) const noexcept
        {
            return lhs < TraitsT::symbol(rhs);
        }
        bool operator()(const ValueT& lhs, Symbol rhs) const noexcept
        {
            return TraitsT::symbol(lhs) < rhs;
        }
    };
    using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
    using CompareOption = boost::intrusive::compare<Compare>;
    using MemberHookOption = boost::intrusive::member_hook<ValueT, decltype(ValueT::symbol_hook),
                                                           &ValueT::symbol_hook>;
    using Set
        = boost::intrusive::set<ValueT, ConstantTimeSizeOption, CompareOption, MemberHookOption>;
    using ValuePtr = std::unique_ptr<ValueT>;

  public:
    using Iterator = typename Set::iterator;
    using ConstIterator = typename Set::const_iterator;

    SymbolSet() = default;
    ~SymbolSet()
    {
        set_.clear_and_dispose([](ValueT* ptr) { delete ptr; });
    }

    // Copy.
    SymbolSet(const SymbolSet&) = delete;
    SymbolSet& operator=(const SymbolSet&) = delete;

    // Move.
    SymbolSet(SymbolSet&&) = default;
    SymbolSet& operator=(SymbolSet&&) = default;

    // Begin.
    ConstIterator begin() const noexcept { return set_.begin(); }
    ConstIterator cbegin() const noexcept { return set_.cbegin(); }
    Iterator begin() noexcept { return set_.begin(); }

    // End.
    ConstIterator end() const noexcept { return set_.end(); }
    ConstIterator cend() const noexcept { return set_.cend(); }
    Iterator end() noexcept { return set_.end(); }

    // Find.
    ConstIterator find(Symbol symbol) const noexcept { return set_.find(symbol, Compare{}); }
    Iterator find(Symbol symbol) noexcept { return set_.find(symbol, Compare{}); }
    std::pair<ConstIterator, bool> find_hint(Symbol symbol) const noexcept
    {
        const auto comp = Compare{};
        auto it = set_.lower_bound(symbol, comp);
        return std::make_pair(it, it != set_.end() && !comp(symbol, *it));
    }
    std::pair<Iterator, bool> find_hint(Symbol symbol) noexcept
    {
        const auto comp = Compare{};
        auto it = set_.lower_bound(symbol, comp);
        return std::make_pair(it, it != set_.end() && !comp(symbol, *it));
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
    Iterator insert_hint(ConstIterator hint, ValuePtr value) noexcept
    {
        auto it = set_.insert(hint, *value);
        // Take ownership.
        value.release();
        return it;
    }
    Iterator insert_or_replace(ValuePtr value) noexcept
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
    Iterator emplace_hint(ConstIterator hint, ArgsT&&... args)
    {
        return insert_hint(hint, std::make_unique<ValueT>(std::forward<ArgsT>(args)...));
    }
    template <typename... ArgsT>
    Iterator emplace_or_replace(ArgsT&&... args)
    {
        return insert_or_replace(std::make_unique<ValueT>(std::forward<ArgsT>(args)...));
    }

  private:
    Set set_;
};

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_SET_HPP
