// -*- C++ -*-
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
#include "RestRequest.hpp"

#include <swirly/elm/Exception.hpp>

using namespace std;

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
        fields_ &= ~Mnem;
        mnem_.len = 0;
    }
    action beginMnem {
        str_.len = &mnem_.len;
        str_.buf = mnem_.buf;
        str_.max = MnemMax;
    }
    action endMnem {
        fields_ |= Mnem;
    }
    mnem = 'null' %nullMnem
         | str >beginMnem %endMnem;

    action nullDisplay {
        fields_ &= ~Display;
        display_.len = 0;
    }
    action beginDisplay {
        str_.len = &display_.len;
        str_.buf = display_.buf;
        str_.max = DisplayMax;
    }
    action endDisplay {
        fields_ |= Display;
    }
    display = 'null' %nullDisplay
            | str >beginDisplay %endDisplay;

    action nullEmail {
        fields_ &= ~Email;
        email_.len = 0;
    }
    action beginEmail {
        str_.len = &email_.len;
        str_.buf = email_.buf;
        str_.max = EmailMax;
    }
    action endEmail {
        fields_ |= Email;
    }
    email = 'null' %nullEmail
          | str >beginEmail %endEmail;

    action nullTrader {
        fields_ &= ~Trader;
        trader_.len = 0;
    }
    action beginTrader {
        str_.len = &trader_.len;
        str_.buf = trader_.buf;
        str_.max = MnemMax;
    }
    action endTrader {
        fields_ |= Trader;
    }
    trader = 'null' %nullTrader
           | str >beginTrader %endTrader;

    action nullContr {
        fields_ &= ~Contr;
        contr_.len = 0;
    }
    action beginContr {
        str_.len = &contr_.len;
        str_.buf = contr_.buf;
        str_.max = MnemMax;
    }
    action endContr {
        fields_ |= Contr;
    }
    contr = 'null' %nullContr
          | str >beginContr %endContr;

    action nullSettlDate {
        fields_ &= ~SettlDate;
        settlDate_ = 0_dt;
    }
    action endSettlDate {
        fields_ |= SettlDate;
        settlDate_ = box<IsoDate>(num());
    }
    settlDate = 'null' %nullSettlDate
              | num %endSettlDate;

    action nullExpiryDate {
        fields_ &= ~ExpiryDate;
        expiryDate_ = 0_dt;
    }
    action endExpiryDate {
        fields_ |= ExpiryDate;
        expiryDate_ = box<IsoDate>(num());
    }
    expiryDate = 'null' %nullExpiryDate
               | num %endExpiryDate;

    action nullRef {
        fields_ &= ~Ref;
        ref_.len = 0;
    }
    action beginRef {
        str_.len = &ref_.len;
        str_.buf = ref_.buf;
        str_.max = RefMax;
    }
    action endRef {
        fields_ |= Ref;
    }
    ref = 'null' %nullRef
        | str >beginRef %endRef;

    action nullState {
        fields_ &= ~State;
        state_ = 0;
    }
    action endState {
        if (num_.sign >= 0) {
            fields_ |= State;
            state_ = num();
        } else {
            cs = json_error; msg = "negative state field";
        }
    }
    state = 'null' %nullState
          | num %endState;

    action nullTicks {
        fields_ &= ~Ticks;
        ticks_ = 0_tks;
    }
    action endTicks {
        fields_ |= Ticks;
        ticks_ = box<swirly::Ticks>(num());
    }
    ticks = 'null' %nullTicks
          | num %endTicks;

    action nullSide {
        fields_ &= ~Side;
        side_ = box<swirly::Side>(0);
    }
    action buySide {
        fields_ |= Side;
        side_ = swirly::Side::Buy;
    }
    action sellSide {
        fields_ |= Side;
        side_ = swirly::Side::Sell;
    }
    side = 'null' %nullSide
         | '"BUY"'i %buySide
         | '"SELL"'i %sellSide;

    action nullLots {
        fields_ &= ~Lots;
        lots_ = 0_lts;
    }
    action endLots {
        fields_ |= Lots;
        lots_ = box<swirly::Lots>(num());
    }
    lots = 'null' %nullLots
         | num %endLots;

    action nullMinLots {
        fields_ &= ~MinLots;
        minLots_ = 0_lts;
    }
    action endMinLots {
        fields_ |= MinLots;
        minLots_ = box<swirly::Lots>(num());
    }
    minLots = 'null' %nullMinLots
            | num %endMinLots;

    action nullRole {
        fields_ &= ~Role;
        role_ = swirly::Role::None;
    }
    action makerRole {
        fields_ |= Role;
        role_ = swirly::Role::Maker;
    }
    action takerRole {
        fields_ |= Role;
        role_ = swirly::Role::Taker;
    }
    role = 'null' %nullRole
         | '"MAKER"'i %makerRole
         | '"TAKER"'i %takerRole;

    action nullCpty {
        fields_ &= ~Cpty;
        cpty_.len = 0;
    }
    action beginCpty {
        str_.len = &cpty_.len;
        str_.buf = cpty_.buf;
        str_.max = MnemMax;
    }
    action endCpty {
        fields_ |= Cpty;
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
    side_ = box<swirly::Side>(0);
    lots_ = 0_lts;
    ticks_ = 0_tks;
    minLots_ = 0_lts;
    role_ = swirly::Role::None;
    cpty_.len = 0;
}

bool RestRequest::parse(const string_view& buf)
{
    const char* p{buf.data()};
    const char* pe{p + buf.size()};
    const char* msg{"parse error"};

    auto cs = cs_;
    %% write exec;
    cs_ = cs;

    if (cs == json_error)
        throw BadRequestException{msg};

    if (cs < json_first_final)
        return false;

    return true;
}

} // swirly
