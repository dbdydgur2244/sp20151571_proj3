#include "memory.h"
#include "structure.h"
#include "string_mani.h"

Shell_Memory Shmemory; 

void memory_init(){
    Shmemory.last_address = 1048575;
    Shmemory.max_address = 1048575;
    for( int i = 0; i < Shmemory.max_address; ++i)
        Shmemory.memory[i] = 0;
}

void set_memory(int address, int value){
    Shmemory.memory[address] = value;
}

int get_memory(int address){
    return Shmemory.memory[address];
}

/* start부터 end까지 memory 정보를 출력하는 함수
 * boundary check 에서 boundary error가 발견되면 return -1
 * 정상적인 경우 return 1
 */
int print_memory( int start, int end){
    int str_hex = start / 16 * 16,
        end_hex = end / 16 * 16; // 16단위로 끊어서 세기 위함
    int i, j;
    unsigned char *memory = Shmemory.memory;

    if(start <= end && start >= 0){ // boundary check
        end = end <= Shmemory.max_address ? end : Shmemory.max_address ; // end boundary check  
        for ( i = str_hex; i <= end_hex; i += 16){
            printf("%05X ", i);
            for(j = 0; j < 16; ++j){
                if(start <= i + j && i + j <= end)
                    printf("%02X ", memory[i+j]);
                else
                    printf("   ");
            }
            printf("; ");

            for(j = 0; j < 16; ++j){
                if(i + j >= start && i + j <= end){
                    if(memory[i+j] >= 0x20 && memory[i+j] <= 0x7E){
                        printf("%c", memory[i+j]);
                    }
                    else
                        printf(".");
                }
                else
                    printf(".");
            }
            puts("");
        }
        return 1; // 정상인 경우
    }
    else // boundary check에서 에러확인된경우
        return -1;
}

/* dump 명령어를 처리해주는 함수
 * dump만 입력한 경우 last_address 다음 주소에서 160칸 출력(메모리 범위를 넘어가지 않는 범위에서)
 * dump start만 입력한 경우 start에서 160칸 출력(단 메모리 범위를 넘어가지 않는 범위에서)
 * dump start. end만 입력한 경우 start부터 end까지 출력해준다.
 * start. end가 16진수가 아니거나 범위를 넘어가는 경우는 출력하지 않고 에러처리한다.
 */
int command_dump(char *buffer, int *arr){
    int len = 0, num = 0;
    int start_address = ( Shmemory.last_address + 1 ) % 
        ( Shmemory.max_address + 1 ) ,
        end_address = min ( start_address + 159, Shmemory.max_address ); 
    // start_address와 end_address 기본 dump인 경우를 위한  초기화 부분.
    char tmp[256];
    for ( int i = 0; i  < (int)strlen(buffer); ++i )
        if ( buffer[i] == ',' )
            num++;

    strncpy(tmp, buffer, sizeof(tmp)); 
    str_replace(tmp, "," , " , ");
    len = get_values(tmp, arr);
    if ( len == -1 || len > 2 ){
        fprintf(stderr, "get value error!\n");
        return -1;
    }
    if ( len == 1){
        if( num != 0)
            return -1;
        start_address = arr[0], end_address = min ( start_address + 159, Shmemory.max_address );
    }

    else if ( len == 2){
        if ( num != 1)
            return -1;
        start_address = arr[0], end_address = arr[1];
    }
    else if ( len > 2)
        return -1;

    if ( start_address > Shmemory.max_address ||
            start_address > end_address){
        printf("Address Error!\n");
        return -1;
    }

    print_memory (start_address, end_address );
    Shmemory.last_address = end_address;
    return 1;
}

/* edit 명령어 처리해주는 함수
 * 정상적인 경우 return 1
 * error check에 걸린 경우 return -1
 */
int command_edit( char *buffer, int *arr){
    int address, value;
    int num = 0;
    char tmp[256];
    strncpy(tmp, buffer, sizeof(tmp));

    for ( int i = 0; i < (int)strlen(tmp); ++i)
        if ( tmp[i] == ',')
            num++; // ','의 개수를 샌다.
    if(num != 1){ // 1개가 아닌 경우 에러
        fprintf( stderr, "Format error!\nFormat is e[dit] address, value\n");
        return -1;
    }

    str_replace(tmp, ",",  " , "); // ','를 ' , '로 다 바꿔준다.
    if( get_values(tmp, arr) == -1){ // values에 에러 있는지 확인
        fprintf( stderr, "get values error!\n");
        return -1;
    }

    address = arr[0]; value = arr[1];

    if ( ! ( 0 <= value && value <= 0xFF ) ){
        fprintf(stderr, "value boundary error!\n( 0 <= value <= 0xFF )\n");
        return -1;
    }

    if( 0 <= address && address <= Shmemory.max_address){ // boundary check
        Shmemory.memory[address] = value;
        return 1;
    }
    return -1;
}

/* fill 명령어 처리해주는 함수 
 * 정상적인 경우 return 1
 * error check에 걸린 경우 return -1
 */
int command_fill( char *buffer, int *arr){
    int num = 0;
    int start, end, value;
    char tmp[256];

    strncpy(tmp, buffer, sizeof(tmp)); // tmp에 buffer string copy

    for( int i = 0; i < (int)strlen(buffer); ++i)
        if( buffer[i] == ',') // ',' 개수 확인
            num++;
    if(num != 2){ // ','가 2개가 아니면 에러
        fprintf(stderr, "Format Error!\nFormat is f[ill] start, end, value\n");
        return -1;
    }

    str_replace(tmp, ",", " , "); // ',' 를 ' , '로 치환해준다.

    if ( get_values(tmp, arr ) == - 1){ // value에 에러 있는지 확인
        fprintf(stderr, "Get values error!\n");
        return -1;
    }

    start = arr[0], end = arr[1], value = arr[2];

    if ( ! (0 <= value && value <= 0xFF) ){
        fprintf ( stderr, "value boundary error!\nvalue must 0 <= value <= 0xFF");
        return -1;
    }
    if(start >= 0 && start <= end && end <= Shmemory.max_address){ // boundary check
        for(int i = start; i <= end; ++i)
            Shmemory.memory[i] = value;
        return 1;
    }
    return -1;
}

/* reset 명령어를 처리해주는 함수
 * 메모리를 모두 0x00으로 초기화해준다.
 */
void command_reset(){
    unsigned char *memory = Shmemory.memory;
    int i;
    for ( i = 0; i < Shmemory.max_address; ++i )
        memory[i] = 0;
}

