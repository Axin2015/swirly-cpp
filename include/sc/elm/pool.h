/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_POOL_H
#define SC_ASH_POOL_H

/**
 * @file
 * Memory pool.
 */

#include <sc/elm/types.h>

struct ScSlNode;

/**
 * @addtogroup Pool
 * @{
 */

SC_API ScPool
sc_pool_create(size_t capacity);

SC_API void
sc_pool_destroy(ScPool pool);

SC_API struct ScRec*
sc_pool_alloc_rec(ScPool pool);

SC_API void
sc_pool_free_rec(ScPool pool, struct ScRec* rec);

SC_API struct ScPerm*
sc_pool_alloc_perm(ScPool pool);

SC_API void
sc_pool_free_perm(ScPool pool, struct ScPerm* perm);

SC_API struct ScOrder*
sc_pool_alloc_order(ScPool pool);

SC_API void
sc_pool_free_order(ScPool pool, struct ScOrder* order);

SC_API struct ScLevel*
sc_pool_alloc_level(ScPool pool);

SC_API void
sc_pool_free_level(ScPool pool, struct ScLevel* level);

SC_API struct ScExec*
sc_pool_alloc_exec(ScPool pool);

SC_API void
sc_pool_free_exec(ScPool pool, struct ScExec* exec);

SC_API struct ScMatch*
sc_pool_alloc_match(ScPool pool);

SC_API void
sc_pool_free_match(ScPool pool, struct ScMatch* match);

SC_API struct ScPosn*
sc_pool_alloc_posn(ScPool pool);

SC_API void
sc_pool_free_posn(ScPool pool, struct ScPosn* posn);

SC_API struct ScView*
sc_pool_alloc_view(ScPool pool);

SC_API void
sc_pool_free_view(ScPool pool, struct ScView* view);

SC_API struct ScBook*
sc_pool_alloc_book(ScPool pool);

SC_API void
sc_pool_free_book(ScPool pool, struct ScBook* book);

SC_API struct ScSub*
sc_pool_alloc_sub(ScPool pool);

SC_API void
sc_pool_free_sub(ScPool pool, struct ScSub* sub);

SC_API struct ScSess*
sc_pool_alloc_sess(ScPool pool);

SC_API void
sc_pool_free_sess(ScPool pool, struct ScSess* sess);

SC_API void
sc_pool_free_entity_list(ScPool pool, int type, struct ScSlNode* first);

/** @} */

#endif // SC_ASH_POOL_H
