/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

#include <swirly/ws/Rest.hpp>

#include <swirly/fin/Exception.hpp>

#include <swirly/util/Finally.hpp>
#include <swirly/util/Log.hpp>

#include <chrono>

using namespace std;

namespace swirly {
namespace {

class ScopedIds {
  public:
    ScopedIds(string_view sv, vector<Id64>& ids) noexcept : ids_{ids}
    {
        Tokeniser toks{sv, ","_sv};
        while (!toks.empty()) {
            ids.push_back(static_cast<Id64>(stou64(toks.top())));
            toks.pop();
        }
    }
    ~ScopedIds() noexcept { ids_.clear(); }

  private:
    vector<Id64>& ids_;
};

string_view getAccnt(const HttpRequest& req)
{
    const string_view accnt{req.accnt()};
    if (accnt.empty()) {
        throw UnauthorizedException{"user account not specified"_sv};
    }
    return accnt;
}

uint32_t getPerm(const HttpRequest& req)
{
    return stou32(req.perm());
}

Time getTime(const HttpRequest& req) noexcept
{
    const string_view val{req.time()};
    return val.empty() ? UnixClock::now() : msToTime(stou64(val));
}

string_view getAdmin(const HttpRequest& req)
{
    const auto accnt = getAccnt(req);
    const auto perm = getPerm(req);
    if (!(perm & 0x1)) {
        throw ForbiddenException{"user account does not have admin permission "_sv};
    }
    return accnt;
}

string_view getTrader(const HttpRequest& req)
{
    const auto accnt = getAccnt(req);
    const auto perm = getPerm(req);
    if (!(perm & 0x2)) {
        throw ForbiddenException{"user account does not have trade permission "_sv};
    }
    return accnt;
}

} // anonymous

RestServ::~RestServ() noexcept = default;

void RestServ::handleRequest(const HttpRequest& req, HttpResponse& resp) noexcept
{
    TimeRecorder tr{profile_};
    auto finally = makeFinally([this]() {
        if (this->profile_.size() % 10 == 0) {
            this->profile_.report();
        }
    });
    const auto cache = reset(req); // noexcept
    const auto now = getTime(req); // noexcept

    if (req.method() != HttpMethod::Delete) {
        resp.reset(200, "OK", cache); // noexcept
    } else {
        resp.reset(204, "No Content"); // noexcept
    }
    try {
        const auto& body = req.body();
        if (req.partial()) {
            throw BadRequestException{"request body is incomplete"_sv};
        }
        restRequest(req, now, resp);
        if (!matchPath_) {
            throw NotFoundException{errMsg() << "resource '" << req.path() << "' does not exist"};
        }
        if (!matchMethod_) {
            throw MethodNotAllowedException{errMsg() << "method '" << req.method()
                                                     << "' is not allowed"};
        }
    } catch (const ServException& e) {
        SWIRLY_ERROR(logMsg() << "exception: status=" << e.httpStatus()
                              << ", reason=" << e.httpReason() << ", detail=" << e.what());
        resp.reset(e.httpStatus(), e.httpReason());
        resp << e;
    } catch (const exception& e) {
        const int status{500};
        const char* const reason{"Internal Server Error"};
        SWIRLY_ERROR(logMsg() << "exception: status=" << status << ", reason=" << reason
                              << ", detail=" << e.what());
        resp.reset(status, reason);
        ServException::toJson(status, reason, e.what(), resp);
    }
    resp.setContentLength(); // noexcept
}

bool RestServ::reset(const HttpRequest& req) noexcept
{
    matchMethod_ = false;
    matchPath_ = false;

    auto path = req.path();
    // Remove leading slash.
    if (path.front() == '/') {
        path.remove_prefix(1);
    }
    path_.reset(path, "/"_sv);

    if (req.method() != HttpMethod::Get) {
        // No cache.
        return false;
    }
    // Cache if GET for refdata.
    return !path.empty() && path_.top() == "refdata"_sv;
}

void RestServ::restRequest(const HttpRequest& req, Time now, HttpResponse& resp)
{
    if (path_.empty()) {
        return;
    }

    const auto tok = path_.top();
    path_.pop();

    if (tok == "refdata"_sv) {
        // /refdata
        refDataRequest(req, now, resp);
    } else if (tok == "accnt"_sv) {
        // /accnt
        accntRequest(req, now, resp);
    } else if (tok == "market"_sv) {
        // /market
        marketRequest(req, now, resp);
    }
}

void RestServ::refDataRequest(const HttpRequest& req, Time now, HttpResponse& resp)
{
    if (path_.empty()) {

        // /refdata
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata
            matchMethod_ = true;
            const int bs{EntitySet::Asset | EntitySet::Contr};
            rest_.getRefData(bs, now, resp);
        }
        return;
    }

