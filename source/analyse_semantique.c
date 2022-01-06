#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/analyse_semantique.h"

char *enum_name[7] = {"DIGIT", "OPERATOR", "CHARACTER", "KEYWORD", "DELIMITER", "STRING", "COMMA"};

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

bool analyse_semantique(analyse_syntaxique_resultat *ASy_res) {
    // check etat types
    Type etat_type = ASy_res->etats->container[0].type;
    int etat_num = ASy_res->etats->length;
    for (int i = 0; i < etat_num; i++) {
        if (etat_type != ASy_res->etats->container[i].type) {
            printf("Semantic error! Detected the first etat's type is %s, but %d-th etat's type is %s.\n",
                   enum_name[etat_type], i + 1, enum_name[ASy_res->etats->container[i].type]);
            return false;
        }
    }

    // check initial
    if (ASy_res->initial >= etat_num) {
        printf("Semantic error! Initial etat index out of range.\n");
        return false;
    }

    // check finals
    for (int i = 0; i < ASy_res->finals->length; i++) {
        if (ASy_res->finals->container[i] >= etat_num) {
            printf("Semantic error! %d-th final etat index out of range.\n", i + 1);
            return false;
        }
    }

    transition curr;
    char *stack1_char = NULL, *stack2_char = NULL;
    // check transitions
    for (int i = 0; i < ASy_res->transitions->length; i++) {
        curr = ASy_res->transitions->container[i];
        // check start etat
        if (curr.start >= etat_num) {
            printf("Semantic error! Initial etat index out of range.\n");
            return false;
        }

        // check end etat
        if (curr.end >= etat_num) {
            printf("Semantic error! Initial etat index out of range.\n");
            return false;
        }

        // check character's length
        if (strlen(curr.character) != 3) {
            printf("Semantic error! Detected more than one character in transition %d.\n", i + 1);
            return false;
        }

        // check stack operations
        if (curr.stack_op1[0].value) {
            if (strcmp(curr.stack_op1[0].value, "→") == 0) {
                if ((strcmp(curr.stack_op1[1].value, "→") == 0)) {
                    printf("Semantic error! Invalid expression in transition %d.\n", i + 1);
                    return false;
                }
                if (stack1_char == NULL) stack1_char = curr.stack_op1[1].value;
                if (strcmp(stack1_char, curr.stack_op1[1].value) != 0) {
                    printf("Semantic error! Different element pushed/popped of stack 1 in transition %d.\n", i + 1);
                    return false;
                }
            }
            else {
                if ((strcmp(curr.stack_op1[1].value, "→") != 0)) {
                    printf("Semantic error! Invalid expression in transition %d.\n", i + 1);
                    return false;
                }
                if (stack1_char == NULL) stack1_char = curr.stack_op1[0].value;
                if (strcmp(stack1_char, curr.stack_op1[0].value) != 0) {
                    printf("Semantic error! Different element pushed/popped of stack 1 in transition %d.\n", i + 1);
                    return false;
                }
            }
        }
        if (curr.stack_op2[0].value) {
            if (strcmp(curr.stack_op2[0].value, "→") == 0) {
                if ((strcmp(curr.stack_op2[1].value, "→") == 0)) {
                    printf("Semantic error! Invalid expression in transition %d.\n", i + 1);
                    return false;
                }
                if (stack2_char == NULL) stack2_char = curr.stack_op2[1].value;
                if (strcmp(stack2_char, curr.stack_op2[1].value) != 0) {
                    printf("Semantic error! Different element pushed/popped of stack 2 in transition %d.\n", i + 1);
                    return false;
                }
            }
            else {
                if ((strcmp(curr.stack_op2[1].value, "→") != 0)) {
                    printf("Semantic error! Invalid expression in transition %d.\n", i + 1);
                    return false;
                }
                if (stack2_char == NULL) stack2_char = curr.stack_op2[0].value;
                if (strcmp(stack2_char, curr.stack_op2[0].value) != 0) {
                    printf("Semantic error! Different element pushed/popped of stack 2 in transition %d.\n", i + 1);
                    return false;
                }
            }
        }
    }
    return true;
}
