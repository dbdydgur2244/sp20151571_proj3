#include "opcode.h"

Hash HashTable;

void opcode_init(){
    Hnode hptr, Hhead;
    HashTable.size = 20;
    for (int i = 0; i < HashTable.size; ++i ){
       for ( ; Hhead != NULL; ){
        Hhead = HashTable.Table[i];
            hptr = Hhead;
            Hhead = Hhead->next;
            free(hptr);
        }
        HashTable.Table[i] = NULL;
    }
}

Hash * get_hashTable(){
    return &HashTable;
}


//opcode find function
//존재하면 opcode pointer를 return
//없으면 return NULL
Hnode opcode_find(char *mnemonic){
    int i;
    Hnode ptr;
    for ( i = 0; i < HashTable.size; ++i ){ // hash_table에서 일일이 찾는다
        for ( ptr = HashTable.Table[i]; ptr != NULL; ptr = ptr -> next ){
            if ( strcmp( ptr->str_opcode , mnemonic ) == 0)
                return ptr;
        }
    }
    return NULL; // 존재하지 않는 경우
}

Hnode nopcode_find(int opcode){
    Hnode ptr;
    for ( int i = 0; i < HashTable.size; ++i ){ // hash_table에서 일일이 찾는다
        for ( ptr = HashTable.Table[i]; ptr != NULL; ptr = ptr -> next ){
            if ( opcode == ptr->n_opcode)
                return ptr;
        }
    }
    return NULL; // 존재하지 않는 경우
}

/* Hash에 mnemonic을 insert하는 함수
 * 적절한 key를 사용하여 insert한다(여기서는 key % hash_size를 이용한다)
 */
void opcode_insert(int n_opcode, char *mnemonic, char *code){
    Hnode ptr;
    Hnode nptr;
    int Hash_size = HashTable.size;
    int key = 0;
    nptr = malloc(sizeof(Hash_Node));
    strncpy ( nptr->str_opcode, mnemonic, sizeof(nptr->str_opcode) );
    strncpy( nptr->code , code, sizeof(nptr->code) ); 
    if ( strcmp ( code, "1") == 0)
        nptr->format = 1;
    else if ( strcmp ( code, "2") == 0)
        nptr->format = 2;
    else if ( strcmp( code, "3/4") == 0)
        nptr->format = 3;

    for( int i = 0; i < (int)strlen(mnemonic); ++i )
        key += mnemonic[i];
    nptr -> n_opcode = n_opcode;
    nptr -> next = NULL;

    key = key % Hash_size;
    ptr = HashTable.Table[key];

    if(ptr != NULL)
        nptr -> next = ptr;

    HashTable.Table[key] = nptr;
}
// opcode.txt에서 opcode를입력받아서 저장하는 함수
void get_opcode(){
    FILE *fp;
    char buffer[256];
    int n_opcode;
    char str_opcode[100];
    char code[100];

    fp = fopen("opcode.txt", "r");
    if ( fp == NULL){
        printf("FILE OPEN ERROR\n");
        return;
    }

    while ( fgets(buffer, sizeof(buffer), fp) != NULL ){
        sscanf(buffer, "%x %s %s", &n_opcode, str_opcode, code ); //space나 tab 같은 것들 처리
        opcode_insert(n_opcode, str_opcode, code); //hash에 insert
    }
    fclose(fp);
}

//opcodelist를 출력하는 함수
void command_opcodelist(){
    Hnode ptr;
    for ( int i = 0; i < 20; ++i){
        printf("%d : ", i);
        for ( ptr = HashTable.Table[i]; ptr != NULL; ptr = ptr -> next){
            printf("[%s,%X]", ptr->str_opcode, ptr->n_opcode);
            if(ptr -> next != NULL)
                printf(" -> ");
        }
        puts("");
    }
}

/* opcode 명령어를 처리해주는 함수
 * 
 */
int command_opcode( char *buffer ){
    char tmp[256];
    int len = 0;
    char sep[] = " \t";
    char *token = NULL;
    char *mnemonic;
    Hnode ptr;
    strncpy(tmp, buffer, sizeof(tmp));

    token = strtok(tmp, sep);
    while( token != NULL){
        token = strtok(NULL, sep);
        if(token == NULL)
            break;
        mnemonic = token;
        len++;
    }

    if ( len != 1 ){
        fprintf(stderr, "Format error!\nFormat is [opcode mnemonic]\n");
        return -1;
    }

    if( ( ptr = opcode_find(mnemonic) ) == NULL ){
        fprintf(stderr, "There is no mnemonic\n");
        return -1;
    }

    printf("opcode is %X\n", ptr->n_opcode);
    return 1;
}
