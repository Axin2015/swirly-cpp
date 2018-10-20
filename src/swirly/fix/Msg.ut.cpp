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
        BOOST_TEST(get<Tag::MdEntryType>(md_entry).side == Side::Buy);
        BOOST_TEST(get<Tag::MdEntryPx>(md_entry) == 12341_tks);
        BOOST_TEST(get<Tag::MdEntrySize>(md_entry) == 2000_lts);
        BOOST_TEST(lex.empty());
    }
    {
        FixLexer lex{"269=0\1"
                     "270=12341\1"
                     "271=2000\1"
                     "10=097\1"sv};
        MdEntry md_entry;

        parse_group(lex, md_entry);
        BOOST_TEST(get<Tag::MdEntryType>(md_entry).side == Side::Buy);
        BOOST_TEST(get<Tag::MdEntryPx>(md_entry) == 12341_tks);
        BOOST_TEST(get<Tag::MdEntrySize>(md_entry) == 2000_lts);
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
        BOOST_TEST(get<Tag::MdEntryType>(md_entry).side == Side::Buy);
        BOOST_TEST(get<Tag::MdEntryPx>(md_entry) == 12344_tks);
        BOOST_TEST(get<Tag::MdEntrySize>(md_entry) == 1000_lts);
        BOOST_TEST_REQUIRE(!lex.empty());

        parse_group(lex, md_entry);
        BOOST_TEST(get<Tag::MdEntryType>(md_entry).side == Side::Sell);
        BOOST_TEST(get<Tag::MdEntryPx>(md_entry) == 12346_tks);
        BOOST_TEST(get<Tag::MdEntrySize>(md_entry) == 2000_lts);
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
        BOOST_TEST(get<Tag::MdEntryType>(md_entry).side == Side::Buy);
        BOOST_TEST(get<Tag::MdEntryPx>(md_entry) == 12344_tks);
        BOOST_TEST(get<Tag::MdEntrySize>(md_entry) == 1000_lts);
        BOOST_TEST_REQUIRE(!lex.empty());

        parse_group(lex, md_entry);
        BOOST_TEST(get<Tag::MdEntryType>(md_entry).side == Side::Sell);
        BOOST_TEST(get<Tag::MdEntryPx>(md_entry) == 12346_tks);
        BOOST_TEST(get<Tag::MdEntrySize>(md_entry) == 2000_lts);
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
    get<Tag::Symbol>(mds) = "EURUSD"sv;
    get<Tag::SettlDate>(mds) = 20181004_ymd;

    MdEntry md_entry;
    get<Tag::MdEntryType>(md_entry).side = Side::Buy;
    get<Tag::MdEntryPx>(md_entry) = 12344_tks;
    get<Tag::MdEntrySize>(md_entry) = 1000_lts;

    mds.md_entries.push_back(md_entry);

    stringstream ss;
    ss << mds;
    BOOST_TEST(ss.str()
               == "55=EURUSD\1"
                  "64=20181004\1"
                  "268=1\1"
                  "269=0\1"
                  "270=12344\1"
                  "271=1000\1");
}

BOOST_AUTO_TEST_CASE(ParseMarketDataSnapshotCase)
{
    FixLexer lex{"55=EURUSD\1"
                 "64=20181004\1"
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

    BOOST_TEST(get<Tag::Symbol>(mds) == "EURUSD"sv);
    BOOST_TEST(get<Tag::SettlDate>(mds) == 20181004_ymd);
    BOOST_TEST_REQUIRE(mds.md_entries.size() == 4);

    BOOST_TEST(get<Tag::MdEntryType>(mds.md_entries[0]).side == Side::Buy);
    BOOST_TEST(get<Tag::MdEntryPx>(mds.md_entries[0]) == 12343_tks);
    BOOST_TEST(get<Tag::MdEntrySize>(mds.md_entries[0]) == 2000_lts);

    BOOST_TEST(get<Tag::MdEntryType>(mds.md_entries[1]).side == Side::Buy);
    BOOST_TEST(get<Tag::MdEntryPx>(mds.md_entries[1]) == 12344_tks);
    BOOST_TEST(get<Tag::MdEntrySize>(mds.md_entries[1]) == 1000_lts);

    BOOST_TEST(get<Tag::MdEntryType>(mds.md_entries[2]).side == Side::Sell);
    BOOST_TEST(get<Tag::MdEntryPx>(mds.md_entries[2]) == 12346_tks);
    BOOST_TEST(get<Tag::MdEntrySize>(mds.md_entries[2]) == 1000_lts);

    BOOST_TEST(get<Tag::MdEntryType>(mds.md_entries[3]).side == Side::Sell);
    BOOST_TEST(get<Tag::MdEntryPx>(mds.md_entries[3]) == 12347_tks);
    BOOST_TEST(get<Tag::MdEntrySize>(mds.md_entries[3]) == 2000_lts);

    BOOST_TEST(lex.empty());
}

BOOST_AUTO_TEST_SUITE_END()
