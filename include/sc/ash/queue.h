/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_QUEUE_H
#define SC_ASH_QUEUE_H

/**
 * @file
 * Singly-linked queue.
 */

#include <sc/ash/defs.h>
#include <sc/ash/slnode.h>

/**
 * @addtogroup Queue
 * @{
 */

struct ScQueue {
    struct ScSlNode* first;
    struct ScSlNode** last;
};

#define SC_QUEUE_INIT(q) { .first = NULL, .last = &q.first }
#define SC_QUEUE_END NULL

static inline void
sc_queue_init(struct ScQueue* queue)
{
	queue->first = NULL;
	queue->last = &queue->first;
}

static inline void
sc_queue_insert_after(struct ScQueue* queue, struct ScSlNode* node, struct ScSlNode* new_node)
{
	if (!(new_node->next = node->next))
		queue->last = &new_node->next;
	node->next = new_node;
}

static inline void
sc_queue_insert_front(struct ScQueue* queue, struct ScSlNode* new_node)
{
	if (!(new_node->next = queue->first))
		queue->last = &new_node->next;
	queue->first = new_node;
}

static inline void
sc_queue_insert_back(struct ScQueue* queue, struct ScSlNode* new_node)
{
	new_node->next = NULL;
	*queue->last = new_node;
	queue->last = &new_node->next;
}

static inline struct ScSlNode*
sc_queue_remove_first(struct ScQueue* queue)
{
    struct ScSlNode* first = queue->first;
	if (!(queue->first = queue->first->next))
		queue->last = &queue->first;
    return first;
}

static inline void
sc_queue_push(struct ScQueue* queue, struct ScSlNode* new_node)
{
    sc_queue_insert_front(queue, new_node);
}

static inline struct ScSlNode*
sc_queue_pop(struct ScQueue* queue)
{
    return sc_queue_remove_first(queue);
}

static inline struct ScSlNode*
sc_queue_first(const struct ScQueue* queue)
{
    return queue->first;
}

static inline ScBool
sc_queue_empty(const struct ScQueue* queue)
{
    return queue->first == NULL;
}

static inline void
sc_queue_join(struct ScQueue* lhs, struct ScQueue* rhs)
{
    if (rhs->first) {
        if (lhs->first)
            *lhs->last = rhs->first;
        else
            lhs->first = rhs->first;
        lhs->last = rhs->last;
    }
}

/** @} */

#endif // SC_ASH_QUEUE_H
