#include <stdlib.h>
#include "../include/analyse_semantique.h"

static inline void set_left(TreeNode *root, lexeme *content) { root->left_child = NewNode(content); }

static inline void set_right(TreeNode *root, lexeme *content) { root->right_child = NewNode(content); }

TreeNode *NewNode(lexeme *content) {
    TreeNode *this = malloc(sizeof(TreeNode));
    this->content = content;
    this->left_child = NULL;
    this->right_child = NULL;
    this->set_left = set_left;
    this->set_right = set_right;
    return this;
}