#include "structure.h"
#include "loader.h"
#include "memory.h"
#include "string_mani.h"
#include "run.h"
#include "debug.h"
int start_address = 0;
ESTAB ExtTable = { 37 };

/*
 * EXTERN SYMBOL TABLE init function
 */
void Est_init(){
    extsptr nptr;
    for ( int i = 0 ; i < ExtTable.size; ++i){
        for ( extsptr ptr = ExtTable.table[i]; ptr != NULL; ){
            nptr = ptr->next;
            free(ptr);
            ptr = nptr;
        }
        ExtTable.table[i] = NULL;
    }

    for ( int i = 0; i < 4; ++i )
        ExtTable.ConSec[i].Dnum = 0;
}

/*
 * external symbol table key 값을 구하는함수
 */
int Est_key(char *string){
    int key;
    size_t len = strlen(string);
    for ( size_t i = 0; i < len; ++i )
        key += string[i];
    return key % ExtTable.size;
}

/*
 * external symbol Table insert function
 * Table에 존재하지 않는 경우 0
 * 있는 경우 1을 return
 */
int Est_insert(char *string, int address, int flag){
    int key = Est_key(string);
    extsptr nptr;

    if ( Est_find(string) != NULL ){
        fprintf(stderr, "Already exists EXTERNAL SYMBOL NAME %s\n", string);
        return 0;
    }

    nptr = malloc(sizeof(ExtSymbol));
    strncpy(nptr->symbol, string, sizeof(nptr->symbol));
    nptr->address = address;
    nptr->next = ExtTable.table[key];
    nptr->cs_flag = flag;
    ExtTable.table[key] = nptr;
    return 1;
}

/*
 * ESTAB에 있는지 확인하는 함수
 */
extsptr Est_find(char *string){
    int key = Est_key(string);
    for ( extsptr ptr = ExtTable.table[key]; ptr != NULL; ptr = ptr->next){
        if ( strcmp ( string, ptr->symbol ) == 0 )
            return ptr;
    }
    return NULL;
}

/* 
 * Pass1 for linking loader
 * H, D record만 관련해서 수행한다.
 * 만약에 에러가 있으면 return 0, else return 1
 */
int link_pass1(FILE *fp, int csnum, int *csaddr){
    char buffer[256];
    char str[256];
    char name[7];
    char recode;
    int length;
    int len;
    int address;
    int diff;
    int flag = 1;
    CSADDR *ConSec = &(ExtTable.ConSec[csnum]);
    EXTSYM *ExtSym = ConSec->ExtSym;

    while( fgets( buffer, sizeof(buffer), fp) != NULL ) {
        if ( !flag )
            return 0;

        len = strlen(buffer);
        if ( ! ( len > 0 ) )
            break;
        buffer[ len - 1 ] = 
            buffer[ len - 1] == '\n' ? '\0' : buffer[ len - 1];

        if ( !flag )
            return 0;

        memset(str, 0 , sizeof(str));

        sscanf(buffer, "%c%[^\n]", &recode, str);
        if ( recode == 'H'){
            sscanf(str, "%6s%6X%6X", name, &address, &length);
            delete_whitespace(name);
            str_replace(name, " ", "");
            diff = *csaddr - address;
            flag = Est_insert( name, address + diff, 1);
            if ( !flag )
                return 0;
            strcpy( ConSec[csnum].name, name);
            ConSec->address = address + diff;
            ConSec->length = length;
            ExtSym[0].address = address + diff;
            strcpy( ExtSym[0].name, name);
            // for rellocation
        }

        else if ( recode == 'D'){
            len = (int) strlen(str);
            for ( int i = 0; i < len; i += 12){
                sscanf( str + i, "%6s%6X", name, &address);
                delete_whitespace(name);
                str_replace(name, " ", "");
                flag = Est_insert(name, address + diff, 0);
                if ( !flag )
                    return 0;
                strcpy(ExtSym[ i / 12 + 1 ].name , name);
                ExtSym[ i / 12 + 1].address = address + diff;
                // for rellocation
                ConSec->Dnum++;
            }
        }

        else if ( recode == 'E' ) {
            sscanf( str, "%6X", &address);

        }
    }
    *csaddr += length;
    rewind(fp);
    return 1;
}
/*
 * linking loader pass2 algorithm function
 * In this fucntion, process R and M, T recode.
 * So loading this objfile to memory in this function.
 * if there is error, then return 0, else return 1
 */
