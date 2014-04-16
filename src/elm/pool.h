/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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
#ifndef ELM_POOL_H
#define ELM_POOL_H

#include <dbr/pool.h>

#include <dbr/log.h>
#include <dbr/slnode.h>
#include <dbr/types.h>

struct ElmSmallBlock;
struct ElmLargeBlock;

struct ElmPool {
    void* addr;
    size_t used;
    size_t capacity;
    size_t pagesize;
    // Free lists.
    struct ElmSmallEntry* first_small;
    struct ElmLargeEntry* first_large;
#if DBR_DEBUG_ALLOC
    struct DbrTree allocs_small;
    struct DbrTree allocs_large;
#endif // DBR_DEBUG_ALLOC
};

struct ElmSmallEntry {
    union {
        struct ElmSmallEntry* next;
        // Small data structures.
        struct DbrRbNode rbnode;
        struct DbrMemb memb;
        struct DbrLevel level;
        struct DbrMatch match;
        struct DbrSub sub;
        struct DbrSess sess;
    };
#if DBR_DEBUG_ALLOC
    const char* file;
    int line;
    struct DbrRbNode pool_node_;
#endif // DBR_DEBUG_ALLOC
};

struct ElmLargeEntry {
    union {
        struct ElmLargeEntry* next;
        // Large data structures.
        struct DbrRec rec;
        struct DbrOrder order;
        struct DbrExec exec;
        struct DbrPosn posn;
        struct DbrView view;
        struct DbrBook book;
    };
#if DBR_DEBUG_ALLOC
    const char* file;
    int line;
    struct DbrRbNode pool_node_;
#endif // DBR_DEBUG_ALLOC
};

DBR_EXTERN DbrBool
elm_pool_init(struct ElmPool* pool, size_t capacity);

// Assumes that pointer is not null.

DBR_EXTERN void
elm_pool_term(struct ElmPool* pool);

DBR_EXTERN void
elm_pool_free_small(struct ElmPool* pool, struct ElmSmallEntry* entry);

DBR_EXTERN void
elm_pool_free_large(struct ElmPool* pool, struct ElmLargeEntry* entry);

#if !DBR_DEBUG_ALLOC

DBR_EXTERN struct ElmSmallEntry*
elm_pool_alloc_small(struct ElmPool* pool);

DBR_EXTERN struct ElmLargeEntry*
elm_pool_alloc_large(struct ElmPool* pool);

static inline struct DbrRbNode*
elm_pool_alloc_rbnode(struct ElmPool* pool)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool);
    return entry ? &entry->rbnode : NULL;
}

static inline void
elm_pool_free_rbnode(struct ElmPool* pool, struct DbrRbNode* rbnode)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)rbnode;
    elm_pool_free_small(pool, entry);
}

static inline struct DbrRec*
elm_pool_alloc_rec(struct ElmPool* pool)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool);
    return entry ? &entry->rec : NULL;
}

static inline void
elm_pool_free_rec(struct ElmPool* pool, struct DbrRec* rec)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)rec;
    elm_pool_free_large(pool, entry);
}

static inline struct DbrMemb*
elm_pool_alloc_memb(struct ElmPool* pool)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool);
    return entry ? &entry->memb : NULL;
}

static inline void
elm_pool_free_memb(struct ElmPool* pool, struct DbrMemb* memb)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)memb;
    elm_pool_free_small(pool, entry);
}

static inline struct DbrOrder*
elm_pool_alloc_order(struct ElmPool* pool)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool);
    return entry ? &entry->order : NULL;
}

static inline void
elm_pool_free_order(struct ElmPool* pool, struct DbrOrder* order)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)order;
    elm_pool_free_large(pool, entry);
}

static inline struct DbrLevel*
elm_pool_alloc_level(struct ElmPool* pool)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool);
    return entry ? &entry->level : NULL;
}

static inline void
elm_pool_free_level(struct ElmPool* pool, struct DbrLevel* level)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)level;
    elm_pool_free_small(pool, entry);
}

static inline struct DbrExec*
elm_pool_alloc_exec(struct ElmPool* pool)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool);
    return entry ? &entry->exec : NULL;
}

