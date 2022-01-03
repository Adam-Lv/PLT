cmake_minimum_required(VERSION 3.20)
project(PLT C)

set(CMAKE_C_STANDARD 99)

add_executable(Exec Executeur.c)
add_executable(Compile source/Compilateur.c)
add_executable(Test_AL test/test_AL.c source/analyse_lexicale.c)
add_executable(Test_ASy test/test_ASy.c source/analyse_syntaxique.c source/analyse_lexicale.c)
add_executable(Test_ASe test/test_ASe.c source/analyse_semantique.c include/analyse_semantique.h)
add_executable(Test_compil test/test_compil.c)
