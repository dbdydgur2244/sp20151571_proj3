#include "20151571.h"
#include "opcode.h"

//가능한 명령어를 출력하는 함수
void command_help(const char **help_list, int size){
    int i;
    for(i = 0; i < size; ++i)
        printf("%s\n", help_list[i]);
}

// dir를 출력하는 함수
void command_dir(){
    DIR *dirp;
    struct dirent *direntp;
    struct stat buf;
    if( (dirp = opendir(".")) == NULL){ //diropen error check
        printf("Can not Open Directory\n");
        return;
    }

    while( (direntp = readdir(dirp)) != NULL){ // dir에 있는 모든 파일을 읽을 때 까지
        stat(direntp->d_name, &buf);
        printf("%s", direntp->d_name);

        if( S_ISDIR(buf.st_mode) ) //directory인 경우
            printf("/");

        else if( (buf.st_mode & S_IXUSR) ||
                (buf.st_mode & S_IXGRP) ||
                (buf.st_mode & S_IXOTH) ) // 실행 파일인 경우
            printf("*");
        printf("\t");
    }
    puts("");
    closedir(dirp);
}

//history 목록을 출력하는 함수
void command_history(History head){
    History ptr;
    int i = 1;
    for(ptr = head; ptr != NULL; ptr = ptr -> next)
        printf("%-5d %s\n", i++, ptr->command);
}

/* quit 명령어를 처리해주는 함수 
 * 추가로 동적할당한 부분을 free 해주고 종료한다.
 */
void command_quit(History *hi_head){
    opcode_init();
    *hi_head = NULL;
    exit(0);
}
