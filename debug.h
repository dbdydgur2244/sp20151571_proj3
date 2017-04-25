#ifndef __DEBUG__
#define __DEBUG__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int cmp(const void *, const void *);
int command_bp(char *buffer);
void print_bp();
void add_bp(int address);
void bp_init(int option);
void currbp_init();
int get_nextbp(int curr);

#endif
