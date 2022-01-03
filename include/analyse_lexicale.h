#ifndef PLT_ANALYSE_LEXICALE_H
#define PLT_ANALYSE_LEXICALE_H

#include <regex.h>

#define bool int
#define true 1
#define false 0
#define MAX_ETAT_NUM 10
#define MAX_TRANSITION_NUM 30

/**
 * enum type of a lexeme
 * */
typedef enum {
    DIGIT,        // [0-9]+
    OPERATOR,     // =, →
    CHARACTER,    // `.+`
    KEYWORD,      // Automate, etats, initial, final, transitions
    DELIMITER,    // ()[]{}
    STRING,       // ".*"
    COMMA         // ,
} Type;

extern char *enum_name[];

/**
 * Data structure of a lexeme
 * */
typedef struct {
    Type type;
    char *value;
} lexeme;

/**
 * The struct to put the result. If there is has_error, the filed error_char will not be '\0'
 * */
typedef struct analysis_lexical_result {
    lexeme *res;
    char error_char;
} analysis_lexical_result;

analysis_lexical_result *analyse_lexicale(char *in_file);

#endif //PLT_ANALYSE_LEXICALE_H
