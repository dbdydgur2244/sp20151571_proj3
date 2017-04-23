#ifndef __RUN__
#define __RUN__

typedef struct{
    int A; int X; int L; int B; int S; int T; int SW; int PC;
}REGISTER;

typedef enum{ LOAD, STORE }REG_OPTION;

int compare(int , int );
void reg_init();
void set_end_addr(int address);
void set_start_addr(int address);

void print_register();
int immediate(int xbpe, int data);
int indirect(int xbpe, int data, int byte, int option, int save);
int simple(int xbpe, int data, int byte, int option, int save);
void execute_opcode(int opcode, int ni, int xbpe, int data);
void ADD();
void ADDR();
void STA(int ni, int xbpe, int data);
void STB();
void STT();
void STS();
void STL(int ni, int xbpe, int data);
void STCH(int ni, int xbpe, int data);
void STX(int ni, int xbpe, int data);

void LDA(int ni, int xbpe, int data);
void LDB(int ni, int xbpe, int data);
void LDT(int ni, int xbpe, int data);
void LDS();
void LDX(int ni, int xbpe, int data);
void LDCH(int ni, int xbpe, int data);
void JSUB(int ni, int xbpe, int data);
void JEQ(int ni, int xbpe, int data);
void JGT();

void MUL();
void MULR();

void J(int ni, int xbpe, int data);
void JLT(int ni, int xbpe, int data);
void RSUB();
void RMO();
void COMP(int ni, int xbpe, int data);
void COMPR(int data);
void TD();  
void RD();
void WD();
void CLEAR(int );
void TIXR(int );
void SUB();
void SUBR();
int command_run();

#endif
