// -*- C++ -*-
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
#include "RestRequest.hpp"

#include "Exception.hpp"

namespace swirly {

namespace {

%%{
    machine json;

    action beginNum {
        num_.sign = 1;
        num_.digits = 0;
    }
    action negate {
        num_.sign = -1;
    }
    action addDigit {
        num_.digits = num_.digits * 10 + (fc - '0');
    }

    num = (
        start: (
            '-' @negate ->reqDigit
          | '+' ->reqDigit
          | [0-9] @addDigit ->optDigit
        ),
        reqDigit: (
            [0-9] @addDigit ->optDigit
        ),
        optDigit: (
            [0-9] @addDigit ->optDigit
          | '' -> final
        )
    ) >beginNum;

    action beginStr {
        *str_.len = 0;
    }
    action addChar {
        auto& len = *str_.len;
        if (len < str_.max)
            str_.buf[len++] = fc;
        else {
            cs = json_error; msg = "max length exceeded";
            fbreak;
        }
    }
    str = (
        start: (
            '"' ->char
        ),
        char: (
            [^"\\] @addChar -> char
          | '\\' any @addChar ->char
          | '"' ->final
        )
    ) >beginStr;

    action nullMnem {
        fields_ &= ~RestRequest::MNEM;
        mnem_.len = 0;
    }
    action beginMnem {
        str_.len = &mnem_.len;
        str_.buf = mnem_.buf;
        str_.max = MNEM_MAX;
    }
    action endMnem {
        fields_ |= RestRequest::MNEM;
    }
    mnem = 'null' %nullMnem
         | str >beginMnem %endMnem;

    action nullDisplay {
        fields_ &= ~RestRequest::DISPLAY;
        display_.len = 0;
    }
    action beginDisplay {
        str_.len = &display_.len;
        str_.buf = display_.buf;
        str_.max = DISPLAY_MAX;
    }
    action endDisplay {
        fields_ |= RestRequest::DISPLAY;
    }
    display = 'null' %nullDisplay
            | str >beginDisplay %endDisplay;

    action nullEmail {
        fields_ &= ~RestRequest::EMAIL;
        email_.len = 0;
    }
    action beginEmail {
        str_.len = &email_.len;
        str_.buf = email_.buf;
        str_.max = EMAIL_MAX;
    }
    action endEmail {
        fields_ |= RestRequest::EMAIL;
    }
    email = 'null' %nullEmail
          | str >beginEmail %endEmail;

    action nullTrader {
        fields_ &= ~RestRequest::TRADER;
        trader_.len = 0;
    }
    action beginTrader {
        str_.len = &trader_.len;
        str_.buf = trader_.buf;
        str_.max = MNEM_MAX;
    }
    action endTrader {
        fields_ |= RestRequest::TRADER;
    }
    trader = 'null' %nullTrader
           | str >beginTrader %endTrader;

    action nullContr {
        fields_ &= ~RestRequest::CONTR;
        contr_.len = 0;
    }
    action beginContr {
        str_.len = &contr_.len;
        str_.buf = contr_.buf;
        str_.max = MNEM_MAX;
    }
    action endContr {
        fields_ |= RestRequest::CONTR;
    }
    contr = 'null' %nullContr
          | str >beginContr %endContr;

    action nullSettlDate {
        fields_ &= ~RestRequest::SETTL_DATE;
        settlDate_ = 0_dt;
    }
    action endSettlDate {
        fields_ |= RestRequest::SETTL_DATE;
        settlDate_ = box<IsoDate>(num());
    }
    settlDate = 'null' %nullSettlDate
              | num %endSettlDate;

    action nullExpiryDate {
        fields_ &= ~RestRequest::EXPIRY_DATE;
        expiryDate_ = 0_dt;
    }
    action endExpiryDate {
        fields_ |= RestRequest::EXPIRY_DATE;
        expiryDate_ = box<IsoDate>(num());
    }
    expiryDate = 'null' %nullExpiryDate
               | num %endExpiryDate;

    action nullRef {
        fields_ &= ~RestRequest::REF;
        ref_.len = 0;
    }
    action beginRef {
        str_.len = &ref_.len;
        str_.buf = ref_.buf;
        str_.max = REF_MAX;
    }
    action endRef {
        fields_ |= RestRequest::REF;
    }
    ref = 'null' %nullRef
        | str >beginRef %endRef;

    action nullState {
        fields_ &= ~RestRequest::STATE;
        state_ = 0;
    }
    action endState {
        if (num_.sign >= 0) {
            fields_ |= RestRequest::STATE;
            state_ = num();
        } else {
            cs = json_error; msg = "negative state field";
        }
    }
    state = 'null' %nullState
          | num %endState;

    action nullTicks {
        fields_ &= ~RestRequest::TICKS;
        ticks_ = 0_tks;
    }
    action endTicks {
        fields_ |= RestRequest::TICKS;
        ticks_ = box<Ticks>(num());
    }
    ticks = 'null' %nullTicks
          | num %endTicks;

    action nullSide {
        fields_ &= ~RestRequest::SIDE;
        side_ = box<Side>(0);
    }
    action buySide {
        fields_ |= RestRequest::SIDE;
        side_ = Side::BUY;
    }
    action sellSide {
        fields_ |= RestRequest::SIDE;
        side_ = Side::SELL;
    }
    side = 'null' %nullSide
         | '"BUY"'i %buySide
         | '"SELL"'i %sellSide;

    action nullLots {
        fields_ &= ~RestRequest::LOTS;
        lots_ = 0_lts;
    }
    action endLots {
        fields_ |= RestRequest::LOTS;
        lots_ = box<Lots>(num());
    }
    lots = 'null' %nullLots
         | num %endLots;

    action nullMinLots {
        fields_ &= ~RestRequest::MIN_LOTS;
        minLots_ = 0_lts;
    }
    action endMinLots {
        fields_ |= RestRequest::MIN_LOTS;
        minLots_ = box<Lots>(num());
    }
    minLots = 'null' %nullMinLots
            | num %endMinLots;

    action nullRole {
        fields_ &= ~RestRequest::ROLE;
        role_ = Role::NONE;
    }
    action makerRole {
        fields_ |= RestRequest::ROLE;
        role_ = Role::MAKER;
    }
    action takerRole {
        fields_ |= RestRequest::ROLE;
        role_ = Role::TAKER;
    }
    role = 'null' %nullRole
         | '"MAKER"'i %makerRole
         | '"TAKER"'i %takerRole;

    action nullCpty {
        fields_ &= ~RestRequest::CPTY;
        cpty_.len = 0;
    }
    action beginCpty {
        str_.len = &cpty_.len;
        str_.buf = cpty_.buf;
        str_.max = MNEM_MAX;
    }
    action endCpty {
        fields_ |= RestRequest::CPTY;
    }
    cpty = 'null' %nullCpty
         | str >beginCpty %endCpty;

    colon = space* ':' space*;
    comma = space* ',' space*;

    pair = '"mnem"'i colon mnem
         | '"display"'i colon display
         | '"email"'i colon email
         | '"trader"'i colon trader
         | '"contr"'i colon contr
         | '"settlDate"'i colon settlDate
         | '"expiryDate"'i colon expiryDate
         | '"ref"'i colon ref
         | '"state"'i colon state
         | '"side"'i colon side
         | '"lots"'i colon lots
         | '"ticks"'i colon ticks
         | '"minLots"'i colon minLots
         | '"role"'i colon role
         | '"cpty"'i colon cpty;

    members = pair (comma pair)*;

    object = '{' space* '}'
           | '{' space* members space* '}';

    main := space* object space*;
}%%

%% write data;

} // anonymous

RestRequest::~RestRequest() noexcept = default;

void RestRequest::reset(bool clear) noexcept
{
    decltype(cs_) cs;
    %% write init;
    cs_ = cs;

    if (!clear)
        return;

    fields_ = 0;

    mnem_.len = 0;
    display_.len = 0;
    email_.len = 0;
    trader_.len = 0;
    contr_.len = 0;
    settlDate_ = 0_dt;
    expiryDate_ = 0_dt;
    ref_.len = 0;
    state_ = 0;
    side_ = box<Side>(0);
    lots_ = 0_lts;
    ticks_ = 0_tks;
    minLots_ = 0_lts;
    role_ = Role::NONE;
    cpty_.len = 0;
}

bool RestRequest::parse(const StringView& buf)
{
    const char* p{buf.data()};
    const char* pe{p + buf.size()};
    const char* msg{"parse error"};

    auto cs = cs_;
    %% write exec;
    cs_ = cs;

    if (cs == json_error)
        swirly::throwException<ParseException>("%s", msg);

    if (cs < json_first_final)
        return false;

    return true;
}

} // swirly
