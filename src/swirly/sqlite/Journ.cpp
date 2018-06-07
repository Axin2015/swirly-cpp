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
#include "Journ.hpp"

#include "Utility.hxx"

#include <swirly/fin/Exec.hpp>

#include <swirly/util/Config.hpp>

namespace swirly {
inline namespace sqlite {
using namespace std;
namespace {

constexpr auto BeginSql = "BEGIN TRANSACTION"sv;
constexpr auto CommitSql = "COMMIT TRANSACTION"sv;
constexpr auto RollbackSql = "ROLLBACK TRANSACTION"sv;

constexpr auto InsertMarketSql =                         //
    "INSERT INTO market_t (id, instr, settl_day, state)" //
    " VALUES (?, ?, ?, ?)"sv;

constexpr auto UpdateMarketSql =     //
    "UPDATE Market_t SET state = ?2" //
    " WHERE id = ?1"sv;

constexpr auto InsertExecSql =                                                     //
    "INSERT INTO exec_t (market_id, instr, settl_day, id, order_id, accnt, ref,"   //
    " state_id, side_id, lots, ticks, resd_lots, exec_lots, exec_cost, last_lots," //
    " last_ticks, min_lots, match_id, posn_lots, posn_cost, liq_ind_id, cpty,"     //
    " created)"                                                                    //
    " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"sv;

constexpr auto UpdateExecSql =       //
    "UPDATE exec_t SET archive = ?3" //
    " WHERE market_id = ?1 AND id = ?2"sv;

} // namespace

SqlJourn::SqlJourn(const Config& config)
: db_{open_db(config.get("sqlite_journ", "swirly.db"), SQLITE_OPEN_READWRITE, config)}
, begin_stmt_{prepare(*db_, BeginSql)}
, commit_stmt_{prepare(*db_, CommitSql)}
, rollback_stmt_{prepare(*db_, RollbackSql)}
, insert_market_stmt_{prepare(*db_, InsertMarketSql)}
, update_market_stmt_{prepare(*db_, UpdateMarketSql)}
, insert_exec_stmt_{prepare(*db_, InsertExecSql)}
, update_exec_stmt_{prepare(*db_, UpdateExecSql)}
{
}

SqlJourn::~SqlJourn() = default;

SqlJourn::SqlJourn(SqlJourn&&) = default;

SqlJourn& SqlJourn::operator=(SqlJourn&&) = default;

void SqlJourn::do_write(const Msg& msg)
{
    dispatch(msg);
}

void SqlJourn::begin()
{
    step_once(*begin_stmt_);
}

void SqlJourn::commit()
{
    step_once(*commit_stmt_);
}

void SqlJourn::rollback() noexcept
{
    try {
        step_once(*rollback_stmt_);
    } catch (const std::exception& e) {
        SWIRLY_ERROR << "failed to rollback transaction: "sv << e.what();
    }
}

void SqlJourn::on_create_market(const CreateMarket& body)
{
    auto& stmt = *insert_market_stmt_;

    ScopedBind bind{stmt};
    bind(body.id);
    bind(to_string_view(body.instr));
    bind(body.settl_day, MaybeNull);
    bind(body.state);

    step_once(stmt);
}

void SqlJourn::on_update_market(const UpdateMarket& body)
{
    auto& stmt = *update_market_stmt_;

    ScopedBind bind{stmt};
    bind(body.id);
    bind(body.state);

    step_once(stmt);
}

void SqlJourn::on_create_exec(const CreateExec& body)
{
    Transaction trans{*this};
    auto& stmt = *insert_exec_stmt_;

    ScopedBind bind{stmt};
    bind(body.market_id);
    bind(to_string_view(body.instr));
    bind(body.settl_day, MaybeNull);
    bind(body.id);
    bind(body.order_id, MaybeNull);
    bind(to_string_view(body.accnt));
    bind(to_string_view(body.ref), MaybeNull);
    bind(body.state);
    bind(body.side);
    bind(body.lots);
    bind(body.ticks);
    bind(body.resd_lots);
    bind(body.exec_lots);
    bind(body.exec_cost);
    if (body.last_lots > 0_lts) {
        bind(body.last_lots);
        bind(body.last_ticks);
    } else {
        bind(nullptr);
        bind(nullptr);
    }
    bind(body.min_lots);
    bind(body.match_id, MaybeNull);
    bind(body.posn_lots);
    bind(body.posn_cost);
    bind(body.liq_ind, MaybeNull);
    bind(to_string_view(body.cpty), MaybeNull);
    bind(body.created); // Created.

    step_once(stmt);
    trans.commit();
}

void SqlJourn::on_archive_trade(const ArchiveTrade& body)
{
    Transaction trans{*this};
    auto& stmt = *update_exec_stmt_;

    for (size_t i{0}; i < MaxIds; ++i) {
        const auto id = body.ids[i];
        if (id == 0_id64) {
            break;
        }
        ScopedBind bind{stmt};
        bind(body.market_id);
        bind(id);
        bind(body.modified);

        step_once(stmt);
    }
    trans.commit();
}

} // namespace sqlite
} // namespace swirly
