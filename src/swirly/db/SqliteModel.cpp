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
#include "SqliteModel.hpp"

#include "Sql.hxx"
#include "Sqlite.hxx"

#include <swirly/fin/Asset.hpp>
#include <swirly/fin/Exec.hpp>
#include <swirly/fin/Instr.hpp>
#include <swirly/fin/Market.hpp>

#include <swirly/fin/Order.hpp>
#include <swirly/fin/Posn.hpp>

#include <swirly/util/Config.hpp>

namespace swirly {
inline namespace db {
using namespace sqlite;
using namespace std;

SqliteModel::SqliteModel(const DbPtr& db)
: db_{db}
{
}

SqliteModel::SqliteModel(const Config& config)
: SqliteModel{open_db(config.get("db_name", "swirly.db"), SQLITE_OPEN_READONLY, config)}
{
}

SqliteModel::~SqliteModel() = default;

SqliteModel::SqliteModel(SqliteModel&&) = default;

SqliteModel& SqliteModel::operator=(SqliteModel&&) = default;

void SqliteModel::do_read_asset(const ModelCallback<AssetPtr>& cb) const
{
    using namespace asset;
    StmtPtr stmt{prepare(*db_, SelectSql)};
    while (step(*stmt)) {
        cb(Asset::make(column<Id32>(*stmt, Id),             //
                       column<string_view>(*stmt, Symbol),  //
                       column<string_view>(*stmt, Display), //
                       column<AssetType>(*stmt, TypeId)));
    }
}

void SqliteModel::do_read_instr(const ModelCallback<InstrPtr>& cb) const
{
    using namespace instr;
    StmtPtr stmt{prepare(*db_, SelectSql)};
    while (step(*stmt)) {
        cb(Instr::make(column<Id32>(*stmt, Id),               //
                       column<string_view>(*stmt, Symbol),    //
                       column<string_view>(*stmt, Display),   //
                       column<string_view>(*stmt, BaseAsset), //
                       column<string_view>(*stmt, TermCcy),   //
                       column<int>(*stmt, LotNumer),          //
                       column<int>(*stmt, LotDenom),          //
                       column<int>(*stmt, TickNumer),         //
                       column<int>(*stmt, TickDenom),         //
                       column<int>(*stmt, PipDp),             //
                       column<Lots>(*stmt, MinLots),          //
                       column<Lots>(*stmt, MaxLots)));
    }
}

void SqliteModel::do_read_market(const ModelCallback<MarketPtr>& cb) const
{
    using namespace market;
    StmtPtr stmt{prepare(*db_, SelectSql)};
    while (step(*stmt)) {
        cb(Market::make(column<Id64>(*stmt, Id),           //
                        column<string_view>(*stmt, Instr), //
                        column<JDay>(*stmt, SettlDay),     //
                        column<MarketState>(*stmt, State), //
                        column<Lots>(*stmt, LastLots),     //
                        column<Ticks>(*stmt, LastTicks),   //
                        column<Time>(*stmt, LastTime),     //
                        column<Id64>(*stmt, MaxId)));
    }
}

void SqliteModel::do_read_order(const ModelCallback<OrderPtr>& cb) const
{
    using namespace order;
    StmtPtr stmt{prepare(*db_, SelectSql)};
    while (step(*stmt)) {
        cb(Order::make(column<string_view>(*stmt, Accnt),       //
                       column<Id64>(*stmt, MarketId),           //
                       column<string_view>(*stmt, Instr),       //
                       column<JDay>(*stmt, SettlDay),           //
                       column<Id64>(*stmt, Id),                 //
                       column<string_view>(*stmt, Ref),         //
                       column<swirly::State>(*stmt, State),     //
                       column<swirly::Side>(*stmt, Side),       //
                       column<swirly::Lots>(*stmt, Lots),       //
                       column<swirly::Ticks>(*stmt, Ticks),     //
                       column<swirly::Lots>(*stmt, ResdLots),   //
                       column<swirly::Lots>(*stmt, ExecLots),   //
                       column<swirly::Cost>(*stmt, ExecCost),   //
                       column<swirly::Lots>(*stmt, LastLots),   //
                       column<swirly::Ticks>(*stmt, LastTicks), //
                       column<swirly::Lots>(*stmt, MinLots),    //
                       column<Time>(*stmt, Created),            //
                       column<Time>(*stmt, Modified)));
    }
}

void SqliteModel::do_read_exec(Time since, const ModelCallback<ExecPtr>& cb) const
{
    using namespace exec;
    StmtPtr stmt{prepare(*db_, SelectSql)};
    ScopedBind bind{*stmt};
    bind(since);
    while (step(*stmt)) {
        cb(Exec::make(column<string_view>(*stmt, Accnt),       //
                      column<Id64>(*stmt, MarketId),           //
                      column<string_view>(*stmt, Instr),       //
                      column<JDay>(*stmt, SettlDay),           //
                      column<Id64>(*stmt, Id),                 //
                      column<Id64>(*stmt, OrderId),            //
                      column<string_view>(*stmt, Ref),         //
                      column<swirly::State>(*stmt, State),     //
                      column<swirly::Side>(*stmt, Side),       //
                      column<swirly::Lots>(*stmt, Lots),       //
                      column<swirly::Ticks>(*stmt, Ticks),     //
                      column<swirly::Lots>(*stmt, ResdLots),   //
                      column<swirly::Lots>(*stmt, ExecLots),   //
                      column<swirly::Cost>(*stmt, ExecCost),   //
                      column<swirly::Lots>(*stmt, LastLots),   //
                      column<swirly::Ticks>(*stmt, LastTicks), //
                      column<swirly::Lots>(*stmt, MinLots),    //
                      column<Id64>(*stmt, MatchId),            //
                      column<swirly::Lots>(*stmt, PosnLots),   //
                      column<swirly::Cost>(*stmt, PosnCost),   //
                      column<swirly::LiqInd>(*stmt, LiqInd),   //
                      column<string_view>(*stmt, Cpty),        //
                      column<Time>(*stmt, Created)));
    }
}

void SqliteModel::do_read_trade(const ModelCallback<ExecPtr>& cb) const
{
    using namespace trade;
    StmtPtr stmt{prepare(*db_, SelectSql)};
    while (step(*stmt)) {
        cb(Exec::make(column<string_view>(*stmt, Accnt),       //
                      column<Id64>(*stmt, MarketId),           //
                      column<string_view>(*stmt, Instr),       //
                      column<JDay>(*stmt, SettlDay),           //
                      column<Id64>(*stmt, Id),                 //
                      column<Id64>(*stmt, OrderId),            //
                      column<string_view>(*stmt, Ref),         //
                      State::Trade,                            //
                      column<swirly::Side>(*stmt, Side),       //
                      column<swirly::Lots>(*stmt, Lots),       //
                      column<swirly::Ticks>(*stmt, Ticks),     //
                      column<swirly::Lots>(*stmt, ResdLots),   //
                      column<swirly::Lots>(*stmt, ExecLots),   //
                      column<swirly::Cost>(*stmt, ExecCost),   //
                      column<swirly::Lots>(*stmt, LastLots),   //
                      column<swirly::Ticks>(*stmt, LastTicks), //
                      column<swirly::Lots>(*stmt, MinLots),    //
                      column<Id64>(*stmt, MatchId),            //
                      column<swirly::Lots>(*stmt, PosnLots),   //
                      column<swirly::Cost>(*stmt, PosnCost),   //
                      column<swirly::LiqInd>(*stmt, LiqInd),   //
                      column<string_view>(*stmt, Cpty),        //
                      column<Time>(*stmt, Created)));
    }
}

void SqliteModel::do_read_posn(JDay bus_day, const ModelCallback<PosnPtr>& cb) const
{
    using namespace posn;

    PosnSet ps;
    PosnSet::Iterator it;

    StmtPtr stmt{prepare(*db_, SelectSql)};
    while (step(*stmt)) {
        const auto accnt = column<string_view>(*stmt, Accnt);
        auto market_id = column<Id64>(*stmt, MarketId);
        const auto instr = column<string_view>(*stmt, Instr);
        auto settl_day = column<JDay>(*stmt, SettlDay);

        // FIXME: review when end of day is implemented.
        if (settl_day != 0_jd && settl_day <= bus_day) {
            market_id &= Id64{~0xffff};
            settl_day = 0_jd;
        }

        bool found;
        tie(it, found) = ps.find_hint(accnt, market_id);
        if (!found) {
            it = ps.insert_hint(it, Posn::make(accnt, market_id, instr, settl_day));
        }

        const auto side = column<swirly::Side>(*stmt, Side);
        const auto lots = column<swirly::Lots>(*stmt, Lots);
        const auto cost = column<swirly::Cost>(*stmt, Cost);
        if (side == swirly::Side::Buy) {
            it->add_buy(lots, cost);
        } else {
            it->add_sell(lots, cost);
        }
    }

    for (it = ps.begin(); it != ps.end();) {
        cb(ps.remove(it++));
    }
}

} // namespace db
} // namespace swirly
