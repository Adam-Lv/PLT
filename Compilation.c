#include "include/Compilation.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int compile(char *file_name) {
    char *path = "../resources/";
    size_t length = strlen(path) + strlen(file_name) + 1;
    char *in_file = malloc(sizeof(char) * length);
    strcat(in_file, path);
    strcat(in_file, file_name);

    // analyse lexicale
    analyse_lexicale_resultat *AL_res = analyse_lexicale(in_file);
    if (AL_res->error_char != '\0') {
        printf("Lexical analysis has error! Unknown character: %c\n", AL_res->error_char);
        return 1;
    }
    printf("Lexical analysis completed. No error.\n");
    free(in_file);

    // analyse syntaxique
    analyse_syntaxique_resultat *ASy_res = analyse_syntaxique(AL_res->res);
    if (ASy_res->has_error) return 1;
    printf("Syntactic analysis completed. No error.\n");

    // analyse semantique
    int ASe_res = analyse_semantique(ASy_res);
    if (!ASe_res) return 1;
    printf("Semantic analysis completed. No error.\n");

    // compilation
    free(generate_VM_and_TS(ASy_res));
    free(AL_res->res);
    free(AL_res);
    free(ASy_res->etats);
    free(ASy_res->finals);
    free(ASy_res->transitions->container);
    free(ASy_res);

    printf("Compilation completed. VM.txt and TS.txt are written in directory output.\n");
    return 0;
}

int main(int argc, char *argv[]) {
    char file_name[256];
    if (argc == 1) strcpy(file_name, "Zpile.txt");
    else if (argc == 2) strcpy(file_name, argv[1]);
    return compile(file_name);
}
