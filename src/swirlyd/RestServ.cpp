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
#include "RestServ.hpp"

#include <swirly/web/Request.hpp>
#include <swirly/web/Rest.hpp>
#include <swirly/web/Stream.hpp>

#include <swirly/fin/Exception.hpp>

#include <swirly/util/Finally.hpp>
#include <swirly/util/Log.hpp>

#include <chrono>

namespace swirly {
using namespace std;
namespace {

class ScopedIds {
  public:
    ScopedIds(string_view sv, vector<Id64>& ids) noexcept
    : ids_{ids}
    {
        Tokeniser toks{sv, ","sv};
        while (!toks.empty()) {
            ids.push_back(static_cast<Id64>(stou64(toks.top())));
            toks.pop();
        }
    }
    ~ScopedIds() { ids_.clear(); }

  private:
    vector<Id64>& ids_;
};

string_view get_accnt(const HttpRequest& req)
{
    const string_view accnt{req.accnt()};
    if (accnt.empty()) {
        throw UnauthorizedException{"user account not specified"sv};
    }
    return accnt;
}

uint32_t get_perm(const HttpRequest& req)
{
    return stou32(req.perm());
}

Time get_time(const HttpRequest& req) noexcept
{
    const string_view val{req.time()};
    return val.empty() ? UnixClock::now() : to_time(Millis{stou64(val)});
}

string_view get_admin(const HttpRequest& req)
{
    const auto accnt = get_accnt(req);
    const auto perm = get_perm(req);
    if (!(perm & 0x1)) {
        throw ForbiddenException{"user account does not have admin permission "sv};
    }
    return accnt;
}

string_view get_trader(const HttpRequest& req)
{
    const auto accnt = get_accnt(req);
    const auto perm = get_perm(req);
    if (!(perm & 0x2)) {
        throw ForbiddenException{"user account does not have trade permission "sv};
    }
    return accnt;
}

} // namespace

RestServ::~RestServ() = default;

void RestServ::handle_request(const HttpRequest& req, HttpStream& os) noexcept
{
    const auto cache = reset(req);  // noexcept
    const auto now = get_time(req); // noexcept

    if (req.method() != HttpMethod::Delete) {
        os.reset(HttpStatus::Ok, cache); // noexcept
    } else {
        os.reset(HttpStatus::NoContent); // noexcept
    }
    try {
        const auto& body = req.body();
        if (req.partial()) {
            throw BadRequestException{"request body is incomplete"sv};
        }
        rest_request(req, now, os);
        if (!match_path_) {
            throw NotFoundException{err_msg()
                                    << "resource '"sv << req.path() << "' does not exist"sv};
        }
        if (!match_method_) {
            throw MethodNotAllowedException{err_msg() << "method '"sv << req.method()
                                                      << "' is not allowed"sv};
        }
    } catch (const Exception& e) {
        const auto status = http_status(e.code());
        const char* const reason = http_reason(status);
        SWIRLY_ERROR << "exception: status="sv << status << ", reason="sv << reason << ", detail="sv
                     << e.what();
        os.reset(status, reason);
        Exception::to_json(os, static_cast<int>(status), reason, e.what());
    } catch (const exception& e) {
        const auto status = HttpStatus::InternalServerError;
        const char* const reason = http_reason(status);
        SWIRLY_ERROR << "exception: status="sv << status << ", reason="sv << reason << ", detail="sv
                     << e.what();
        os.reset(status, reason);
        Exception::to_json(os, static_cast<int>(status), reason, e.what());
    }
    os.commit(); // noexcept
}

bool RestServ::reset(const HttpRequest& req) noexcept
{
    match_method_ = false;
    match_path_ = false;

    auto path = req.path();
    // Remove leading slash.
    if (path.front() == '/') {
        path.remove_prefix(1);
    }
    path_.reset(path, "/"sv);

    if (req.method() != HttpMethod::Get) {
        // No cache.
        return false;
    }
    // Cache if GET for refdata.
    return !path.empty() && path_.top() == "refdata"sv;
}

void RestServ::rest_request(const HttpRequest& req, Time now, HttpStream& os)
{
    if (path_.empty()) {
        return;
    }

    auto tok = path_.top();
    path_.pop();

    if (tok == "refdata"sv) {
        // /refdata
        ref_data_request(req, now, os);
    } else if (tok == "accnt"sv) {
        // /accnt
        accnt_request(req, now, os);
    } else {
        // Support both plural and singular forms.
        if (!tok.empty() && tok.back() == 's') {
            tok.remove_suffix(1);
        }
        if (tok == "market"sv) {
            // /markets
            market_request(req, now, os);
        }
    }
}

void RestServ::ref_data_request(const HttpRequest& req, Time now, HttpStream& os)
{
    if (path_.empty()) {

        // /refdata
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata
            match_method_ = true;
            const int bs{EntitySet::Asset | EntitySet::Instr};
            rest_.get_ref_data(bs, now, os);
        }
        return;
    }

