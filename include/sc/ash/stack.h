/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_STACK_H
#define SC_ASH_STACK_H

/**
 * @file
 * Stack.
 */

#include <sc/ash/defs.h>
#include <sc/ash/slnode.h>

/**
 * @addtogroup Stack
 * @{
 */

struct ScStack {
    struct ScSlNode* first;
};

#define SC_STACK_INIT(s) { .first = NULL }
#define SC_STACK_END NULL

static inline void
sc_stack_init(struct ScStack* stack)
{
	stack->first = NULL;
}

static inline void
sc_stack_insert_after(struct ScStack* stack, struct ScSlNode* node, struct ScSlNode* new_node)
{
	new_node->next = node->next;
	node->next = new_node;
}

static inline void
sc_stack_insert_front(struct ScStack* stack, struct ScSlNode* new_node)
{
	new_node->next = stack->first;
	stack->first = new_node;
}

static inline struct ScSlNode*
sc_stack_remove_first(struct ScStack* stack)
{
    struct ScSlNode* first = stack->first;
	stack->first = stack->first->next;
    return first;
}

static inline struct ScSlNode*
sc_stack_pop(struct ScStack* stack)
{
    return sc_stack_remove_first(stack);
}

static inline void
sc_stack_push(struct ScStack* stack, struct ScSlNode* new_node)
{
    sc_stack_insert_front(stack, new_node);
}

static inline struct ScSlNode*
sc_stack_first(const struct ScStack* stack)
{
    return stack->first;
}

static inline ScBool
sc_stack_empty(const struct ScStack* stack)
{
    return stack->first == NULL;
}

/** @} */

#endif // SC_ASH_STACK_H
