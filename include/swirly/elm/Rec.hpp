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
#ifndef SWIRLY_ELM_REC_HPP
#define SWIRLY_ELM_REC_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Compare.hpp>

#include <boost/intrusive/set.hpp>

#include <memory> // unique_ptr<>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

class SWIRLY_API Rec : public Comparable<Rec> {
 public:
  Rec(RecType type, const std::string_view& mnem, const std::string_view& display) noexcept
    : type_{type},
      mnem_{mnem},
      display_{display}
  {
  }

  virtual ~Rec() noexcept;

  // Copy.
  Rec(const Rec&);
  Rec& operator=(const Rec&) = delete;

  // Move.
  Rec(Rec&&);
  Rec& operator=(Rec&&) = delete;

  int compare(const Rec& rhs) const noexcept
  {
    int result{swirly::compare(type_, rhs.type_)};
    if (result == 0)
      result = mnem_.compare(rhs.mnem_);
    return result;
  }
  auto type() const noexcept { return type_; }
  auto mnem() const noexcept { return +mnem_; }
  auto display() const noexcept { return +display_; }
  void setDisplay(const std::string_view& display) noexcept { display_ = display; }

 protected:
  const RecType type_;
  const Mnem mnem_;
  Display display_;
};

/**
 * Record set keyed by mnemonic. Records are identified by mnemonic only, so instances should not be
 * used as heterogeneous Record containers, where Records of different types may share the same
 * identity.
 */
template <typename RecT>
class RecSet {
  struct ValueCompare {
    bool operator()(const Rec& lhs, const Rec& rhs) const noexcept
    {
      return lhs.mnem() < rhs.mnem();
    }
  };
  struct KeyValueCompare {
    bool operator()(const std::string_view& lhs, const Rec& rhs) const noexcept
    {
      return lhs < rhs.mnem();
    }
    bool operator()(const Rec& lhs, const std::string_view& rhs) const noexcept
    {
      return lhs.mnem() < rhs;
    }
  };
  using ConstantTimeSizeOption = boost::intrusive::constant_time_size<false>;
  using CompareOption = boost::intrusive::compare<ValueCompare>;
  using MemberHookOption
    = boost::intrusive::member_hook<RecT, decltype(RecT::mnemHook_), &RecT::mnemHook_>;
  using Set = boost::intrusive::set<RecT, ConstantTimeSizeOption, CompareOption, MemberHookOption>;
  using ValuePtr = std::unique_ptr<RecT>;

 public:
  using Iterator = typename Set::iterator;
  using ConstIterator = typename Set::const_iterator;

  RecSet() = default;
  ~RecSet() noexcept
  {
    set_.clear_and_dispose([](Rec* ptr) { delete ptr; });
  }

  // Copy.
  RecSet(const RecSet&) = delete;
  RecSet& operator=(const RecSet&) = delete;

  // Move.
  RecSet(RecSet&&) = default;
  RecSet& operator=(RecSet&&) = default;

  // Begin.
  ConstIterator begin() const noexcept { return set_.begin(); }
  ConstIterator cbegin() const noexcept { return set_.cbegin(); }
  Iterator begin() noexcept { return set_.begin(); }

  // End.
  ConstIterator end() const noexcept { return set_.end(); }
  ConstIterator cend() const noexcept { return set_.cend(); }
  Iterator end() noexcept { return set_.end(); }

  // Find.
  ConstIterator find(const std::string_view& mnem) const noexcept
  {
    return set_.find(mnem, KeyValueCompare());
  }
  Iterator find(const std::string_view& mnem) noexcept
  {
    return set_.find(mnem, KeyValueCompare());
  }
  std::pair<ConstIterator, bool> findHint(const std::string_view& mnem) const noexcept
  {
    const auto comp = KeyValueCompare();
    auto it = set_.lower_bound(mnem, comp);
    return std::make_pair(it, it != set_.end() && !comp(mnem, *it));
  }
  std::pair<Iterator, bool> findHint(const std::string_view& mnem) noexcept
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
    return insert(std::make_unique<RecT>(std::forward<ArgsT>(args)...));
  }
  template <typename... ArgsT>
  Iterator emplaceHint(ConstIterator hint, ArgsT&&... args)
  {
    return insertHint(hint, std::make_unique<RecT>(std::forward<ArgsT>(args)...));
  }
  template <typename... ArgsT>
  Iterator emplaceOrReplace(ArgsT&&... args)
  {
    return insertOrReplace(std::make_unique<RecT>(std::forward<ArgsT>(args)...));
  }

 private:
  Set set_;
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_REC_HPP
