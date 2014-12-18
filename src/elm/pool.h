/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef ELM_POOL_H
#define ELM_POOL_H

#include <sc/elm/pool.h>
#include <sc/elm/types.h>

#include <sc/ash/log.h>
#include <sc/ash/slnode.h>

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
#if SC_DEBUG_ALLOC
    struct ScTree allocs_small;
    struct ScTree allocs_large;
#endif // SC_DEBUG_ALLOC
};

struct ElmSmallEntry {
    union {
        struct ElmSmallEntry* next;
        // Small data structures.
        struct ScRbNode rbnode;
        struct ScPerm perm;
        struct ScLevel level;
        struct ScMatch match;
        struct ScSub sub;
        struct ScSess sess;
    };
#if SC_DEBUG_ALLOC
    const char* file;
    int line;
    struct ScRbNode pool_node_;
#endif // SC_DEBUG_ALLOC
};

struct ElmLargeEntry {
    union {
        struct ElmLargeEntry* next;
        // Large data structures.
        struct ScRec rec;
        struct ScOrder order;
        struct ScExec exec;
        struct ScPosn posn;
        struct ScView view;
        struct ScBook book;
    };
#if SC_DEBUG_ALLOC
    const char* file;
    int line;
    struct ScRbNode pool_node_;
#endif // SC_DEBUG_ALLOC
};

SC_EXTERN ScBool
elm_pool_init(struct ElmPool* pool, size_t capacity);

// Assumes that pointer is not null.

SC_EXTERN void
elm_pool_term(struct ElmPool* pool);

SC_EXTERN void
elm_pool_free_small(struct ElmPool* pool, struct ElmSmallEntry* entry);

SC_EXTERN void
elm_pool_free_large(struct ElmPool* pool, struct ElmLargeEntry* entry);

#if !SC_DEBUG_ALLOC

SC_EXTERN struct ElmSmallEntry*
elm_pool_alloc_small(struct ElmPool* pool);

SC_EXTERN struct ElmLargeEntry*
elm_pool_alloc_large(struct ElmPool* pool);

static inline struct ScRbNode*
elm_pool_alloc_rbnode(struct ElmPool* pool)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool);
    return entry ? &entry->rbnode : NULL;
}

static inline void
elm_pool_free_rbnode(struct ElmPool* pool, struct ScRbNode* rbnode)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)rbnode;
    elm_pool_free_small(pool, entry);
}

static inline struct ScRec*
elm_pool_alloc_rec(struct ElmPool* pool)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool);
    return entry ? &entry->rec : NULL;
}

static inline void
elm_pool_free_rec(struct ElmPool* pool, struct ScRec* rec)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)rec;
    elm_pool_free_large(pool, entry);
}

static inline struct ScPerm*
elm_pool_alloc_perm(struct ElmPool* pool)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool);
    return entry ? &entry->perm : NULL;
}

static inline void
elm_pool_free_perm(struct ElmPool* pool, struct ScPerm* perm)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)perm;
    elm_pool_free_small(pool, entry);
}

static inline struct ScOrder*
elm_pool_alloc_order(struct ElmPool* pool)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool);
    return entry ? &entry->order : NULL;
}

static inline void
elm_pool_free_order(struct ElmPool* pool, struct ScOrder* order)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)order;
    elm_pool_free_large(pool, entry);
}

static inline struct ScLevel*
elm_pool_alloc_level(struct ElmPool* pool)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool);
    return entry ? &entry->level : NULL;
}

static inline void
elm_pool_free_level(struct ElmPool* pool, struct ScLevel* level)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)level;
    elm_pool_free_small(pool, entry);
}

static inline struct ScExec*
elm_pool_alloc_exec(struct ElmPool* pool)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool);
    return entry ? &entry->exec : NULL;
}

static inline void
elm_pool_free_exec(struct ElmPool* pool, struct ScExec* exec)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)exec;
    elm_pool_free_large(pool, entry);
}

static inline struct ScMatch*
elm_pool_alloc_match(struct ElmPool* pool)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool);
    return entry ? &entry->match : NULL;
}

static inline void
elm_pool_free_match(struct ElmPool* pool, struct ScMatch* match)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)match;
    elm_pool_free_small(pool, entry);
}

static inline struct ScPosn*
elm_pool_alloc_posn(struct ElmPool* pool)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool);
    return entry ? &entry->posn : NULL;
}

static inline void
elm_pool_free_posn(struct ElmPool* pool, struct ScPosn* posn)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)posn;
    elm_pool_free_large(pool, entry);
}

static inline struct ScView*
elm_pool_alloc_view(struct ElmPool* pool)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool);
    return entry ? &entry->view : NULL;
}

static inline void
elm_pool_free_view(struct ElmPool* pool, struct ScView* view)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)view;
    elm_pool_free_large(pool, entry);
}

static inline struct ScBook*
elm_pool_alloc_book(struct ElmPool* pool)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool);
    return entry ? &entry->book : NULL;
}

static inline void
elm_pool_free_book(struct ElmPool* pool, struct ScBook* book)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)book;
    elm_pool_free_large(pool, entry);
}

static inline struct ScSub*
elm_pool_alloc_sub(struct ElmPool* pool)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool);
    return entry ? &entry->sub : NULL;
}

static inline void
elm_pool_free_sub(struct ElmPool* pool, struct ScSub* sub)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)sub;
    elm_pool_free_small(pool, entry);
}

static inline struct ScSess*
elm_pool_alloc_sess(struct ElmPool* pool)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool);
    return entry ? &entry->sess : NULL;
}

static inline void
elm_pool_free_sess(struct ElmPool* pool, struct ScSess* sess)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)sess;
    elm_pool_free_small(pool, entry);
}

