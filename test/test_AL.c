#include <stdio.h>
#include "../main/analyse_lexicale.c"

int main() {
    lexeme *AL_res = analyse_lexicale("../resources/Upile.txt");
    for (int i = 0; AL_res[i].value; i++) {
        printf("%d: Type %d, content %s\n", i + 1, AL_res[i].type, AL_res[i].value);
    }
    /*lexeme a, b;
    a.value = "asdf";
    a.type = DIGIT;
    b = a;*/
    /*FILE *file = fopen("../resources/test.txt", "r");
    int c, length = 0;
    unsigned char buffer[64];
    while ((c = fgetc(file)) != EOF) {
        buffer[length] = (char) c;
        length++;
    }
    buffer[length] = '\0';
    printf("%s\n", buffer);
    for (int i = 0; i < length; i++) putchar(buffer[i]);
    for (int i = 0; i < length; i++) printf("%d ", buffer[i]);*/
}