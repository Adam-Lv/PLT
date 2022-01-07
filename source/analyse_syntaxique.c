#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/analyse_syntaxique.h"

StackNode *NewStackNode(char *content) {
    StackNode *this = malloc(sizeof(StackNode));
    this->content = content;
    this->prev = NULL;
    this->next = NULL;
    return this;
}

static inline bool is_empty(Stack *stack) { return stack->head == NULL; }

static void append(Stack *stack, StackNode *stackNode) {
    if (stack->is_empty(stack)) {
        stack->head = stack->tail = stackNode;
    } else {
        stack->tail->next = stackNode;
        stackNode->prev = stack->tail;
        stack->tail = stackNode;
    }
}

static StackNode *pop(Stack *stack) {
    StackNode *temp = stack->tail;
    if (stack->head != stack->tail) {
        stack->tail = temp->prev;
        stack->tail->next = NULL;
        temp->prev = NULL;
    } else {
        stack->head = stack->tail = NULL;
    }
    return temp;
}

static inline void clean(Stack *stack) {
    while (!stack->is_empty(stack)) free(pop(stack));
}

Stack *NewStack() {
    Stack *this = malloc(sizeof(Stack));
    this->head = NULL;
    this->tail = NULL;
    this->is_empty = is_empty;
    this->append = append;
    this->pop = pop;
    this->clean = clean;
    return this;
}

static inline void print_error(char *exp, char *got) {
    printf("Syntax error! Expected %s but got %s.\n", exp, got);
}

etat_list *automate_etats(lexeme *AL_res, int head, int tail) {
    etat_list *res = malloc(sizeof(etat_list));
    lexeme temp[MAX_ETAT_NUM];
    int length = 0;
    int etat_actif = 0;
    if (AL_res[head].type == COMMA) {
        print_error("an etat", AL_res[head].value);
        free(res);
        return NULL;
    }
    if ((tail - head) == 0) {
        print_error("etat", "nothing");
        free(res);
        return NULL;
    }
    temp[length++] = AL_res[head];
    for (int i = head + 1; i < tail; i++) {
        if (etat_actif == 1) // COMMA
        {
            if (AL_res[i].type == COMMA) {
                print_error("an etat", AL_res[i].value);
                free(res);
                return NULL;
            } else {
                temp[length++] = AL_res[i];
                etat_actif = 0;
            }
        } else {  // others
            if (AL_res[i].type != COMMA) {
                print_error(",", AL_res[i].value);
                free(res);
                return NULL;
            } else {
                etat_actif = 1;
            }
        }
    }
    if (etat_actif == 1) {
        print_error("an etat after comma", "nothing");
        free(res);
        return NULL;
    }
    res->length = length;
    res->container = malloc(sizeof(lexeme) * res->length);
    for (int i = 0; i < res->length; i++) {
        res->container[i] = temp[i];
    }
    return res;
}

static int str2int(char *str) {
    int res = 0;
    for (int i = 0; i < strlen(str); i++) {
        if (res == 0 && str[i] == '0') continue;
        res = 10 * res + (str[i] - '0');
    }
    return res;

}

final_list *automate_finals(lexeme *AL_res, int head, int tail) {
    final_list *res = malloc(sizeof(final_list));
    int temp[MAX_ETAT_NUM];
    int length = 0;
    int etat_actif = 0;
    if (AL_res[head].type == COMMA) {
        print_error("an etat", AL_res[head].value);
        free(res);
        return NULL;
    }
    if ((tail - head) == 0) {
        print_error("digit", "nothing");
        free(res);
        return NULL;
    }
    temp[length++] = str2int(AL_res[head].value);
    for (int i = head + 1; i < tail; i++) {
        if (etat_actif == 1) // COMMA
        {
            if (AL_res[i].type == COMMA) {
                print_error("an etat", AL_res[i].value);
                free(res);
                return NULL;
            } else {
                temp[length++] = str2int(AL_res[i].value);
                etat_actif = 0;
            }
        } else {  // others
            if (AL_res[i].type != COMMA) {
                print_error(",", AL_res[i].value);
                free(res);
                return NULL;
            } else {
                etat_actif = 1;
            }
        }
    }
    if (etat_actif == 1) {
        print_error("an etat after comma", "nothing");
        free(res);
        return NULL;
    }
    res->length = length;
    res->container = malloc(sizeof(int) * res->length);
    for (int i = 0; i < res->length; i++) {
        res->container[i] = temp[i];
    }
    return res;
}

