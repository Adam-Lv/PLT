#include "../include/Compilation.h"
#include <string.h>

int main(int argc, char *argv[]) {
    char file_name[256];
    if (argc == 1) strcpy(file_name, "../resources/Zpile.txt");
    else if (argc == 2) strcpy(file_name, argv[1]);
    analyse_lexicale_resultat *AL_res = analyse_lexicale(file_name);
    if (AL_res->error_char) return 1;
    analyse_syntaxique_resultat *ASy_res = analyse_syntaxique(AL_res->res);
    if (ASy_res->has_error) return 1;
    if (!analyse_semantique(ASy_res)) return 1;
    generate_VM_and_TS(ASy_res);
    return 0;
}