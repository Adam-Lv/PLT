#ifndef PLT_ANALYSE_SEMANTIQUE_H
#define PLT_ANALYSE_SEMANTIQUE_H

#include "../include/analyse_lexicale.h"
#include "../include/analyse_syntaxique.h"
typedef struct TreeNode {
    lexeme *content;
    struct TreeNode *left_child;
    struct TreeNode *right_child;
    void (*set_left)(struct TreeNode *, lexeme *);
    void (*set_right)(struct TreeNode *, lexeme *);
} TreeNode;

TreeNode *NewNode(lexeme *content);

#endif //PLT_ANALYSE_SEMANTIQUE_H
