#include <stdio.h>
#include <string.h>

#define ADD 0b0111
#define SUB 0b0001
#define LOAD 0b0010
#define STORE 0b0011
#define SKIP 0b0100
#define JALR 0b0101
#define SLI 0b1100
#define NAND 0b0110
#define INC 0b1000
#define DEC 0b1000
#define IN 0b1000
#define OUT 0b1000
#define NOP 0b0000
#define HALT 0b00000001

unsigned char PC;
unsigned char registers[4]; // A, B, C, D
unsigned char memory[256];
int debugflag = 0;



void debug(char* iname, unsigned char opcode);

int main(int argc, char* argv[])
{
        registers[0] = 0;
        registers[1] = 0;
        registers[2] = 0;
        registers[3] = 0;

        if (argc > 1) {
                if (strcmp(argv[1], "d") == 0) {
                        debugflag = 1;
                } else {
                        debugflag = 0;
                }
        }

        unsigned char fib[29] = {0x15, 0xc8, 0xc9, 0x10, 0xc0, 0xd2, 0x0, 0x1f, 0x7d, 0x7e, 0x15, 0x76, 0x1a, 0x1a, 0x7b, 0x$
        unsigned char printmem[85] = {0xff, 0xff, 0x1a, 0x3b, 0xff, 0xff, 0x23, 0x20, 0xf7, 0xc4, 0x61, 0x60, 0xc5, 0xc4, 0x$
        unsigned char conway[256] = {0xf0, 0xc1, 0xef, 0xde, 0x5b, 0x83, 0xcf, 0xcc, 0x13, 0xef, 0xff, 0x33, 0xf2, 0xc0, 0xc$

        int i;
        for (i = 0; i < sizeof(printmem); i++) {
                memory[i] = printmem[i];
        }

        memory[0xFE] = 2
        
        //for (int k = 0; k < 1000000; k++) {
        while (1) {

        unsigned char instruction = memory[PC];
        unsigned char Rs = (instruction & 0b00000011); // instruction & 3
        unsigned char Rd = (instruction & 0b00001100) >> 2; // (instruction >> 2) & 3
        unsigned char temp = instruction & 0b00110011; // Used for imm
        unsigned char imm = (temp >> 2 | temp) & 0b00001111; // (temp >> | temp) & 15
        unsigned char opcode = (instruction & 0b11110000) >> 4; // (instruction & 0xf0) >> 4

        char* iname;
        if (opcode == ADD) {
                // ADD
                registers[Rd] = registers[Rd] + registers[Rs];

                if (debugflag) {
                        iname = "ADD";
                        debug(iname, opcode);
                }
        } else if (opcode == SUB) {
                // SUB
                registers[Rd] = registers[Rd] - registers[Rs];

                if (debugflag) {
                        iname = "SUB";
                        debug(iname, opcode);
                }
        } else if (opcode == LOAD) {
                // LOAD
                registers[Rd] = memory[registers[Rs]];

                if (debugflag) {
                        iname = "LOAD";
                        debug(iname, opcode);
                }
        } else if (opcode == STORE) {
                // STORE
                memory[registers[Rs]] = registers[Rd];

                if (debugflag) {
                        iname = "STORE";
                        debug(iname, opcode);
                }
        } else if (opcode == SKIP) {
                // SKIPZ OR SKIPNZ
                if (Rs == 0 && registers[Rd] == 0) {
                        // SKIPZ
                        PC += 1;
                        iname = "SKIPZ";
                } else if (Rs == 1 && registers[Rd] != 0) {
                        // SKIPNZ
                        PC += 1;
                        iname = "SKIPNZ";
                }

                iname = "SKIP";

				if (debugflag) {
                        debug(iname, opcode);
                }
        } else if (opcode == JALR) {
                // JALR
                unsigned char temp = registers[Rs];
                registers[Rd] = PC + 1;
                PC = temp - 1;

                if (debugflag) {
                        iname = "JALR";
                        debug(iname, opcode);
                }
        } else if ((opcode & SLI) == SLI) {
                // SLI
                unsigned char temp = registers[Rd];
                temp = (temp << 4) & 0b11110000;
                temp = temp | imm;
                registers[Rd] = temp;

                if (debugflag) {
                        iname = "SLI";
                        debug(iname, opcode);
                }
        } else if (instruction == HALT) {
                // HALT
                if (debugflag) {
                        iname = "HALT";
                        debug(iname, opcode);
                }
                break;
                
        } else if (opcode == NAND) {
                // NAND
                unsigned char nand = ~(registers[Rs] & registers[Rd]);
				registers[Rd] = nand;

                if (debugflag) {
                        iname = "NAND";
                        debug(iname, opcode);
                }
        } else if (opcode == IN && Rs == 3) {
                // IN
                unsigned char userInput;
                scanf("%c", &userInput);
                registers[Rd] = '0'; // - '0';
                printf("%d", registers[Rd]);

                if (debugflag) {
                        iname = "IN";
                        debug(iname, opcode);
                }
        } else if (opcode == OUT && Rs == 2) {
                // OUT
                printf("%c", registers[Rd]);
                if (registers[Rd] == 0x0d)
                        printf("\n");                  
                                                        
                if (debugflag) {
                        iname = "OUT";
                        debug(iname, opcode);
                }
        } else if (opcode == DEC && Rs == 1) {
                // DEC
                registers[Rd] = registers[Rd] - 1;

                if (debugflag) {
                        iname = "DEC";
                        debug(iname, opcode);
                }
		} else if (opcode == INC && Rs == 0) {
                // INC
                unsigned char temp = registers[Rd] + 1;
                registers[Rd] = temp;

                if (debugflag) {
                        iname = "INC";
                        debug(iname, opcode);
                }
        } else if (opcode == NOP && Rd == 0 && Rs == 0) {
                // NOP
                if (debugflag) {
                        iname = "NOP";
                        debug(iname, opcode);
                }
                ;
        }

        PC++;
        }

        return 0;
}

void debug(char* iname, unsigned char opcode)
{
    printf("PC=%x, inst=%x, op=%s, A=%x, B=%x, C=%x, D=%x\n", PC, opcode, iname, registers[0], registers[1], registers[2], registers[3]);
}