transition_list *transitions_zero_stack(lexeme *AL_res, int head, int tail) {
    transition_list *transitions = malloc(sizeof(transition_list));
    int length = 0;
    int etat_actif = 0;
    transition temp[MAX_TRANSITION_NUM];
    for (int i = 0; i < MAX_TRANSITION_NUM; i++) {
        temp[i].start = temp[i].end = 0;
        temp[i].character = NULL;
        temp[i].stack_op1[0].value = temp[i].stack_op2[0].value =
        temp[i].stack_op1[1].value = temp[i].stack_op2[1].value = NULL;
        temp[i].stack_op1[0].type = temp[i].stack_op2[0].type =
        temp[i].stack_op1[1].type = temp[i].stack_op2[1].type = DIGIT;
    }
    if (strcmp(AL_res[head].value, "(") != 0) {
        print_error("(", AL_res[head].value);
        free(transitions);
        return NULL;
    }
    if ((tail - head) == 0) {
        print_error("transition", "nothing");
        free(transitions);
        return NULL;
    }
    Stack *stack = NewStack();
    stack->append(stack, NewStackNode("("));
    for (int i = head + 1; i < tail; i++) {
        switch (etat_actif) {
            case 0:  // (
                if (AL_res[i].type != DIGIT) {
                    print_error("a digit", AL_res[i].value);
                    stack->clean(stack);
                    free(stack);
                    free(transitions);
                    return NULL;
                }
                temp[length].start = str2int(AL_res[i].value);
                etat_actif = 1;
                break;
            case 1:  // DIGIT
                switch (AL_res[i].type) {
                    case OPERATOR:
                        if (AL_res[i].value[0] != '=') etat_actif = 2;
                        break;
                    case COMMA:
                        etat_actif = 3;
                        break;
                    default:
                        print_error("→ or , ", AL_res[i].value);
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
                break;
            case 2:  // →
                if (AL_res[i].type != DIGIT) {
                    print_error("a digit", AL_res[i].value);
                    stack->clean(stack);
                    free(stack);
                    free(transitions);
                    return NULL;
                }
                temp[length].end = str2int(AL_res[i].value);
                etat_actif = 1;
                break;
            case 3:  // ,
                switch (AL_res[i].type) {
                    case CHARACTER:
                        temp[length].character = AL_res[i].value;
                        etat_actif = 4;
                        break;
                    case DELIMITER:
                        if (AL_res[i].value[0] == '(' && stack->is_empty(stack)) {
                            stack->append(stack, NewStackNode("("));
                            etat_actif = 0;
                            break;
                        } else {
                            print_error("nothing", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                    default:
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
                break;
            case 4:  // CHARACTER
                if (AL_res[i].value[0] != ')') {
                    print_error(")", AL_res[i].value);
                    stack->clean(stack);
                    free(stack);
                    free(transitions);
                    return NULL;
                } else if (stack->is_empty(stack)) {
                    print_error("nothing", ")");
                    stack->clean(stack);
                    free(stack);
                    free(transitions);
                    return NULL;
                } else {
                    free(stack->pop(stack));
                    etat_actif = 5;
                    length++;
                }
                break;
            case 5:  // )
                if (AL_res[i].type != COMMA) {
                    print_error(",", AL_res[i].value);
                    stack->clean(stack);
                    free(stack);
                    free(transitions);
                    return NULL;
                }
                etat_actif = 3;
                break;
        }
    }
    if (etat_actif != 5 || !stack->is_empty(stack)) {
        print_error(")", AL_res[tail].value);
        stack->clean(stack);
        free(stack);
        free(transitions);
        return NULL;
    }
    stack->clean(stack);
    free(stack);
    transitions->length = length;
    transitions->container = malloc(sizeof(transition) * length);
    for (int i = 0; i < length; i++) transitions->container[i] = temp[i];
    return transitions;
}

transition_list *transitions_one_stack(lexeme *AL_res, int head, int tail) {
    transition_list *transitions = malloc(sizeof(transition_list));
    int length = 0;
    int etat_actif = 0;
    int op_num = 1;
    transition temp[MAX_TRANSITION_NUM];
    for (int i = 0; i < MAX_TRANSITION_NUM; i++) {
        temp[i].start = temp[i].end = 0;
        temp[i].character = NULL;
        temp[i].stack_op1[0].value = temp[i].stack_op2[0].value =
        temp[i].stack_op1[1].value = temp[i].stack_op2[1].value = NULL;
        temp[i].stack_op1[0].type = temp[i].stack_op2[0].type =
        temp[i].stack_op1[1].type = temp[i].stack_op2[1].type = DIGIT;
    }
    if (strcmp(AL_res[head].value, "(") != 0) {
        print_error("(", AL_res[head].value);
        free(transitions);
        return NULL;
    }
    if ((tail - head) == 0) {
        print_error("transition", "nothing");
        free(transitions);
        return NULL;
    }
    Stack *stack = NewStack();
    stack->append(stack, NewStackNode("("));
    for (int i = head + 1; i < tail; i++) {
        switch (etat_actif) {
            case 0:  // (
                switch (AL_res[i].type) {
                    case DIGIT:
                        temp[length].start = str2int(AL_res[i].value);
                        etat_actif = 1;
                        break;
                    case OPERATOR:
                        if (stack->head->next == stack->tail && AL_res[i].value[0] != '=') {
                            if (op_num != 1) {
                                print_error("nothing", "more than one transition expression");
                                stack->clean(stack);
                                free(stack);
                                free(transitions);
                                return NULL;
                            }
                            temp[length].stack_op1[0] = AL_res[i];
                            etat_actif = 2;
                            break;
                        } else {
                            print_error("a digit or →", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                    case CHARACTER:
                        etat_actif = 4;
                        if (op_num != 1) {
                            print_error("nothing", "more than one transition expression");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        temp[length].stack_op1[0] = AL_res[i];
                        break;
                    case DELIMITER:  // Pas d'action de pile
                        if (stack->is_empty(stack)) {
                            print_error("nothing", ")");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (stack->head == stack->tail) {
                            print_error("transition expression", "nothing");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        free(stack->pop(stack));
                        op_num++;
                        etat_actif = 5;
                        break;
                    default:
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
                break;
            case 1:  // DIGIT
                switch (AL_res[i].type) {
                    case OPERATOR:
                        if (AL_res[i].value[0] != '=') etat_actif = 2;
                        break;
                    case COMMA:
                        etat_actif = 3;
                        break;
                    default:
                        print_error("→ or , ", AL_res[i].value);
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
                break;
            case 2:  // →
                switch (AL_res[i].type) {
                    case COMMA:
                        etat_actif = 3;
                        break;
                    case DELIMITER:
                        if (stack->head->next == stack->tail && !stack->is_empty(stack) && AL_res[i].value[0] == ')') {
                            if (temp[length].stack_op1[0].value != NULL && temp[length].stack_op1[1].value == NULL) {
                                print_error(", and expression", ")");
                                stack->clean(stack);
                                free(stack);
                                free(transitions);
                                return NULL;
                            }
                            free(stack->pop(stack));
                            op_num++;
                            etat_actif = 4;
                            break;
                        } else {
                            print_error("nothing", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                    case DIGIT:
                        temp[length].end = str2int(AL_res[i].value);
                        etat_actif = 1;
                        break;
                    default:
                        print_error("a digit or ) or , ", AL_res[i].value);
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
                break;
            case 3:  // ,
                switch (AL_res[i].type) {
                    case CHARACTER:
                        if (stack->head == stack->tail) {
                            temp[length].character = AL_res[i].value;
                        } else {
                            if (op_num != 1) {
                                print_error("nothing", "more than one transition expression");
                                stack->clean(stack);
                                free(stack);
                                free(transitions);
                                return NULL;
                            }
                            temp[length].stack_op1[1] = AL_res[i];
                        }
                        etat_actif = 4;
                        break;
                    case DELIMITER:
                        if (AL_res[i].value[0] == '(') {
                            stack->append(stack, NewStackNode("("));
                            etat_actif = 0;
                            break;
                        } else {
                            print_error("(", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                    case OPERATOR:
                        if (stack->head == stack->tail) {
                            print_error("character or → ", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (op_num != 1) {
                            print_error("nothing", "more than one transition expression");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        temp[length].stack_op1[1] = AL_res[i];
                        etat_actif = 2;
                        break;
                    default:
                        print_error(") character or → ", AL_res[i].value);
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
                break;
            case 4:  // CHARACTER
                switch (AL_res[i].type) {
                    case DELIMITER:
                        if (AL_res[i].value[0] != ')') {
                            print_error(")", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (stack->is_empty(stack)) {
                            print_error("nothing", ")");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (temp[length].stack_op1[0].value != NULL && temp[length].stack_op1[1].value == NULL) {
                            print_error(", and expression", ")");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        free(stack->pop(stack));
                        etat_actif = 5;
                        if (stack->is_empty(stack)) {
                            length++;
                            op_num = 1;
                        } else {
                            op_num++;
                        }
                        break;
                    case COMMA:
                        etat_actif = 3;
                        break;
                    default:
                        print_error(") or , ", AL_res[i].value);
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
                break;
            case 5:  // )
                switch (AL_res[i].type) {
                    case COMMA:
                        etat_actif = 3;
                        break;
                    case DELIMITER:
                        if (stack->is_empty(stack)) {
                            print_error("nothing", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (AL_res[i].value[0] != ')') {
                            print_error(")", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (temp[length].stack_op1[0].value != NULL && temp[length].stack_op1[1].value == NULL) {
                            print_error(", and expression", ")");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        free(stack->pop(stack));
                        etat_actif = 5;
                        if (stack->is_empty(stack)) {
                            length++;
                            op_num = 1;
                        }
                        break;
                    default:
                        print_error(") or , ", AL_res[i].value);
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
        }
    }
    if (etat_actif != 5 || !stack->is_empty(stack)) {
        print_error(")", AL_res[tail].value);
        stack->clean(stack);
        free(stack);
        free(transitions);
        return NULL;
    }
    stack->clean(stack);
    free(stack);
    transitions->length = length;
    transitions->container = malloc(sizeof(transition) * length);
    for (int i = 0; i < length; i++) transitions->container[i] = temp[i];
    return transitions;
}

transition_list *transitions_two_stack(lexeme *AL_res, int head, int tail) {
    transition_list *transitions = malloc(sizeof(transition_list));
    int length = 0;
    int etat_actif = 0;
    int op_num = 1;
    transition temp[MAX_TRANSITION_NUM];
    for (int i = 0; i < MAX_TRANSITION_NUM; i++) {
        temp[i].start = temp[i].end = 0;
        temp[i].character = NULL;
        temp[i].stack_op1[0].value = temp[i].stack_op2[0].value =
        temp[i].stack_op1[1].value = temp[i].stack_op2[1].value = NULL;
        temp[i].stack_op1[0].type = temp[i].stack_op2[0].type =
        temp[i].stack_op1[1].type = temp[i].stack_op2[1].type = DIGIT;
    }
    if (strcmp(AL_res[head].value, "(") != 0) {
        print_error("(", AL_res[head].value);
        free(transitions);
        return NULL;
    }
    if ((tail - head) == 0) {
        print_error("transition", "nothing");
        free(transitions);
        return NULL;
    }
    Stack *stack = NewStack();
    stack->append(stack, NewStackNode("("));
    for (int i = head + 1; i < tail; i++) {
        switch (etat_actif) {
            case 0:  // (
                switch (AL_res[i].type) {
                    case DIGIT:
                        temp[length].start = str2int(AL_res[i].value);
                        etat_actif = 1;
                        break;
                    case OPERATOR:
                        if (stack->head->next == stack->tail && AL_res[i].value[0] != '=') {
                            if (op_num == 1) temp[length].stack_op1[0] = AL_res[i];
                            else if (op_num == 2) temp[length].stack_op2[0] = AL_res[i];
                            else {
                                print_error("nothing", "more than two transition expressions");
                                stack->clean(stack);
                                free(stack);
                                free(transitions);
                                return NULL;
                            }
                            etat_actif = 2;
                            break;
                        } else {
                            print_error("a digit or →", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                    case CHARACTER:
                        if (op_num == 1) temp[length].stack_op1[0] = AL_res[i];
                        else if (op_num == 2) temp[length].stack_op2[0] = AL_res[i];
                        else {
                            print_error("nothing", "more than two transition expressions");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        etat_actif = 4;
                        break;
                    case DELIMITER:  // Pas d'action de pile
                        if (stack->is_empty(stack)) {
                            print_error("nothing", ")");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (stack->head == stack->tail) {
                            print_error("transition expression", "nothing");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        free(stack->pop(stack));
                        op_num++;
                        etat_actif = 5;
                        break;
                    default:
                        print_error("expression", AL_res[i].value);
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
                break;
            case 1:  // DIGIT
                switch (AL_res[i].type) {
                    case OPERATOR:
                        if (AL_res[i].value[0] != '=') etat_actif = 2;
                        break;
                    case COMMA:
                        etat_actif = 3;
                        break;
                    default:
                        print_error("→ or , ", AL_res[i].value);
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
                break;
            case 2:  // →
                switch (AL_res[i].type) {
                    case COMMA:
                        etat_actif = 3;
                        break;
                    case DELIMITER:
                        if (stack->head->next == stack->tail && !stack->is_empty(stack) && AL_res[i].value[0] == ')') {
                            if (op_num == 1 && temp[length].stack_op1[0].value != NULL &&
                                temp[length].stack_op1[1].value == NULL) {
                                print_error(", and expression", ")");
                                stack->clean(stack);
                                free(stack);
                                free(transitions);
                                return NULL;
                            }
                            if (op_num == 2 && temp[length].stack_op2[0].value != NULL &&
                                temp[length].stack_op2[1].value == NULL) {
                                print_error(", and expression", ")");
                                stack->clean(stack);
                                free(stack);
                                free(transitions);
                                return NULL;
                            }
                            free(stack->pop(stack));
                            op_num++;
                            etat_actif = 4;
                            break;
                        } else {
                            print_error("nothing", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                    case DIGIT:
                        temp[length].end = str2int(AL_res[i].value);
                        etat_actif = 1;
                        break;
                    default:
                        print_error("a digit or ) or , ", AL_res[i].value);
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
                break;
            case 3:  // ,
                switch (AL_res[i].type) {
                    case CHARACTER:
                        if (stack->head == stack->tail) {
                            temp[length].character = AL_res[i].value;
                        } else {
                            if (op_num == 1) temp[length].stack_op1[1] = AL_res[i];
                            else if (op_num == 2) temp[length].stack_op2[1] = AL_res[i];
                            else {
                                print_error("nothing", "more than two transition expressions");
                                stack->clean(stack);
                                free(stack);
                                free(transitions);
                                return NULL;
                            }
                        }
                        etat_actif = 4;
                        break;
                    case DELIMITER:
                        if (AL_res[i].value[0] == '(') {
                            stack->append(stack, NewStackNode("("));
                            etat_actif = 0;
                            break;
                        } else {
                            print_error("nothing", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                    case OPERATOR:
                        if (stack->head == stack->tail) {
                            print_error("character or →", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (op_num == 1) temp[length].stack_op1[1] = AL_res[i];
                        else if (op_num == 2) temp[length].stack_op2[1] = AL_res[i];
                        else {
                            print_error("nothing", "more than two transition expressions");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        etat_actif = 2;
                        break;
                    default:
                        print_error("( ) character or →", AL_res[i].value);
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
                break;
            case 4:  // CHARACTER
                switch (AL_res[i].type) {
                    case DELIMITER:
                        if (AL_res[i].value[0] != ')') {
                            print_error(")", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (stack->is_empty(stack)) {
                            print_error("nothing", ")");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (op_num == 1 && temp[length].stack_op1[0].value != NULL &&
                            temp[length].stack_op1[1].value == NULL) {
                            print_error(", and expression", ")");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (op_num == 2 && temp[length].stack_op2[0].value != NULL &&
                            temp[length].stack_op2[1].value == NULL) {
                            print_error(", and expression", ")");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        free(stack->pop(stack));
                        etat_actif = 5;
                        if (stack->is_empty(stack)) {
                            length++;
                            op_num = 1;
                        } else {
                            op_num++;
                        }
                        break;
                    case COMMA:
                        etat_actif = 3;
                        break;
                    default:
                        print_error(") or , ", AL_res[i].value);
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
                break;
            case 5:  // )
                switch (AL_res[i].type) {
                    case COMMA:
                        etat_actif = 3;
                        break;
                    case DELIMITER:
                        if (stack->is_empty(stack)) {
                            print_error("nothing", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (AL_res[i].value[0] != ')') {
                            print_error(")", AL_res[i].value);
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (op_num == 1 && temp[length].stack_op1[0].value != NULL &&
                            temp[length].stack_op1[1].value == NULL) {
                            print_error(", and expression", ")");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        if (op_num == 2 && temp[length].stack_op2[0].value != NULL &&
                            temp[length].stack_op2[1].value == NULL) {
                            print_error(", and expression", ")");
                            stack->clean(stack);
                            free(stack);
                            free(transitions);
                            return NULL;
                        }
                        free(stack->pop(stack));
                        etat_actif = 5;
                        if (stack->is_empty(stack)) {
                            length++;
                            op_num = 1;
                        }
                        break;
                    default:
                        print_error(") or , ", AL_res[i].value);
                        stack->clean(stack);
                        free(stack);
                        free(transitions);
                        return NULL;
                }
        }
    }
    if (etat_actif != 5 || !stack->is_empty(stack)) {
        print_error(")", AL_res[tail].value);
        stack->clean(stack);
        free(stack);
        free(transitions);
        return NULL;
    }
    stack->clean(stack);
    free(stack);
    transitions->length = length;
    transitions->container = malloc(sizeof(transition) * length);
    for (int i = 0; i < length; i++) {
        transitions->container[i] = temp[i];
    }
    return transitions;
}

transition_list *automate_transitions(lexeme *AL_res, int head, int tail, int stack_num) {
    transition_list *transitions;
    switch (stack_num) {
        case 0:
            transitions = transitions_zero_stack(AL_res, head, tail);
            break;
        case 1:
            transitions = transitions_one_stack(AL_res, head, tail);
            break;
        case 2:
            transitions = transitions_two_stack(AL_res, head, tail);
            break;
        default:
            return NULL;
    }
    return transitions;
}

analyse_syntaxique_resultat *analyse_syntaxique(lexeme *AL_res) {
    int cursor = 0;
    int etats_tail, final_tail, transitions_tail;
    analyse_syntaxique_resultat *res = malloc(sizeof(analyse_syntaxique_resultat));
    res->has_error = false;

    while (true) {
        if (strcmp(AL_res[cursor].value, "Automate") != 0) {
            print_error("Automate", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;  //1
        if (AL_res[cursor].value[0] != '(') {
            print_error("(", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;  //2
        if (AL_res[cursor].type != DIGIT) {
            print_error("a digit", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        res->stack_num = str2int(AL_res[cursor].value);
        if (res->stack_num > 2) {
            print_error("a number less or equal than 2", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;  //3
        if (AL_res[cursor].value[0] != ')') {
            print_error(")", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;  //4
        if (AL_res[cursor].value[0] != '=') {
            print_error("=", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;  //5
        if (AL_res[cursor].value[0] != '{') {
            print_error("{", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;  //6

        //extract etats
        if (strcmp(AL_res[cursor].value, "etats") != 0) {
            print_error("etats", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;  //7
        if (AL_res[cursor].value[0] != '=') {
            print_error("=", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;  //8
        if (AL_res[cursor].value[0] != '[') {
            print_error("[", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;  //9
        int temp = cursor;
        Stack *stack = NewStack();
        stack->append(stack, NewStackNode("["));
        while (!stack->is_empty(stack) && AL_res[cursor].value) {
            if (AL_res[cursor].value[0] == '[') {
                print_error("an etat", AL_res[cursor].value);
                res->has_error = true;
                while (!stack->is_empty(stack)) {
                    free(stack->pop(stack));
                }
                stack->clean(stack);
                free(stack);
                return res;
            } else if (AL_res[cursor].value[0] == ']') {
                free(stack->pop(stack));
            }
            cursor++;
        }
        if (!stack->is_empty(stack)) {
            print_error("]", "");
            res->has_error = true;
            while (!stack->is_empty(stack)) {
                free(stack->pop(stack));
            }
            stack->clean(stack);
            free(stack);
            break;
        }
        etats_tail = cursor - 1;
        res->etats = automate_etats(AL_res, temp, etats_tail);
        if (res->etats == NULL) {
            res->has_error = true;
            stack->clean(stack);
            free(stack);
            break;
        }

        //extract initial
        if (strcmp(AL_res[cursor].value, "initial") != 0) {
            print_error("initial", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;
        if (AL_res[cursor].value[0] != '=') {
            print_error("=", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;
        if (AL_res[cursor].type != DIGIT) {
            print_error("a digit", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        res->initial = str2int(AL_res[cursor].value);
        cursor++;

        //extract finals
        if (strcmp(AL_res[cursor].value, "final") != 0) {
            print_error("final", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;
        if (AL_res[cursor].value[0] != '=') {
            print_error("=", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;
        if (AL_res[cursor].value[0] != '[') {
            print_error("[", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;
        temp = cursor;
        stack->append(stack, NewStackNode("["));
        while (!stack->is_empty(stack) && AL_res[cursor].value) {
            if (AL_res[cursor].value[0] == '[') {
                print_error("a digit", AL_res[cursor].value);
                res->has_error = true;
                while (!stack->is_empty(stack)) {
                    free(stack->pop(stack));
                }
                stack->clean(stack);
                free(stack);
                return res;
            } else if (AL_res[cursor].value[0] == ']') {
                free(stack->pop(stack));
            }
            cursor++;
        }
        if (!stack->is_empty(stack)) {
            print_error("]", "");
            res->has_error = true;
            while (!stack->is_empty(stack)) {
                free(stack->pop(stack));
            }
            stack->clean(stack);
            free(stack);
            break;
        }
        final_tail = cursor - 1;
        res->finals = automate_finals(AL_res, temp, final_tail);
        if (res->finals == NULL) {
            res->has_error = true;
            stack->clean(stack);
            free(stack);
            break;
        }

        //extract transitions
        if (strcmp(AL_res[cursor].value, "transitions") != 0) {
            print_error("transitions", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;
        if (AL_res[cursor].value[0] != '=') {
            print_error("=", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;
        if (AL_res[cursor].value[0] != '[') {
            print_error("[", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;
        temp = cursor;
        stack->append(stack, NewStackNode("["));
        while (!stack->is_empty(stack) && AL_res[cursor].value) {
            if (AL_res[cursor].value[0] == '[') {
                print_error("transition expression", AL_res[cursor].value);
                res->has_error = true;
                while (!stack->is_empty(stack)) {
                    free(stack->pop(stack));
                }
                stack->clean(stack);
                free(stack);
                return res;
            } else if (AL_res[cursor].value[0] == ']') {
                free(stack->pop(stack));
            }
            cursor++;
        }
        if (!stack->is_empty(stack)) {
            print_error("]", "");
            res->has_error = true;
            while (!stack->is_empty(stack)) {
                free(stack->pop(stack));
            }
            stack->clean(stack);
            free(stack);
            break;
        }
        transitions_tail = cursor - 1;
        res->transitions = automate_transitions(AL_res, temp, transitions_tail, res->stack_num);
        if (res->transitions == NULL) {
            res->has_error = true;
            stack->clean(stack);
            free(stack);
            break;
        }

        if (AL_res[cursor].value[0] != '}') {
            print_error("}", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        cursor++;
        if (AL_res[cursor].value) {
            print_error("no expression", AL_res[cursor].value);
            res->has_error = true;
            break;
        }
        stack->clean(stack);
        free(stack);
        break;
    }
    return res;
}