    const auto tok = path_.top();
    path_.pop();

    const auto es = EntitySet::parse(tok);
    if (es.many()) {

        if (path_.empty()) {

            // /refdata/entity,entity...
            match_path_ = true;

            if (req.method() == HttpMethod::Get) {
                // GET /refdata/entity,entity...
                match_method_ = true;
                rest_.get_ref_data(es, now, os);
            }
        }
        return;
    }

    switch (es.get()) {
    case EntitySet::Asset:
        asset_request(req, now, os);
        break;
    case EntitySet::Instr:
        instr_request(req, now, os);
        break;
    }
}

void RestServ::asset_request(const HttpRequest& req, Time now, HttpStream& os)
{
    if (path_.empty()) {

        // /refdata/assets
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata/assets
            match_method_ = true;
            rest_.get_asset(now, os);
        }
        return;
    }

    const auto symbol = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /refdata/assets/SYMBOL
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata/assets/SYMBOL
            match_method_ = true;
            rest_.get_asset(symbol, now, os);
        }
        return;
    }
}

void RestServ::instr_request(const HttpRequest& req, Time now, HttpStream& os)
{
    if (path_.empty()) {

        // /refdata/instrs
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata/instrs
            match_method_ = true;
            rest_.get_instr(now, os);
        }
        return;
    }

    const auto symbol = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /refdata/instrs/SYMBOL
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /refdata/instrs/SYMBOL
            match_method_ = true;
            rest_.get_instr(symbol, now, os);
        }
        return;
    }
}

void RestServ::accnt_request(const HttpRequest& req, Time now, HttpStream& os)
{
    if (path_.empty()) {

        // /accnt
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt
            match_method_ = true;
            const auto es = EntitySet::Market | EntitySet::Order | EntitySet::Exec
                | EntitySet::Trade | EntitySet::Posn;
            rest_.get_accnt(get_trader(req), es, parse_query(req.query()), now, os);
        }
        return;
    }

    const auto tok = path_.top();
    path_.pop();

    const auto es = EntitySet::parse(tok);
    if (es.many()) {

        if (path_.empty()) {

            // /accnt/entity,entity...
            match_path_ = true;

            if (req.method() == HttpMethod::Get) {
                // GET /accnt/entity,entity...
                match_method_ = true;
                rest_.get_accnt(get_trader(req), es, parse_query(req.query()), now, os);
            }
        }
        return;
    }

    switch (es.get()) {
    case EntitySet::Market:
        market_request(req, now, os);
        break;
    case EntitySet::Order:
        order_request(req, now, os);
        break;
    case EntitySet::Exec:
        exec_request(req, now, os);
        break;
    case EntitySet::Trade:
        trade_request(req, now, os);
        break;
    case EntitySet::Posn:
        posn_request(req, now, os);
        break;
    }
}

