#ifndef __DEBUG__
#define __DEBUG__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int command_bp(char *buffer);
void print_bp();
void add_at_bp(int address);
void bp_init(int option);
int get_nextbp(int curr);

#endif