    const auto tok = path_.top();
    path_.pop();

    const auto es = EntitySet::parse(tok);
    if (es.many()) {

        if (path_.empty()) {

            // /refdata/entity,entity...
            matchPath_ = true;

            if (req.method() == HttpMethod::Get) {
                // GET /refdata/entity,entity...
                matchMethod_ = true;
                rest_.getRefData(es, now, resp);
            }
        }
        return;
    }

    switch (es.get()) {
    case EntitySet::Asset:
        assetRequest(req, now, resp);
        break;
    case EntitySet::Contr:
        contrRequest(req, now, resp);
        break;
    }
}

void RestServ::assetRequest(const HttpRequest& req, Time now, HttpResponse& resp)
{
    if (path_.empty()) {

        // /refdata/asset
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata/asset
            matchMethod_ = true;
            rest_.getAsset(now, resp);
        }
        return;
    }

    const auto mnem = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /refdata/asset/MNEM
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata/asset/MNEM
            matchMethod_ = true;
            rest_.getAsset(mnem, now, resp);
        }
        return;
    }
}

void RestServ::contrRequest(const HttpRequest& req, Time now, HttpResponse& resp)
{
    if (path_.empty()) {

        // /refdata/contr
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata/contr
            matchMethod_ = true;
            rest_.getContr(now, resp);
        }
        return;
    }

    const auto mnem = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /refdata/contr/MNEM
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata/contr/MNEM
            matchMethod_ = true;
            rest_.getContr(mnem, now, resp);
        }
        return;
    }
}

void RestServ::accntRequest(const HttpRequest& req, Time now, HttpResponse& resp)
{
    if (path_.empty()) {

        // /accnt
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt
            matchMethod_ = true;
            const auto es = EntitySet::Market | EntitySet::Order | EntitySet::Exec
                | EntitySet::Trade | EntitySet::Posn;
            rest_.getAccnt(getTrader(req), es, parseQuery(req.query()), now, resp);
        }
        return;
    }

    const auto tok = path_.top();
    path_.pop();

    const auto es = EntitySet::parse(tok);
    if (es.many()) {

        if (path_.empty()) {

            // /accnt/entity,entity...
            matchPath_ = true;

            if (req.method() == HttpMethod::Get) {
                // GET /accnt/entity,entity...
                matchMethod_ = true;
                rest_.getAccnt(getTrader(req), es, parseQuery(req.query()), now, resp);
            }
        }
        return;
    }

    switch (es.get()) {
    case EntitySet::Market:
        marketRequest(req, now, resp);
        break;
    case EntitySet::Order:
        orderRequest(req, now, resp);
        break;
    case EntitySet::Exec:
        execRequest(req, now, resp);
        break;
    case EntitySet::Trade:
        tradeRequest(req, now, resp);
        break;
    case EntitySet::Posn:
        posnRequest(req, now, resp);
        break;
    }
}

void RestServ::marketRequest(const HttpRequest& req, Time now, HttpResponse& resp)
{
    if (path_.empty()) {

        // /market
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /market
            matchMethod_ = true;
            rest_.getMarket(now, resp);
            break;
        case HttpMethod::Post:
            // POST /market
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields = RestBody::Contr | RestBody::SettlDate;
                constexpr auto OptFields = RestBody::State;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postMarket(req.body().contr(), req.body().settlDate(), req.body().state(),
                                 now, resp);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto contr = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /market/CONTR
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /market/CONTR
            matchMethod_ = true;
            rest_.getMarket(contr, now, resp);
            break;
        case HttpMethod::Post:
            // POST /market/CONTR
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields = RestBody::SettlDate;
                constexpr auto OptFields = RestBody::State;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postMarket(contr, req.body().settlDate(), req.body().state(), now, resp);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto settlDate = IsoDate{stou64(path_.top())};
    path_.pop();

    if (path_.empty()) {

        // /market/CONTR/SETTL_DATE
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /market/CONTR/SETTL_DATE
            matchMethod_ = true;
            rest_.getMarket(contr, settlDate, now, resp);
            break;
        case HttpMethod::Post:
            // POST /market/CONTR/SETTL_DATE
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields = 0;
                constexpr auto OptFields = RestBody::State;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postMarket(contr, settlDate, req.body().state(), now, resp);
            }
            break;
        case HttpMethod::Put:
            // PUT /market/CONTR/SETTL_DATE
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields = RestBody::State;
                if (!req.body().valid(ReqFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.putMarket(contr, settlDate, req.body().state(), now, resp);
            }
            break;
        default:
            break;
        }
        return;
    }
}

