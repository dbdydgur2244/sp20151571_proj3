#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void memory_init();
void set_memory(int address, int value);
int get_memory(int address);
int print_memory( int start, int end); // memory를 start부터 end까지 출력하는 함수
int command_dump( char *bufffer, int *arr); // 입력한 명령어가 dump인 경우 처리해주는 함수
int command_edit( char *buffer, int *arr); // 입력한 명령어가 edit인 경우 처리해주는 함수
int command_fill( char *buffer, int *arr); // fill인 경우 처리해주는 함수
void command_reset( ); // reset인 경우 처리해주는 함수

#endif
