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
#include "RestServ.hpp"

#include <swirly/fir/Rest.hpp>

#include <swirly/elm/Exception.hpp>

#include <swirly/ash/Finally.hpp>
#include <swirly/ash/Log.hpp>
#include <swirly/ash/Numeric.hpp>
#include <swirly/ash/Time.hpp>

#include <chrono>

using namespace std;

namespace swirly {
namespace mg {
namespace {

class ScopedIds {
 public:
  ScopedIds(string_view sv, vector<Id64>& ids) noexcept : ids_{ids}
  {
    Tokeniser<','> toks{sv};
    while (!toks.empty()) {
      ids.push_back(static_cast<Id64>(stou64(toks.top())));
      toks.pop();
    }
  }
  ~ScopedIds() noexcept { ids_.clear(); }

 private:
  vector<Id64>& ids_;
};

Millis getTime(HttpMessage data) noexcept
{
  const string_view val{data.header("Swirly-Time")};
  return val.empty() ? getTimeOfDay() : box<Millis>(stou64(val));
}

string_view getAccnt(HttpMessage data)
{
  const string_view accnt{data.header("Swirly-Accnt")};
  if (accnt.empty()) {
    throw UnauthorizedException{"user account not specified"_sv};
  }
  return accnt;
}

uint32_t getPerm(HttpMessage data)
{
  return stou32(data.header("Swirly-Perm"));
}

string_view getAdmin(HttpMessage data)
{
  const auto accnt = getAccnt(data);
  const auto perm = getPerm(data);
  if (!(perm & 0x1)) {
    throw ForbiddenException{"user account does not have admin permission "_sv};
  }
  return accnt;
}

string_view getTrader(HttpMessage data)
{
  const auto accnt = getAccnt(data);
  const auto perm = getPerm(data);
  if (!(perm & 0x2)) {
    throw ForbiddenException{"user account does not have trade permission "_sv};
  }
  return accnt;
}

auto getOffset(HttpMessage data)
{
  size_t offset{0};
  if (!data.queryString().empty()) {
    char value[20];
    if (data.getVar("offset", value)) {
      offset = numericCast<size_t>(value);
    }
  }
  return offset;
}

auto getLimit(HttpMessage data)
{
  optional<size_t> limit;
  if (!data.queryString().empty()) {
    char value[20];
    if (data.getVar("limit", value)) {
      limit = numericCast<size_t>(value);
    }
  }
  return limit;
}

} // anonymous

RestServ::~RestServ() noexcept = default;

bool RestServ::reset(HttpMessage data) noexcept
{
  bool cache{false};
  state_ = 0;

  auto uri = data.uri();
  // Remove leading slash.
  if (uri.front() == '/') {
    uri.remove_prefix(1);
  }
  uri_.reset(uri);

  const auto method = data.method();
  if (method == "GET"_sv) {
    cache = !uri.empty() && uri_.top() == "rec"_sv;
    state_ |= MethodGet;
  } else if (method == "POST"_sv) {
    state_ |= MethodPost;
  } else if (method == "PUT"_sv) {
    state_ |= MethodPut;
  } else if (method == "DELETE"_sv) {
    state_ |= MethodDelete;
  }

  request_.reset();
  return cache;
}

void RestServ::httpRequest(mg_connection& nc, HttpMessage data)
{
  using namespace chrono;

  TimeRecorder tr{profile_};
  auto finally = makeFinally([this]() {
    if (this->profile_.size() % 10 == 0) {
      this->profile_.report();
    }
  });
  const auto cache = reset(data);
  const auto now = getTime(data);
  // See mg_send().
  nc.last_io_time = unbox(now) / 1000;

  StreamBuf buf{nc.send_mbuf};
  out_.rdbuf(&buf);
  if (!isSet(MethodDelete)) {
    out_.reset(200, "OK", cache);
  } else {
    out_.reset(204, "No Content");
  }
  try {
    const auto body = data.body();
    if (!body.empty()) {
      if (!request_.parse(data.body())) {
        throw BadRequestException{"request body is incomplete"_sv};
      }
    }
    restRequest(data, now);
    if (!isSet(MatchUri)) {
      throw NotFoundException{errMsg() << "resource '" << data.uri() << "' does not exist"};
    }
    if (!isSet(MatchMethod)) {
      throw MethodNotAllowedException{errMsg() << "method '" << data.method()
                                               << "' is not allowed"};
    }
  } catch (const ServException& e) {
    SWIRLY_ERROR(logMsg() << "exception: status=" << e.httpStatus() << ", reason=" << e.httpReason()
                          << ", detail=" << e.what());
    out_.reset(e.httpStatus(), e.httpReason());
    out_ << e;
  } catch (const exception& e) {
    const int status{500};
    const char* const reason{"Internal Server Error"};
    SWIRLY_ERROR(logMsg() << "exception: status=" << status << ", reason=" << reason
                          << ", detail=" << e.what());
    out_.reset(status, reason);
    ServException::toJson(status, reason, e.what(), out_);
  }
  out_.setContentLength();
}

void RestServ::restRequest(HttpMessage data, Millis now)
{
  if (uri_.empty()) {
    return;
  }

  const auto tok = uri_.top();
  uri_.pop();

  if (tok == "ref"_sv) {
    // /ref
    recRequest(data, now);
  } else if (tok == "market"_sv) {
    // /market
    marketRequest(data, now);
  } else if (tok == "accnt"_sv) {
    // /accnt
    accntRequest(data, now);
  }
}

void RestServ::recRequest(HttpMessage data, Millis now)
{
  if (uri_.empty()) {

    // /ref
    state_ |= MatchUri;

    if (isSet(MethodGet)) {
      // GET /ref
      state_ |= MatchMethod;
      const int bs{EntitySet::Asset | EntitySet::Contr};
      rest_.getRec(bs, now, out_);
    }
    return;
  }

  const auto tok = uri_.top();
  uri_.pop();

  const auto es = EntitySet::parse(tok);
  if (es.many()) {

    if (uri_.empty()) {

      // /ref/entity,entity...
      state_ |= MatchUri;

      if (isSet(MethodGet)) {
        // GET /ref/entity,entity...
        state_ |= MatchMethod;
        rest_.getRec(es, now, out_);
      }
    }
    return;
  }

  switch (es.get()) {
  case EntitySet::Asset:
    assetRequest(data, now);
    break;
  case EntitySet::Contr:
    contrRequest(data, now);
    break;
  }
}

void RestServ::assetRequest(HttpMessage data, Millis now)
{
  if (uri_.empty()) {

    // /ref/asset
    state_ |= MatchUri;

    if (isSet(MethodGet)) {
      // GET /ref/asset
      state_ |= MatchMethod;
      rest_.getAsset(now, out_);
    }
    return;
  }

  const auto mnem = uri_.top();
  uri_.pop();

  if (uri_.empty()) {

    // /ref/asset/MNEM
    state_ |= MatchUri;

    if (isSet(MethodGet)) {
      // GET /ref/asset/MNEM
      state_ |= MatchMethod;
      rest_.getAsset(mnem, now, out_);
    }
    return;
  }
}

void RestServ::contrRequest(HttpMessage data, Millis now)
{
  if (uri_.empty()) {

    // /ref/contr
    state_ |= MatchUri;

    if (isSet(MethodGet)) {
      // GET /ref/contr
      state_ |= MatchMethod;
      rest_.getContr(now, out_);
    }
    return;
  }

  const auto mnem = uri_.top();
  uri_.pop();

  if (uri_.empty()) {

    // /ref/contr/MNEM
    state_ |= MatchUri;

    if (isSet(MethodGet)) {
      // GET /ref/contr/MNEM
      state_ |= MatchMethod;
      rest_.getContr(mnem, now, out_);
    }
    return;
  }
}

void RestServ::marketRequest(HttpMessage data, Millis now)
{
  if (uri_.empty()) {

    // /market
    state_ |= MatchUri;

    switch (state_ & MethodMask) {
    case MethodGet:
      // GET /market
      state_ |= MatchMethod;
      rest_.getMarket(now, out_);
      break;
    case MethodPost:
      // POST /market
      state_ |= MatchMethod;
      getAdmin(data);
      constexpr auto reqFields = RestRequest::Contr | RestRequest::SettlDate;
      constexpr auto optFields = RestRequest::State;
      if (!request_.valid(reqFields, optFields)) {
        throw InvalidException{"request fields are invalid"_sv};
      }
      rest_.postMarket(request_.contr(), request_.settlDate(), request_.state(), now, out_);
      break;
    }
    return;
  }

  const auto contr = uri_.top();
  uri_.pop();

  if (uri_.empty()) {

    // /accnt/market/CONTR
    state_ |= MatchUri;
    return;
  }

  const auto settlDate = box<IsoDate>(stou64(uri_.top()));
  uri_.pop();

  if (uri_.empty()) {

    // /market/CONTR/SETTL_DATE
    state_ |= MatchUri;

    switch (state_ & MethodMask) {
    case MethodGet:
      // GET /market/CONTR/SETTL_DATE
      state_ |= MatchMethod;
      rest_.getMarket(contr, settlDate, now, out_);
      break;
    case MethodPut:
      // PUT /market/CONTR/SETTL_DATE
      state_ |= MatchMethod;
      getAdmin(data);
      constexpr auto reqFields = RestRequest::State;
      if (!request_.valid(reqFields)) {
        throw InvalidException{"request fields are invalid"_sv};
      }
      rest_.putMarket(contr, settlDate, request_.state(), now, out_);
      break;
    }
    return;
  }
}

void RestServ::accntRequest(HttpMessage data, Millis now)
{
  if (uri_.empty()) {

    // /accnt
    state_ |= MatchUri;

    if (isSet(MethodGet)) {
      // GET /accnt
      state_ |= MatchMethod;
      const auto es = EntitySet::Market | EntitySet::Order | EntitySet::Exec | EntitySet::Trade
        | EntitySet::Posn;
      rest_.getAccnt(getTrader(data), es, getOffset(data), getLimit(data), now, out_);
    }
    return;
  }

  const auto tok = uri_.top();
  uri_.pop();

  const auto es = EntitySet::parse(tok);
  if (es.many()) {

    if (uri_.empty()) {

      // /accnt/entity,entity...
      state_ |= MatchUri;

      if (isSet(MethodGet)) {
        // GET /accnt/entity,entity...
        state_ |= MatchMethod;
        rest_.getAccnt(getTrader(data), es, getOffset(data), getLimit(data), now, out_);
      }
    }
    return;
  }

  switch (es.get()) {
  case EntitySet::Market:
    marketRequest(data, now);
    break;
  case EntitySet::Order:
    orderRequest(data, now);
    break;
  case EntitySet::Exec:
    execRequest(data, now);
    break;
  case EntitySet::Trade:
    tradeRequest(data, now);
    break;
  case EntitySet::Posn:
    posnRequest(data, now);
    break;
  }
}

void RestServ::orderRequest(HttpMessage data, Millis now)
{
  if (uri_.empty()) {

    // /accnt/order
    state_ |= MatchUri;

    if (isSet(MethodGet)) {
      // GET /accnt/order
      state_ |= MatchMethod;
      rest_.getOrder(getTrader(data), now, out_);
    }
    return;
  }

  const auto contr = uri_.top();
  uri_.pop();

  if (uri_.empty()) {

    // /accnt/order/CONTR
    state_ |= MatchUri;
    return;
  }

  const auto settlDate = box<IsoDate>(stou64(uri_.top()));
  uri_.pop();

  if (uri_.empty()) {

    // /accnt/order/CONTR/SETTL_DATE
    state_ |= MatchUri;

    switch (state_ & MethodMask) {
    case MethodGet:
      // GET /accnt/order/CONTR/SETTL_DATE
      state_ |= MatchMethod;
      rest_.getOrder(getTrader(data), contr, settlDate, now, out_);
      break;
    case MethodPost:
      // POST /accnt/order/CONTR/SETTL_DATE
      state_ |= MatchMethod;
      {
        // Validate account before request.
        const auto accnt = getTrader(data);
        constexpr auto reqFields = RestRequest::Side | RestRequest::Lots | RestRequest::Ticks;
        constexpr auto optFields = RestRequest::Ref | RestRequest::MinLots;
        if (!request_.valid(reqFields, optFields)) {
          throw InvalidException{"request fields are invalid"_sv};
        }
        rest_.postOrder(accnt, contr, settlDate, request_.ref(), request_.side(), request_.lots(),
                        request_.ticks(), request_.minLots(), now, out_);
      }
      break;
    }
    return;
  }

  ScopedIds ids{uri_.top(), ids_};
  uri_.pop();

  if (uri_.empty()) {

    // /accnt/order/CONTR/SETTL_DATE/ID,ID...
    state_ |= MatchUri;

    switch (state_ & MethodMask) {
    case MethodGet:
      // GET /accnt/order/CONTR/SETTL_DATE/ID
      state_ |= MatchMethod;
      rest_.getOrder(getTrader(data), contr, settlDate, ids_[0], now, out_);
      break;
    case MethodPut:
      // PUT /accnt/order/CONTR/SETTL_DATE/ID,ID...
      state_ |= MatchMethod;
      {
        // Validate account before request.
        const auto accnt = getTrader(data);
        constexpr auto reqFields = RestRequest::Lots;
        if (request_.fields() != reqFields) {
          throw InvalidException{"request fields are invalid"_sv};
        }
        rest_.putOrder(accnt, contr, settlDate, ids_, request_.lots(), now, out_);
      }
      break;
    }
    return;
  }
}

void RestServ::execRequest(HttpMessage data, Millis now)
{
  if (uri_.empty()) {

    // /accnt/exec
    state_ |= MatchUri;

    if (isSet(MethodGet)) {
      // GET /accnt/exec
      state_ |= MatchMethod;
      rest_.getExec(getTrader(data), getOffset(data), getLimit(data), now, out_);
    }
    return;
  }
}

void RestServ::tradeRequest(HttpMessage data, Millis now)
{
  if (uri_.empty()) {

    // /accnt/trade
    state_ |= MatchUri;

    if (isSet(MethodGet)) {
      // GET /accnt/trade
      state_ |= MatchMethod;
      rest_.getTrade(getTrader(data), now, out_);
    }
    return;
  }

  const auto contr = uri_.top();
  uri_.pop();

  if (uri_.empty()) {

    // /accnt/trade/CONTR
    state_ |= MatchUri;
    return;
  }

  const auto settlDate = box<IsoDate>(stou64(uri_.top()));
  uri_.pop();

  if (uri_.empty()) {

    // /accnt/trade/CONTR/SETTL_DATE
    state_ |= MatchUri;

    switch (state_ & MethodMask) {
    case MethodGet:
      // GET /accnt/trade/CONTR/SETTL_DATE
      state_ |= MatchMethod;
      rest_.getTrade(getTrader(data), contr, settlDate, now, out_);
      break;
    case MethodPost:
      // POST /accnt/trade/CONTR/SETTL_DATE
      state_ |= MatchMethod;
      getAdmin(data);
      {
        constexpr auto reqFields = RestRequest::Accnt | RestRequest::Side | RestRequest::Lots;
        constexpr auto optFields
          = RestRequest::Ref | RestRequest::Ticks | RestRequest::LiqInd | RestRequest::Cpty;
        if (!request_.valid(reqFields, optFields)) {
          throw InvalidException{"request fields are invalid"_sv};
        }
        rest_.postTrade(request_.accnt(), contr, settlDate, request_.ref(), request_.side(),
                        request_.lots(), request_.ticks(), request_.liqInd(), request_.cpty(), now,
                        out_);
      }
      break;
    }
    return;
  }

  ScopedIds ids{uri_.top(), ids_};
  uri_.pop();

  if (uri_.empty()) {

    // /accnt/trade/CONTR/SETTL_DATE/ID,ID...
    state_ |= MatchUri;

    switch (state_ & MethodMask) {
    case MethodGet:
      // GET /accnt/trade/CONTR/SETTL_DATE/ID
      state_ |= MatchMethod;
      rest_.getTrade(getTrader(data), contr, settlDate, ids_[0], now, out_);
      break;
    case MethodDelete:
      // DELETE /accnt/trade/CONTR/SETTL_DATE/ID,ID...
      state_ |= MatchMethod;
      rest_.deleteTrade(getTrader(data), contr, settlDate, ids_, now);
      break;
    }
    return;
  }
}

void RestServ::posnRequest(HttpMessage data, Millis now)
{
  if (uri_.empty()) {

    // /accnt/posn
    state_ |= MatchUri;

    if (isSet(MethodGet)) {
      // GET /accnt/posn
      state_ |= MatchMethod;
      rest_.getPosn(getTrader(data), now, out_);
    }
    return;
  }

  const auto contr = uri_.top();
  uri_.pop();

  if (uri_.empty()) {

    // /accnt/posn/CONTR
    state_ |= MatchUri;

    if (isSet(MethodGet)) {
      // GET /accnt/posn/CONTR
      state_ |= MatchMethod;
      rest_.getPosn(getTrader(data), contr, now, out_);
    }
    return;
  }

  const auto settlDate = box<IsoDate>(stou64(uri_.top()));
  uri_.pop();

  if (uri_.empty()) {

    // /accnt/posn/CONTR/SETTL_DATE
    state_ |= MatchUri;

    if (isSet(MethodGet)) {
      // GET /accnt/posn/CONTR/SETTL_DATE
      state_ |= MatchMethod;
      rest_.getPosn(getTrader(data), contr, settlDate, now, out_);
    }
    return;
  }
}

} // mg
} // swirly