void RestServ::orderRequest(const HttpRequest& req, Time now, HttpResponse& resp)
{
    if (path_.empty()) {

        // /accnt/order
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/order
            matchMethod_ = true;
            rest_.getOrder(getTrader(req), now, resp);
            break;
        case HttpMethod::Post:
            // POST /accnt/order
            matchMethod_ = true;
            {
                // Validate account before request.
                const auto accnt = getTrader(req);
                constexpr auto ReqFields = RestBody::Contr | RestBody::SettlDate | RestBody::Side
                    | RestBody::Lots | RestBody::Ticks;
                constexpr auto OptFields = RestBody::Ref | RestBody::MinLots;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postOrder(accnt, req.body().contr(), req.body().settlDate(), req.body().ref(),
                                req.body().side(), req.body().lots(), req.body().ticks(),
                                req.body().minLots(), now, resp);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto contr = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /accnt/order/CONTR
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/order/CONTR
            matchMethod_ = true;
            rest_.getOrder(getTrader(req), contr, now, resp);
            break;
        case HttpMethod::Post:
            // POST /accnt/order/CONTR
            matchMethod_ = true;
            {
                // Validate account before request.
                const auto accnt = getTrader(req);
                constexpr auto ReqFields
                    = RestBody::SettlDate | RestBody::Side | RestBody::Lots | RestBody::Ticks;
                constexpr auto OptFields = RestBody::Ref | RestBody::MinLots;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postOrder(accnt, contr, req.body().settlDate(), req.body().ref(),
                                req.body().side(), req.body().lots(), req.body().ticks(),
                                req.body().minLots(), now, resp);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto settlDate = IsoDate{stou64(path_.top())};
    path_.pop();

    if (path_.empty()) {

        // /accnt/order/CONTR/SETTL_DATE
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/order/CONTR/SETTL_DATE
            matchMethod_ = true;
            rest_.getOrder(getTrader(req), contr, settlDate, now, resp);
            break;
        case HttpMethod::Post:
            // POST /accnt/order/CONTR/SETTL_DATE
            matchMethod_ = true;
            {
                // Validate account before request.
                const auto accnt = getTrader(req);
                constexpr auto ReqFields = RestBody::Side | RestBody::Lots | RestBody::Ticks;
                constexpr auto OptFields = RestBody::Ref | RestBody::MinLots;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postOrder(accnt, contr, settlDate, req.body().ref(), req.body().side(),
                                req.body().lots(), req.body().ticks(), req.body().minLots(), now,
                                resp);
            }
            break;
        default:
            break;
        }
        return;
    }

    ScopedIds ids{path_.top(), ids_};
    path_.pop();

    if (path_.empty()) {

        // /accnt/order/CONTR/SETTL_DATE/ID,ID...
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/order/CONTR/SETTL_DATE/ID
            matchMethod_ = true;
            rest_.getOrder(getTrader(req), contr, settlDate, ids_[0], now, resp);
            break;
        case HttpMethod::Put:
            // PUT /accnt/order/CONTR/SETTL_DATE/ID,ID...
            matchMethod_ = true;
            {
                // Validate account before request.
                const auto accnt = getTrader(req);
                constexpr auto ReqFields = RestBody::Lots;
                if (req.body().fields() != ReqFields) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.putOrder(accnt, contr, settlDate, ids_, req.body().lots(), now, resp);
            }
            break;
        default:
            break;
        }
        return;
    }
}

void RestServ::execRequest(const HttpRequest& req, Time now, HttpResponse& resp)
{
    if (path_.empty()) {

        // /accnt/exec
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt/exec
            matchMethod_ = true;
            rest_.getExec(getTrader(req), parseQuery(req.query()), now, resp);
        }
        return;
    }
}

