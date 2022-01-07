#include <stdio.h>
#include "../include/analyse_semantique.h"

int main() {
    analyse_lexicale_resultat *AL_res = analyse_lexicale("../resources/Dpile.txt");
    analyse_syntaxique_resultat *ASy_res = analyse_syntaxique(AL_res->res);
    if (analyse_semantique(ASy_res)) {
        printf("No semantic error!\n");
        return 0;
    }
    return 1;
}
