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
#include "Msg.hpp"

#include "Lexer.hpp"

#include <swirly/app/Exception.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(MsgSuite)

BOOST_AUTO_TEST_CASE(ParseMdEntrySingleCase)
{
    {
        FixLexer lex{"269=0\1"
                     "270=12341\1"
                     "271=2000\1"sv};
        MdEntry md_entry;

        parse_group(lex, md_entry);
        BOOST_TEST(md_entry.type.value == '0');
        BOOST_TEST(md_entry.px.value == 12341_tks);
        BOOST_TEST(md_entry.size.value == 2000_lts);
        BOOST_TEST(lex.empty());
    }
    {
        FixLexer lex{"269=0\1"
                     "270=12341\1"
                     "271=2000\1"
                     "10=097\1"sv};
        MdEntry md_entry;

        parse_group(lex, md_entry);
        BOOST_TEST(md_entry.type.value == '0');
        BOOST_TEST(md_entry.px.value == 12341_tks);
        BOOST_TEST(md_entry.size.value == 2000_lts);
        BOOST_TEST_REQUIRE(!lex.empty());
        BOOST_TEST(lex.top().first == 10);
    }
}

BOOST_AUTO_TEST_CASE(ParseMdEntryMultiCase)
{
    {
        FixLexer lex{"269=0\1"
                     "270=12344\1"
                     "271=1000\1"
                     "269=1\1"
                     "270=12346\1"
                     "271=2000\1"sv};
        MdEntry md_entry;

        parse_group(lex, md_entry);
        BOOST_TEST(md_entry.type.value == '0');
        BOOST_TEST(md_entry.px.value == 12344_tks);
        BOOST_TEST(md_entry.size.value == 1000_lts);
        BOOST_TEST_REQUIRE(!lex.empty());

        parse_group(lex, md_entry);
        BOOST_TEST(md_entry.type.value == '1');
        BOOST_TEST(md_entry.px.value == 12346_tks);
        BOOST_TEST(md_entry.size.value == 2000_lts);
        BOOST_TEST(lex.empty());
    }
    {
        FixLexer lex{"269=0\1"
                     "270=12344\1"
                     "271=1000\1"
                     "269=1\1"
                     "270=12346\1"
                     "271=2000\1"
                     "10=097\1"sv};
        MdEntry md_entry;

        parse_group(lex, md_entry);
        BOOST_TEST(md_entry.type.value == '0');
        BOOST_TEST(md_entry.px.value == 12344_tks);
        BOOST_TEST(md_entry.size.value == 1000_lts);
        BOOST_TEST_REQUIRE(!lex.empty());

        parse_group(lex, md_entry);
        BOOST_TEST(md_entry.type.value == '1');
        BOOST_TEST(md_entry.px.value == 12346_tks);
        BOOST_TEST(md_entry.size.value == 2000_lts);
        BOOST_TEST_REQUIRE(!lex.empty());
        BOOST_TEST(lex.top().first == 10);
    }
}

BOOST_AUTO_TEST_CASE(ParseMdEntryMissingCase)
{
    {
        FixLexer lex{""sv};
        MdEntry md_entry;
        BOOST_CHECK_THROW(parse_group(lex, md_entry), ProtocolException);
    }
    {
        // MdEntryType(269) must be first field in group.
        FixLexer lex{"270=12341\1"
                     "271=2000\1"sv};
        MdEntry md_entry;
        BOOST_CHECK_THROW(parse_group(lex, md_entry), ProtocolException);
    }
}

BOOST_AUTO_TEST_CASE(FormatMarketDataSnapshotCase)
{
    MarketDataSnapshot mds;
    mds.symbol.value = "EURUSD"sv;
    mds.maturity_date.value = 20181004_ymd;

    MdEntry md_entry;
    md_entry.type.value = '0';
    md_entry.px.value = 12344_tks;
    md_entry.size.value = 1000_lts;

    mds.md_entries.push_back(md_entry);

    stringstream ss;
    ss << mds;
    BOOST_TEST(ss.str()
               == "55=EURUSD\1"
                  "541=20181004\1"
                  "268=1\1"
                  "269=0\1"
                  "270=12344\1"
                  "271=1000\1");
}

BOOST_AUTO_TEST_CASE(ParseMarketDataSnapshotCase)
{
    FixLexer lex{"55=EURUSD\1"
                 "541=20181004\1"
                 "268=4\1"
                 "269=0\1"
                 "270=12343\1"
                 "271=2000\1"
                 "269=0\1"
                 "270=12344\1"
                 "271=1000\1"
                 "269=1\1"
                 "270=12346\1"
                 "271=1000\1"
                 "269=1\1"
                 "270=12347\1"
                 "271=2000\1"sv};

    MarketDataSnapshot mds;
    parse_body(lex, mds);

    BOOST_TEST(mds.symbol.value == "EURUSD"sv);
    BOOST_TEST(mds.maturity_date.value == 20181004_ymd);
    BOOST_TEST_REQUIRE(mds.md_entries.size() == 4);

    BOOST_TEST(mds.md_entries[0].type.value == '0');
    BOOST_TEST(mds.md_entries[0].px.value == 12343_tks);
    BOOST_TEST(mds.md_entries[0].size.value == 2000_lts);

    BOOST_TEST(mds.md_entries[1].type.value == '0');
    BOOST_TEST(mds.md_entries[1].px.value == 12344_tks);
    BOOST_TEST(mds.md_entries[1].size.value == 1000_lts);

    BOOST_TEST(mds.md_entries[2].type.value == '1');
    BOOST_TEST(mds.md_entries[2].px.value == 12346_tks);
    BOOST_TEST(mds.md_entries[2].size.value == 1000_lts);

    BOOST_TEST(mds.md_entries[3].type.value == '1');
    BOOST_TEST(mds.md_entries[3].px.value == 12347_tks);
    BOOST_TEST(mds.md_entries[3].size.value == 2000_lts);

    BOOST_TEST(lex.empty());
}

BOOST_AUTO_TEST_SUITE_END()
