#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/analyse_lexicale.h"

char *enum_name[7] = {"DIGIT", "OPERATOR", "CHARACTER", "KEYWORD", "DELIMITER", "STRING", "COMMA"};

int test_analyse_lexicale(char *file_name) {
    char *path = "../resources/";
    int length = strlen(path) + strlen(file_name) + 1;
    char *in_file = malloc(sizeof(char) * length);
    strcat(in_file, path);
    strcat(in_file, file_name);
    analyse_lexicale_resultat *AL_res = analyse_lexicale(in_file);
    if (AL_res->error_char != '\0') {
        printf("Lexical analysis has error! Unknown character: %c\n", AL_res->error_char);
        return 1;
    }
    lexeme *res = AL_res->res;
    printf("No lexical has_error. Lexical analysis result:\n");
    printf("------------------------------\n");
    for (int i = 0; res[i].value; i++) {
        printf("%03d | %-9s | %s\n", i + 1, enum_name[res[i].type], res[i].value);
    }
    printf("------------------------------\n");
    return 0;
}

int main() {
    return test_analyse_lexicale("Upile.txt");
}