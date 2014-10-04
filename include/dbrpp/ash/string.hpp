/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#ifndef DBRPP_ASH_STRING_HPP
#define DBRPP_ASH_STRING_HPP

#include <dbr/ash/string.h>

#include <cstring>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace dbr {

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

template <std::size_t N>
struct MaxSize {
    static const std::size_t VALUE = N;
};

class RString {
    const char* begin_;
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
    RString(const char* begin) noexcept
        : begin_{begin},
          size_{-std::numeric_limits<std::ptrdiff_t>::max()}
    {
    }
    RString(const char* begin, std::size_t size) noexcept
        : begin_{begin},
          size_{static_cast<std::ptrdiff_t>(size)}
    {
    }
    template <std::size_t N>
    RString(const char* begin, MaxSize<N>) noexcept
        : begin_{begin},
          size_{-static_cast<std::ptrdiff_t>(MaxSize<N>::VALUE)}
    {
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
    RString
    substr(SizeType pos = 0,
           SizeType count = std::numeric_limits<std::size_t>::max()) const noexcept
    {
        // TODO: keep it simple for now, but may be able to avoid call to size()?
        count = std::min(count, size() - pos);
        return RString(begin() + pos, count);
    }

    // Accessor.

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
to_string(const RString& rstr)
{
    return rstr.str();
}

inline std::ostream&
operator <<(std::ostream& os, const RString& rstr)
{
    // FIXME: write() ignores stream formatters.
    // os.write(rstr.data(), rstr.size());
    // Work-around:
    return os << rstr.str();
}

inline bool
operator ==(const RString& lhs, const RString& rhs) noexcept
{
    return lexncmp(lhs.data(), rhs.data(), lhs.max_size(), rhs.max_size()) == 0;
}

inline bool
operator ==(const RString& lhs, const char* rhs) noexcept
{
    return lexncmp(lhs.data(), rhs, lhs.max_size(), std::numeric_limits<std::size_t>::max()) == 0;
}

inline bool
operator ==(const char* lhs, const RString& rhs) noexcept
{
    return lexncmp(lhs, rhs.data(), std::numeric_limits<std::size_t>::max(), rhs.max_size()) == 0;
}

inline bool
operator !=(const RString& lhs, const RString& rhs) noexcept
{
    return lexncmp(lhs.data(), rhs.data(), lhs.max_size(), rhs.max_size()) != 0;
}

inline bool
operator !=(const RString& lhs, const char* rhs) noexcept
{
    return lexncmp(lhs.data(), rhs, lhs.max_size(), std::numeric_limits<std::size_t>::max()) != 0;
}

inline bool
operator !=(const char* lhs, const RString& rhs) noexcept
{
    return lexncmp(lhs, rhs.data(), std::numeric_limits<std::size_t>::max(), rhs.max_size()) != 0;
}

inline bool
operator <(const RString& lhs, const RString& rhs) noexcept
{
    return lexncmp(lhs.data(), rhs.data(), lhs.max_size(), rhs.max_size()) < 0;
}

inline bool
operator <(const RString& lhs, const char* rhs) noexcept
{
    return lexncmp(lhs.data(), rhs, lhs.max_size(), std::numeric_limits<std::size_t>::max()) < 0;
}

inline bool
operator <(const char* lhs, const RString& rhs) noexcept
{
    return lexncmp(lhs, rhs.data(), std::numeric_limits<std::size_t>::max(), rhs.max_size()) < 0;
}

inline bool
operator <=(const RString& lhs, const RString& rhs) noexcept
{
    return lexncmp(lhs.data(), rhs.data(), lhs.max_size(), rhs.max_size()) <= 0;
}

inline bool
operator <=(const RString& lhs, const char* rhs) noexcept
{
    return lexncmp(lhs.data(), rhs, lhs.max_size(), std::numeric_limits<std::size_t>::max()) <= 0;
}

inline bool
operator <=(const char* lhs, const RString& rhs) noexcept
{
    return lexncmp(lhs, rhs.data(), std::numeric_limits<std::size_t>::max(), rhs.max_size()) <= 0;
}

inline bool
operator >(const RString& lhs, const RString& rhs) noexcept
{
    return lexncmp(lhs.data(), rhs.data(), lhs.max_size(), rhs.max_size()) > 0;
}

inline bool
operator >(const RString& lhs, const char* rhs) noexcept
{
    return lexncmp(lhs.data(), rhs, lhs.max_size(), std::numeric_limits<std::size_t>::max()) > 0;
}

inline bool
operator >(const char* lhs, const RString& rhs) noexcept
{
    return lexncmp(lhs, rhs.data(), std::numeric_limits<std::size_t>::max(), rhs.max_size()) > 0;
}

inline bool
operator >=(const RString& lhs, const RString& rhs) noexcept
{
    return lexncmp(lhs.data(), rhs.data(), lhs.max_size(), rhs.max_size()) >= 0;
}

inline bool
operator >=(const RString& lhs, const char* rhs) noexcept
{
    return lexncmp(lhs.data(), rhs, lhs.max_size(), std::numeric_limits<std::size_t>::max()) >= 0;
}

inline bool
operator >=(const char* lhs, const RString& rhs) noexcept
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

struct RStringHash {

    typedef RString ArgumentType;
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

} // dbr

#endif // DBRPP_ASH_STRING_HPP
