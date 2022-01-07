CC := gcc -Wall
BIN := bin/
SOURCE := source/
TEST := test/

AL_OBJ = $(BIN)analyse_lexicale.o
ASy_OBJ = $(AL_OBJ) $(BIN)analyse_syntaxique.o
ASe_OBJ = $(ASy_OBJ) $(BIN)analyse_semantique.o
C_OBJ = $(ASe_OBJ) $(BIN)Compilation.o
Com_OBJ = $(C_OBJ) $(BIN)Compilateur.o
Exe_OBJ = Executeur.c

TAL_OBJ = $(AL_OBJ) $(BIN)test_AL.o
TASy_OBJ = $(ASy_OBJ) $(BIN)test_ASy.o
TASe_OBJ = $(ASe_OBJ) $(BIN)test_ASe.o
TC_OBJ = $(C_OBJ) $(BIN)test_compil.o

all:Test_AL Test_ASy Test_ASe Test_Com Compile Executeur
.PHONY:

Test_AL:$(TAL_OBJ)
	$(CC) $(TAL_OBJ) -o $(BIN)Test_AL

Test_ASy:$(TASy_OBJ)
	$(CC) $(TASy_OBJ) -o $(BIN)Test_ASy

Test_ASe:$(TASe_OBJ)
	$(CC) $(TASe_OBJ) -o $(BIN)Test_ASe

Test_Com:$(TC_OBJ)
	$(CC) $(TC_OBJ) -o $(BIN)Test_Com

Compile:$(Com_OBJ)
	$(CC) $(Com_OBJ) -o $(BIN)Compile

Executeur:$(Exe_OBJ)
	$(CC) $(Exe_OBJ) -o $(BIN)Executeur

$(BIN)%.o:$(SOURCE)%.c
	$(CC) -c $^ -o $@

$(BIN)%.o:$(TEST)%.c
	$(CC) -c $^ -o $@

$(BIN)%.o:%.c
	$(CC) -c $^ -o $@

clean:
	rm $(BIN)*
	rm output/*
