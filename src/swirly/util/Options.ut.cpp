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
#include "Options.hpp"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(OptionsSuite)

BOOST_AUTO_TEST_CASE(OptionsNormalCase)
{
    int var{-1};
    bool flag{false};
    std::string command;

    Options opts{"Unit Test options [OPTIONS] [COMMAND]"};
    // clang-format off
    opts('o', "option", Value{var}.default_value(3).required(), "Option Description")
        ('s', NoOp{}, "ShortOption Description")
        ("long_opt", NoOp{}, "LongOption Description")
        ('x', Switch{flag}, "Switch Description")
        (Value{command}, "Positional Command");
    // clang-format on

    BOOST_CHECK_EQUAL(var, 3);

    stringstream ss;
    ss << opts;
    const string expected{"Usage: Unit Test options [OPTIONS] [COMMAND]\n"
                          "Options:\n"
                          "  --long_opt     LongOption Description\n"
                          "  -o, --option   Option Description\n"
                          "  -s             ShortOption Description\n"
                          "  -x             Switch Description\n"};
    BOOST_CHECK_EQUAL(ss.str(), expected);

    {
        const char* argv[] = {"executable_name", "-o", "123", "print"};
        int argc = 4;

        opts.parse(argc, const_cast<char**>(argv));
        BOOST_CHECK_EQUAL(var, 123);
        BOOST_CHECK_EQUAL(flag, false);
        BOOST_CHECK_EQUAL(command, "print");
    }
    {
        const char* argv[] = {"executable_name", "-x"};
        int argc = 2;

        opts.parse(argc, const_cast<char**>(argv));
        BOOST_CHECK_EQUAL(flag, true);
    }
}

BOOST_AUTO_TEST_CASE(OptionsExceptionsCase)
{
    int var{-1};

    Options opts{"Exception Test options"};
    opts('l', "long_option", Value{var}, "LongOption Description");

    // Trying to add a duplicate option fails
    BOOST_CHECK_THROW(opts('x', "long_option", Value{var}, "Description"), std::runtime_error);
    BOOST_CHECK_THROW(opts('l', "l_exists", Value{var}, "Description"), std::runtime_error);
    BOOST_CHECK_THROW(opts('l', "long_option", Value{var}, "Description"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(OptionsInvalidOptionCase)
{
    {
        int var{-1};
        Options opts{"Options"};
        opts('l', "long_option", Value{var}, "LongOption Description");

        const char* argv[] = {"executable_name", "--bad", "123"};
        int argc = 3;

        BOOST_CHECK_THROW(opts.parse(argc, const_cast<char**>(argv)), std::runtime_error);
    }

    {
        int var{-1};
        Options opts{"Options"};
        opts('l', "long_option", Value{var}, "LongOption Description");

        const char* argv[] = {"executable_name", "-l123", "456"};
        int argc = 3;

        // FIXME: this is incorrect.
        opts.parse(argc, const_cast<char**>(argv));
        BOOST_TEST(var == 456);
    }
}

BOOST_AUTO_TEST_CASE(OptionsNoValueCase)
{
    int var{-1};
    Options opts{"Options"};
    opts('l', "long_option", Value{var}, "LongOption Description");

    const char* argv[] = {"executable_name", "-l"};
    int argc = 2;

    BOOST_CHECK_THROW(opts.parse(argc, const_cast<char**>(argv));, std::runtime_error);
}

BOOST_AUTO_TEST_CASE(OptionsMultiToken)
{
    int var{-1};
    std::vector<int> single;
    std::vector<int> multiple;

    {
        Options opts{"Multitoken Test options"};
        opts('v', Value{var}, "Raw int")('s', Value{single}, "Single int")(
            'm', Value{multiple}.multitoken(), "Multiple ints");

        const char* argv[] = {"executable_name", "-v", "1", "-s", "2", "-m", "3", "-m", "4"};
        int argc = 9;

        opts.parse(argc, const_cast<char**>(argv));
        BOOST_CHECK_EQUAL(var, 1);
        BOOST_CHECK_EQUAL(single[0], 2);
        BOOST_CHECK_EQUAL(multiple[0], 3);
        BOOST_CHECK_EQUAL(multiple[1], 4);
    }

    {
        Options opts{"Multitoken Test options"};
        opts('v', Value{var}, "Raw int")('s', Value{single}, "Single int")(
            'm', Value{multiple}.multitoken(), "Multiple ints");

        const char* argv[] = {"executable_name", "-s", "1", "-s", "2"};
        int argc = 5;

        BOOST_CHECK_THROW(opts.parse(argc, const_cast<char**>(argv)), std::runtime_error);
    }
}

BOOST_AUTO_TEST_SUITE_END()
