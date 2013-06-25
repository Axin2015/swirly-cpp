/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#include <dbr/model.h>

#include <dbr/pool.h>

#include <fig/sqlite.h>

#include <stdlib.h> // malloc()

struct SqliteImpl {
    DbrIden id;
    struct FigSqlite sqlite;
    struct DbrIModel model_;
};

static inline struct SqliteImpl*
sqlite_impl(DbrModel model)
{
    return dbr_implof(struct SqliteImpl, model_, model);
}

static DbrIden
alloc_id(DbrModel model)
{
    struct SqliteImpl* impl = sqlite_impl(model);
    return impl->id++;
}

static DbrBool
begin(DbrModel model)
{
    struct SqliteImpl* impl = sqlite_impl(model);
    struct FigSqlite* sqlite = &impl->sqlite;
    return fig_sqlite_begin(sqlite);
}

static DbrBool
commit(DbrModel model)
{
    struct SqliteImpl* impl = sqlite_impl(model);
    struct FigSqlite* sqlite = &impl->sqlite;
    return fig_sqlite_commit(sqlite);
}

static DbrBool
rollback(DbrModel model)
{
    struct SqliteImpl* impl = sqlite_impl(model);
    struct FigSqlite* sqlite = &impl->sqlite;
    return fig_sqlite_rollback(sqlite);
}

static DbrBool
insert_order(DbrModel model, const struct DbrOrder* order)
{
    struct SqliteImpl* impl = sqlite_impl(model);
    struct FigSqlite* sqlite = &impl->sqlite;
    return fig_sqlite_insert_order(sqlite, order);
}

static DbrBool
update_order(DbrModel model, DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
             DbrLots lots, DbrMillis now)
{
    struct SqliteImpl* impl = sqlite_impl(model);
    struct FigSqlite* sqlite = &impl->sqlite;
    return fig_sqlite_update_order(sqlite, id, rev, status, resd, exec, lots, now);
}

static DbrBool
archive_order(DbrModel model, DbrIden id, DbrMillis now)
{
    struct SqliteImpl* impl = sqlite_impl(model);
    struct FigSqlite* sqlite = &impl->sqlite;
    return fig_sqlite_archive_order(sqlite, id, now);
}

static DbrBool
insert_trade(DbrModel model, const struct DbrTrade* trade)
{
    struct SqliteImpl* impl = sqlite_impl(model);
    struct FigSqlite* sqlite = &impl->sqlite;
    return fig_sqlite_insert_trade(sqlite, trade);
}

static DbrBool
archive_trade(DbrModel model, DbrIden id, DbrMillis now)
{
    struct SqliteImpl* impl = sqlite_impl(model);
    struct FigSqlite* sqlite = &impl->sqlite;
    return fig_sqlite_archive_trade(sqlite, id, now);
}

static ssize_t
select_entity(DbrModel model, int type, struct DbrSlNode** first)
{
    struct SqliteImpl* impl = sqlite_impl(model);
    struct FigSqlite* sqlite = &impl->sqlite;
    return fig_sqlite_select_entity(sqlite, type, first);
}

static struct DbrSlNode*
end_entity(DbrModel model)
{
    struct SqliteImpl* impl = sqlite_impl(model);
    struct FigSqlite* sqlite = &impl->sqlite;
    return fig_sqlite_end_entity(sqlite);
}

static const struct DbrModelVtbl SQLITE_MODEL_VTBL = {
    .alloc_id = alloc_id,
    .begin = begin,
    .commit = commit,
    .rollback = rollback,
    .insert_order = insert_order,
    .update_order = update_order,
    .archive_order = archive_order,
    .insert_trade = insert_trade,
    .archive_trade = archive_trade,
    .select_entity = select_entity,
    .end_entity = end_entity
};

DBR_API DbrModel
dbr_sqlite_create(DbrPool pool, DbrIden seed, const char* path)
{
    struct SqliteImpl* impl = malloc(sizeof(struct SqliteImpl));
    if (dbr_unlikely(!impl))
        goto fail1;

    // Seed identity.
    impl->id = seed;

    struct FigSqlite* sqlite = &impl->sqlite;
    if (!fig_sqlite_init(sqlite, pool, path))
        goto fail2;

    impl->model_.vtbl = &SQLITE_MODEL_VTBL;
    return &impl->model_;
 fail2:
    free(impl);
 fail1:
    return NULL;
}

DBR_API void
dbr_sqlite_destroy(DbrModel model)
{
    if (model) {
        struct SqliteImpl* impl = sqlite_impl(model);
        struct FigSqlite* sqlite = &impl->sqlite;
        fig_sqlite_term(sqlite);
        free(impl);
    }
}
