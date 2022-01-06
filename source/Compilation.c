#include "../include/Compilation.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Compilation_result *generate_VM_and_TS(analyse_syntaxique_resultat *ASy_res) {
    int etat_num = ASy_res->etats->length;
    int final_num = ASy_res->finals->length;
    int transition_num = ASy_res->transitions->length;
    int transition_offset = 2 * (ASy_res->stack_num + 1);

    int VM_length = 3 + final_num + etat_num + transition_num * transition_offset;
    int etat_start_addr = 3 + final_num;

    etat etatList[etat_num];
    for (int i = 0; i < etat_num; i++) {
        etatList[i].out_transition_num = 0;
        etatList[i].address = 0;
        etatList[i].name = ASy_res->etats->container[i].value;
    }

    // Connect etats
    for (int i = 0; i < transition_num; i++) {
        transition curr = ASy_res->transitions->container[i];
        int *curr_tran_num = &etatList[curr.start].out_transition_num;
        etatList[curr.start].end_etat_list[*curr_tran_num] = curr.end;
        etatList[curr.start].char_list[*curr_tran_num] = (int) curr.character[1];
        if (ASy_res->stack_num >= 1) {
            switch (curr.stack_op1[0].type) {
                case DIGIT:
                    etatList[curr.start].stack1_char_list[*curr_tran_num] = 0;
                    etatList[curr.start].stack1_op_list[*curr_tran_num] = 0;
                    break;
                case OPERATOR:
                    etatList[curr.start].stack1_char_list[*curr_tran_num] = (int) curr.stack_op1[1].value[1];
                    etatList[curr.start].stack1_op_list[*curr_tran_num] = 1;
                    break;
                case CHARACTER:
                    etatList[curr.start].stack1_char_list[*curr_tran_num] = (int) curr.stack_op1[0].value[1];
                    etatList[curr.start].stack1_op_list[*curr_tran_num] = -1;
                    break;
                default:
                    perror("There is syntax error!");
            }
        }
        if (ASy_res->stack_num == 2) {
            switch (curr.stack_op2[0].type) {
                case DIGIT:
                    etatList[curr.start].stack2_char_list[*curr_tran_num] = 0;
                    etatList[curr.start].stack2_op_list[*curr_tran_num] = 0;
                    break;
                case OPERATOR:
                    etatList[curr.start].stack2_char_list[*curr_tran_num] = (int) curr.stack_op2[1].value[1];
                    etatList[curr.start].stack2_op_list[*curr_tran_num] = 1;
                    break;
                case CHARACTER:
                    etatList[curr.start].stack2_char_list[*curr_tran_num] = (int) curr.stack_op2[0].value[1];
                    etatList[curr.start].stack2_op_list[*curr_tran_num] = -1;
                    break;
                default:
                    perror("There is syntax error!");
            }
        }
        (*curr_tran_num)++;
    }

    // Find address
    for (int i = 0; i < etat_num; i++) {
        if (i == 0) etatList[i].address = etat_start_addr;
        else etatList[i].address = etatList[i - 1].address + transition_offset * etatList[i - 1].out_transition_num + 1;
    }
    int initial_addr = etatList[ASy_res->initial].address;

    // Fill the VM
    int *VM = malloc(sizeof(int) * VM_length);
    VM[0] = ASy_res->stack_num;
    VM[1] = initial_addr;
    VM[2] = final_num;
    for (int i = 0; i < final_num; i++) VM[i + 3] = etatList[ASy_res->finals->container[i]].address;
    int VM_cursor = etat_start_addr;
    while (VM_cursor < VM_length) {
        for (int i = 0; i < etat_num; i++) {
            VM[VM_cursor++] = etatList[i].out_transition_num;
            for (int j = 0; j < etatList[i].out_transition_num; j++) {
                VM[VM_cursor++] = etatList[i].char_list[j];
                VM[VM_cursor++] = etatList[etatList[i].end_etat_list[j]].address;

                if (VM[0] >= 1) {
                    VM[VM_cursor++] = etatList[i].stack1_char_list[j];
                    VM[VM_cursor++] = etatList[i].stack1_op_list[j];
                }
                if (VM[0] == 2) {
                    VM[VM_cursor++] = etatList[i].stack2_char_list[j];
                    VM[VM_cursor++] = etatList[i].stack2_op_list[j];
                }
            }
        }
    }

    // Generate TS struct
    TS_etat *TS = malloc(sizeof(TS_etat) * etat_num);
    for (int i = 0; i < etat_num; i++) {
        TS[i].address = etatList[i].address;
        TS[i].etat_name = etatList[i].name;
        size_t name_length = strlen(TS[i].etat_name);
        TS[i].etat_name[name_length - 1] = '\0';
        TS[i].etat_name += sizeof(char);
    }
    Compilation_result *res = malloc(sizeof(Compilation_result));
    res->VM_length = VM_length;
    res->VM = VM;
    res->etat_num = etat_num;
    res->TS = TS;

    // write into files
    FILE *TS_file;
    TS_file = fopen("../output/TS.txt", "w");
    fprintf(TS_file, "état\taddress\n");
    for (int i = 0; i < res->etat_num; i++)
        fprintf(TS_file, " %s\t\t%2d\n", res->TS[i].etat_name, res->TS[i].address);
    fclose(TS_file);

    FILE *VM_file = fopen("../output/VM.txt", "w");
    for (int i = 0; i < res->VM_length; i++)
        fprintf(VM_file, "%2d ", i);
    fprintf(VM_file, "\n");
    for (int i = 0; i < res->VM_length; i++)
        fprintf(VM_file, "%2d ", res->VM[i]);
    fprintf(VM_file, "\n");
    fclose(VM_file);

    return res;
}