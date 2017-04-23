#ifndef __STRING_MANI__
#define __STRING_MANI__
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// a ,b  값중 작은 값을 반환해준다.
//

int min(int , int);
void str_replace(char *target, const char *orig, const char *repl);
int get_values(char *buffer, int *arr);
void delete_whitespace(char* str);

#endif