void RestServ::tradeRequest(const HttpRequest& req, Time now, HttpResponse& resp)
{
    if (path_.empty()) {

        // /accnt/trade
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/trade
            matchMethod_ = true;
            rest_.getTrade(getTrader(req), now, resp);
            break;
        case HttpMethod::Post:
            // POST /accnt/trade
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields = RestBody::Contr | RestBody::SettlDate | RestBody::Accnt
                    | RestBody::Side | RestBody::Lots;
                constexpr auto OptFields
                    = RestBody::Ref | RestBody::Ticks | RestBody::LiqInd | RestBody::Cpty;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postTrade(req.body().accnt(), req.body().contr(), req.body().settlDate(),
                                req.body().ref(), req.body().side(), req.body().lots(),
                                req.body().ticks(), req.body().liqInd(), req.body().cpty(), now,
                                resp);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto contr = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /accnt/trade/CONTR
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/trade/CONTR
            matchMethod_ = true;
            rest_.getTrade(getTrader(req), contr, now, resp);
            break;
        case HttpMethod::Post:
            // POST /accnt/trade/CONTR
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields
                    = RestBody::SettlDate | RestBody::Accnt | RestBody::Side | RestBody::Lots;
                constexpr auto OptFields
                    = RestBody::Ref | RestBody::Ticks | RestBody::LiqInd | RestBody::Cpty;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postTrade(req.body().accnt(), contr, req.body().settlDate(), req.body().ref(),
                                req.body().side(), req.body().lots(), req.body().ticks(),
                                req.body().liqInd(), req.body().cpty(), now, resp);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto settlDate = IsoDate{stou64(path_.top())};
    path_.pop();

    if (path_.empty()) {

        // /accnt/trade/CONTR/SETTL_DATE
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/trade/CONTR/SETTL_DATE
            matchMethod_ = true;
            rest_.getTrade(getTrader(req), contr, settlDate, now, resp);
            break;
        case HttpMethod::Post:
            // POST /accnt/trade/CONTR/SETTL_DATE
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields = RestBody::Accnt | RestBody::Side | RestBody::Lots;
                constexpr auto OptFields
                    = RestBody::Ref | RestBody::Ticks | RestBody::LiqInd | RestBody::Cpty;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postTrade(req.body().accnt(), contr, settlDate, req.body().ref(),
                                req.body().side(), req.body().lots(), req.body().ticks(),
                                req.body().liqInd(), req.body().cpty(), now, resp);
            }
            break;
        default:
            break;
        }
        return;
    }

    ScopedIds ids{path_.top(), ids_};
    path_.pop();

    if (path_.empty()) {

        // /accnt/trade/CONTR/SETTL_DATE/ID,ID...
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/trade/CONTR/SETTL_DATE/ID
            matchMethod_ = true;
            rest_.getTrade(getTrader(req), contr, settlDate, ids_[0], now, resp);
            break;
        case HttpMethod::Delete:
            // DELETE /accnt/trade/CONTR/SETTL_DATE/ID,ID...
            matchMethod_ = true;
            rest_.deleteTrade(getTrader(req), contr, settlDate, ids_, now);
            break;
        default:
            break;
        }
        return;
    }
}

void RestServ::posnRequest(const HttpRequest& req, Time now, HttpResponse& resp)
{
    if (path_.empty()) {

        // /accnt/posn
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt/posn
            matchMethod_ = true;
            rest_.getPosn(getTrader(req), now, resp);
        }
        return;
    }

    const auto contr = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /accnt/posn/CONTR
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt/posn/CONTR
            matchMethod_ = true;
            rest_.getPosn(getTrader(req), contr, now, resp);
        }
        return;
    }

    const auto settlDate = IsoDate{stou64(path_.top())};
    path_.pop();

    if (path_.empty()) {

        // /accnt/posn/CONTR/SETTL_DATE
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt/posn/CONTR/SETTL_DATE
            matchMethod_ = true;
            rest_.getPosn(getTrader(req), contr, settlDate, now, resp);
        }
        return;
    }
}

} // swirly
