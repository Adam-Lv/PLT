#ifndef PLT_ANALYSE_SYNTAXIQUE_H
#define PLT_ANALYSE_SYNTAXIQUE_H

#include "../include/analyse_lexicale.h"

typedef struct StackNode {
    char *content;
    struct StackNode *prev;
    struct StackNode *next;
} StackNode;

StackNode *NewStackNode(char *content);

typedef struct Stack {
    StackNode *head;
    StackNode *tail;
    bool (*is_empty)(struct Stack *);
    void (*append)(struct Stack *, StackNode *);
    StackNode *(*pop)(struct Stack *);
} Stack;

Stack *NewStack();

typedef struct {
    int length;
    lexeme *container;
} etat_list;

typedef struct {
    int length;
    int *container;
} final_list;

typedef struct {
    int start;
    int end;
    char *character;
    lexeme stack_op1[2];
    lexeme stack_op2[2];
} transition;

typedef struct {
    int length;
    transition *container;
} transition_list;

typedef struct {
    bool has_error;
    int stack_num;
    etat_list *etats;
    int initial;
    final_list *finals;
    transition_list *transitions;
} analyse_syntaxique_result;

etat_list *automate_etats(lexeme *AL_res, int head, int tail);

final_list *automate_finals(lexeme *AL_res, int head, int tail);

transition_list *automate_transitions(lexeme *AL_res, int head, int tail, int stack_num);

analyse_syntaxique_result *analyse_syntaxique(lexeme *AL_res);

#endif //PLT_ANALYSE_SYNTAXIQUE_H
