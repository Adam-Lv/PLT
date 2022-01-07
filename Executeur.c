#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/analyse_syntaxique.h"

#define MAX_VM_LENGTH 128
#define MAX_INPUT_LENGTH 1024
#define MAX_ETAT_NAME_LENGTH 32
#define MAX_STACK_DEEP 512

int VM[MAX_VM_LENGTH];
char *Vide = "Vide";

typedef struct {
    char name[MAX_ETAT_NAME_LENGTH];
    int address;
} etat;

typedef struct {
    int etat_num;
    etat *etats;
} TS;

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

int str2int(const char *str) {
    int res = 0;
    int sign = 1;
    int i = 0;
    if (str[0] == '-') {
        sign = -1;
        i = 1;
    }
    for (; str[i]; i++) {
        if (res == 0 && str[i] == '0') continue;
        res = 10 * res + (str[i] - '0');
    }
    return res * sign;
}

int readVM(char *file_name) {
    char *path = "../output/";
    size_t length = strlen(path) + strlen(file_name) + 1;
    char *in_file = malloc(sizeof(char) * length);
    strcat(in_file, path);
    strcat(in_file, file_name);

    FILE *VM_file = fopen(in_file, "r");
    if (VM_file == NULL) {
        printf("No such file or directory: %s\n", file_name);
        free(in_file);
        exit(1);
    }
    free(in_file);
    char strInt[4] = {'\0'};
    int pos = 0;

    int c, VM_length = 0;
    while ((c = fgetc(VM_file)) != '\n');
    while ((c = fgetc(VM_file)) != '\n') {
        if ((c >= 48 && c <= 57) || c == '-') {
            strInt[pos++] = (char) c;
        } else {
            if (pos != 0) {
                VM[VM_length++] = str2int(strInt);
                strInt[0] = strInt[1] = strInt[2] = strInt[3] = '\0';
                pos = 0;
            }
        }
    }
    return VM_length;
}

TS readTS(char *file_name) {
    char *path = "../output/";
    size_t length = strlen(path) + strlen(file_name) + 1;
    char *in_file = malloc(sizeof(char) * length);
    strcat(in_file, path);
    strcat(in_file, file_name);

    FILE *TS_file = fopen(in_file, "r");
    if (TS_file == NULL) {
        printf("No such file or directory: %s\n", file_name);
        free(in_file);
        exit(1);
    }
    free(in_file);
    char strInt[4] = {'\0'};
    int pos = 0;

    etat temp[MAX_ETAT_NUM];
    for (int i = 0; i < MAX_ETAT_NUM; i++) {
        temp[i].address = 0;
        for (int j = 0; j < MAX_ETAT_NAME_LENGTH; j++) temp[i].name[j] = '\0';
    }

    int c, etat_num = 0, flag = 0;  // operate name if flag = 0, operate address if flag = 1
    while ((c = fgetc(TS_file)) != '\n');
    while ((c = fgetc(TS_file)) != EOF) {
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
            if (flag == 1) strInt[pos++] = (char) c;
            else temp[etat_num].name[pos++] = (char) c;
        } else {
            if (pos != 0) {
                if (flag == 1) {
                    temp[etat_num++].address = str2int(strInt);
                    strInt[0] = strInt[1] = strInt[2] = strInt[3] = '\0';
                }
                pos = 0;
                flag = !flag;
            }
        }
    }
    etat *etats = malloc(sizeof(etat) * etat_num);
    for (int i = 0; i < etat_num; i++) {
        etats[i].address = temp[i].address;
        for (int j = 0; j < MAX_ETAT_NAME_LENGTH; j++) etats[i].name[j] = temp[i].name[j];
    }
    TS res = {etat_num, etats};
    return res;
}

char *get_name(TS TS_struct, int address) {
    for (int i = 0; i < TS_struct.etat_num; i++) {
        if (TS_struct.etats[i].address == address) return TS_struct.etats[i].name;
    }
    return NULL;
}

char *stack_ele(Stack *stack) {
    if (stack->is_empty(stack)) return Vide;
    char *res = malloc(sizeof(char) * (MAX_STACK_DEEP + 1));
    StackNode *curr = stack->head;
    int pos = 0;
    res[pos++] = curr->content[0];
    while (curr->next != NULL && pos < MAX_STACK_DEEP) {
        curr = curr->next;
        res[pos++] = curr->content[0];
    }
    res[pos] = '\0';
    return res;
}

