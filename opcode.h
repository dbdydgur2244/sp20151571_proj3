#ifndef __OPCODE__
#define __OPCODE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structure.h"

void opcode_init();
Hnode opcode_find(char *); // OPTAB에서 찾는 함수
Hnode nopcode_find(int );
void get_opcode(); // opcode find funct ion
void opcode_insert(int value, char *mnemonic, char *code); // hash에 insert하는 함수
Hash * get_hashTable();
void command_opcodelist(); // opcodelist를 출력하는 함수

int command_opcode(char *mnemonic); // opcode인 경우 처리해주는 함수



#endif
