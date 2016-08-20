/*
 * The Restful Matching-Engine.
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
#ifndef SWIRLY_ASH_MNEM_HPP
#define SWIRLY_ASH_MNEM_HPP

#include <swirly/ash/Compare.hpp>
#include <swirly/ash/Defs.hpp>

#include <boost/intrusive/set.hpp>

#include <experimental/string_view>

#include <cstddef> // ptrdiff_t
#include <cstdint>
#include <cstring>
#include <memory>

namespace std {
using experimental::string_view;
}

namespace swirly {

/**
 * Maximum mnemonic characters.
 */
constexpr std::size_t MaxMnem{16};

/**
 * Memorable identifier.
 */
class Mnem {
 public:
  using value_type = char;

  using pointer = const char*;
  using const_pointer = const char*;

  using reference = const char&;
  using const_reference = const char&;

  using const_iterator = const char*;
  using iterator = const_iterator;

  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator = const_reverse_iterator;

  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  Mnem(std::string_view rhs) noexcept { assign(rhs.data(), rhs.size()); }
  constexpr Mnem() noexcept : u64_{0, 0} {}
  ~Mnem() noexcept = default;

  // Copy.
  constexpr Mnem(const Mnem& rhs) noexcept : u64_{rhs.u64_[0], rhs.u64_[1]} {}
  constexpr Mnem& operator=(const Mnem& rhs) noexcept
  {
    u64_[0] = rhs.u64_[0];
    u64_[1] = rhs.u64_[1];
    return *this;
  }

  // Move.
  constexpr Mnem(Mnem&&) noexcept = default;
  Mnem& operator=(Mnem&&) noexcept = default;

  Mnem& operator=(std::string_view rhs) noexcept
  {
    assign(rhs.data(), rhs.size());
    return *this;
  }
  constexpr const_iterator begin() const noexcept { return buf_; }
  const_iterator end() const noexcept { return buf_ + size(); }
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
  constexpr const_iterator cbegin() const noexcept { return buf_; }
  const_iterator cend() const noexcept { return buf_ + size(); }
  const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
  const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
  constexpr const char& operator[](size_type pos) const noexcept { return buf_[pos]; }
  constexpr const char& front() const noexcept { return buf_[0]; }
  const char& back() const noexcept { return buf_[size() - 1]; }

  int compare(const Mnem& rhs) const noexcept
  {
    // This function is on the critical path. Avoid memcmp when possible.
    return equal(rhs) ? 0 : std::memcmp(buf_, rhs.buf_, sizeof(buf_));
  }
  int compare(std::string_view rhs) const noexcept { return compare(rhs.data(), rhs.size()); }
  constexpr const char* data() const noexcept { return buf_; }
  constexpr bool empty() const noexcept { return u64_[0] == 0 && u64_[1] == 0; }
  constexpr bool equal(const Mnem& rhs) const noexcept
  {
    return u64_[0] == rhs.u64_[0] && u64_[1] == rhs.u64_[1];
  }
  std::size_t size() const noexcept { return strnlen(buf_, sizeof(buf_)); }
  constexpr void clear() noexcept
  {
    u64_[0] = 0;
    u64_[1] = 0;
  }

 private:
  int compare(const char* rdata, std::size_t rlen) const noexcept
  {
    const std::size_t len{size()};
    int result{std::memcmp(buf_, rdata, std::min(len, rlen))};
    if (result == 0) {
      result = swirly::compare(len, rlen);
    }
    return result;
  }
  constexpr void assign(const char* rdata, std::size_t rlen) noexcept
  {
    const std::size_t len{std::min(sizeof(buf_), rlen)};
    std::size_t i{0};
    for (; i < len; ++i) {
      buf_[i] = rdata[i];
    }
    for (; i < sizeof(buf_); ++i) {
      buf_[i] = '\0';
    }
  }
  union {
    int64_t u64_[2];
    char buf_[MaxMnem];
  };
};

inline std::string_view operator+(Mnem mnem) noexcept
{
  return {mnem.data(), mnem.size()};
}

constexpr bool operator==(Mnem lhs, Mnem rhs) noexcept
{
  return lhs.equal(rhs);
}

inline bool operator==(Mnem lhs, std::string_view rhs) noexcept
{
  return lhs.compare(rhs) == 0;
}

inline bool operator==(std::string_view lhs, Mnem rhs) noexcept
{
  return 0 == rhs.compare(lhs);
}

constexpr bool operator!=(Mnem lhs, Mnem rhs) noexcept
{
  return !lhs.equal(rhs);
}

inline bool operator!=(Mnem lhs, std::string_view rhs) noexcept
{
  return lhs.compare(rhs) != 0;
}

inline bool operator!=(std::string_view lhs, Mnem rhs) noexcept
{
  return 0 != rhs.compare(lhs);
}

inline bool operator<(Mnem lhs, Mnem rhs) noexcept
{
  return lhs.compare(rhs) < 0;
}

inline bool operator<(Mnem lhs, std::string_view rhs) noexcept
{
  return lhs.compare(rhs) < 0;
}

inline bool operator<(std::string_view lhs, Mnem rhs) noexcept
{
  return 0 < rhs.compare(lhs);
}

inline bool operator<=(Mnem lhs, Mnem rhs) noexcept
{
  return lhs.compare(rhs) <= 0;
}

inline bool operator<=(Mnem lhs, std::string_view rhs) noexcept
{
  return lhs.compare(rhs) <= 0;
}

inline bool operator<=(std::string_view lhs, Mnem rhs) noexcept
{
  return 0 <= rhs.compare(lhs);
}

inline bool operator>(Mnem lhs, Mnem rhs) noexcept
{
  return lhs.compare(rhs) > 0;
}

inline bool operator>(Mnem lhs, std::string_view rhs) noexcept
{
  return lhs.compare(rhs) > 0;
}

inline bool operator>(std::string_view lhs, Mnem rhs) noexcept
{
  return 0 > rhs.compare(lhs);
}

inline bool operator>=(Mnem lhs, Mnem rhs) noexcept
{
  return lhs.compare(rhs) >= 0;
}

inline bool operator>=(Mnem lhs, std::string_view rhs) noexcept
{
  return lhs.compare(rhs) >= 0;
}

inline bool operator>=(std::string_view lhs, Mnem rhs) noexcept
{
  return 0 >= rhs.compare(lhs);
}

inline std::ostream& operator<<(std::ostream& os, Mnem rhs)
{
  return os << +rhs;
}

template <std::size_t SizeN>
void setCString(char (&lhs)[SizeN], Mnem rhs) noexcept
{
  constexpr std::size_t len{std::min(SizeN, MaxMnem)};
  std::size_t i{0};
  for (; i < len; ++i) {
    lhs[i] = rhs[i];
  }
  for (; i < SizeN; ++i) {
    lhs[i] = '\0';
  }
}

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

#endif // SWIRLY_ASH_MNEM_HPP
