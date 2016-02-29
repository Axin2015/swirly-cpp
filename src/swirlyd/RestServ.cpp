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
#include <swirly/ash/Time.hpp>

#include <chrono>

using namespace std;

namespace swirly {
namespace mg {

RestServ::~RestServ() noexcept = default;

void RestServ::setUri(string_view uri) noexcept
{
  // Remove leading slash.
  if (uri.front() == '/')
    uri.remove_prefix(1);
  uri_.reset(uri);
}

void RestServ::httpRequest(mg_connection& nc, mg::HttpMessage data)
{
  using namespace chrono;

  const auto start = high_resolution_clock::now();
  auto finally = makeFinally([&data, &start]() {
    const auto end = high_resolution_clock::now();
    const auto usec = duration_cast<microseconds>(end - start);
    SWIRLY_INFO(logMsg() << data.method() << ' ' << data.uri() << " processed in " << usec.count()
                         << " usec");
  });

  setUri(data.uri());

  if (uri_.empty() || uri_.top() != "api") {
    mg_serve_http(&nc, data.get(), httpOpts_);
    return;
  }
  uri_.pop();

  const auto now = getTimeOfDay();
  // See mg_send().
  nc.last_io_time = unbox(now) / 1000;

  StreamBuf buf{nc.send_mbuf};
  out_.rdbuf(&buf);
  out_.reset(200, "OK");
  match_ = false;
  try {
    restRequest(data, now);
    if (!match_)
      throw NotFoundException{"resource does not exist"_sv};
  } catch (const ServException& e) {
    out_.reset(e.httpStatus(), e.httpReason());
    out_ << e;
  } catch (const exception& e) {
    const int status{500};
    const char* const reason{"Internal Server Error"};
    out_.reset(status, reason);
    ServException::toJson(status, reason, e.what(), out_);
  }
  out_.setContentLength();
}

void RestServ::restRequest(mg::HttpMessage data, Millis now)
{
  if (uri_.empty())
    return;

  const auto tok = uri_.top();
  uri_.pop();
  const auto method = data.method();

  if (tok == "rec") {
    if (method == "GET") {
      getRec(data, now);
    } else if (method == "POST") {
      postRec(data, now);
    } else if (method == "PUT") {
      putRec(data, now);
    }
  } else if (tok == "sess") {
    if (method == "GET") {
      getSess(data, now);
    } else if (method == "POST") {
      postSess(data, now);
    } else if (method == "PUT") {
      putSess(data, now);
    } else if (method == "DELETE") {
      deleteSess(data, now);
    }
  } else if (tok == "view") {
    if (method == "GET") {
      getView(data, now);
    }
  }
}

void RestServ::getRec(mg::HttpMessage data, Millis now)
{
  if (uri_.empty()) {

    const int bs{EntitySet::Asset | EntitySet::Contr | EntitySet::Market};
    rest_.getRec(bs, now, out_);
    match_ = true;

  } else {

    const auto tok = uri_.top();
    uri_.pop();

    const auto es = EntitySet::parse(tok);
    if (es.many()) {
      if (uri_.empty()) {
        rest_.getRec(es, now, out_);
        match_ = true;
      }
    } else {
      switch (es.get()) {
      case EntitySet::Asset:
        getAsset(data, now);
        break;
      case EntitySet::Contr:
        getContr(data, now);
        break;
      case EntitySet::Market:
        getMarket(data, now);
        break;
      case EntitySet::Trader:
        getTrader(data, now);
        break;
      }
    }
  }
}

void RestServ::getAsset(mg::HttpMessage data, Millis now)
{
  if (uri_.empty()) {
    rest_.getAsset(now, out_);
    match_ = true;
  } else {
    const auto mnem = uri_.top();
    uri_.pop();
    if (uri_.empty()) {
      rest_.getAsset(mnem, now, out_);
      match_ = true;
    }
  }
}

void RestServ::getContr(mg::HttpMessage data, Millis now)
{
  if (uri_.empty()) {
    rest_.getContr(now, out_);
    match_ = true;
  } else {
    const auto mnem = uri_.top();
    uri_.pop();
    if (uri_.empty()) {
      rest_.getContr(mnem, now, out_);
      match_ = true;
    }
  }
}

void RestServ::getMarket(mg::HttpMessage data, Millis now)
{
  if (uri_.empty()) {
    rest_.getMarket(now, out_);
    match_ = true;
  } else {
    const auto mnem = uri_.top();
    uri_.pop();
    if (uri_.empty()) {
      rest_.getMarket(mnem, now, out_);
      match_ = true;
    }
  }
}

void RestServ::getTrader(mg::HttpMessage data, Millis now)
{
  if (uri_.empty()) {
    rest_.getTrader(now, out_);
    match_ = true;
  } else {
    const auto mnem = uri_.top();
    uri_.pop();
    if (uri_.empty()) {
      rest_.getTrader(mnem, now, out_);
      match_ = true;
    }
  }
}

void RestServ::postRec(mg::HttpMessage data, Millis now)
{
}

void RestServ::putRec(mg::HttpMessage data, Millis now)
{
}

void RestServ::getSess(mg::HttpMessage data, Millis now)
{
}

void RestServ::postSess(mg::HttpMessage data, Millis now)
{
}

void RestServ::putSess(mg::HttpMessage data, Millis now)
{
}

void RestServ::deleteSess(mg::HttpMessage data, Millis now)
{
}

void RestServ::getView(mg::HttpMessage data, Millis now)
{
}

} // mg
} // swirly
