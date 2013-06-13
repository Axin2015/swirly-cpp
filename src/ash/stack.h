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
#ifndef ASH_STACK_H
#define ASH_STACK_H

#include <dbr/bool.h>
#include <dbr/node.h>

#include <assert.h>
#include <stddef.h> // NULL

struct AshStack {
    struct DbrSlNode* first;
};

#define ASH_STACK_INIT(s) { .first = NULL }

static inline void
ash_stack_init(struct AshStack* stack)
{
	stack->first = NULL;
}

static inline void
ash_stack_insert_after(struct AshStack* stack, struct DbrSlNode* node, struct DbrSlNode* new_node)
{
	new_node->next = node->next;
	node->next = new_node;
}

static inline void
ash_stack_insert_front(struct AshStack* stack, struct DbrSlNode* new_node)
{
	new_node->next = stack->first;
	stack->first = new_node;
}

static inline struct DbrSlNode*
ash_stack_remove_first(struct AshStack* stack)
{
    struct DbrSlNode* first = stack->first;
	stack->first = stack->first->next;
    return first;
}

static inline struct DbrSlNode*
ash_stack_pop(struct AshStack* stack)
{
    return ash_stack_remove_first(stack);
}

static inline void
ash_stack_push(struct AshStack* stack, struct DbrSlNode* new_node)
{
    ash_stack_insert_front(stack, new_node);
}

static inline struct DbrSlNode*
ash_stack_first(const struct AshStack* stack)
{
    return stack->first;
}

static inline struct DbrSlNode*
ash_stack_end(const struct AshStack* stack)
{
    return NULL;
}

static inline DbrBool
ash_stack_empty(const struct AshStack* stack)
{
    return stack->first == NULL;
}

#endif // ASH_STACK_H