#else  // SC_DEBUG_ALLOC

SC_EXTERN struct ElmSmallEntry*
elm_pool_alloc_small(struct ElmPool* pool, const char* file, int line);

SC_EXTERN struct ElmLargeEntry*
elm_pool_alloc_large(struct ElmPool* pool, const char* file, int line);

static inline struct ScRbNode*
elm_pool_alloc_rbnode_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool, file, line);
    sc_log_debug3("allocating rbnode %p in %s at %d", entry, file, line);
    return entry ? &entry->rbnode : NULL;
}

static inline void
elm_pool_free_rbnode(struct ElmPool* pool, struct ScRbNode* rbnode)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)rbnode;
    sc_log_debug3("freeing rbnode %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_small(pool, entry);
}

static inline struct ScRec*
elm_pool_alloc_rec_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool, file, line);
    sc_log_debug3("allocating %p rec in %s at %d", entry, file, line);
    return entry ? &entry->rec : NULL;
}

static inline void
elm_pool_free_rec(struct ElmPool* pool, struct ScRec* rec)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)rec;
    sc_log_debug3("freeing rec %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_large(pool, entry);
}

static inline struct ScPerm*
elm_pool_alloc_perm_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool, file, line);
    sc_log_debug3("allocating perm %p in %s at %d", entry, file, line);
    return entry ? &entry->perm : NULL;
}

static inline void
elm_pool_free_perm(struct ElmPool* pool, struct ScPerm* perm)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)perm;
    sc_log_debug3("freeing perm %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_small(pool, entry);
}

static inline struct ScOrder*
elm_pool_alloc_order_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool, file, line);
    sc_log_debug3("allocating order %p in %s at %d", entry, file, line);
    return entry ? &entry->order : NULL;
}

static inline void
elm_pool_free_order(struct ElmPool* pool, struct ScOrder* order)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)order;
    sc_log_debug3("freeing order %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_large(pool, entry);
}

static inline struct ScLevel*
elm_pool_alloc_level_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool, file, line);
    sc_log_debug3("allocating level %p in %s at %d", entry, file, line);
    return entry ? &entry->level : NULL;
}

static inline void
elm_pool_free_level(struct ElmPool* pool, struct ScLevel* level)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)level;
    sc_log_debug3("freeing level %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_small(pool, entry);
}

static inline struct ScExec*
elm_pool_alloc_exec_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool, file, line);
    sc_log_debug3("allocating exec %p in %s at %d", entry, file, line);
    return entry ? &entry->exec : NULL;
}

static inline void
elm_pool_free_exec(struct ElmPool* pool, struct ScExec* exec)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)exec;
    sc_log_debug3("freeing exec %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_large(pool, entry);
}

static inline struct ScMatch*
elm_pool_alloc_match_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool, file, line);
    sc_log_debug3("allocating match %p in %s at %d", entry, file, line);
    return entry ? &entry->match : NULL;
}

static inline void
elm_pool_free_match(struct ElmPool* pool, struct ScMatch* match)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)match;
    sc_log_debug3("freeing match %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_small(pool, entry);
}

static inline struct ScPosn*
elm_pool_alloc_posn_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool, file, line);
    sc_log_debug3("allocating posn %p in %s at %d", entry, file, line);
    return entry ? &entry->posn : NULL;
}

static inline void
elm_pool_free_posn(struct ElmPool* pool, struct ScPosn* posn)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)posn;
    sc_log_debug3("freeing posn %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_large(pool, entry);
}

static inline struct ScView*
elm_pool_alloc_view_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool, file, line);
    sc_log_debug3("allocating view %p in %s at %d", entry, file, line);
    return entry ? &entry->view : NULL;
}

static inline void
elm_pool_free_view(struct ElmPool* pool, struct ScView* view)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)view;
    sc_log_debug3("freeing view %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_large(pool, entry);
}

static inline struct ScBook*
elm_pool_alloc_book_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeEntry* entry = elm_pool_alloc_large(pool, file, line);
    sc_log_debug3("allocating book %p in %s at %d", entry, file, line);
    return entry ? &entry->book : NULL;
}

static inline void
elm_pool_free_book(struct ElmPool* pool, struct ScBook* book)
{
    struct ElmLargeEntry* entry = (struct ElmLargeEntry*)book;
    sc_log_debug3("freeing book %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_large(pool, entry);
}

static inline struct ScSub*
elm_pool_alloc_sub_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool, file, line);
    sc_log_debug3("allocating sub %p in %s at %d", entry, file, line);
    return entry ? &entry->sub : NULL;
}

static inline void
elm_pool_free_sub(struct ElmPool* pool, struct ScSub* sub)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)sub;
    sc_log_debug3("freeing sub %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_small(pool, entry);
}

static inline struct ScSess*
elm_pool_alloc_sess_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallEntry* entry = elm_pool_alloc_small(pool, file, line);
    sc_log_debug3("allocating sess %p in %s at %d", entry, file, line);
    return entry ? &entry->sess : NULL;
}

static inline void
elm_pool_free_sess(struct ElmPool* pool, struct ScSess* sess)
{
    struct ElmSmallEntry* entry = (struct ElmSmallEntry*)sess;
    sc_log_debug3("freeing sess %p from %s at %d", entry, entry->file, entry->line);
    elm_pool_free_small(pool, entry);
}

#define elm_pool_alloc_rec(pool)                    \
    elm_pool_alloc_rec_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_perm(pool)                   \
    elm_pool_alloc_perm_(pool, __FILE__, __LINE__)
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

#endif // SC_DEBUG_ALLOC

SC_EXTERN void
elm_pool_free_entity_list(struct ElmPool* pool, int type, struct ScSlNode* first);

#endif // ELM_POOL_H
