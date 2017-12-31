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

#include <swirly/web/Rest.hpp>

#include <swirly/fin/Exception.hpp>

#include <swirly/util/Finally.hpp>
#include <swirly/util/Log.hpp>

#include <chrono>

using namespace std;

namespace swirly {
namespace {

class ScopedIds {
  public:
    ScopedIds(string_view sv, vector<Id64>& ids) noexcept
      : ids_{ids}
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
    return val.empty() ? UnixClock::now() : toTime(Millis{stou64(val)});
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

} // namespace

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
            throw MethodNotAllowedException{errMsg()
                                            << "method '" << req.method() << "' is not allowed"};
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

    auto tok = path_.top();
    path_.pop();

    if (tok == "refdata"_sv) {
        // /refdata
        refDataRequest(req, now, resp);
    } else if (tok == "accnt"_sv) {
        // /accnt
        accntRequest(req, now, resp);
    } else {
        // Support both plural and singular forms.
        if (!tok.empty() && tok.back() == 's') {
            tok.remove_suffix(1);
        }
        if (tok == "market"_sv) {
            // /markets
            marketRequest(req, now, resp);
        }
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
            const int bs{EntitySet::Asset | EntitySet::Instr};
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
    case EntitySet::Instr:
        instrRequest(req, now, resp);
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

    const auto symbol = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /refdata/asset/SYMBOL
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata/asset/SYMBOL
            matchMethod_ = true;
            rest_.getAsset(symbol, now, resp);
        }
        return;
    }
}

