#include "../include/analyse_lexicale.h"
#include "../include/analyse_syntaxique.h"
#include <stdio.h>

int main() {
    analysis_lexical_result *AL_res = analyse_lexicale("../resources/Dpile.txt");
    if (AL_res->error_char != '\0') {
        printf("Lexical analysis has print_error! Unknown character: %c\n", AL_res->error_char);
        return 1;
    }
    analyse_syntaxique_result *ASy_res = analyse_syntaxique(AL_res->res);
    if (ASy_res->has_error) return 1;
    printf("Syntax analysis has no print_error. Here is the results\n");
    printf("stack numbers: %d\n", ASy_res->stack_num);
    printf("etats: ");
    for (int i = 0; i < ASy_res->etats->length - 1; i++)
        printf("%s, ", ASy_res->etats->container[i].value);
    printf("%s\n", ASy_res->etats->container[ASy_res->etats->length - 1].value);
    printf("initial: %d\n", ASy_res->initial);
    printf("finals: ");
    for (int i = 0; i < ASy_res->finals->length - 1; i++)
        printf("%d, ", ASy_res->finals->container[i]);
    printf("%d\n", ASy_res->finals->container[ASy_res->finals->length - 1]);
    printf("transitions:\n");
    for (int i = 0; i < ASy_res->transitions->length; i++) {
        printf("\ttransition %d:\n\t\tstart etat: %d\n"
               "\t\tend etat: %d\n"
               "\t\tcharacter: %s\n",
               i + 1,
               ASy_res->transitions->container[i].start,
               ASy_res->transitions->container[i].end,
               ASy_res->transitions->container[i].character);
        if (ASy_res->transitions->container[i].stack_op1[0].value) {
            printf("\t\tstack 1 operation: %s %s\n",
                   ASy_res->transitions->container[i].stack_op1[0].value,
                   ASy_res->transitions->container[i].stack_op1[1].value);
        } else printf("\t\tNo operation on stack 1\n");
        if (ASy_res->transitions->container[i].stack_op2[0].value) {
            printf("\t\tstack 2 operation: %s %s\n",
                   ASy_res->transitions->container[i].stack_op2[0].value,
                   ASy_res->transitions->container[i].stack_op2[1].value);
        } else printf("\t\tNo operation on stack 2\n");
    }
    return 0;
}