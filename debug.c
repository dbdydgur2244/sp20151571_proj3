#include "debug.h"
#include "structure.h"

typedef BP_NODE * node;
node head = NULL;
node curr = NULL;

/*
 * command breakpoint 명령어를 수행하는 함수
 */

int command_bp(char *buffer){
	char *token;
	int addr;
    char *error;
    int bp = -1;
	//int error = 0;
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

    if ( len == 0 )
        print_bp();

    else if ( len > 1)
        return 0;
   
    if ( bp != -1 ){
        add_at_bp(bp);
        printf("\t[ok] create breakpoint %04X\n",addr);

    }
	return 0;
}

/**** print every breakpoint ****/
void print_bp(){
	node ptr = head;
	printf("\tbreakpoint\n");
	printf("\t----------\n");
	while(ptr != NULL){
		printf("\t%04X\n",ptr->address);
		ptr = ptr->next;
	}
}

/**** add at bp linked list ****/
void add_at_bp( int address ){
	node ptr;
    node prev;
	node nptr;
    nptr = (node)malloc(sizeof(*nptr));
    nptr->address = address;
    nptr->next = NULL;

    if ( head == NULL )
        head = nptr;
	
    else{
		prev = head;
		
		while ( prev->next != NULL){
			if( prev->address < address ){
				ptr = prev->next;
				if(ptr->address > address)
					break;
			}
			else if(prev->address == address){
				fprintf(stderr, "ALREADY EXIST breakpoint.\n");
                return;
			}
			else
				break;
			prev = prev->next;
		}

		if(prev == head){
			if(prev->address == address){
                fprintf(stderr, "ALREADY EXIST breakpoint.\n");
                return;
            }

			else if(prev->address < address){
				nptr->next = prev->next;
				prev->next = nptr;
			}

			else{
				nptr->next = prev;
				head = nptr;
			}
		}
		else if(prev->next == NULL)
			prev->next = nptr;

		else{
            nptr->next = prev->next;
			prev->next = nptr;
		}
	}
}

void bp_init(int option){
	node ptr = head;
	while(ptr != NULL){
		ptr = head->next;
		free(head);
		head = ptr;
	}
	if(option == 1)
		printf("\t[ok] clear all breakpoints\n");
}

int get_nextbp(int curr){
	int address = -1;
	node prev = head;
	node tmp;

	while(prev != NULL && prev->next != NULL){
		/**** curr <= prev_addr ****/
		if(prev->address >= curr){
			if(prev->address == curr && curr != NULL && prev == curr)
				tmp = prev->next;
			else{
				curr = prev;
				return prev->address;
			}
		}
		/**** prev < curr ****/
		else
			tmp = prev->next;
		
		/**** prev < curr <= tmp ****/
		if(tmp->address >= curr){
			if(tmp->address == curr && curr != NULL && tmp == curr)
				prev = prev->next;
			else{
				curr = tmp;
				return tmp->address;
			}
		}

		/**** prev < tmp < curr ****/
		else
			prev = prev->next;
	}
	if(prev != NULL && prev->address >= curr && prev != curr){
		curr = prev;
		return prev->address;
	}
	return address;
}