int link_pass2(FILE *fp, int csnum, int start_address){
    char buffer[256];
    char str[256];
    char name[7];
    char recode;
    char sign;
    int length;
    int len;
    int address;
    int flag = 1;
    int curr, idx;
    int diff;
    int value;
    int size;
    EXTSYM  EXTARR[50];
    extsptr ptr;

    while( fgets( buffer, sizeof(buffer), fp) != NULL ) {
        len = strlen(buffer);
        if ( ! ( len > 0 ) )
            break;
        buffer[ len - 1 ] =
            buffer[ len - 1] == '\n' ? '\0' : buffer[ len - 1];
        memset(str, 0 , sizeof(str));
        if ( !flag )
            return 0;
        sscanf(buffer, "%c%[^\n]", &recode, str);


        if ( recode == 'H'){
            sscanf(str, "%6s%6X%6X", name, &address, &length);
            delete_whitespace(name);
            str_replace(name, " ", "");
            ptr = Est_find(name);
            if ( ptr == NULL ){
                fprintf(stderr, "NO [%s] EXTSYM ERROR!\n", name);
                return 0;
            }
            diff = start_address - address;
            EXTARR[0].address = ptr->address;
            strcpy(EXTARR[0].name, name);
        }

        if ( recode == 'T'){
            sscanf(str, "%6X%2X", &address, &length);
            curr = 8; idx = 0;
            while ( idx < length ){
                sscanf(str + curr, "%2X", &value);
                set_memory(address + diff + idx, value);
                idx++;
                curr += 2;
            }
        }

        else if ( recode == 'R' ){
            if ( ! recode_R(EXTARR, str ) )
                return 0;
        }

        else if ( recode == 'M'){
            sscanf(str, "%6X%2X%c%2X", &address, &size, &sign, &idx);
            recode_M(EXTARR, sign, address + diff, size, csnum, idx - 1);
        }
    }
    rewind(fp);
    return 1;
}

/*
 * progadder 명령어를 수행하는 함수
 * loading 할 때 시작 주소를 정해준다.
 * error가 있으면 return 0
 */
int command_progaddr(char *buffer){
    char *token = NULL;
    char sep[] = " \t";
    char *error;
    int str_address;
    int len = 0;

    token = strtok(buffer, sep);

    if ( token == NULL ) {
        fprintf(stderr, "PLEASE INPUT ADDRESS PLEASE\n");
        return 0;
    }

    while ( token != NULL ) {
        token = strtok(NULL, sep);
        if ( token == NULL )
            break; 
        len++;
        if ( len > 1 ){
            fprintf(stderr, "PLEASE INPUT ONLY ADDRESS PLEASE\n");
            return 0;
        }

        str_address = ( int ) strtol(token, &error, 16);
        if ( *error != '\0'){
            fprintf(stderr, "PLEASE INPUT ADDRESS BY HEX NUMBER\n");
            return 0;
        }
    }
    if ( len < 1 )
        return 0;
    start_address = str_address;
    return 1;
}

/*
 * loader 명령어를 수행하는 함수.
 * 주어진 시작 주소로 부터 obj 파일을 memory에 적재한다.
 * 에러가 있으면 return 0, else return 1이다.
 */
