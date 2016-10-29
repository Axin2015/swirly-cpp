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
#include <swirly/ws/RestRequest.hpp>

#include <swirly/fin/Exception.hpp>

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
    if (len < str_.max) {
      str_.buf[len++] = fc;
    } else {
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
    str_.max = MaxMnem;
  }
  action endMnem {
    fields_ |= Mnem;
  }
  mnem = 'null' %nullMnem
    | str >beginMnem %endMnem;

  action nullContr {
    fields_ &= ~Contr;
    contr_.len = 0;
  }
  action beginContr {
    str_.len = &contr_.len;
    str_.buf = contr_.buf;
    str_.max = MaxMnem;
  }
  action endContr {
    fields_ |= Contr;
  }
  contr = 'null' %nullContr
    | str >beginContr %endContr;

  action nullSettlDate {
    fields_ &= ~SettlDate;
    settlDate_ = 0_ymd;
  }
  action endSettlDate {
    fields_ |= SettlDate;
    settlDate_ = IsoDate{num()};
  }
  settlDate = 'null' %nullSettlDate
    | num %endSettlDate;

  action nullAccnt {
    fields_ &= ~Accnt;
    accnt_.len = 0;
  }
  action beginAccnt {
    str_.len = &accnt_.len;
    str_.buf = accnt_.buf;
    str_.max = MaxMnem;
  }
  action endAccnt {
    fields_ |= Accnt;
  }
  accnt = 'null' %nullAccnt
    | str >beginAccnt %endAccnt;

  action nullRef {
    fields_ &= ~Ref;
    ref_.len = 0;
  }
  action beginRef {
    str_.len = &ref_.len;
    str_.buf = ref_.buf;
    str_.max = MaxRef;
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
    ticks_ = swirly::Ticks{num()};
  }
  ticks = 'null' %nullTicks
    | num %endTicks;

  action buySide {
    fields_ |= Side;
    side_ = swirly::Side::Buy;
  }
  action sellSide {
    fields_ |= Side;
    side_ = swirly::Side::Sell;
  }
  side = '"BUY"'i %buySide
    | '"SELL"'i %sellSide;

  action nullLots {
    fields_ &= ~Lots;
    lots_ = 0_lts;
  }
  action endLots {
    fields_ |= Lots;
    lots_ = swirly::Lots{num()};
  }
  lots = 'null' %nullLots
    | num %endLots;

  action nullMinLots {
    fields_ &= ~MinLots;
    minLots_ = 0_lts;
  }
  action endMinLots {
    fields_ |= MinLots;
    minLots_ = swirly::Lots{num()};
  }
  minLots = 'null' %nullMinLots
    | num %endMinLots;

  action nullLiqInd {
    fields_ &= ~LiqInd;
    liqInd_ = swirly::LiqInd::None;
  }
  action makerLiqInd {
    fields_ |= LiqInd;
    liqInd_ = swirly::LiqInd::Maker;
  }
  action takerLiqInd {
    fields_ |= LiqInd;
    liqInd_ = swirly::LiqInd::Taker;
  }
  liqInd = 'null' %nullLiqInd
    | '"MAKER"'i %makerLiqInd
    | '"TAKER"'i %takerLiqInd;

  action nullCpty {
    fields_ &= ~Cpty;
    cpty_.len = 0;
  }
  action beginCpty {
    str_.len = &cpty_.len;
    str_.buf = cpty_.buf;
    str_.max = MaxMnem;
  }
  action endCpty {
    fields_ |= Cpty;
  }
  cpty = 'null' %nullCpty
    | str >beginCpty %endCpty;

  colon = space* ':' space*;
  comma = space* ',' space*;

  pair = '"mnem"'i colon mnem
    | '"contr"'i colon contr
    | '"settlDate"'i colon settlDate
    | '"accnt"'i colon accnt
    | '"ref"'i colon ref
    | '"state"'i colon state
    | '"side"'i colon side
    | '"lots"'i colon lots
    | '"ticks"'i colon ticks
    | '"minLots"'i colon minLots
    | '"liqInd"'i colon liqInd
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

  if (!clear) {
    return;
  }
  fields_ = 0;

  mnem_.len = 0;
  contr_.len = 0;
  settlDate_ = 0_ymd;
  accnt_.len = 0;
  ref_.len = 0;
  state_ = 0;
  side_ = static_cast<swirly::Side>(0);
  lots_ = 0_lts;
  ticks_ = 0_tks;
  minLots_ = 0_lts;
  liqInd_ = swirly::LiqInd::None;
  cpty_.len = 0;
}

bool RestRequest::parse(string_view buf)
{
  const char* p{buf.data()};
  const char* pe{p + buf.size()};
  const char* msg{"parse error"};

  auto cs = cs_;
  %% write exec;
  cs_ = cs;

  if (cs == json_error) {
    throw BadRequestException{msg};
  }
  if (cs < json_first_final) {
    return false;
  }
  return true;
}

} // swirly
