#include "run.h"
#include "debug.h"
#include "memory.h"
#include "opcode.h"
REGISTER reg;

int curr = -1 & 0xFFFFFF;
int end = -1 & 0xFFFFFF;
int bp = 0;

int compare(int a, int b){
    if ( a > b)
        return 1;
    else if ( a == b)
        return 0;
    return -1;
}

void reg_init(){
    reg.A = reg.X = reg.L = reg.B = 
        reg.S = reg.T = reg.SW = reg.PC = 0;
}

int command_run(){
    int value;
    int ni;
    int xbpe;
    int ext_flag;
    Hnode opcode;

    reg.PC = curr;
    bp = get_nextbp(reg.PC);
    if ( bp == 0 )
        return 0;
    if(bp < reg.PC ) 
        bp = -1;

    printf("%d %d %d\n", reg.PC, bp, end);
    // end of program 또는 end of memory까지
    while( reg.PC < end && reg.PC <=0xFFFFF) {
        ext_flag = 0;
        xbpe = 0;
        ni = 0;
        value = 0;
        printf("PC : %06X\n", reg.PC);
        print_register();

        if ( bp != -1 && reg.PC >= bp ){
            print_register();
            printf("\tStop at checkpoint[%04X]\n",bp);
            return 0;
        }

        value = get_memory(reg.PC);
        reg.PC++;

        opcode = nopcode_find(value & 0xFC); // 6bit 비교
        if(opcode->format >= 1){
            if(opcode->format == 3){ // format 3
                ni = value & 0x3; // 2bit 비교
                if(ni != 0){
                    value = get_memory(reg.PC);
                    xbpe = value >> 4; // nixbpe중xbpe만가지고 옴. 
                    if(xbpe & 0x01) // e = 1
                        ext_flag = 1;
                    value &= 0x0F;
                    reg.PC++;
                    value <<= 8;
                    value += get_memory(reg.PC);
                    reg.PC++;
                    if ( ext_flag ){ // format 4
                        value <<= 8; // 3 bit 추가
                        value += get_memory(reg.PC);
                        reg.PC++;
                    }
                }
            }

            /**** format 2 ****/
            else if(opcode->format == 2){
                value = get_memory(reg.PC);
                reg.PC++;
            }
            execute_opcode(opcode->n_opcode, ni, xbpe, value);
        }
        curr = reg.PC;
    }

    print_register();
    printf("\tEnd Program\n");
    currbp_init();
    return 0;
}

void set_start_address(int address){
    curr = address;
}

void set_end_address(int address){
    end = address;
}

int immediate(int xbpe, int data){
    int value = 0;
    if(!(xbpe & 0x01) && (data & 0x800)) // e = 0 and 
        data |= 0xFFFFF000;

    if ( xbpe == 0 ) // value = disp
        value = data;
    else if ( xbpe & 0x01 ) // e = 1, extend and value = addr
        value = data;
    else if ( xbpe & 0x02 ) // p = 1, pc relocative
        value = (data + reg.PC);
    else if ( xbpe & 0x04 ) // b = 1, base relocative
        value = ( data + reg.B);
    return value;
}

int indirect(int xbpe, int data, int byte, int option, int reg_value){
    int value;
    int address; 
    if( xbpe & 0x01 ) // e = 1, extend and value = addr
        address = simple(xbpe, data, 5, LOAD, 0);

    else  // value = disp
        address = simple(xbpe, data, byte, LOAD, 0);

    if(byte == 1) { // CH 인 경우
        if(option == LOAD){
            if(address <= 0xFFFFF)
                value = get_memory(address);
            return value;
        }
        else{
            if(address <= 0xFFFFF)
                set_memory(address, reg_value);
            return 0;
        }
    }

    if(option == LOAD){
        for ( int i = 0; i <= 2; ++i){
            value = 0;
            if ( address + i <= 0xFFFFF ){
                value <<= 8; 
                value += get_memory(address + i);
            }
        }
    }

    else{
        for ( int i = 0; i  <= 2; ++i)
            if (address + i <= 0xFFFFF )
                set_memory(address, ( (reg_value >> (2 - i) * 8) & 0xFF ) );
        return 0;
    }
    return value;
}