int command_loader(char *command){
    char copy[256];
    char *token = NULL;
    int address = start_address;
    char *filename[3];
    int len = 0;
    FILE *fp[3];
    char sep[] = " \t";
    int total_length = 0;
    int error_flag = 1; // if there is error -> 1
    CSADDR *csaddr;
    Est_init();
    command_reset();
    strncpy(copy, command, sizeof(copy));

    token = strtok( copy, sep );
    while( token != NULL ) {
        token = strtok( NULL, " \t");
        if ( token == NULL)
            break;
        filename[len++] = token;
    }

    if ( len < 1 || len > 3 ) {
        fprintf(stderr, "PLEASE INPUT COMMAND CORRECTLY\n");
        return 0;
    }

    for ( int i = 0; i < len; ++i ){
        fp[i] = fopen (filename[i], "r");
        if ( fp[i] == NULL ){
            fprintf(stderr, "NO FILE. So can not load files\n");
            return 0;
        }
    }

    for ( int i = 0; i  < len; ++i ){
        error_flag = link_pass1(fp[i], i, &address);
        if ( !error_flag ){
            fprintf(stderr, "There is error in Lingking loader pass1\n");
            break;
        }
    }

    for ( int i = 0; i  < len; ++i ){
        if ( ! error_flag )
            break;
        error_flag = link_pass2(fp[i], i, ExtTable.ConSec[i].address);
        if( !error_flag ){
            
            break;
        }
    }

    for ( int i = 0; i < len; ++i )
        fclose(fp[i]);

    if ( ! error_flag){
        Est_init();
        return 0;
    }

    printf("control\t\tsymbol\t\taddress\t\tlength\n");
    printf("section\t\tname\n");
    printf("---------------------------------------------------------------\n");
    for ( int i = 0; i < len; ++i ){
        csaddr = &(ExtTable.ConSec[i]);
        printf("%-6s\t\t\t\t%04X\t\t%04X\n", csaddr->name, csaddr->address, csaddr->length);
        total_length += csaddr->length;
        for ( int j = 0; j < csaddr->Dnum; ++j )
            printf("\t\t%-6s\t\t%04X\t\t\n", csaddr->ExtSym[j].name, csaddr->ExtSym[j].address);
    }
    printf("---------------------------------------------------------------\n");
    printf("\t\t\t\ttotal length\t%04X\n", total_length);
    
    set_start_address(start_address);
    set_end_address(start_address + total_length);

    reg_init();
    currbp_init();
    return 1;
}

/*
 * 이 함수는 recode가 M인 경우 처리해주는 함수이다.
 * 여기서 relocative addressing을 적용한다.
 * EXTARR에 reference number에 대한 저장된 정보를 가지고서 relocative addressing을 한다.
 */
void recode_M(EXTSYM *EXTARR, char sign, int address, int size, int csnum, int idx){
    int value = 0;
    int mask = 0;
    int addr, tmp;
    int signb;

    signb = ( sign == '-' ) ? -1 : 1;

    tmp = size / 2 + size % 2;
    addr = address;
    while( tmp ){
        value <<= 8;
        value += get_memory(addr);
        tmp--;
        addr++;
    }

    tmp = size ;        
    while ( tmp ) {
        mask <<= 4;
        mask += 0xF;
        tmp--;
    }

    tmp = mask & value;

    tmp += EXTARR[idx].address * signb;
    tmp = mask & tmp;

    value = value & (~mask);
    value += tmp;

    addr = address;
    tmp = size / 2 + size % 2;
    while( tmp ){
        set_memory(addr, ((value >> ( ( tmp - 1 ) * 8 ) ) & 0xFF));
        addr++;
        tmp--;
    }

}

/*
 * 이 함수는 recode가 R인 경우 처리해주는 함수이다.
 * 이 함수에서 reference number 처리를 해준다.
 */
int recode_R(EXTSYM *EXTARR, char *str){
    int len;
    int idx;
    char name[7];
    extsptr ptr;
    len = (int) strlen(str);
    for ( int i = 0; i < len; i+= 8 ){
        sscanf(str + i, "%2X%6s", &idx, name);
        delete_whitespace(name);
        str_replace(name, " ", "");
        ptr = Est_find(name);
        if  ( ptr == NULL ){
            fprintf(stderr, "NO [%s] EXTSYM ERROR!\n", name);
            return 0;
        }
        EXTARR[idx - 1].address = ptr->address;
        strcpy( EXTARR[ idx - 1 ].name, ptr->symbol);
    }
    return 1;
}