void RestServ::market_request(const HttpRequest& req, Time now, HttpStream& os)
{
    if (path_.empty()) {

        // /markets
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /markets
            match_method_ = true;
            rest_.get_market(now, os);
            break;
        case HttpMethod::Post:
            // POST /markets
            match_method_ = true;
            get_admin(req);
            {
                constexpr auto ReqFields = RestBody::Instr | RestBody::SettlDate;
                constexpr auto OptFields = RestBody::State;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                rest_.post_market(req.body().instr(), req.body().settl_date(), req.body().state(),
                                  now, os);
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

        // /markets/INSTR
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /markets/INSTR
            match_method_ = true;
            rest_.get_market(instr, now, os);
            break;
        case HttpMethod::Post:
            // POST /markets/INSTR
            match_method_ = true;
            get_admin(req);
            {
                constexpr auto ReqFields = RestBody::SettlDate;
                constexpr auto OptFields = RestBody::State;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                rest_.post_market(instr, req.body().settl_date(), req.body().state(), now, os);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto settl_date = IsoDate{stou64(path_.top())};
    path_.pop();

    if (path_.empty()) {

        // /markets/INSTR/SETTL_DATE
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /markets/INSTR/SETTL_DATE
            match_method_ = true;
            rest_.get_market(instr, settl_date, now, os);
            break;
        case HttpMethod::Post:
            // POST /markets/INSTR/SETTL_DATE
            match_method_ = true;
            get_admin(req);
            {
                constexpr auto ReqFields = 0;
                constexpr auto OptFields = RestBody::State;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                rest_.post_market(instr, settl_date, req.body().state(), now, os);
            }
            break;
        case HttpMethod::Put:
            // PUT /markets/INSTR/SETTL_DATE
            match_method_ = true;
            get_admin(req);
            {
                constexpr auto ReqFields = RestBody::State;
                if (!req.body().valid(ReqFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                rest_.put_market(instr, settl_date, req.body().state(), now, os);
            }
            break;
        default:
            break;
        }
        return;
    }
}

void RestServ::order_request(const HttpRequest& req, Time now, HttpStream& os)
{
    if (path_.empty()) {

        // /accnts/orders
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnts/orders
            match_method_ = true;
            rest_.get_order(get_trader(req), now, os);
            break;
        case HttpMethod::Post:
            // POST /accnts/orders
            match_method_ = true;
            {
                // Validate account before request.
                const auto accnt = get_trader(req);
                constexpr auto ReqFields = RestBody::Instr | RestBody::SettlDate | RestBody::Side
                    | RestBody::Lots | RestBody::Ticks;
                constexpr auto OptFields = RestBody::Ref | RestBody::MinLots;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                rest_.post_order(accnt, req.body().instr(), req.body().settl_date(),
                                 req.body().ref(), req.body().side(), req.body().lots(),
                                 req.body().ticks(), req.body().min_lots(), now, os);
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
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/orders/INSTR
            match_method_ = true;
            rest_.get_order(get_trader(req), instr, now, os);
            break;
        case HttpMethod::Post:
            // POST /accnt/orders/INSTR
            match_method_ = true;
            {
                // Validate account before request.
                const auto accnt = get_trader(req);
                constexpr auto ReqFields
                    = RestBody::SettlDate | RestBody::Side | RestBody::Lots | RestBody::Ticks;
                constexpr auto OptFields = RestBody::Ref | RestBody::MinLots;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                rest_.post_order(accnt, instr, req.body().settl_date(), req.body().ref(),
                                 req.body().side(), req.body().lots(), req.body().ticks(),
                                 req.body().min_lots(), now, os);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto settl_date = IsoDate{stou64(path_.top())};
    path_.pop();

    if (path_.empty()) {

        // /accnt/orders/INSTR/SETTL_DATE
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/orders/INSTR/SETTL_DATE
            match_method_ = true;
            rest_.get_order(get_trader(req), instr, settl_date, now, os);
            break;
        case HttpMethod::Post:
            // POST /accnt/orders/INSTR/SETTL_DATE
            match_method_ = true;
            {
                // Validate account before request.
                const auto accnt = get_trader(req);
                constexpr auto ReqFields = RestBody::Side | RestBody::Lots | RestBody::Ticks;
                constexpr auto OptFields = RestBody::Ref | RestBody::MinLots;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                rest_.post_order(accnt, instr, settl_date, req.body().ref(), req.body().side(),
                                 req.body().lots(), req.body().ticks(), req.body().min_lots(), now,
                                 os);
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
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/orders/INSTR/SETTL_DATE/ID
            match_method_ = true;
            rest_.get_order(get_trader(req), instr, settl_date, ids_[0], now, os);
            break;
        case HttpMethod::Put:
            // PUT /accnt/orders/INSTR/SETTL_DATE/ID,ID...
            match_method_ = true;
            {
                // Validate account before request.
                const auto accnt = get_trader(req);
                constexpr auto ReqFields = RestBody::Lots;
                if (req.body().fields() != ReqFields) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                rest_.put_order(accnt, instr, settl_date, ids_, req.body().lots(), now, os);
            }
            break;
        default:
            break;
        }
        return;
    }
}

void RestServ::exec_request(const HttpRequest& req, Time now, HttpStream& os)
{
    if (path_.empty()) {

        // /accnt/execs
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt/execs
            match_method_ = true;
            rest_.get_exec(get_trader(req), parse_query(req.query()), now, os);
        }
        return;
    }
}

void RestServ::trade_request(const HttpRequest& req, Time now, HttpStream& os)
{
    if (path_.empty()) {

        // /accnt/trades
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/trades
            match_method_ = true;
            rest_.get_trade(get_trader(req), now, os);
            break;
        case HttpMethod::Post:
            // POST /accnt/trades
            match_method_ = true;
            get_admin(req);
            {
                constexpr auto ReqFields = RestBody::Instr | RestBody::SettlDate | RestBody::Accnt
                    | RestBody::Side | RestBody::Lots;
                constexpr auto OptFields
                    = RestBody::Ref | RestBody::Ticks | RestBody::LiqInd | RestBody::Cpty;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                rest_.post_trade(req.body().accnt(), req.body().instr(), req.body().settl_date(),
                                 req.body().ref(), req.body().side(), req.body().lots(),
                                 req.body().ticks(), req.body().liq_ind(), req.body().cpty(), now,
                                 os);
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
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/trades/INSTR
            match_method_ = true;
            rest_.get_trade(get_trader(req), instr, now, os);
            break;
        case HttpMethod::Post:
            // POST /accnt/trades/INSTR
            match_method_ = true;
            get_admin(req);
            {
                constexpr auto ReqFields
                    = RestBody::SettlDate | RestBody::Accnt | RestBody::Side | RestBody::Lots;
                constexpr auto OptFields
                    = RestBody::Ref | RestBody::Ticks | RestBody::LiqInd | RestBody::Cpty;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                rest_.post_trade(req.body().accnt(), instr, req.body().settl_date(),
                                 req.body().ref(), req.body().side(), req.body().lots(),
                                 req.body().ticks(), req.body().liq_ind(), req.body().cpty(), now,
                                 os);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto settl_date = IsoDate{stou64(path_.top())};
    path_.pop();

    if (path_.empty()) {

        // /accnt/trades/INSTR/SETTL_DATE
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/trades/INSTR/SETTL_DATE
            match_method_ = true;
            rest_.get_trade(get_trader(req), instr, settl_date, now, os);
            break;
        case HttpMethod::Post:
            // POST /accnt/trades/INSTR/SETTL_DATE
            match_method_ = true;
            get_admin(req);
            {
                constexpr auto ReqFields = RestBody::Accnt | RestBody::Side | RestBody::Lots;
                constexpr auto OptFields
                    = RestBody::Ref | RestBody::Ticks | RestBody::LiqInd | RestBody::Cpty;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                rest_.post_trade(req.body().accnt(), instr, settl_date, req.body().ref(),
                                 req.body().side(), req.body().lots(), req.body().ticks(),
                                 req.body().liq_ind(), req.body().cpty(), now, os);
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
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /accnt/trades/INSTR/SETTL_DATE/ID
            match_method_ = true;
            rest_.get_trade(get_trader(req), instr, settl_date, ids_[0], now, os);
            break;
        case HttpMethod::Delete:
            // DELETE /accnt/trades/INSTR/SETTL_DATE/ID,ID...
            match_method_ = true;
            rest_.delete_trade(get_trader(req), instr, settl_date, ids_, now);
            break;
        default:
            break;
        }
        return;
    }
}

void RestServ::posn_request(const HttpRequest& req, Time now, HttpStream& os)
{
    if (path_.empty()) {

        // /accnt/posns
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt/posns
            match_method_ = true;
            rest_.get_posn(get_trader(req), now, os);
        }
        return;
    }

    const auto instr = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /accnt/posns/INSTR
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt/posns/INSTR
            match_method_ = true;
            rest_.get_posn(get_trader(req), instr, now, os);
        }
        return;
    }

    const auto settl_date = IsoDate{stou64(path_.top())};
    path_.pop();

    if (path_.empty()) {

        // /accnt/posns/INSTR/SETTL_DATE
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /accnt/posns/INSTR/SETTL_DATE
            match_method_ = true;
            rest_.get_posn(get_trader(req), instr, settl_date, now, os);
        }
        return;
    }
}

} // namespace swirly
