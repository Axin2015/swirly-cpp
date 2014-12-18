/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SCPP_ASH_STRING_HPP
#define SCPP_ASH_STRING_HPP

#include <sc/ash/string.h>

#include <cstring>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace sc {

/**
 * @addtogroup String
 * @{
 */

template <typename T>
T
ston(const char* s);

template<>
inline double
ston(const char* s)
{
    errno = 0;
    char* end;
    double d = strtod(s, &end);
    if (0 != errno || *end)
        throw std::invalid_argument(s);
    return d;
}

template<>
inline long
ston(const char* s)
{
    errno = 0;
    char* end;
    long l = strtol(s, &end, 10);
    if (0 != errno || *end)
        throw std::invalid_argument(s);
    return l;
}

template<>
inline int
ston(const char* s)
{
    long l = ston<long>(s);
    if (l < std::numeric_limits<int>::min() || std::numeric_limits<int>::max() < l)
        throw std::invalid_argument(s);
    return static_cast<int>(l);
}

template <typename T>
inline std::string
join(T begin, T end, const char* delim = " ")
{
    std::ostringstream ss;
    std::copy(begin, end, std::ostream_iterator<std::string>(ss, delim));
    return ss.str();
}

inline std::string
join(const std::vector<std::string>& v, const char* delim = " ")
{
    return join(v.cbegin(), v.cend(), delim);
}

inline std::vector<std::string>&
split(const std::string& s, std::vector<std::string>& v)
{
    std::istringstream is(s);
    std::string tok;
    while (is >> tok && tok[0] != '#')
        v.push_back(tok);
    return v;
}

inline std::string
strncpy(const char* src, std::size_t n)
{
    std::string dst;
    for (; n > 0; --n, ++src) {
        if (*src == '\0')
            break;
        dst += *src;
    }
    return dst;
}

inline int
lexncmp(const char* lhs, const char* rhs, std::size_t lhn, std::size_t rhn)
{
    std::size_t n = std::min(lhn, rhn);
    for ( ; n > 0; ++lhs, ++rhs, --n) {
        if (*lhs != *rhs)
            return *lhs < *rhs ? -1 : 1;
        if (*lhs == '\0')
            return 0;
    }
    if (lhn < rhn)
        return *rhs == '\0' ? 0 : -1;
    if (lhn > rhn)
        return *lhs == '\0' ? 0 : 1;
    return 0;
}

class MaxSize {
    size_t max_size_;
public:
    constexpr explicit
    MaxSize(size_t max_size) noexcept
      : max_size_(static_cast<std::ptrdiff_t>(max_size))
    {
    }
    constexpr
    operator std::ptrdiff_t() const noexcept
    {
        return max_size_;
    }
};

class StringRef {
    const char* begin_;
    // Length calculation may be deferred.
    mutable std::ptrdiff_t size_;
public:
    typedef char ValueType;
    typedef const char* ConstPointer;
    typedef const char& ConstReference;

    typedef const char* ConstIterator;
    typedef std::reverse_iterator<ConstIterator> ConstReverseIterator;

    typedef std::ptrdiff_t DifferenceType;
    typedef std::size_t SizeType;

    // Standard typedefs.

    typedef ValueType value_type;
    typedef ConstPointer const_pointer;
    typedef ConstReference const_reference;

    typedef ConstIterator const_iterator;
    typedef ConstReverseIterator const_reverse_iterator;

    typedef DifferenceType difference_type;
    typedef DifferenceType distance_type;
    typedef SizeType size_type;
public:
    constexpr
    StringRef() noexcept
        : begin_{""},
          size_{0}
    {
    }
    constexpr
    StringRef(const char* begin) noexcept
        : begin_{begin},
          size_{-std::numeric_limits<std::ptrdiff_t>::max()}
    {
    }
    constexpr
    StringRef(const char* begin, std::size_t size) noexcept
        : begin_{begin},
          size_{static_cast<std::ptrdiff_t>(size)}
    {
    }
    constexpr
    StringRef(const char* begin, MaxSize max_size) noexcept
        : begin_{begin},
          size_{-static_cast<std::ptrdiff_t>(max_size)}
    {
    }
    void
    rebase(std::ptrdiff_t delta) noexcept
    {
        begin_ += delta;
    }

    // Iterator.

    const_iterator
    begin() const noexcept
    {
        return begin_;
    }
    const_iterator
    end() const noexcept
    {
        return begin() + size();
    }
    ConstReverseIterator
    rbegin() const noexcept
    {
        return ConstReverseIterator{end()};
    }
    ConstReverseIterator
    rend() const noexcept
    {
        return ConstReverseIterator{begin()};
    }
    ConstIterator
    find(char ch) const noexcept
    {
        const char* match = strchr(begin(), ch);
        return match ? ConstIterator{match} : end();
    }
    ConstIterator
    find(const char* str) const noexcept
    {
        const char* match = strstr(begin(), str);
        return match ? ConstIterator{match} : end();
    }
    StringRef
    substr(SizeType pos = 0,
           SizeType count = std::numeric_limits<std::size_t>::max()) const noexcept
    {
        if (size_ < 0) {
            count = std::min(count, max_size() - pos);
            return StringRef(begin() + pos, MaxSize(count));
        }
        count = std::min(count, size() - pos);
        return StringRef(begin() + pos, count);
    }

    // Accessor.