int simple(int xbpe, int data, int byte, int option, int reg_value){
    int value = 0;
    int address = data;

    if ( xbpe & 0x01 )
        byte = 5;

    if ( byte == 3)
        if(data & 0x800) // if negative
            data |= 0xFFFFF000;

    if ( xbpe & 0x08 ) // x = 1, index
        address = data + reg.X;
    if ( xbpe & 0x02 ) // p = 1, pc relative
        address += reg.PC;
    else if( xbpe & 0x04 ) // b = 1, base relative
        address += reg.B;

    if(byte == 1) { // CH 인 경우
        if(option == LOAD){
            if(address <= 0xFFFFF)
                value = get_memory(address);
            return value;
        }
        else{
            if(address <= 0xFFFFF)
                set_memory(address, reg_value);
            return 0;
        }
    }

    if(option == LOAD){
        for ( int i = 0; i <= 2; ++i){
            value = 0;
            if ( address + i <= 0xFFFFF ){
                value <<= 8; 
                value += get_memory(address + i);
            }
        }
    }

    else{
        for ( int i = 0; i  <= 2; ++i)
            if (address + i <= 0xFFFFF )
                set_memory(address, ( (reg_value >> (2 - i) * 8) & 0xFF ) );
        return 0;
    }
    return value;
}

void ADD(int ni, int xbpe, int data){
    int value;
    if ( ni == 2)
        value = indirect(xbpe, data, 3, LOAD, reg.A);
    else if ( ni == 3)
        value = simple(xbpe, data, 3, LOAD, reg.A);
    reg.A += value; 
}

void ADDR(int ni, int xbpe, int data){
    int *value1, *value2;
    switch(data >> 4){
        case 0:
            value1 = &reg.A;
            break;
        case 2:
            value1 = &reg.L;
            break;
        case 3:
            value1 = &reg.B;
            break;
        case 4:
            value1 = &reg.S;
            break;
        case 5:
            value1 = &reg.T;
            break;
        default:
            break;
    }

    switch(data & 0x0F){
        case 0:
            value2 = &reg.A;
            break;
        case 2:
            value2 = &reg.L;
            break;
        case 3:
            value2 = &reg.B;
            break;
        case 4:
            value2 = &reg.S;
            break;
        case 5:
            value2 = &reg.T;
            break;
        default:
            break;
    }
    *value2 += *value1;
}

void STA(int ni, int xbpe, int data){
    if ( ni == 2)
        indirect(xbpe, data, 3, STORE, reg.A);
    else if ( ni == 3 )
        simple(xbpe, data, 3, STORE, reg.A);
}

void STB(int ni, int xbpe, int data){
    if ( ni == 2)
        indirect(xbpe, data, 3, STORE, reg.B);
    else if ( ni == 3 )
        simple(xbpe, data, 3, STORE, reg.B);
}

void STT(int ni, int xbpe, int data){
    if ( ni == 2)
        indirect(xbpe, data, 3, STORE, reg.T);
    else if ( ni == 3 )
        simple(xbpe, data, 3, STORE, reg.T);
}

void STS(int ni, int xbpe, int data){
    if ( ni == 2)
        indirect(xbpe, data, 3, STORE, reg.S);
    else if ( ni == 3 )
        simple(xbpe, data, 3, STORE, reg.S);
}

void STL(int ni, int xbpe, int data){
    if ( ni == 2 )
        indirect(xbpe, data, 3, STORE, reg.L);
    else if ( ni == 3 )
        simple(xbpe, data, 3, STORE, reg.L);
}

void STCH(int ni, int xbpe, int data){
    if ( ni == 2 )
        indirect(xbpe, data, 1, STORE, reg.A & 0xFF);
    else if(ni == 3)
        simple(xbpe, data, 1, STORE, reg.A & 0xFF);
}