static inline void
elm_pool_free_exec(struct ElmPool* pool, struct DbrExec* exec)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)exec;
    elm_pool_free_large(pool, entry);
}

static inline struct DbrMatch*
elm_pool_alloc_match(struct ElmPool* pool)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool);
    return entry ? &entry->match : NULL;
}

static inline void
elm_pool_free_match(struct ElmPool* pool, struct DbrMatch* match)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)match;
    elm_pool_free_small(pool, entry);
}

static inline struct DbrPosn*
elm_pool_alloc_posn(struct ElmPool* pool)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool);
    return entry ? &entry->posn : NULL;
}

static inline void
elm_pool_free_posn(struct ElmPool* pool, struct DbrPosn* posn)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)posn;
    elm_pool_free_large(pool, entry);
}

static inline struct DbrView*
elm_pool_alloc_view(struct ElmPool* pool)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool);
    return entry ? &entry->view : NULL;
}

static inline void
elm_pool_free_view(struct ElmPool* pool, struct DbrView* view)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)view;
    elm_pool_free_large(pool, entry);
}

static inline struct DbrBook*
elm_pool_alloc_book(struct ElmPool* pool)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool);
    return entry ? &entry->book : NULL;
}

static inline void
elm_pool_free_book(struct ElmPool* pool, struct DbrBook* book)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)book;
    elm_pool_free_large(pool, entry);
}

static inline struct DbrSub*
elm_pool_alloc_sub(struct ElmPool* pool)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool);
    return entry ? &entry->sub : NULL;
}

static inline void
elm_pool_free_sub(struct ElmPool* pool, struct DbrSub* sub)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)sub;
    elm_pool_free_small(pool, entry);
}

static inline struct DbrSess*
elm_pool_alloc_sess(struct ElmPool* pool)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool);
    return entry ? &entry->sess : NULL;
}

static inline void
elm_pool_free_sess(struct ElmPool* pool, struct DbrSess* sess)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)sess;
    elm_pool_free_small(pool, entry);
}

#else  // DBR_DEBUG_ALLOC

DBR_EXTERN struct ElmSmallEntry*
elm_pool_alloc_small(struct ElmPool* pool, const char* file, int line);

DBR_EXTERN struct ElmLargeEntry*
elm_pool_alloc_large(struct ElmPool* pool, const char* file, int line);

static inline struct DbrRbNode*
elm_pool_alloc_rbnode_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool, file, line);
    dbr_log_debug3("allocating rbnode %p in %s at %d", entry, file, line);
    return entry ? &entry->rbnode : NULL;
}

static inline void
elm_pool_free_rbnode(struct ElmPool* pool, struct DbrRbNode* rbnode)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)rbnode;
    dbr_log_debug3("freeing rbnode %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_small(pool, entry);
}

static inline struct DbrRec*
elm_pool_alloc_rec_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool, file, line);
    dbr_log_debug3("allocating %p rec in %s at %d", entry, file, line);
    return entry ? &entry->rec : NULL;
}

static inline void
elm_pool_free_rec(struct ElmPool* pool, struct DbrRec* rec)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)rec;
    dbr_log_debug3("freeing rec %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_large(pool, entry);
}

static inline struct DbrMemb*
elm_pool_alloc_memb_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool, file, line);
    dbr_log_debug3("allocating memb %p in %s at %d", entry, file, line);
    return entry ? &entry->memb : NULL;
}

static inline void
elm_pool_free_memb(struct ElmPool* pool, struct DbrMemb* memb)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)memb;
    dbr_log_debug3("freeing memb %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_small(pool, entry);
}

static inline struct DbrOrder*
elm_pool_alloc_order_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool, file, line);
    dbr_log_debug3("allocating order %p in %s at %d", entry, file, line);
    return entry ? &entry->order : NULL;
}

static inline void
elm_pool_free_order(struct ElmPool* pool, struct DbrOrder* order)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)order;
    dbr_log_debug3("freeing order %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_large(pool, entry);
}