    const char&
    operator [](SizeType pos) const
    {
        return *(begin() + pos);
    }
    ValueType
    front() const noexcept
    {
        return *begin();
    }
    ValueType
    back() const noexcept
    {
        return *(end() - 1);
    }
    SizeType
    size() const noexcept
    {
        if (size_ < 0)
            size_ = strnlen(begin_, -size_);
        return size_;
    }
    SizeType
    max_size() const noexcept
    {
        return size_ < 0 ? -size_ : size_;
    }
    bool
    empty() const noexcept
    {
        return size() == 0;
    }
    const char*
    data() const noexcept
    {
        return begin();
    }
    std::string
    str() const
    {
        return std::string(begin(), end());
    }
};

inline std::string
to_string(const StringRef& rstr)
{
    return rstr.str();
}

inline std::ostream&
operator <<(std::ostream& os, const StringRef& rstr)
{
    std::copy(rstr.begin(), rstr.end(), std::ostream_iterator<char>{os});
    return os;
}

inline bool
operator ==(const StringRef& lhs, const StringRef& rhs) noexcept
{
    return lexncmp(lhs.data(), rhs.data(), lhs.max_size(), rhs.max_size()) == 0;
}

inline bool
operator ==(const StringRef& lhs, const char* rhs) noexcept
{
    return lexncmp(lhs.data(), rhs, lhs.max_size(), std::numeric_limits<std::size_t>::max()) == 0;
}

inline bool
operator ==(const char* lhs, const StringRef& rhs) noexcept
{
    return lexncmp(lhs, rhs.data(), std::numeric_limits<std::size_t>::max(), rhs.max_size()) == 0;
}

inline bool
operator !=(const StringRef& lhs, const StringRef& rhs) noexcept
{
    return lexncmp(lhs.data(), rhs.data(), lhs.max_size(), rhs.max_size()) != 0;
}

inline bool
operator !=(const StringRef& lhs, const char* rhs) noexcept
{
    return lexncmp(lhs.data(), rhs, lhs.max_size(), std::numeric_limits<std::size_t>::max()) != 0;
}

inline bool
operator !=(const char* lhs, const StringRef& rhs) noexcept
{
    return lexncmp(lhs, rhs.data(), std::numeric_limits<std::size_t>::max(), rhs.max_size()) != 0;
}

inline bool
operator <(const StringRef& lhs, const StringRef& rhs) noexcept
{
    return lexncmp(lhs.data(), rhs.data(), lhs.max_size(), rhs.max_size()) < 0;
}

inline bool
operator <(const StringRef& lhs, const char* rhs) noexcept
{
    return lexncmp(lhs.data(), rhs, lhs.max_size(), std::numeric_limits<std::size_t>::max()) < 0;
}

inline bool
operator <(const char* lhs, const StringRef& rhs) noexcept
{
    return lexncmp(lhs, rhs.data(), std::numeric_limits<std::size_t>::max(), rhs.max_size()) < 0;
}

inline bool
operator <=(const StringRef& lhs, const StringRef& rhs) noexcept
{
    return lexncmp(lhs.data(), rhs.data(), lhs.max_size(), rhs.max_size()) <= 0;
}

inline bool
operator <=(const StringRef& lhs, const char* rhs) noexcept
{
    return lexncmp(lhs.data(), rhs, lhs.max_size(), std::numeric_limits<std::size_t>::max()) <= 0;
}

inline bool
operator <=(const char* lhs, const StringRef& rhs) noexcept
{
    return lexncmp(lhs, rhs.data(), std::numeric_limits<std::size_t>::max(), rhs.max_size()) <= 0;
}

inline bool
operator >(const StringRef& lhs, const StringRef& rhs) noexcept
{
    return lexncmp(lhs.data(), rhs.data(), lhs.max_size(), rhs.max_size()) > 0;
}

inline bool
operator >(const StringRef& lhs, const char* rhs) noexcept
{
    return lexncmp(lhs.data(), rhs, lhs.max_size(), std::numeric_limits<std::size_t>::max()) > 0;
}

inline bool
operator >(const char* lhs, const StringRef& rhs) noexcept
{
    return lexncmp(lhs, rhs.data(), std::numeric_limits<std::size_t>::max(), rhs.max_size()) > 0;
}

inline bool
operator >=(const StringRef& lhs, const StringRef& rhs) noexcept
{
    return lexncmp(lhs.data(), rhs.data(), lhs.max_size(), rhs.max_size()) >= 0;
}

inline bool
operator >=(const StringRef& lhs, const char* rhs) noexcept
{
    return lexncmp(lhs.data(), rhs, lhs.max_size(), std::numeric_limits<std::size_t>::max()) >= 0;
}

inline bool
operator >=(const char* lhs, const StringRef& rhs) noexcept
{
    return lexncmp(lhs, rhs.data(), std::numeric_limits<std::size_t>::max(), rhs.max_size()) >= 0;
}

inline std::size_t
strnhash(const char* s, std::size_t n) noexcept
{
    std::size_t h(0);
    for (; *s && n > 0; ++s, --n)
        h = 31 * h + *s;
    return h;
}

struct StringRefHash {

    typedef StringRef ArgumentType;
    typedef std::size_t ResultType;

    // Standard typedefs.

    typedef ArgumentType argument_type;
    typedef ResultType result_type;

    ResultType
    operator ()(const ArgumentType& rstr) const noexcept
    {
        return strnhash(rstr.data(), rstr.max_size());
    }
};

/** @} */

} // sc

#endif // SCPP_ASH_STRING_HPP
