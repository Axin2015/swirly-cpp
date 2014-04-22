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
#ifndef DBRPP_STRING_HPP
#define DBRPP_STRING_HPP

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
strncpy(const char* src, size_t n)
{
    std::string dst;
    for (; n > 0; --n, ++src) {
        if (*src == '\0')
            break;
        dst += *src;
    }
    return dst;
}

template <size_t MaxN>
class NString {
    const char* begin_;
    mutable size_t size_;
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
    NString(const char* begin, size_t size = std::numeric_limits<SizeType>::max()) noexcept
        : begin_{begin},
          size_{size}
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
        return begin_ + size();
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
        const char* match{strchr(begin(), ch)};
        return match ? ConstIterator{match} : end();
    }
    ConstIterator
    find(const char* str) const noexcept
    {
        const char* match{strstr(begin(), str)};
        return match ? ConstIterator{match} : end();
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
        if (size_ > MaxN)
            size_ = strnlen(begin_, MaxN);
        return size_;
    }
    SizeType
    max_size() const noexcept
    {
        return MaxN;
    }
    bool
    empty() const noexcept
    {
        return size() == 0;
    }
    const char*
    data() const noexcept
    {
        return begin_;
    }
    std::string
    str() const
    {
        return strncpy(begin_, MaxN);
    }
};

template <std::size_t MaxN>
std::ostream&
operator <<(std::ostream& os, const NString<MaxN>& nstr)
{
    // FIXME: write() ignores stream formatters.
    // os.write(nstr.data(), nstr.size());
    // Work-around:
    return os << nstr.str();
}

template <std::size_t MaxN>
bool
operator ==(const NString<MaxN>& lhs, const NString<MaxN>& rhs) noexcept
{
    return strncmp(lhs.data(), rhs.data(), MaxN) == 0;
}

template <std::size_t MaxN>
bool
operator !=(const NString<MaxN>& lhs, const NString<MaxN>& rhs) noexcept
{
    return strncmp(lhs.data(), rhs.data(), MaxN) != 0;
}

template <std::size_t MaxN>
bool
operator <(const NString<MaxN>& lhs, const NString<MaxN>& rhs) noexcept
{
    return strncmp(lhs.data(), rhs.data(), MaxN) < 0;
}

template <std::size_t MaxN>
bool
operator <=(const NString<MaxN>& lhs, const NString<MaxN>& rhs) noexcept
{
    return strncmp(lhs.data(), rhs.data(), MaxN) <= 0;
}

template <std::size_t MaxN>
bool
operator >(const NString<MaxN>& lhs, const NString<MaxN>& rhs) noexcept
{
    return strncmp(lhs.data(), rhs.data(), MaxN) > 0;
}

template <std::size_t MaxN>
bool
operator >=(const NString<MaxN>& lhs, const NString<MaxN>& rhs) noexcept
{
    return strncmp(lhs.data(), rhs.data(), MaxN) >= 0;
}

inline std::size_t
strnhash(const char* s, std::size_t n) noexcept
{
    std::size_t h(0);
    for (; *s && n > 0; ++s, --n)
        h = 31 * h + *s;
    return h;
}

template <std::size_t MaxN>
struct NStringHash {

    typedef NString<MaxN> ArgumentType;
    typedef std::size_t ResultType;

    // Standard typedefs.

    typedef ArgumentType argument_type;
    typedef ResultType result_type;

    ResultType
    operator ()(const ArgumentType& nstr) const noexcept
    {
        return strnhash(nstr.data(), MaxN);
    }
};

/** @} */

} // dbr

#endif // DBRPP_STRING_HPP
