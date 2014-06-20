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
#ifndef DBR_ASH_QUEUE_H
#define DBR_ASH_QUEUE_H

#include <dbr/ash/defs.h>
#include <dbr/ash/slnode.h>

/**
 * @addtogroup Queue
 * @{
 */

struct DbrQueue {
    struct DbrSlNode* first;
    struct DbrSlNode** last;
};

#define DBR_QUEUE_INIT(q) { .first = NULL, .last = &q.first }
#define DBR_QUEUE_END NULL

static inline void
dbr_queue_init(struct DbrQueue* queue)
{
	queue->first = NULL;
	queue->last = &queue->first;
}

static inline void
dbr_queue_insert_after(struct DbrQueue* queue, struct DbrSlNode* node, struct DbrSlNode* new_node)
{
	if (!(new_node->next = node->next))
		queue->last = &new_node->next;
	node->next = new_node;
}

static inline void
dbr_queue_insert_front(struct DbrQueue* queue, struct DbrSlNode* new_node)
{
	if (!(new_node->next = queue->first))
		queue->last = &new_node->next;
	queue->first = new_node;
}

static inline void
dbr_queue_insert_back(struct DbrQueue* queue, struct DbrSlNode* new_node)
{
	new_node->next = NULL;
	*queue->last = new_node;
	queue->last = &new_node->next;
}

static inline struct DbrSlNode*
dbr_queue_remove_first(struct DbrQueue* queue)
{
    struct DbrSlNode* first = queue->first;
	if (!(queue->first = queue->first->next))
		queue->last = &queue->first;
    return first;
}

static inline void
dbr_queue_push(struct DbrQueue* queue, struct DbrSlNode* new_node)
{
    dbr_queue_insert_front(queue, new_node);
}

static inline struct DbrSlNode*
dbr_queue_pop(struct DbrQueue* queue)
{
    return dbr_queue_remove_first(queue);
}

static inline struct DbrSlNode*
dbr_queue_first(const struct DbrQueue* queue)
{
    return queue->first;
}

static inline DbrBool
dbr_queue_empty(const struct DbrQueue* queue)
{
    return queue->first == NULL;
}

static inline void
dbr_queue_join(struct DbrQueue* lhs, struct DbrQueue* rhs)
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

#endif // DBR_ASH_QUEUE_H