void STX(int ni, int xbpe, int data){
    if(ni == 2)
        indirect(xbpe, data, 3, STORE, reg.X);
    else if(ni == 3)
        simple(xbpe, data, 3, STORE, reg.X);
}

void LDA(int ni, int xbpe, int data){
    if(ni == 1)
        reg.A = immediate(xbpe, data);
    else if(ni == 2)
        reg.A = indirect(xbpe, data, 3, LOAD, 0);
    else if(ni == 3)
        reg.A = simple(xbpe, data, 3, LOAD, 0);
}

void LDB(int ni, int xbpe, int data){
    if(ni == 1)
        reg.B = immediate(xbpe, data);
    else if(ni == 2)
        reg.B = indirect(xbpe, data, 3, LOAD, 0);
    else if(ni == 3)
        reg.B = simple(xbpe, data, 3, LOAD, 0);
}

void LDL(int ni, int xbpe, int data){
    if(ni == 1)
        reg.L = immediate(xbpe, data);
    else if(ni == 2)
        reg.L = indirect(xbpe, data, 3, LOAD, 0);
    else if(ni == 3)
        reg.L = simple(xbpe, data, 3, LOAD, 0);
}

void LDT(int ni, int xbpe, int data){
    if(ni == 1)
        reg.T = immediate(xbpe, data);
    else if(ni == 2)
        reg.T = indirect(xbpe, data, 3, LOAD, 0);
    else if(ni == 3)
        reg.T = simple(xbpe, data, 3, LOAD, 0);
}

void LDX(int ni, int xbpe, int data){
    if(ni == 1)
        reg.X = immediate(xbpe, data);
    else if(ni == 2)
        reg.X = indirect(xbpe, data, 3, LOAD, 0);
    else if(ni == 3)
        reg.X = simple(xbpe, data, 3, LOAD, 0);
}

void LDS(int ni, int xbpe, int data){
    if(ni == 1)
        reg.S = immediate(xbpe, data);
    else if(ni == 2)
        reg.S = indirect(xbpe, data, 3, LOAD, 0);
    else if(ni == 3)
        reg.S = simple(xbpe, data, 3, LOAD, 0);
}

void LDCH(int ni, int xbpe, int data){
    if(ni == 1)
        reg.A = immediate(xbpe, data);
    else if(ni == 2)
        reg.A = indirect(xbpe, data, 1, LOAD, 0);
    else if(ni == 3)
        reg.A = simple(xbpe, data, 1, LOAD, 0);
}

void JSUB(int ni, int xbpe, int data){
    int value;
    if(ni == 2)
        value = simple(xbpe, data, 3, LOAD, 0);
    if(ni == 3)
        value = immediate(xbpe, data);
    reg.L = reg.PC;
    reg.PC = value;
    currbp_init();
    bp = get_nextbp(reg.PC);
}

void JEQ(int ni, int xbpe, int data){
    int value;
    if(reg.SW == 0){
        if(ni == 2)
            value = simple(xbpe, data, 3, LOAD, 0);
        if(ni == 3)
            value = immediate(xbpe, data);
        reg.PC = value;
        currbp_init();
        bp = get_nextbp(reg.PC);
    }
}

void J(int ni, int xbpe, int data){
    int value;	
    if(ni == 2)
        value = simple(xbpe, data, 3, LOAD, 0);
    if(ni == 3)
        value = immediate(xbpe, data);

    reg.PC = value;
    currbp_init();
    bp = get_nextbp(reg.PC);
}

void JLT(int ni, int xbpe, int data){
    int value;
    if(reg.SW < 0){
        if(ni == 2)
            value = simple(xbpe, data, 3, LOAD, 0);
        if(ni == 3)
            value = immediate(xbpe, data);

        reg.PC = value;
        currbp_init();
        bp = get_nextbp(reg.PC);
    }
}

void RSUB(){
    reg.PC = reg.L;
    currbp_init();
    bp = get_nextbp(reg.PC);	
}

