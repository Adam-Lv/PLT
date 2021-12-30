cmake_minimum_required(VERSION 3.20)
project(PLT C)

set(CMAKE_C_STANDARD 11)

add_executable(runtime runtime.c)
#add_executable(AL main/analyse_lexicale.c main/analyse_lexicale.h)
add_executable(ASy main/analyse_syntaxique.c)
add_executable(ASe main/analyse_semantique.c)
add_executable(Compile main/compile_automate.c)
add_executable(Test_AL test/test_AL.c)
add_executable(Test_ASy test/test_ASy.c)
add_executable(Test_ASe test/test_ASe.c)
add_executable(Test_compil test/test_compil.c)