void execution(int VM_length, TS TS_struct, bool debug, char *input_str, int length) {
    int stack_num = VM[0];
    int initial_addr = VM[1];
    int transition_offset = 2 * (stack_num + 1);
    int finals_num = VM[2];
    int curr_etat_addr = initial_addr;
    int curr_etat_out_num = VM[initial_addr];
    int curr_trans_start_addr = 0;
    char input;
    char *etat_name, *stack1_ele, *stack2_ele;
    Stack *stack1 = NewStack();
    Stack *stack2 = NewStack();
    char *format = "%c -> Etat : %-6s Pile 1 : %-6s Pile 2 : %-6s\n";
    char *refuse_format = "Le mot %s est refusé !\n";
    if (debug) printf(format, ' ', get_name(TS_struct, initial_addr), Vide, Vide);
    for (int i = 0; i < length; i++) {
        input = -1;  // input can be used to judge whether there is next etat.
        for (int j = 0; j < curr_etat_out_num; j++) {
            curr_trans_start_addr = curr_etat_addr + 1 + j * transition_offset;
            // find the transition which has the same character
            if (VM[curr_trans_start_addr] != input_str[i]) continue;

            input = input_str[i];
            curr_etat_addr = VM[curr_trans_start_addr + 1];
            etat_name = get_name(TS_struct, curr_etat_addr);
            // Handle stack-related operations correctly
            if (stack_num == 0) {
                stack1_ele = stack2_ele = Vide;
            } else {
                if (stack_num >= 1) {
                    if (VM[curr_trans_start_addr + 3] == 1) {
                        char *ele = malloc(sizeof(char));
                        ele[0] = (char) VM[curr_trans_start_addr + 2];
                        stack1->append(stack1, NewStackNode(ele));
                    } else if (VM[curr_trans_start_addr + 3] == -1) {
                        if (stack1->is_empty(stack1)) {
                            if (debug) printf("%c -> Erreur : Pile 1 vide !\n", input);
                            printf(refuse_format, input_str);
                            stack1->clean(stack1);
                            stack2->clean(stack2);
                            exit(1);
                        }
                        free(stack1->pop(stack1));
                    }
                    stack1_ele = stack_ele(stack1);
                }
                if (stack_num == 2) {
                    if (VM[curr_trans_start_addr + 5] == 1) {
                        char *ele = malloc(sizeof(char));
                        ele[0] = (char) VM[curr_trans_start_addr + 4];
                        stack2->append(stack2, NewStackNode(ele));
                    } else if (VM[curr_trans_start_addr + 5] == -1) {
                        if (stack2->is_empty(stack2)) {
                            if (debug) printf("%c -> Erreur : Pile 2 vide !\n", input);
                            printf(refuse_format, input_str);
                            stack1->clean(stack1);
                            stack2->clean(stack2);
                            exit(1);
                        }
                        free(stack2->pop(stack2));
                    }
                    stack2_ele = stack_ele(stack2);
                } else {
                    stack2_ele = Vide;
                }
            }
            break;
        }
        if (input == -1) {
            if (debug)
                printf("%c -> Erreur : pas de transition correspondante issue de cet état.\n", input_str[i]);
            printf(refuse_format, input_str);
            stack1->clean(stack1);
            stack2->clean(stack2);
            exit(1);
        }
        if (debug) printf(format, input, etat_name, stack1_ele, stack2_ele);
        if (stack1_ele != Vide) free(stack1_ele);
        if (stack2_ele != Vide) free(stack2_ele);
    }
    int stack_bit = (stack1->is_empty(stack1) << 1) + stack2->is_empty(stack2);
    while (true) {
        if (stack_bit == 3) break;
        else {
            while (true) {
                if (stack_bit == 0) {
                    if (debug)
                        printf("Le mot %s est refusé ! Pile 1 et Pile 2 non vides.\n", input_str);
                    else
                        printf(refuse_format, input_str);
                    break;
                } else if (stack_bit == 1) {
                    if (debug)
                        printf("Le mot %s est refusé ! Pile 1 non vide.\n", input_str);
                    else
                        printf(refuse_format, input_str);
                    break;
                } else {
                    if (debug)
                        printf("Le mot %s est refusé ! Pile 2 non vide.\n", input_str);
                    else
                        printf(refuse_format, input_str);
                    break;
                }
            }
        }
        stack1->clean(stack1);
        stack2->clean(stack2);
        exit(1);
    }
    bool at_final = false;
    for (int i = 0; i < finals_num; i++) {
        if (curr_etat_addr == VM[3 + i]) {
            at_final = true;
            break;
        }
    }
    if (!at_final) {
        if (debug)
            printf("Le mot %s est refusé ! Dernier état n'est pas état final.\n", input_str);
        else
            printf(refuse_format, input_str);
        stack1->clean(stack1);
        stack2->clean(stack2);
        exit(1);
    }
    stack1->clean(stack1);
    stack2->clean(stack2);
    printf("Le mot %s est accepté !\n", input_str);
}

int main(int argc, char *argv[]) {
    bool debug;
    char *VM_file_name, *TS_file_name;
    switch (argc) {
        case 3:
            debug = false;
            VM_file_name = argv[1];
            TS_file_name = argv[2];
            break;
        case 4:
            if (strcmp(argv[1], "-debug") != 0) {
                printf("Invalid input arguments.\n");
                return 1;
            }
            debug = true;
            VM_file_name = argv[2];
            TS_file_name = argv[3];
            break;
        default:
            printf("Invalid input arguments.\n");
            return 1;
    }
    int VM_length = readVM(VM_file_name);
    TS TS_struct = readTS(TS_file_name);

    char input_str[MAX_INPUT_LENGTH] = {'\0'};
    int length;
    printf("Donner le mot d'entrée : ");
    scanf("%1023s%n", input_str, &length);
    if (getchar() != '\n') {
        printf("Caractère trop longue.\n");
        return 1;
    }
    execution(VM_length, TS_struct, debug, input_str, length);
    return 0;
}