void COMP(int ni, int xbpe, int data){
    int value;
    if(ni == 1)
        value = immediate(xbpe, data);
    else if(ni == 2)
        value = indirect(xbpe, data, 3, LOAD, 0);
    else if(ni == 3)
        value = simple(xbpe, data, 3, LOAD, 0);

    reg.SW = compare(reg.A, value);
}

void COMPR(int data){
    int value1, value2;
    switch(data >> 4){
        case 0:
            value1 = reg.A;
            break;
        case 2:
            value1 = reg.L;
            break;
        case 3:
            value1 = reg.B;
            break;
        case 4:
            value1 = reg.S;
            break;
        case 5:
            value1 = reg.T;
            break;
        default:
            break;
    }

    switch(data & 0x0F){
        case 0:
            value2 = reg.A;
            break;
        case 2:
            value2 = reg.L;
            break;
        case 3:
            value2 = reg.B;
            break;
        case 4:
            value2 = reg.S;
            break;
        case 5:
            value2 = reg.T;
            break;
        default:
            break;
    }

    reg.SW = compare(value1, value2);
}

void TD(){
    reg.SW = 1;
}


void RD(){

}

void WD(){
    reg.SW = -2;
}

void CLEAR(int data){
    switch(data >> 4){
        case 0:
            reg.A = 0;
            break;
        case 1:
            reg.X = 0;
            break;
        case 2:
            reg.L = 0;
            break;
        case 3:
            reg.B = 0;
            break;
        case 4:
            reg.S = 0;
            break;
        case 5:
            reg.T = 0;
            break;
        default:
            break;
    }
}

void TIXR(int data){
    int value;
    reg.X++;
    switch(data >> 4){
        case 0:
            value = reg.A;
            break;
        case 2:
            value = reg.L;
            break;
        case 3:
            value =	reg.B;
            break;
        case 4:
            value =	reg.S;
            break;
        case 5:
            value =	reg.T;
            break;
        default:
            break;
    }
    reg.SW = compare(reg.X, value);
}

void print_register(){
    printf("\t\tA  : %06X X  : %06X\n\t\tL  : %06X PC : %06X\n\t\tB  : %06X S  : %06X\n\t\tT  : %06X\n",
            reg.A, reg.X, reg.L, reg.PC, reg.B, reg.S, reg.T);
}

void execute_opcode(int opcode, int ni, int xbpe, int data){
    switch(opcode){
        case 0x18:
            ADD(ni, xbpe, data);
            break;
        
        case 0x90:
            ADDR(ni,xbpe, data);
            break;
        case 0x00:
            LDA(ni, xbpe, data);
            break;

        case 0x68:
            LDB(ni, xbpe, data);
            break;

        case 0x74:
            LDT(ni, xbpe, data);
            break;

        case 0x08:
            LDL(ni, xbpe, data);
            break;

        case 0x6C:
            LDS(ni, xbpe, data);
            break;

        case 0x04:
            LDX(ni, xbpe, data);
            break;

        case 0x50:
            LDCH(ni, xbpe, data);
            break;

        case 0x0C:
            STA(ni, xbpe, data);
            break;

        case 0x14:
            STL(ni, xbpe, data);
            break;

        case 0x10:
            STX(ni, xbpe, data);
            break;

        case 0x54:
            STCH(ni, xbpe, data);
            break;

        case 0x48:
            JSUB(ni, xbpe, data);
            break;

        case 0x30:
            JEQ(ni, xbpe, data);
            break;

        case 0x38:
            JLT(ni, xbpe, data);
            break;

        case 0x3C:
            J(ni, xbpe, data);
            break;

        case 0x28:
            COMP(ni, xbpe, data);
            break;

        case 0xE0:
            TD();
            break;

        case 0xD8:
            RD();
            break;

        case 0x4C:
            RSUB();
            break;

        case 0xDC:
            WD();
            break;

        case 0xB4:
            CLEAR(data);
            break;

        case 0xA0:
            COMPR(data);
            break;

        case 0xB8:
            TIXR(data);
            break;
    }
}
