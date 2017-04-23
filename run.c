#include "run.h"
#include "debug.h"
#include "memory.h"
#include "opcode.h"
REGISTER reg;

int curr = 0xFFFFFF;
int end = 0xFFFFFF;
int bp;

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
	int data;
	int ni;
	int xbpe;
	int ext_flag;
    Hnode opcode;

	reg.PC = curr;
	bp = get_nextbp(reg.PC);

	if(bp < reg.PC) 
		bp = -1;

	/**** execute until meet breakpoint or end of program or end of memory ****/
	while(reg.PC < end && reg.PC <=0xFFFFF){
		ext_flag = 0;
		xbpe = 0;
		ni = 0;
		data = 0;

		if(bp != -1 && reg.PC >= bp){
			print_register();
			printf("\tStop at checkpoint[%04X]\n",bp);
			return 0;
		}

		data = get_memory(reg.PC);
		reg.PC++;

		opcode = nopcode_find(data & 0xFC); // 6bit 비교
		if(opcode->format >= 1){
			
            if(opcode->format == 3){ // format 3
				ni = data & 0x3; // 2bit 비교
				if(ni != 0){
					xbpe = get_memory(reg.PC) >> 4;
					if(xbpe & 0x01)
                        ext_flag = 1;

					data = get_memory(reg.PC) & 0x0F;
					reg.PC++;

					data = data << 8;
					data += get_memory(reg.PC);
					reg.PC++;

					if ( ext_flag ){ // format 4
						data = data << 8;					
						data += get_memory(reg.PC);
						reg.PC++;
					}
				}
			}

			/**** format 2 ****/
			else if(opcode->format == 2){
				data = get_memory(reg.PC);
				reg.PC++;
			}
			execute_opcode(opcode->n_opcode, ni, xbpe, data);
		}
		curr = reg.PC;	
	}

	print_register();
	printf("\tEnd Program\n");
	//init_curr_bp();
	return 0;
}

void STA(int ni, int xbpe, int data){
	if ( ni == 2)
		indirect(xbpe, data, 3, STORE, reg.A);
	else if ( ni == 3 )
		simple(xbpe, data, 3, STORE, reg.A);
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
	init_curr_bp();
	bp = get_next_bp(reg.PC);
}

void JEQ(int ni, int xbpe, int data){
	int value;
	if(reg.SW == 0){
		if(ni == 2)
			value = simple(xbpe, data, 3, LOAD, 0);
		if(ni == 3)
			value = immediate(xbpe, data);
		reg.PC = value;
		init_curr_bp();
		bp = get_next_bp(reg.PC);
	}
}

void J(int ni, int xbpe, int data){
	int value;	
	if(ni == 2)
		value = simple(xbpe, data, 3, LOAD, 0);
	if(ni == 3)
		value = immediate(xbpe, data);

	reg.PC = value;
	init_curr_bp();
	bp = get_next_bp(reg.PC);
}

void JLT(int ni, int xbpe, int data){
	int value;
	if(reg.SW < 0){
		if(ni == 2)
			value = simple(xbpe, data, 3, LOAD, 0);
		if(ni == 3)
			value = immediate(xbpe, data);

		reg.PC = value;
		init_curr_bp();
		bp = get_next_bp(reg.PC);
	}
}

void RSUB(){
	reg.PC = reg.L;
	init_curr_bp();
	bp = get_next_bp(reg.PC);	
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
	}

    reg.SW = compare(value1, value2);
}

void TD(){
    reg.SW = 1;
}


void RD(){

}

void WD(){
    reg.SW = -1;
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
	}
    reg.SW = compare(reg.X, value);
}
