#include "debug.h"
#include "structure.h"

//typedef BP_NODE * node;
int breakpoint[200];
int currbp = -1;
int length = 0;

/*
 * command breakpoint 명령어를 수행하는 함수
 */
void bp_init(int option){
   for ( int i = 0; i  < 200; ++i)
       breakpoint[i] = -1 & 0xFFFFFF;
   if ( option == 1)
       printf("\t[ok] clear all breakpoints\n");
   length = 0;
}

void currbp_init(){
    currbp = -1;
}

int command_bp(char *buffer){
	char *token;
    char *error;
    int bp = -1;
    int len = 0;
    token = strtok(buffer, " \t");

    while(token != NULL ){
        token = strtok(NULL, " \t");
        if ( token == NULL)
            break;
        len++;
        if ( strcmp(token, "clear") == 0)
            bp_init(1);
        bp = strtol(token, &error, 16);
        if ( *error != '\0' )
            return 0;
    }

    if ( len == 0 ){
        print_bp();
        return 0;
    }

    else if ( len > 1)
        return 0;

    add_bp(bp);
    printf("\t[ok] create breakpoint %04X\n", bp);
    return 0;
}

int cmp(const void *a, const void *b){
    int *A = (int *)a, *B = (int *)b;
    if ( *A < *B )
        return -1;
    else if ( *A == *B )
        return 0;
    return 1;
}

void print_bp(){
	printf("\tbreakpoint\n");
	printf("\t----------\n");
	for ( int i = 0; i < length; ++i)
		printf("\t%04X\n", breakpoint[i]);
}

void add_bp( int address ){
    int i = 0;
    for ( i = 0; i < length; ++i){
        if( breakpoint[i] == address) {
            fprintf(stderr, "ALREADY EXIST breakpoint.\n");
            break;
        }
        if ( breakpoint[i] > address)
            break;
    }
    breakpoint[length] = address;
    if ( currbp != -1 && breakpoint[currbp] >= address )
        currbp++;
    length++;
    qsort(breakpoint,length, sizeof(breakpoint), cmp);
}

int get_nextbp(int curr){
    if ( length == 0)
        return -1;

    if ( currbp == -1 )
        return breakpoint[++currbp];
    
    else if ( currbp >= length )
        return 0;

    else if ( breakpoint[currbp] != (-1 & 0xFFFFFF))
        return breakpoint[++currbp];
   
    return (-1 & 0xFFFFFF);
}

