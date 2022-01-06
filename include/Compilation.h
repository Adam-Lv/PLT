#ifndef PLT_COMPILATION_H
#define PLT_COMPILATION_H

#include "../include/analyse_semantique.h"

typedef struct {
    int address;
    char *name;
    int out_transition_num;
    int end_etat_list[MAX_ETAT_NUM];
    int char_list[MAX_ETAT_NUM];
    int stack1_op_list[MAX_ETAT_NUM];
    int stack1_char_list[MAX_ETAT_NUM];
    int stack2_op_list[MAX_ETAT_NUM];
    int stack2_char_list[MAX_ETAT_NUM];
} etat;

typedef struct {
    char *etat_name;
    int address;
} TS_etat;

typedef struct {
    int VM_length;
    int *VM;
    int etat_num;
    TS_etat *TS;
} Compilation_result;

Compilation_result *generate_VM_and_TS(analyse_syntaxique_resultat *ASy_res);

#endif //PLT_COMPILATION_H
