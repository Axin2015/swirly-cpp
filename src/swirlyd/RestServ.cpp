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
#include <swirly/web/RestApp.hpp>
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
            ids.push_back(static_cast<Id64>(from_string<int64_t>(toks.top())));
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
    return from_string<uint32_t>(req.perm());
}

Time get_time(const HttpRequest& req) noexcept
{
    const string_view val{req.time()};
    return val.empty() ? UnixClock::now() : to_time(from_string<Millis>(val));
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
        rest_request(now, req, os);
        if (!match_path_) {
            throw NotFoundException{err_msg() << "resource '" << req.path() << "' does not exist"};
        }
        if (!match_method_) {
            throw MethodNotAllowedException{err_msg()
                                            << "method '" << req.method() << "' is not allowed"};
        }
    } catch (const Exception& e) {
        const auto status = http_status(e.code());
        const char* const reason = http_reason(status);
        SWIRLY_ERROR << "exception: status=" << status << ", reason=" << reason
                     << ", detail=" << e.what();
        os.reset(status, reason);
        Exception::to_json(os, static_cast<int>(status), reason, e.what());
    } catch (const exception& e) {
        const auto status = HttpStatus::InternalServerError;
        const char* const reason = http_reason(status);
        SWIRLY_ERROR << "exception: status=" << status << ", reason=" << reason
                     << ", detail=" << e.what();
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

void RestServ::rest_request(Time now, const HttpRequest& req, HttpStream& os)
{
    if (path_.empty()) {
        return;
    }

    auto tok = path_.top();
    path_.pop();
    if (tok != "api"sv) {
        return;
    }
    tok = path_.top();
    path_.pop();

    if (tok == "refdata"sv) {
        // /api/refdata
        ref_data_request(now, req, os);
    } else if (tok == "sess"sv) {
        // /api/sess
        sess_request(now, req, os);
    } else {
        // Support both plural and singular forms.
        if (!tok.empty() && tok.back() == 's') {
            tok.remove_suffix(1);
        }
        if (tok == "market"sv) {
            // /api/market
            market_request(now, req, os);
        }
    }
}

void RestServ::ref_data_request(Time now, const HttpRequest& req, HttpStream& os)
{
    if (path_.empty()) {

        // /api/refdata
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /api/refdata
            match_method_ = true;
            const int bs{EntitySet::Asset | EntitySet::Instr};
            app_.get_ref_data(now, bs, os);
        }
        return;
    }

    const auto tok = path_.top();
    path_.pop();

    const auto es = EntitySet::parse(tok);
    if (es.many()) {

        if (path_.empty()) {

            // /api/refdata/entity,entity...
            match_path_ = true;

            if (req.method() == HttpMethod::Get) {
                // GET /api/refdata/entity,entity...
                match_method_ = true;
                app_.get_ref_data(now, es, os);
            }
        }
        return;
    }

    switch (es.get()) {
    case EntitySet::Asset:
        asset_request(now, req, os);
        break;
    case EntitySet::Instr:
        instr_request(now, req, os);
        break;
    }
}

void RestServ::asset_request(Time now, const HttpRequest& req, HttpStream& os)
{
    if (path_.empty()) {

        // /api/refdata/asset
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /api/refdata/asset
            match_method_ = true;
            app_.get_asset(now, os);
        }
        return;
    }

    const auto symbol = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /api/refdata/asset/SYMBOL
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /api/refdata/asset/SYMBOL
            match_method_ = true;
            app_.get_asset(now, symbol, os);
        }
        return;
    }
}

void RestServ::instr_request(Time now, const HttpRequest& req, HttpStream& os)
{
    if (path_.empty()) {

        // /api/refdata/instr
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /api/refdata/instr
            match_method_ = true;
            app_.get_instr(now, os);
        }
        return;
    }

    const auto symbol = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /api/refdata/instr/SYMBOL
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /api/refdata/instr/SYMBOL
            match_method_ = true;
            app_.get_instr(now, symbol, os);
        }
        return;
    }
}

void RestServ::sess_request(Time now, const HttpRequest& req, HttpStream& os)
{
    if (path_.empty()) {

        // /api/sess
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /api/sess
            match_method_ = true;
            const auto es = EntitySet::Market | EntitySet::Order | EntitySet::Exec
                | EntitySet::Trade | EntitySet::Posn;
            app_.get_sess(now, get_trader(req), es, parse_query(req.query()), os);
        }
        return;
    }

    const auto tok = path_.top();
    path_.pop();

    const auto es = EntitySet::parse(tok);
    if (es.many()) {

        if (path_.empty()) {

            // /api/sess/entity,entity...
            match_path_ = true;

            if (req.method() == HttpMethod::Get) {
                // GET /api/sess/entity,entity...
                match_method_ = true;
                app_.get_sess(now, get_trader(req), es, parse_query(req.query()), os);
            }
        }
        return;
    }

    switch (es.get()) {
    case EntitySet::Market:
        market_request(now, req, os);
        break;
    case EntitySet::Order:
        order_request(now, req, os);
        break;
    case EntitySet::Exec:
        exec_request(now, req, os);
        break;
    case EntitySet::Trade:
        trade_request(now, req, os);
        break;
    case EntitySet::Posn:
        posn_request(now, req, os);
        break;
    }
}

