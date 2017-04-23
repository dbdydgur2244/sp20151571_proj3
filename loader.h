#ifndef __LOADERH__
#define __LOADERH__
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "structure.h"

void Est_init();
int Est_key(char *);
int Est_insert( char *, int , int );
extsptr Est_find(char *);
void recode_M(char *name, char , int , int, int , int );
int recode_R(EXTSYM *, char *name);
int link_pass1(FILE *, int secnum, int *);
int link_pass2(FILE *, int secnum, int );

int command_progaddr(char *buffer);
int command_loader(char *command);

#endif
