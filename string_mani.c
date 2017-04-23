#include "string_mani.h"


int min(int a, int b){
    return a < b ? a : b;
}

/* target string에서 orig과 같은 부분을 repl으로 replace 시켜주는 함수이다.
 * char *target이바꿀 string이고 orig은replace될 string, repl은 orig을 replace할 string이다.
 */
void str_replace(char *target, const char *orig, const char *repl){
    char buffer[256] = { 0 }; // target string을 copy할 배열
    char *insert_point = buffer; // replace할 point를 저장할 pointer
    const char *tmp = target; // target의원래 주소를 저장한 pointer
    size_t orig_len = strlen(orig);
    size_t repl_len = strlen(repl);
    while (1) {
        const char *p = strstr(tmp, orig); // p = orig을 포함한 문자열의 위치
        if (p == NULL) { // 더이상 orig을 포함한 문자열이 존재하지 않음
            strcpy(insert_point, tmp); // insert_point에 tmp string copy
            break;
        }
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;
        memcpy(insert_point, repl, repl_len);
        insert_point += repl_len;
        tmp = p + orig_len; // insert_point를 찾고 buffer에 저장하는 부분
    }
    strcpy(target, buffer); //targer에 replacement한string을복사함
}

/* 입력한 명령어에서 값들을 저장하는 함수
 * 값들이 정상적인 경우 return 1
 * 이상한 입력이 들어온 경우 return -1
 */
int get_values(char *buffer, int *arr){
    char *token, *bef;
    char *error;
    char sep[] = " \t";
    int idx= 0, flag = 1; // flag는 에러 체크할 변수
    int value;
    token = strtok(buffer, sep);
    while(token != NULL){
        bef = token;
        token = strtok(NULL, sep);
        if ( token == NULL)
            break;
        if ( strcmp( token , "," ) == 0 ){ // ',' 에러 처리
            if ( flag )
                return -1;
            else
                flag = 1;
        }
        else{
            if(flag != 1)
                return -1;
            value = (int)strtol(token, &error, 16); // 16진수의 값을 10진수로 바꿈
            if( *error != '\0'){ // 16진수가 아닌 경우 에러 처리
                fprintf(stderr, "No Hex number!\n");
                return -1;
            }
            arr[idx++] = value;
            flag = 0;
        }
    }
    if(*bef == ',')
        return -1;
    return idx;
}


/*
 * whitespace('\t', ' ') 같은 것들을 삭제해주는 함수
 */
void delete_whitespace(char* str){
    int white_flag = 0;
    int i, len;
    int idx;
    int first_char = 0;
    int sep_flag = 0;

    len = strlen(str);
    for(i = 0; i <= len; i++){
        if(i == len){
            //EOF 넣음
            if(white_flag >= 1)
                str[i-white_flag] = '\0';
        }
        //whitespace 인경우 flag 증가
        if(str[i] == '\''){
            sep_flag++;
            white_flag = 0;
            if(sep_flag == 2)
                sep_flag = 0;
        }
        if(sep_flag)
            continue;

        if(str[i] == ' ' || str[i] == '\t'){
            white_flag++;
            if(str[i] == '\t')
                str[i] = ' ';
        }

        else{ // whitespace 아닌 경우
            if(white_flag >= 1){ // whitespace가 여려 개인 경우
                if(!first_char)
                    strncpy(str+idx, str+i, len-i+1);
                else if(str[i] == ',')
                    strncpy(str+idx, str+i, len-i+1), i--;
                // whitespace 만큼 당김
                else if(white_flag > 1)
                    strncpy(str+idx+1, str+i, len-i+1);
                str[idx+len-i+1] = '\0';

                len = strlen(str);
                i = idx;
            }
            // ','인 경우
            if(str[i] == ','){
                // Add a space behind of ','
                if(i+1 < 256 && (str[i+1] != ' ' && str[i+1] != '\t')){
                    char* tmp = (char*)malloc((len-i)*sizeof(char));
                    strcpy(tmp, str+i+1);
                    str[i+1] = ' ';
                    str[i+2] = '\0';
                    strcat(str,tmp);
                    len = strlen(str);
                    i--;
                }
            }
            // flag 초기화
            white_flag = 0;
            first_char = 1;
        }
        // idx 초기화
        if(white_flag == 1)
            idx = i;
    }
}

