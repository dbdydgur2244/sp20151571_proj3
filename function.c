#include "20151571.h"
#include "memory.h"
#include "loader.h"
#include "opcode.h"

const char *help_list[] = {
    "h[elp]",
    "d[ir]",
    "q[uit]",
    "hi[story]",
    "du[mp] [start, end]",
    "e[dit] address, value",
    "f[ill] start, end, value",
    "reset",
    "opcode mnemonic",
    "opcodelist",
    "assemble fileneme",
    "type filename",
    "symbol",
    "progaddr",
    "loader",
    "run",
    "bp [address]"
};

const char *Help[] = {
    "h",
    "help",
    "d",
    "dir",
    "q",
    "quit",
    "hi",
    "history",
    "du",
    "dump",
    "e",
    "edit",
    "f",
    "fill",
    "reset",
    "opcode",
    "opcodelist",
    "assemble",
    "type",
    "symbol",
    "progaddr",
    "loader",
    "run",
    "bp"
};


/* 입력한 명령어가존재하는지 찾고 몇번째와 일치하는지 알려주는 함수
 * 존재하는 경우 return 번호
 * 존재하지 않는 경우 return -1
 */
int command_find(char *str_cmp){
    int i;
    int size = sizeof(Help) / sizeof(char *);
    for ( i = 0; i < size; ++i)
        if ( strcmp(Help[i], str_cmp) == 0)
            return i;
    printf("The corresponding command does not exist.\n");
    return -1;
}

/* 입력한 명령어가 존재하면 enum command에 있는 값으로 적절히 값을return하는 함수
 * 명령어가 없는 경우만 return -1을 한다.
 */
int get_command(char *buffer){
    char sep[] = " \t";
    char *token = strtok(buffer, sep);
    int command_num;
    while(token != NULL){
        if ( ( command_num = command_find(token) ) != -1 )
            return ( command_num <= 11 ? command_num / 2 : command_num - 5);
        // dir와 같은 명령어들은 d[ir]이런 형식으로 사용할 수 있어서 이 부분을 처리해주는 부분이다.
        token = strtok(NULL, sep);
    }
    return -1;
}

// history에 입력한 명령어를 linked_list tail에  추가하는 함수
void add_history(History *head, char *command){
    History ptr = *head;
    History nptr;

    nptr = malloc(sizeof(Linked_list));
    nptr -> next = NULL;
    strncpy(nptr->command, command, sizeof(nptr->command));

    if(ptr != NULL){
        for(; ptr-> next != NULL; ptr = ptr -> next); // 가장 최근에 입력한 명령어를 가장 나중에 넣는다.
        ptr -> next = nptr;
    }
    else
        *head = nptr;
}

// 초기화 함수
void sp1_init(History *Hhead){
    *Hhead = NULL;
    opcode_init(); 
    memory_init();
    get_opcode(); //opcode 저장
}

//초기화 함수
void sp2_init(Symbol_table *Stable){
    for ( int i = 0; i < 37; ++i)
        Stable->table[i] = NULL;
}

/* 입력한 명령어가 존재하는지 확인하는 함수
 * 입력한 명령어가 존재하는 경우 enum Command에 해당하는 값에 맞게 출력해준다.
 * 에러인 경우만 return -1
 */
int command_check(char *user_str){
    char sep[] = " \t";
    char *token;
    int i = 0, command_num = -1, len = 0;
    token = strtok(user_str, sep);

    if(token == NULL) // 입력한 것이 없는 경우
        return -1;

    command_num = command_find(token); // 명령어가 존재하는지 확인
    if(command_num == -1) // 없는 경우
        return -1;

    while ( token != NULL ){
        if(i > 6) // 이상한 명령어 입력해준경우 체크
            return -1;
        len++;
        token = strtok(NULL, sep);
    }
    
    if ( ( 0 <= command_num && command_num <= 7 ) || command_num ==  14
            || command_num == 16 || command_num == 19)
        if ( len > 1 )
            return -1; // dir나 help 같은 추가로 입력할 것이 없는 명령어 에러 확인

    if( 0 <= command_num && command_num <= 13)
        return command_num / 2; // du[mp] 같이 입력할 수 있는 경우
    else if ( command_num > 13)
        return command_num - 7; // reset 같이 하나인 경우.
    return -1; // error인 경우
}

/* 사용자가 입력한 명령어를 처리해주는 함수
 * 명령어에 따라 적절하게 처리해준다.
 */
void main_process(char *buffer, History *head, Symbol_table *Stable, symbol_info symbolArr[]){
    int command_num;
    int error_check;
    char str_copy[256];
    int arr[5];
    History Hhead = *head;
    strncpy( str_copy, buffer, sizeof(str_copy));
    command_num = command_check(str_copy); // 명령어 존재하는지 확인
    if(command_num != -1){
        strncpy(str_copy, buffer, sizeof(str_copy));
        error_check = 1;
        switch(command_num){
            case help:
                command_help(help_list, sizeof(help_list) / sizeof(char * ) );
                break;

            case dir:
                command_dir();
                break;

            case quit:
                command_quit(&Hhead);
                break;

            case history:
                add_history(&Hhead, buffer);
                error_check = 0;
                command_history(Hhead);
                break;

            case dump:
                error_check = command_dump( str_copy, arr);
                break;

            case edit:
                error_check = command_edit( str_copy, arr);
                break;

            case Fill:
                error_check = command_fill( str_copy, arr);
                break;

            case Reset:
                command_reset( );
                break;

            case opcode:
                error_check = command_opcode(str_copy);
                break;

            case opcodelist:
                command_opcodelist();
                break;

            case assemble:
                error_check = command_assemble( Stable, symbolArr, str_copy );
                break;

            case type:
                error_check = command_type(str_copy);
                break;

            case symbol:
                command_symbol(symbolArr);
                break;

            case progaddr:
                error_check = command_progaddr(str_copy);
                break;
                
            case loader:
                error_check = command_loader(str_copy);
                break;

            case run:
                break;

            case bp:
                break;
        }
        if ( error_check == 1 )
            add_history(&Hhead, buffer);
    }

    else
        printf("There is no command\nplease rewrite command\n");
    *head = Hhead;
}
