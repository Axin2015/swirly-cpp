/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <dbr/ash/rbnode.h>

DBR_API struct DbrRbNode*
dbr_rbnode_next(struct DbrRbNode* node)
{
    if (node->right) {
        node = node->right;
        while (node->left)
            node = node->left;
    } else {
        if (node->parent && node == node->parent->left)
            node = node->parent;
        else {
            while (node->parent && node == node->parent->right)
                node = node->parent;
            node = node->parent;
        }
    }
    return node;
}

DBR_API struct DbrRbNode*
dbr_rbnode_prev(struct DbrRbNode* node)
{
    if (node->left) {
        node = node->left;
        while (node->right)
            node = node->right;
    } else {
        if (node->parent && node == node->parent->right)
            node = node->parent;
        else {
            while (node->parent && node == node->parent->left)
                node = node->parent;
            node = node->parent;
        }
    }
    return node;
}
