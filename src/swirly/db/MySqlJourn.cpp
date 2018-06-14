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
#include "MySqlJourn.hpp"

#include "MySql.hxx"
#include "Sql.hxx"

#include <swirly/util/Config.hpp>
#include <swirly/util/Enum.hpp>
#include <swirly/util/Log.hpp>
#include <swirly/util/String.hpp>

namespace swirly {
inline namespace db {
using namespace mysql;
using namespace std;
namespace {
constexpr auto BeginSql = "START TRANSACTION"sv;
constexpr auto CommitSql = "COMMIT"sv;
constexpr auto RollbackSql = "ROLLBACK"sv;
} // namespace

MySqlJourn::MySqlJourn(const DbPtr& db)
: db_{db}
, insert_market_stmt_{prepare(*db, market::InsertSql)}
, update_market_stmt_{prepare(*db, market::UpdateSql)}
, insert_exec_stmt_{prepare(*db, exec::InsertSql)}
, update_exec_stmt_{prepare(*db, exec::UpdateSql)}
{
}

MySqlJourn::MySqlJourn(const Config& config)
: MySqlJourn{open_db(config.get("db_host", "localhost"),    //
                     config.get("db_user", "swirlyuser"),   //
                     config.get("db_pass", ""),             //
                     config.get("db_name", "swirlydb"),     //
                     config.get<unsigned>("db_port", 3306), //
                     config)}
{
}

MySqlJourn::~MySqlJourn() = default;

MySqlJourn::MySqlJourn(MySqlJourn&&) = default;
MySqlJourn& MySqlJourn::operator=(MySqlJourn&&) = default;

void MySqlJourn::do_begin()
{
    query(*db_, BeginSql);
}

void MySqlJourn::do_commit()
{
    query(*db_, CommitSql);
}

void MySqlJourn::do_rollback() noexcept
{
    try {
        query(*db_, RollbackSql);
    } catch (const std::exception& e) {
        SWIRLY_ERROR << "failed to rollback transaction: "sv << e.what();
    }
}

void MySqlJourn::do_write(const Msg& msg)
{
    dispatch(msg);
}

void MySqlJourn::on_create_market(const CreateMarket& body)
{
    auto& stmt = *insert_market_stmt_;

    BindArray<4> param;
    auto pit = param.begin();
    field::Id64 id{*pit++, body.id};
    field::Symbol::bind(*pit++, body.instr);
    field::JDay settl_day{*pit++, body.settl_day, MaybeNull};
    field::MarketState state{*pit++, static_cast<int32_t>(body.state)};
    bind_param(stmt, &param[0]);

    execute(stmt);
}

void MySqlJourn::on_update_market(const UpdateMarket& body)
{
    auto& stmt = *update_market_stmt_;

    BindArray<2> param;
    auto pit = param.begin();
    field::MarketState state{*pit++, static_cast<int32_t>(body.state)};
    field::Id64 id{*pit++, body.id};
    bind_param(stmt, &param[0]);

    execute(stmt);
}

void MySqlJourn::on_create_exec(const CreateExec& body)
{
    auto& stmt = *insert_exec_stmt_;

    BindArray<23> param;
    auto pit = param.begin();
    field::Symbol::bind(*pit++, body.accnt);
    field::Id64 market_id{*pit++, body.market_id};
    field::Symbol::bind(*pit++, body.instr);
    field::JDay settl_day{*pit++, body.settl_day, MaybeNull};
    field::Id64 id{*pit++, body.id};
    field::Id64 order_id{*pit++, body.order_id, MaybeNull};
    field::Symbol::bind(*pit++, body.ref, MaybeNull);
    field::State state{*pit++, unbox(body.state)};
    field::Side side{*pit++, unbox(body.side)};
    field::Lots lots{*pit++, body.lots};
    field::Ticks ticks{*pit++, body.ticks};
    field::Lots resd_lots{*pit++, body.resd_lots};
    field::Lots exec_lots{*pit++, body.exec_lots};
    field::Cost exec_cost{*pit++, body.exec_cost};
    field::Lots last_lots{*pit++, body.last_lots};
    field::Ticks last_ticks{*pit++, body.last_ticks};
    if (body.last_lots == 0_lts) {
        last_lots.set_null();
        last_ticks.set_null();
    }
    field::Lots min_lots{*pit++, body.min_lots};
    field::MatchId match_id{*pit++, body.match_id, MaybeNull};
    field::Lots posn_lots{*pit++, body.posn_lots};
    field::Cost posn_cost{*pit++, body.posn_cost};
    field::LiqInd liq_ind{*pit++, unbox(body.liq_ind), MaybeNull};
    field::Symbol::bind(*pit++, body.cpty, MaybeNull);
    field::Time created{*pit++, body.created};
    bind_param(stmt, &param[0]);

    execute(stmt);
    SWIRLY_DEBUG << "affected rows: "sv << affected_rows(stmt);
}

void MySqlJourn::on_archive_trade(const ArchiveTrade& body)
{
    Transaction trans{*this};
    auto& stmt = *update_exec_stmt_;

    BindArray<3> param;
    auto pit = param.begin();
    field::Time archive{*pit++, body.modified};
    field::Id64 market_id{*pit++, body.market_id};
    field::Id64 id{*pit++};
    bind_param(stmt, &param[0]);

    for (size_t i{0}; i < MaxIds; ++i) {
        const auto val = body.ids[i];
        if (val == 0_id64) {
            break;
        }

        id.set_value(val);
        execute(stmt);
    }
    trans.commit();
}

} // namespace db
} // namespace swirly
