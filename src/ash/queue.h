/**
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
#ifndef ASH_QUEUE_H
#define ASH_QUEUE_H

#include <dbr/bool.h>
#include <dbr/defs.h>
#include <dbr/node.h>

#include <assert.h>
#include <stddef.h> // NULL

struct AshQueue {
    struct DbrSlNode* first;
    struct DbrSlNode** last;
};

#define ASH_QUEUE_INIT(q) { .first = NULL, .last = &q.first }

static inline void
ash_queue_init(struct AshQueue* queue)
{
	queue->first = NULL;
	queue->last = &queue->first;
}

static inline void
ash_queue_insert_after(struct AshQueue* queue, struct DbrSlNode* node, struct DbrSlNode* new_node)
{
	if (!(new_node->next = node->next))
		queue->last = &new_node->next;
	node->next = new_node;
}

static inline void
ash_queue_insert_front(struct AshQueue* queue, struct DbrSlNode* new_node)
{
	if (!(new_node->next = queue->first))
		queue->last = &new_node->next;
	queue->first = new_node;
}

static inline void
ash_queue_insert_back(struct AshQueue* queue, struct DbrSlNode* new_node)
{
	new_node->next = NULL;
	*queue->last = new_node;
	queue->last = &new_node->next;
}

static inline struct DbrSlNode*
ash_queue_remove_first(struct AshQueue* queue)
{
    struct DbrSlNode* first = queue->first;
	if (!(queue->first = queue->first->next))
		queue->last = &queue->first;
    return first;
}

static inline void
ash_queue_push(struct AshQueue* queue, struct DbrSlNode* new_node)
{
    ash_queue_insert_front(queue, new_node);
}

static inline struct DbrSlNode*
ash_queue_pop(struct AshQueue* queue)
{
    return ash_queue_remove_first(queue);
}

static inline struct DbrSlNode*
ash_queue_first(const struct AshQueue* queue)
{
    return queue->first;
}

static inline struct DbrSlNode*
ash_queue_end(const struct AshQueue* queue)
{
    return NULL;
}

static inline DbrBool
ash_queue_empty(const struct AshQueue* queue)
{
    return queue->first == NULL;
}

#endif // ASH_QUEUE_H
