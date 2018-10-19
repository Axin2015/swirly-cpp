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
#include "SqliteJourn.hpp"

#include "Sql.hxx"
#include "Sqlite.hxx"

#include <swirly/fin/Exec.hpp>

#include <swirly/util/Config.hpp>
#include <swirly/util/Log.hpp>

namespace swirly {
inline namespace db {
using namespace sqlite;
using namespace std;
namespace {
constexpr auto BeginSql = "BEGIN TRANSACTION"sv;
constexpr auto CommitSql = "COMMIT TRANSACTION"sv;
constexpr auto RollbackSql = "ROLLBACK TRANSACTION"sv;
} // namespace

SqliteJourn::SqliteJourn(const DbPtr& db)
: db_{db}
, begin_stmt_{prepare(*db, BeginSql)}
, commit_stmt_{prepare(*db, CommitSql)}
, rollback_stmt_{prepare(*db, RollbackSql)}
, insert_market_stmt_{prepare(*db, market::InsertSql)}
, update_market_stmt_{prepare(*db, market::UpdateSql)}
, insert_exec_stmt_{prepare(*db, exec::InsertSql)}
, update_exec_stmt_{prepare(*db, exec::UpdateSql)}
{
}

SqliteJourn::SqliteJourn(const Config& config)
: SqliteJourn{open_db(config.get("db_name", "swirly.db"), SQLITE_OPEN_READWRITE, config)}
{
}

SqliteJourn::~SqliteJourn() = default;

SqliteJourn::SqliteJourn(SqliteJourn&&) = default;

SqliteJourn& SqliteJourn::operator=(SqliteJourn&&) = default;

void SqliteJourn::do_begin()
{
    step_once(*begin_stmt_);
}

void SqliteJourn::do_commit()
{
    step_once(*commit_stmt_);
}

void SqliteJourn::do_rollback() noexcept
{
    try {
        step_once(*rollback_stmt_);
    } catch (const std::exception& e) {
        SWIRLY_ERROR << "failed to rollback transaction: " << e.what();
    }
}

void SqliteJourn::do_write(const Msg& msg)
{
    dispatch(msg);
}

void SqliteJourn::on_create_market(WallTime now, const CreateMarket& body)
{
    auto& stmt = *insert_market_stmt_;

    ScopedBind bind{stmt};
    bind(body.id);
    bind(to_string_view(body.product));
    bind(body.settl_day, MaybeNull);
    bind(body.state);

    step_once(stmt);
}

void SqliteJourn::on_update_market(WallTime now, const UpdateMarket& body)
{
    auto& stmt = *update_market_stmt_;

    ScopedBind bind{stmt};
    bind(body.state);
    bind(body.id);

    step_once(stmt);
}

void SqliteJourn::on_create_exec(WallTime now, const CreateExec& body)
{
    auto& stmt = *insert_exec_stmt_;

    ScopedBind bind{stmt};
    bind(now);
    bind(to_string_view(body.accnt));
    bind(body.market_id);
    bind(to_string_view(body.product));
    bind(body.settl_day, MaybeNull);
    bind(body.id);
    bind(body.order_id, MaybeNull);
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

    step_once(stmt);
}

void SqliteJourn::on_archive_trade(WallTime now, const ArchiveTrade& body)
{
    Transaction trans{*this};
    auto& stmt = *update_exec_stmt_;

    for (size_t i{0}; i < MaxIds; ++i) {
        const auto id = body.ids[i];
        if (id == 0_id64) {
            break;
        }
        ScopedBind bind{stmt};
        bind(now);
        bind(body.market_id);
        bind(id);

        step_once(stmt);
    }
    trans.commit();
}

} // namespace db
} // namespace swirly
