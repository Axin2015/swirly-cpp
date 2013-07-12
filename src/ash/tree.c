/*
 * Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
 *
 * This file is derived from work by Niels Provos which is subject to the
 * following license:
 *
 * Copyright 2002 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <dbr/tree.h>

enum { BLACK = 0, RED = 1 };

static int
cmp(DbrKey lhs, DbrKey rhs)
{
    int i;
    if (lhs < rhs)
        i = -1;
    else if (lhs > rhs)
        i = 1;
    else
        i = 0;
    return i;
}

static void
set(struct DbrRbNode* node, struct DbrRbNode* parent)
{
    node->parent = parent;
    node->left = node->right = NULL;
    node->color = RED;
}

static void
set_blackred(struct DbrRbNode* black, struct DbrRbNode* red)
{
    black->color = BLACK;
    red->color = RED;
}

static void
rotate_left(struct DbrTree* tree, struct DbrRbNode* node, struct DbrRbNode* tmp)
{
    tmp = node->right;
    if ((node->right = tmp->left))
        tmp->left->parent = node;
    if ((tmp->parent = node->parent)) {
        if (node == node->parent->left)
            node->parent->left = tmp;
        else
            node->parent->right = tmp;
    } else
        tree->root = tmp;
    tmp->left = node;
    node->parent = tmp;
}

static void
rotate_right(struct DbrTree* tree, struct DbrRbNode* node, struct DbrRbNode* tmp)
{
    tmp = node->left;
    if ((node->left = tmp->right))
        tmp->right->parent = node;
    if ((tmp->parent = node->parent)) {
        if (node == node->parent->left)
            node->parent->left = tmp;
        else
            node->parent->right = tmp;
    } else
        tree->root = tmp;
    tmp->right = node;
    node->parent = tmp;
}

static void
insert_color(struct DbrTree* tree, struct DbrRbNode* node)
{
    struct DbrRbNode* parent, * gparent, * tmp;
    while ((parent = node->parent) && parent->color == RED) {
        gparent = parent->parent;
        if (parent == gparent->left) {
            tmp = gparent->right;
            if (tmp && tmp->color == RED) {
                tmp->color = BLACK;
                set_blackred(parent, gparent);
                node = gparent;
                continue;
            }
            if (parent->right == node) {
                rotate_left(tree, parent, tmp);
                tmp = parent;
                parent = node;
                node = tmp;
            }
            set_blackred(parent, gparent);
            rotate_right(tree, gparent, tmp);
        } else {
            tmp = gparent->left;
            if (tmp && tmp->color == RED) {
                tmp->color = BLACK;
                set_blackred(parent, gparent);
                node = gparent;
                continue;
            }
            if (parent->left == node) {
                rotate_right(tree, parent, tmp);
                tmp = parent;
                parent = node;
                node = tmp;
            }
            set_blackred(parent, gparent);
            rotate_left(tree, gparent, tmp);
        }
    }
    tree->root->color = BLACK;
}

static void
remove_color(struct DbrTree* tree, struct DbrRbNode* parent, struct DbrRbNode* node)
{
    struct DbrRbNode* tmp;
    while ((!node || node->color == BLACK) && node != tree->root) {
        if (parent->left == node) {
            tmp = parent->right;
            if (tmp->color == RED) {
                set_blackred(tmp, parent);
                rotate_left(tree, parent, tmp);
                tmp = parent->right;
            }
            if ((!tmp->left || tmp->left->color == BLACK) &&
                (!tmp->right || tmp->right->color == BLACK)) {
                tmp->color = RED;
                node = parent;
                parent = node->parent;
            } else {
                if (!tmp->right || tmp->right->color == BLACK) {
                    struct DbrRbNode* oleft;
                    if ((oleft = tmp->left))
                        oleft->color = BLACK;
                    tmp->color = RED;
                    rotate_right(tree, tmp, oleft);
                    tmp = parent->right;
                }
                tmp->color = parent->color;
                parent->color = BLACK;
                if (tmp->right)
                    tmp->right->color = BLACK;
                rotate_left(tree, parent, tmp);
                node = tree->root;
                break;
            }
        } else {
            tmp = parent->left;
            if (tmp->color == RED) {
                set_blackred(tmp, parent);
                rotate_right(tree, parent, tmp);
                tmp = parent->left;
            }
            if ((!tmp->left || tmp->left->color == BLACK) &&
                (!tmp->right || tmp->right->color == BLACK)) {
                tmp->color = RED;
                node = parent;
                parent = node->parent;
            } else {
                if (!tmp->left || tmp->left->color == BLACK) {
                    struct DbrRbNode* oright;
                    if ((oright = tmp->right))
                        oright->color = BLACK;
                    tmp->color = RED;
                    rotate_left(tree, tmp, oright);
                    tmp = parent->left;
                }
                tmp->color = parent->color;
                parent->color = BLACK;
                if (tmp->left)
                    tmp->left->color = BLACK;
                rotate_right(tree, parent, tmp);
                node = tree->root;
                break;
            }
        }
    }
    if (node)
        node->color = BLACK;
}

DBR_API struct DbrRbNode*
dbr_tree_insert(struct DbrTree* tree, struct DbrRbNode* node)
{
    struct DbrRbNode* tmp;
    struct DbrRbNode* parent = NULL;
    int comp = 0;
    tmp = tree->root;
    while (tmp) {
        parent = tmp;
        comp = cmp(node->key, parent->key);
        if (comp < 0)
            tmp = tmp->left;
        else if (comp > 0)
            tmp = tmp->right;
        else
            return tmp;
    }
    set(node, parent);
    if (parent) {
        if (comp < 0)
            parent->left = node;
        else
            parent->right = node;
    } else
        tree->root = node;
    insert_color(tree, node);
    return NULL;
}

DBR_API void
dbr_tree_pinsert(struct DbrTree* tree, struct DbrRbNode* node, struct DbrRbNode* parent)
{
    set(node, parent);
    if (parent) {
        int comp = cmp(node->key, parent->key);
        if (comp < 0)
            parent->left = node;
        else
            parent->right = node;
    } else
        tree->root = node;
    insert_color(tree, node);
}

DBR_API struct DbrRbNode*
dbr_tree_remove(struct DbrTree* tree, struct DbrRbNode* node)
{
    struct DbrRbNode* child, * parent, * old = node;
    int color;
    if (!node->left)
        child = node->right;
    else if (!node->right)
        child = node->left;
    else {
        struct DbrRbNode* left;
        node = node->right;
        while ((left = node->left))
            node = left;
        child = node->right;
        parent = node->parent;
        color = node->color;
        if (child)
            child->parent = parent;
        if (parent) {
            if (parent->left == node)
                parent->left = child;
            else
                parent->right = child;
        } else
            tree->root = child;
        if (node->parent == old)
            parent = node;

        node->left = old->left;
        node->right = old->right;
        node->parent = old->parent;
        node->color = old->color;

        if (old->parent) {
            if (old->parent->left == old)
                old->parent->left = node;
            else
                old->parent->right = node;
        } else
            tree->root = node;
        old->left->parent = node;
        if (old->right)
            old->right->parent = node;
        goto color;
    }
    parent = node->parent;
    color = node->color;
    if (child)
        child->parent = parent;
    if (parent) {
        if (parent->left == node)
            parent->left = child;
        else
            parent->right = child;
    } else
        tree->root = child;
 color:
    if (color == BLACK)
        remove_color(tree, parent, child);
    return old;
}

DBR_API struct DbrRbNode*
dbr_tree_find(const struct DbrTree* tree, DbrKey key)
{
    struct DbrRbNode* tmp = tree->root;
    int comp;
    while (tmp) {
        comp = cmp(key, tmp->key);
        if (comp < 0)
            tmp = tmp->left;
        else if (comp > 0)
            tmp = tmp->right;
        else
            return tmp;
    }
    return NULL;
}

DBR_API struct DbrRbNode*
dbr_tree_nfind(const struct DbrTree* tree, DbrKey key)
{
    struct DbrRbNode* tmp = tree->root;
    struct DbrRbNode* res = NULL;
    int comp;
    while (tmp) {
        comp = cmp(key, tmp->key);
        if (comp < 0) {
            res = tmp;
            tmp = tmp->left;
        }
        else if (comp > 0)
            tmp = tmp->right;
        else
            return tmp;
    }
    return res;
}

DBR_API struct DbrRbNode*
dbr_tree_pfind(const struct DbrTree* tree, DbrKey key)
{
    struct DbrRbNode* tmp = tree->root;
    struct DbrRbNode* parent = NULL;
    int comp;
    while (tmp) {
        parent = tmp;
        comp = cmp(key, tmp->key);
        if (comp < 0)
            tmp = tmp->left;
        else if (comp > 0)
            tmp = tmp->right;
        else
            return tmp;
    }
    return parent;
}

DBR_API struct DbrRbNode*
dbr_tree_first(const struct DbrTree* tree)
{
    struct DbrRbNode* tmp = tree->root;
    struct DbrRbNode* parent = NULL;
    while (tmp) {
        parent = tmp;
        tmp = tmp->left;
    }
    return parent;
}

DBR_API struct DbrRbNode*
dbr_tree_last(const struct DbrTree* tree)
{
    struct DbrRbNode* tmp = tree->root;
    struct DbrRbNode* parent = NULL;
    while (tmp) {
        parent = tmp;
        tmp = tmp->right;
    }
    return parent;
}