void RestServ::market_request(Time now, const HttpRequest& req, HttpStream& os)
{
    if (path_.empty()) {

        // /api/market
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /api/market
            match_method_ = true;
            app_.get_market(now, os);
            break;
        case HttpMethod::Post:
            // POST /api/market
            match_method_ = true;
            get_admin(req);
            {
                constexpr auto ReqFields = RestBody::Instr | RestBody::SettlDate;
                constexpr auto OptFields = RestBody::State;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                app_.post_market(now, req.body().instr(), req.body().settl_date(),
                                 req.body().state(), os);
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

        // /api/market/INSTR
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /api/market/INSTR
            match_method_ = true;
            app_.get_market(now, instr, os);
            break;
        case HttpMethod::Post:
            // POST /api/market/INSTR
            match_method_ = true;
            get_admin(req);
            {
                constexpr auto ReqFields = RestBody::SettlDate;
                constexpr auto OptFields = RestBody::State;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                app_.post_market(now, instr, req.body().settl_date(), req.body().state(), os);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto settl_date = from_string<IsoDate>(path_.top());
    path_.pop();

    if (path_.empty()) {

        // /api/market/INSTR/SETTL_DATE
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /api/market/INSTR/SETTL_DATE
            match_method_ = true;
            app_.get_market(now, instr, settl_date, os);
            break;
        case HttpMethod::Post:
            // POST /api/market/INSTR/SETTL_DATE
            match_method_ = true;
            get_admin(req);
            {
                constexpr auto ReqFields = 0;
                constexpr auto OptFields = RestBody::State;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                app_.post_market(now, instr, settl_date, req.body().state(), os);
            }
            break;
        case HttpMethod::Put:
            // PUT /api/market/INSTR/SETTL_DATE
            match_method_ = true;
            get_admin(req);
            {
                constexpr auto ReqFields = RestBody::State;
                if (!req.body().valid(ReqFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                app_.put_market(now, instr, settl_date, req.body().state(), os);
            }
            break;
        default:
            break;
        }
        return;
    }
}

void RestServ::order_request(Time now, const HttpRequest& req, HttpStream& os)
{
    if (path_.empty()) {

        // /api/sess/orders
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /api/sess/orders
            match_method_ = true;
            app_.get_order(now, get_trader(req), os);
            break;
        case HttpMethod::Post:
            // POST /api/sess/orders
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
                app_.post_order(now, accnt, req.body().instr(), req.body().settl_date(),
                                req.body().ref(), req.body().side(), req.body().lots(),
                                req.body().ticks(), req.body().min_lots(), os);
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

        // /api/sess/order/INSTR
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /api/sess/order/INSTR
            match_method_ = true;
            app_.get_order(now, get_trader(req), instr, os);
            break;
        case HttpMethod::Post:
            // POST /api/sess/order/INSTR
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
                app_.post_order(now, accnt, instr, req.body().settl_date(), req.body().ref(),
                                req.body().side(), req.body().lots(), req.body().ticks(),
                                req.body().min_lots(), os);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto settl_date = from_string<IsoDate>(path_.top());
    path_.pop();

    if (path_.empty()) {

        // /api/sess/order/INSTR/SETTL_DATE
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /api/sess/order/INSTR/SETTL_DATE
            match_method_ = true;
            app_.get_order(now, get_trader(req), instr, settl_date, os);
            break;
        case HttpMethod::Post:
            // POST /api/sess/order/INSTR/SETTL_DATE
            match_method_ = true;
            {
                // Validate account before request.
                const auto accnt = get_trader(req);
                constexpr auto ReqFields = RestBody::Side | RestBody::Lots | RestBody::Ticks;
                constexpr auto OptFields = RestBody::Ref | RestBody::MinLots;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                app_.post_order(now, accnt, instr, settl_date, req.body().ref(), req.body().side(),
                                req.body().lots(), req.body().ticks(), req.body().min_lots(), os);
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

        // /api/sess/order/INSTR/SETTL_DATE/ID,ID...
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /api/sess/order/INSTR/SETTL_DATE/ID
            match_method_ = true;
            app_.get_order(now, get_trader(req), instr, settl_date, ids_[0], os);
            break;
        case HttpMethod::Put:
            // PUT /api/sess/order/INSTR/SETTL_DATE/ID,ID...
            match_method_ = true;
            {
                // Validate account before request.
                const auto accnt = get_trader(req);
                constexpr auto ReqFields = RestBody::Lots;
                if (req.body().fields() != ReqFields) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                app_.put_order(now, accnt, instr, settl_date, ids_, req.body().lots(), os);
            }
            break;
        default:
            break;
        }
        return;
    }
}

void RestServ::exec_request(Time now, const HttpRequest& req, HttpStream& os)
{
    if (path_.empty()) {

        // /api/sess/exec
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /api/sess/exec
            match_method_ = true;
            app_.get_exec(now, get_trader(req), parse_query(req.query()), os);
        }
        return;
    }
}

void RestServ::trade_request(Time now, const HttpRequest& req, HttpStream& os)
{
    if (path_.empty()) {

        // /api/sess/trade
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /api/sess/trade
            match_method_ = true;
            app_.get_trade(now, get_trader(req), os);
            break;
        case HttpMethod::Post:
            // POST /api/sess/trade
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
                app_.post_trade(now, req.body().accnt(), req.body().instr(),
                                req.body().settl_date(), req.body().ref(), req.body().side(),
                                req.body().lots(), req.body().ticks(), req.body().liq_ind(),
                                req.body().cpty(), os);
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

        // /api/sess/trade/INSTR
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /api/sess/trade/INSTR
            match_method_ = true;
            app_.get_trade(now, get_trader(req), instr, os);
            break;
        case HttpMethod::Post:
            // POST /api/sess/trade/INSTR
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
                app_.post_trade(now, req.body().accnt(), instr, req.body().settl_date(),
                                req.body().ref(), req.body().side(), req.body().lots(),
                                req.body().ticks(), req.body().liq_ind(), req.body().cpty(), os);
            }
            break;
        default:
            break;
        }
        return;
    }

    const auto settl_date = from_string<IsoDate>(path_.top());
    path_.pop();

    if (path_.empty()) {

        // /api/sess/trade/INSTR/SETTL_DATE
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /api/sess/trade/INSTR/SETTL_DATE
            match_method_ = true;
            app_.get_trade(now, get_trader(req), instr, settl_date, os);
            break;
        case HttpMethod::Post:
            // POST /api/sess/trade/INSTR/SETTL_DATE
            match_method_ = true;
            get_admin(req);
            {
                constexpr auto ReqFields = RestBody::Accnt | RestBody::Side | RestBody::Lots;
                constexpr auto OptFields
                    = RestBody::Ref | RestBody::Ticks | RestBody::LiqInd | RestBody::Cpty;
                if (!req.body().valid(ReqFields, OptFields)) {
                    throw InvalidException{"request fields are invalid"sv};
                }
                app_.post_trade(now, req.body().accnt(), instr, settl_date, req.body().ref(),
                                req.body().side(), req.body().lots(), req.body().ticks(),
                                req.body().liq_ind(), req.body().cpty(), os);
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

        // /api/sess/trade/INSTR/SETTL_DATE/ID,ID...
        match_path_ = true;

        switch (req.method()) {
        case HttpMethod::Get:
            // GET /api/sess/trade/INSTR/SETTL_DATE/ID
            match_method_ = true;
            app_.get_trade(now, get_trader(req), instr, settl_date, ids_[0], os);
            break;
        case HttpMethod::Delete:
            // DELETE /api/sess/trade/INSTR/SETTL_DATE/ID,ID...
            match_method_ = true;
            app_.delete_trade(now, get_trader(req), instr, settl_date, ids_);
            break;
        default:
            break;
        }
        return;
    }
}

void RestServ::posn_request(Time now, const HttpRequest& req, HttpStream& os)
{
    if (path_.empty()) {

        // /api/sess/posn
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /api/sess/posn
            match_method_ = true;
            app_.get_posn(now, get_trader(req), os);
        }
        return;
    }

    const auto instr = path_.top();
    path_.pop();

    if (path_.empty()) {

        // /api/sess/posn/INSTR
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /api/sess/posn/INSTR
            match_method_ = true;
            app_.get_posn(now, get_trader(req), instr, os);
        }
        return;
    }

    const auto settl_date = from_string<IsoDate>(path_.top());
    path_.pop();

    if (path_.empty()) {

        // /api/sess/posn/INSTR/SETTL_DATE
        match_path_ = true;

        if (req.method() == HttpMethod::Get) {
            // GET /api/sess/posn/INSTR/SETTL_DATE
            match_method_ = true;
            app_.get_posn(now, get_trader(req), instr, settl_date, os);
        }
        return;
    }
}

} // namespace swirly