static inline struct DbrLevel*
elm_pool_alloc_level_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool, file, line);
    dbr_log_debug3("allocating level %p in %s at %d", entry, file, line);
    return entry ? &entry->level : NULL;
}

static inline void
elm_pool_free_level(struct ElmPool* pool, struct DbrLevel* level)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)level;
    dbr_log_debug3("freeing level %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_small(pool, entry);
}

static inline struct DbrExec*
elm_pool_alloc_exec_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool, file, line);
    dbr_log_debug3("allocating exec %p in %s at %d", entry, file, line);
    return entry ? &entry->exec : NULL;
}

static inline void
elm_pool_free_exec(struct ElmPool* pool, struct DbrExec* exec)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)exec;
    dbr_log_debug3("freeing exec %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_large(pool, entry);
}

static inline struct DbrMatch*
elm_pool_alloc_match_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool, file, line);
    dbr_log_debug3("allocating match %p in %s at %d", entry, file, line);
    return entry ? &entry->match : NULL;
}

static inline void
elm_pool_free_match(struct ElmPool* pool, struct DbrMatch* match)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)match;
    dbr_log_debug3("freeing match %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_small(pool, entry);
}

static inline struct DbrPosn*
elm_pool_alloc_posn_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool, file, line);
    dbr_log_debug3("allocating posn %p in %s at %d", entry, file, line);
    return entry ? &entry->posn : NULL;
}

static inline void
elm_pool_free_posn(struct ElmPool* pool, struct DbrPosn* posn)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)posn;
    dbr_log_debug3("freeing posn %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_large(pool, entry);
}

static inline struct DbrView*
elm_pool_alloc_view_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool, file, line);
    dbr_log_debug3("allocating view %p in %s at %d", entry, file, line);
    return entry ? &entry->view : NULL;
}

static inline void
elm_pool_free_view(struct ElmPool* pool, struct DbrView* view)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)view;
    dbr_log_debug3("freeing view %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_large(pool, entry);
}

static inline struct DbrBook*
elm_pool_alloc_book_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool, file, line);
    dbr_log_debug3("allocating book %p in %s at %d", entry, file, line);
    return entry ? &entry->book : NULL;
}

static inline void
elm_pool_free_book(struct ElmPool* pool, struct DbrBook* book)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)book;
    dbr_log_debug3("freeing book %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_large(pool, entry);
}

static inline struct DbrSub*
elm_pool_alloc_sub_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool, file, line);
    dbr_log_debug3("allocating sub %p in %s at %d", entry, file, line);
    return entry ? &entry->sub : NULL;
}

static inline void
elm_pool_free_sub(struct ElmPool* pool, struct DbrSub* sub)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)sub;
    dbr_log_debug3("freeing sub %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_small(pool, entry);
}

static inline struct DbrSess*
elm_pool_alloc_sess_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool, file, line);
    dbr_log_debug3("allocating sess %p in %s at %d", entry, file, line);
    return entry ? &entry->sess : NULL;
}

static inline void
elm_pool_free_sess(struct ElmPool* pool, struct DbrSess* sess)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)sess;
    dbr_log_debug3("freeing sess %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_small(pool, entry);
}

#define elm_pool_alloc_rec(pool)                    \
    elm_pool_alloc_rec_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_memb(pool)                   \
    elm_pool_alloc_memb_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_order(pool)                  \
    elm_pool_alloc_order_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_level(pool)                  \
    elm_pool_alloc_level_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_exec(pool)                   \
    elm_pool_alloc_exec_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_match(pool)                  \
    elm_pool_alloc_match_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_posn(pool)                   \
    elm_pool_alloc_posn_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_view(pool)                   \
    elm_pool_alloc_view_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_book(pool)                   \
    elm_pool_alloc_book_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_sub(pool)                   \
    elm_pool_alloc_sub_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_sess(pool)                   \
    elm_pool_alloc_sess_(pool, __FILE__, __LINE__)

#endif // DBR_DEBUG_ALLOC

DBR_EXTERN void
elm_pool_free_entity_list(struct ElmPool* pool, int type, struct DbrSlNode* first);

#endif // ELM_POOL_H
