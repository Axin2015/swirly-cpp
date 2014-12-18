/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "test.hpp"

#include <scpp/ash/string.hpp>

using namespace sc;

TEST_CASE(string_lexncmp)
{
    check(lexncmp("", "", 2, 2) == 0);
    check(lexncmp("a", "a", 2, 2) == 0);
    check(lexncmp("a", "ab", 2, 2) < 0);
    check(lexncmp("ab", "a", 2, 2) > 0);
    check(lexncmp("ab", "ab", 2, 2) == 0);
    check(lexncmp("ab", "ab", 2, 3) == 0);
    check(lexncmp("ab", "ab", 3, 2) == 0);

    check(lexncmp("ab", "abc", 2, 3) < 0);
    check(lexncmp("abc", "ab", 3, 2) > 0);
}

TEST_CASE(string_equality)
{
    StringRef s{"abcxxx", MaxSize{3}};

    check(s == "abc");
    check("abc" == s);

    check(s != "abcxxx");
    check("abcxxx" != s);
}

TEST_CASE(string_substr)
{
    StringRef s{"0123456789", MaxSize{10}};

    check(s.substr() == "0123456789");
    check(s.substr(4) == "456789");
    check(s.substr(0, 4) == "0123");
    check(s.substr(4, 4) == "4567");
}

TEST_CASE(string_size)
{
    StringRef s{"0123456789"};
    check(s.max_size() == std::numeric_limits<std::ptrdiff_t>::max());
    check(s.size() == 10);
    check(s.max_size() == 10);

    s = StringRef{"0123456789", 5};
    check(s.max_size() == 5);
    check(s.size() == 5);

    s = StringRef{"01234", MaxSize{10}};
    check(s.max_size() == 10);
    check(s.size() == 5);
    check(s.max_size() == 5);

    s = StringRef{"0123456789", MaxSize{5}};
    check(s.max_size() == 5);
    check(s.size() == 5);
    check(s.max_size() == 5);

    s = StringRef{"0123456789"};
    check(s.substr().size() == 10);
    check(s.substr(4).size() == 6);
    check(s.substr(0, 4).size() == 4);
    check(s.substr(4, 4).size() == 4);
}
