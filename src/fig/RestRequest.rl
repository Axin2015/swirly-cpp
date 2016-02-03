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
            cs = json_error; msg = "max string length exceeded";
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

    action buySide {
        side_ = Side::BUY;
    }
    action sellSide {
        side_ = Side::SELL;
    }
    side = '"BUY"'i %buySide
         | '"SELL"'i %sellSide;

    action makerRole {
        role_ = Role::MAKER;
    }
    action takerRole {
        role_ = Role::TAKER;
    }
    role = '"MAKER"'i %makerRole
         | '"TAKER"'i %takerRole;

    colon = space* ':' space*;
    comma = space* ',' space*;

    action beginMnem {
        if (fields_ & RestRequest::MNEM) {
            cs = json_error; msg = "mnem already specified";
            fbreak;
        }
        str_.len = &mnem_.len;
        str_.buf = mnem_.buf;
        str_.max = MNEM_MAX;
    }
    action endMnem {
        fields_ |= RestRequest::MNEM;
    }
    action beginDisplay {
        if (fields_ & RestRequest::DISPLAY) {
            cs = json_error; msg = "display already specified";
            fbreak;
        }
        str_.len = &display_.len;
        str_.buf = display_.buf;
        str_.max = DISPLAY_MAX;
    }
    action endDisplay {
        fields_ |= RestRequest::DISPLAY;
    }
    action beginEmail {
        if (fields_ & RestRequest::EMAIL) {
            cs = json_error; msg = "email already specified";
            fbreak;
        }
        str_.len = &email_.len;
        str_.buf = email_.buf;
        str_.max = EMAIL_MAX;
    }
    action endEmail {
        fields_ |= RestRequest::EMAIL;
    }
    action beginTrader {
        if (fields_ & RestRequest::TRADER) {
            cs = json_error; msg = "trader already specified";
            fbreak;
        }
        str_.len = &trader_.len;
        str_.buf = trader_.buf;
        str_.max = MNEM_MAX;
    }
    action endTrader {
        fields_ |= RestRequest::TRADER;
    }
    action beginContr {
        if (fields_ & RestRequest::CONTR) {
            cs = json_error; msg = "contr already specified";
            fbreak;
        }
        str_.len = &contr_.len;
        str_.buf = contr_.buf;
        str_.max = MNEM_MAX;
    }
    action endContr {
        fields_ |= RestRequest::CONTR;
    }
    action beginSettlDate {
        if (fields_ & RestRequest::SETTL_DATE) {
            cs = json_error; msg = "settl-date already specified";
            fbreak;
        }
    }
    action endSettlDate {
        fields_ |= RestRequest::SETTL_DATE;
        settlDate_ = box<IsoDate>(num());
    }
    action beginExpiryDate {
        if (fields_ & RestRequest::EXPIRY_DATE) {
            cs = json_error; msg = "expiry-date already specified";
            fbreak;
        }
    }
    action endExpiryDate {
        fields_ |= RestRequest::EXPIRY_DATE;
        expiryDate_ = box<IsoDate>(num());
    }
    action beginRef {
        if (fields_ & RestRequest::REF) {
            cs = json_error; msg = "ref already specified";
            fbreak;
        }
        str_.len = &ref_.len;
        str_.buf = ref_.buf;
        str_.max = REF_MAX;
    }
    action endRef {
        fields_ |= RestRequest::REF;
    }
    action beginState {
        if (fields_ & RestRequest::STATE) {
            cs = json_error; msg = "state already specified";
            fbreak;
        }
    }
    action endState {
        fields_ |= RestRequest::STATE;
        state_ = num();
    }
    action beginSide {
        if (fields_ & RestRequest::SIDE) {
            cs = json_error; msg = "side already specified";
            fbreak;
        }
    }
    action endSide {
        fields_ |= RestRequest::SIDE;
    }
    action beginTicks {
        if (fields_ & RestRequest::TICKS) {
            cs = json_error; msg = "ticks already specified";
            fbreak;
        }
    }
    action endTicks {
        fields_ |= RestRequest::TICKS;
        ticks_ = box<Ticks>(num());
    }
    action beginLots {
        if (fields_ & RestRequest::LOTS) {
            cs = json_error; msg = "lots already specified";
            fbreak;
        }
    }
    action endLots {
        fields_ |= RestRequest::LOTS;
        lots_ = box<Lots>(num());
    }
    action beginMinLots {
        if (fields_ & RestRequest::MIN_LOTS) {
            cs = json_error; msg = "min-lots already specified";
            fbreak;
        }
    }
    action endMinLots {
        fields_ |= RestRequest::MIN_LOTS;
        minLots_ = box<Lots>(num());
    }
    action beginRole {
        if (fields_ & RestRequest::ROLE) {
            cs = json_error; msg = "role already specified";
            fbreak;
        }
    }
    action endRole {
        fields_ |= RestRequest::ROLE;
    }
    action beginCpty {
        if (fields_ & RestRequest::CPTY) {
            cs = json_error; msg = "cpty already specified";
            fbreak;
        }
        str_.len = &cpty_.len;
        str_.buf = cpty_.buf;
        str_.max = MNEM_MAX;
    }
    action endCpty {
        fields_ |= RestRequest::CPTY;
    }

    pair = '"mnem"'i colon str >beginMnem %endMnem
         | '"display"'i colon str >beginDisplay %endDisplay
         | '"email"'i colon str >beginEmail %endEmail
         | '"trader"'i colon str >beginTrader %endTrader
         | '"contr"'i colon str >beginContr %endContr
         | '"settlDate"'i colon num >beginSettlDate %endSettlDate
         | '"expiryDate"'i colon num >beginExpiryDate %endExpiryDate
         | '"ref"'i colon str >beginRef %endRef
         | '"state"'i colon num >beginState %endState
         | '"side"'i colon side >beginSide %endSide
         | '"lots"'i colon num >beginLots %endLots
         | '"ticks"'i colon num >beginTicks %endTicks
         | '"minMots"'i colon num >beginMinLots %endMinLots
         | '"role"'i colon role >beginRole %endRole
         | '"cpty"'i colon str >beginCpty %endCpty;

    members = pair (comma pair)*;

    object = '{' space* '}'
           | '{' space* members space* '}';

    main := space* object space*;
}%%

%% write data;

} // anonymous

RestRequest::~RestRequest() noexcept = default;

void RestRequest::reset() noexcept
{
    auto cs = cs_;
    %% write init;
    cs_ = cs;

    fields_ = 0;
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
