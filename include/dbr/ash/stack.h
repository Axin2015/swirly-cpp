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
#ifndef DBR_ASH_STACK_H
#define DBR_ASH_STACK_H

/**
 * @file
 * Stack.
 */

#include <dbr/ash/defs.h>
#include <dbr/ash/slnode.h>

/**
 * @addtogroup Stack
 * @{
 */

struct DbrStack {
    struct DbrSlNode* first;
};

#define DBR_STACK_INIT(s) { .first = NULL }
#define DBR_STACK_END NULL

static inline void
dbr_stack_init(struct DbrStack* stack)
{
	stack->first = NULL;
}

static inline void
dbr_stack_insert_after(struct DbrStack* stack, struct DbrSlNode* node, struct DbrSlNode* new_node)
{
	new_node->next = node->next;
	node->next = new_node;
}

static inline void
dbr_stack_insert_front(struct DbrStack* stack, struct DbrSlNode* new_node)
{
	new_node->next = stack->first;
	stack->first = new_node;
}

static inline struct DbrSlNode*
dbr_stack_remove_first(struct DbrStack* stack)
{
    struct DbrSlNode* first = stack->first;
	stack->first = stack->first->next;
    return first;
}

static inline struct DbrSlNode*
dbr_stack_pop(struct DbrStack* stack)
{
    return dbr_stack_remove_first(stack);
}

static inline void
dbr_stack_push(struct DbrStack* stack, struct DbrSlNode* new_node)
{
    dbr_stack_insert_front(stack, new_node);
}

static inline struct DbrSlNode*
dbr_stack_first(const struct DbrStack* stack)
{
    return stack->first;
}

static inline DbrBool
dbr_stack_empty(const struct DbrStack* stack)
{
    return stack->first == NULL;
}

/** @} */

#endif // DBR_ASH_STACK_H
