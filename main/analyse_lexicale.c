#include <stdio.h>
#include <stdlib.h>
#include <regex.h>


#define MAX_LINE_NUM 20
#define MAX_CHAR_NUM 100
#define MAX_MOT_NUM 1024

extern char operatorSet[2][4] = {"=", "→"};  // 226 134 146 = →
extern char keyWordSet[5][11] = {"Automate", "etats", "initial", "final", "transition"};
extern char delimiterSet[6] = {'(', ')', '[', ']', '{', '}'};

typedef enum lexeme_t {
    DIGIT,  // [0-9]+
    OPERATOR,  // =, →
    CHARACTER,  // `.+`
    KEYWORD,  // Automate, etats, initial, final, transition
    DELIMITER, // ()[]{}
    STRING  // ".*"
} lexeme_t;

typedef struct lexeme {
    lexeme_t type;
    char *value;
} lexeme;

typedef struct ListInt {
    int length;
    int *container;
} ListInt;

typedef struct match_result {
    int count;
    int *heads;
    int *tails;
}match_result;

ListInt *eliminate_comments(FILE *file) {
    ListInt *buffer = malloc(sizeof(ListInt));
    buffer->length = MAX_MOT_NUM * MAX_CHAR_NUM;
    buffer->container = malloc(sizeof(int) * MAX_CHAR_NUM * MAX_MOT_NUM);
    int c_previos = fgetc(file);
    int c;
    int c_num = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c_previos == '/') {
            switch (c) {
                case '/':
                    while ((c_previos = fgetc(file)) != EOF) {
                        if (c_previos == '\n') {
                            break;
                        }
                    }
                    if (c_previos == EOF) {
                        buffer->container[c_num++] = '\n';
                        break;
                    }
                    break;
                case '*':
                    c_previos = fgetc(file);
                    while ((c = fgetc(file)) != EOF) {
                        if (c_previos == '*' && c == '/') {
                            c_previos = '\n';
                            break;
                        } else
                            c_previos = c;
                    }
                    break;
                default:
                    buffer->container[c_num++] = c_previos;
                    c_previos = c;
            }
        } else {
            buffer->container[c_num++] = c_previos;
            c_previos = c;
        }
    }
    buffer->container[c_num++] = c_previos;
    buffer->length = c_num;
//    for (int i = 0; i < c_num; i++) { putchar(buffer->container[i]); }
    return buffer;
}

void regular_expression_match(ListInt *buffer, regex_t regexpr) {
    int res, head, tail;
    regmatch_t pmatch[1];
    do {
        res = regexec(&regexpr, (char *)buffer->container, 1, pmatch, 0);
        if (res) break;
        head = pmatch[0].rm_so;
        tail = pmatch[0].rm_eo;

    }while(1);
}

void analyse_lexicale(char *in_file) {
    FILE *file = fopen(in_file, "r");

    // Eliminate the comments
    ListInt *buffer = eliminate_comments(file);


    // Divide the buffer according to lines
    /*int c;
    int **lines = (int **) malloc(sizeof(int *) * line_num * MAX_CHAR_NUM);
    for (int i = 0; i < line_num; i++) lines[i] = (int *) malloc(sizeof(int *) * MAX_CHAR_NUM);
    for (int i = 0, temp_c = 0, temp_l = 0; i < c_num; i++) {
        c = buffer->container[i];
        if (c != '\n') {
            lines[temp_l][temp_c++] = c;
        } else {
            lines[temp_l++][temp_c] = '\0';
            temp_c = 0;
        }
    }*/

    // Find out the information by regular expressions
    regex_t reg_digit, reg_operator, reg_character, reg_keyword, reg_dilimiter, reg_string;
    char *regexp_digit = "[0-9]+";
    char *regexp_operator = "(=|→)";
    char *regexp_character = "`.+`";
    char *regexp_keyword = "(Automate|etats|initial|final|transition)";
    char *regexp_dilimiter = "(\(|\)|\[|\]|\{|\})";
    char *regexp_string = "\".+\"";
    regcomp(&reg_digit, regexp_digit, REG_EXTENDED);
    regcomp(&reg_operator, regexp_operator, REG_EXTENDED);
    regcomp(&reg_character, regexp_character, REG_EXTENDED);
    regcomp(&reg_keyword, regexp_keyword, REG_EXTENDED);
    regcomp(&reg_dilimiter, regexp_dilimiter, REG_EXTENDED);
    regcomp(&reg_string, regexp_string, REG_EXTENDED);

    free(buffer->container);
    free(buffer);
}

/*int **analyse_lexicale(char *in_file) {
    FILE *file = fopen(in_file, "r");
    //Divided by lines.
    int **lines = (int **) malloc(sizeof(int *) * MAX_LINE_NUM * MAX_CHAR_NUM);
    for (int i = 0; i < MAX_LINE_NUM; i++) lines[i] = (int *) malloc(sizeof(int *) * MAX_CHAR_NUM);
    int c;
    int line_num = 0;
    int c_num = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c != '\n') {
            lines[line_num][c_num++] = c;
        } else {
            lines[line_num++][c_num] = '\0';
            c_num = 0;
        }
        if (c>128) {
            putchar(c);
            printf("%d\n", c);
        }
    }
    return lines;
}*/
