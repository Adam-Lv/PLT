#include <stdio.h>
#include "../main/analyse_lexicale.c"

int main() {
    lexeme *AL_res = analyse_lexicale("../resources/Upile.txt");
    for (int i = 0; AL_res[i].value; i++) {
        printf("%d: Type %d, content %s\n", i + 1, AL_res[i].type, AL_res[i].value);
    }
}