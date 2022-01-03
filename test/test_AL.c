#include <stdio.h>
#include "../include/analyse_lexicale.h"

char *enum_name[7] = {"DIGIT", "OPERATOR", "CHARACTER", "KEYWORD", "DELIMITER", "STRING", "COMMA"};

int main() {
    analysis_lexical_result *AL_res = analyse_lexicale("../resources/Dpile.txt");
    if (AL_res->error_char != '\0') {
        printf("Lexical has_error! Unknown character: %c\n", AL_res->error_char);
        return 1;
    }
    lexeme *res = AL_res->res;
    printf("No lexical has_error. Lexical analysis result:\n");
    for (int i = 0; res[i].value; i++) {
        printf("%2d: Type %10s, content %s\n", i + 1, enum_name[res[i].type], res[i].value);
    }
    return 0;
}