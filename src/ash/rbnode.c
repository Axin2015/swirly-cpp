/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/ash/rbnode.h>

SC_API struct ScRbNode*
sc_rbnode_next(struct ScRbNode* node)
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

SC_API struct ScRbNode*
sc_rbnode_prev(struct ScRbNode* node)
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