void RestServ::instrRequest(const HttpRequest& req, Time now, HttpResponse& resp)
{
    if (path_.empty()) {

        // /refdata/instrs
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata/instrs
            matchMethod_ = true;
            rest_.getInstr(now, resp);
        }
        return;
    }

    const auto symbol = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /refdata/instrs/SYMBOL
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata/instrs/SYMBOL
            matchMethod_ = true;
            rest_.getInstr(symbol, now, resp);
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

        // /markets
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /markets
            matchMethod_ = true;
            rest_.getMarket(now, resp);
            break;
        case HttpMethod::Post:
            // POST /markets
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields = RestBody::Instr | RestBody::SettlDate;
                constexpr auto OptFields = RestBody::State;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postMarket(req.body().instr(), req.body().settlDate(), req.body().state(),
                                 now, resp);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto instr = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /market/INSTR
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /market/INSTR
            matchMethod_ = true;
            rest_.getMarket(instr, now, resp);
            break;
        case HttpMethod::Post:
            // POST /market/INSTR
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields = RestBody::SettlDate;
                constexpr auto OptFields = RestBody::State;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postMarket(instr, req.body().settlDate(), req.body().state(), now, resp);
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

        // /market/INSTR/SETTL_DATE
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /market/INSTR/SETTL_DATE
            matchMethod_ = true;
            rest_.getMarket(instr, settlDate, now, resp);
            break;
        case HttpMethod::Post:
            // POST /market/INSTR/SETTL_DATE
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields = 0;
                constexpr auto OptFields = RestBody::State;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postMarket(instr, settlDate, req.body().state(), now, resp);
            }
            break;
        case HttpMethod::Put:
            // PUT /market/INSTR/SETTL_DATE
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields = RestBody::State;
                if (!req.body().valid(ReqFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.putMarket(instr, settlDate, req.body().state(), now, resp);
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

        // /accnt/orders
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/orders
            matchMethod_ = true;
            rest_.getOrder(getTrader(req), now, resp);
            break;
        case HttpMethod::Post:
            // POST /accnt/orders
            matchMethod_ = true;
            {
                // Validate account before request.
                const auto accnt = getTrader(req);
                constexpr auto ReqFields = RestBody::Instr | RestBody::SettlDate | RestBody::Side
                    | RestBody::Lots | RestBody::Ticks;
                constexpr auto OptFields = RestBody::Ref | RestBody::MinLots;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postOrder(accnt, req.body().instr(), req.body().settlDate(), req.body().ref(),
                                req.body().side(), req.body().lots(), req.body().ticks(),
                                req.body().minLots(), now, resp);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto instr = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /accnt/orders/INSTR
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/orders/INSTR
            matchMethod_ = true;
            rest_.getOrder(getTrader(req), instr, now, resp);
            break;
        case HttpMethod::Post:
            // POST /accnt/orders/INSTR
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
                rest_.postOrder(accnt, instr, req.body().settlDate(), req.body().ref(),
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

        // /accnt/orders/INSTR/SETTL_DATE
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/orders/INSTR/SETTL_DATE
            matchMethod_ = true;
            rest_.getOrder(getTrader(req), instr, settlDate, now, resp);
            break;
        case HttpMethod::Post:
            // POST /accnt/orders/INSTR/SETTL_DATE
            matchMethod_ = true;
            {
                // Validate account before request.
                const auto accnt = getTrader(req);
                constexpr auto ReqFields = RestBody::Side | RestBody::Lots | RestBody::Ticks;
                constexpr auto OptFields = RestBody::Ref | RestBody::MinLots;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postOrder(accnt, instr, settlDate, req.body().ref(), req.body().side(),
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

        // /accnt/orders/INSTR/SETTL_DATE/ID,ID...
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/orders/INSTR/SETTL_DATE/ID
            matchMethod_ = true;
            rest_.getOrder(getTrader(req), instr, settlDate, ids_[0], now, resp);
            break;
        case HttpMethod::Put:
            // PUT /accnt/orders/INSTR/SETTL_DATE/ID,ID...
            matchMethod_ = true;
            {
                // Validate account before request.
                const auto accnt = getTrader(req);
                constexpr auto ReqFields = RestBody::Lots;
                if (req.body().fields() != ReqFields) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.putOrder(accnt, instr, settlDate, ids_, req.body().lots(), now, resp);
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

        // /accnt/execs
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt/execs
            matchMethod_ = true;
            rest_.getExec(getTrader(req), parseQuery(req.query()), now, resp);
        }
        return;
    }
}

void RestServ::tradeRequest(const HttpRequest& req, Time now, HttpResponse& resp)
{
    if (path_.empty()) {

        // /accnt/trades
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/trades
            matchMethod_ = true;
            rest_.getTrade(getTrader(req), now, resp);
            break;
        case HttpMethod::Post:
            // POST /accnt/trades
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields = RestBody::Instr | RestBody::SettlDate | RestBody::Accnt
                    | RestBody::Side | RestBody::Lots;
                constexpr auto OptFields
                    = RestBody::Ref | RestBody::Ticks | RestBody::LiqInd | RestBody::Cpty;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postTrade(req.body().accnt(), req.body().instr(), req.body().settlDate(),
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

    const auto instr = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /accnt/trades/INSTR
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/trades/INSTR
            matchMethod_ = true;
            rest_.getTrade(getTrader(req), instr, now, resp);
            break;
        case HttpMethod::Post:
            // POST /accnt/trades/INSTR
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
                rest_.postTrade(req.body().accnt(), instr, req.body().settlDate(), req.body().ref(),
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

        // /accnt/trades/INSTR/SETTL_DATE
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/trades/INSTR/SETTL_DATE
            matchMethod_ = true;
            rest_.getTrade(getTrader(req), instr, settlDate, now, resp);
            break;
        case HttpMethod::Post:
            // POST /accnt/trades/INSTR/SETTL_DATE
            matchMethod_ = true;
            getAdmin(req);
            {
                constexpr auto ReqFields = RestBody::Accnt | RestBody::Side | RestBody::Lots;
                constexpr auto OptFields
                    = RestBody::Ref | RestBody::Ticks | RestBody::LiqInd | RestBody::Cpty;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"_sv};
                }
                rest_.postTrade(req.body().accnt(), instr, settlDate, req.body().ref(),
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

        // /accnt/trades/INSTR/SETTL_DATE/ID,ID...
        matchPath_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/trades/INSTR/SETTL_DATE/ID
            matchMethod_ = true;
            rest_.getTrade(getTrader(req), instr, settlDate, ids_[0], now, resp);
            break;
        case HttpMethod::Delete:
            // DELETE /accnt/trades/INSTR/SETTL_DATE/ID,ID...
            matchMethod_ = true;
            rest_.deleteTrade(getTrader(req), instr, settlDate, ids_, now);
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

        // /accnt/posns
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt/posns
            matchMethod_ = true;
            rest_.getPosn(getTrader(req), now, resp);
        }
        return;
    }

    const auto instr = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /accnt/posns/INSTR
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt/posns/INSTR
            matchMethod_ = true;
            rest_.getPosn(getTrader(req), instr, now, resp);
        }
        return;
    }

    const auto settlDate = IsoDate{stou64(path_.top())};
    path_.pop();

    if (path_.empty()) {

        // /accnt/posns/INSTR/SETTL_DATE
        matchPath_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt/posns/INSTR/SETTL_DATE
            matchMethod_ = true;
            rest_.getPosn(getTrader(req), instr, settlDate, now, resp);
        }
        return;
    }
}

} // namespace swirly
