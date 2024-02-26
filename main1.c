#include <stdio.h>
#include <string.h>

// Define opcodes
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

unsigned char PC = 0;
unsigned char registers[4] = {0}; // A, B, C, D registers
unsigned char memory[256] = {0};
int debugflag = 0;

// Function prototypes
void add(unsigned char Rd, unsigned char Rs);
void sub(unsigned char Rd, unsigned char Rs);
void load(unsigned char Rd, unsigned char Rs);
void store(unsigned char Rd, unsigned char Rs);
void jalr(unsigned char Rd, unsigned char Rs);
void sli(unsigned char Rd, unsigned char imm);
void halt();
void skipz(unsigned char Rd);
void skipnz(unsigned char Rd);
void nand(unsigned char Rd, unsigned char Rs);
void inc(unsigned char Rd);
void dec(unsigned char Rd);
void in(unsigned char Rd);
void out(unsigned char Rd);
void nop();

// Debug function to print the state of the machine
void debug(const char* iname, unsigned char opcode, unsigned char Rd, unsigned char Rs, unsigned char imm) {
    if(debugflag) {
        printf("PC=%02x, inst=%02x, op=%s, A=%02x, B=%02x, C=%02x, D=%02x\n",
               PC, opcode, iname, registers[0], registers[1], registers[2], registers[3]);
    }
}

int main(int argc, char* argv[]) {
    // Check for debug flag
    if (argc > 1 && strcmp(argv[1], "d") == 0) {
        debugflag = 1;
    }

    // Load the test program into memory
    unsigned char fib[29] = {0x15, 0xc8, 0xc9, 0x10, 0xc0, 0xd2, 0x0, 0x1f, 0x7d, 0x7e, 0x15, 0x76, 0x1a, 0x1a, 0x7b, 0x$}
    unsigned char printmem[85] = {0xff, 0xff, 0x1a, 0x3b, 0xff, 0xff, 0x23, 0x20, 0xf7, 0xc4, 0x61, 0x60, 0xc5, 0xc4, 0x$}
    unsigned char conway[256] = {0xf0, 0xc1, 0xef, 0xde, 0x5b, 0x83, 0xcf, 0xcc, 0x13, 0xef, 0xff, 0x33, 0xf2, 0xc0, 0xc$}
    
    // set test_program to fib, printmem, or conway
    unsigned char* test_program = fib;

    // Copy the test program into memory
    memcpy(memory, test_program, sizeof(test_program));
    memory[0xFE] = 6; // Set 6 for fib, 2 for printmem, 3 for conway

    // Main emulator loop
    while (1) {
        unsigned char instruction = memory[PC];
        unsigned char Rs = instruction & 0x03;
        unsigned char Rd = (instruction >> 2) & 0x03;
        unsigned char opcode = instruction >> 4;

       // Decode and execute
    switch (opcode) {
        case ADD:
            add(Rd, Rs);
            break;
        case SUB:
            sub(Rd, Rs);
            break;
        case LOAD:
            load(Rd, Rs);
            break;
        case STORE:
            store(Rd, Rs);
            break;
        case SKIP:
            if (instruction & 0x01) { // Assuming that the LSB indicates SKIPNZ
                skipnz(Rd);
            } else { // Otherwise, it's SKIPZ
                skipz(Rd);
            }
            break;
        case JALR:
            jalr(Rd, Rs);
            break;
        case SLI:
            unsigned char imm = instruction & 0x0F;
            sli(Rd, imm);
            break;
        case NAND:
            nand(Rd, Rs);
            break;
        case INC: // Assuming INC represents the shared opcode for these operations
            switch (Rs) {
                case 0x00:
                    inc(Rd); // Increment the value in register Rd
                    break;
                case 0x01:
                    dec(Rd); // Decrement the value in register Rd
                    break;
                case 0x02:
                    out(Rd); // Output the value in register Rd
                    break;
                case 0x03:
                    in(Rd); // Input a value into register Rd
                    break;
                default:
                    printf("Error: Unrecognized Rs value %02x for opcode %02x\n", Rs, INC);
                    break;
            }
            break;
        case NOP:
            nop();
            break;
        case HALT:
            halt();
            printf("HALT encountered. Program stopped.\n");
            return 0; // Exit the emulator
        default:
            printf("Unknown opcode: %02x\n", opcode);
            return 1; // Exit with error
    }


        PC++; // Move to the next instruction
        if (PC == 0) {
            // This check prevents an infinite loop if HALT is never reached.
            printf("Infinite loop detected. Program stopped.\n");
            break;
        }
    }

    // Print the result
    printf("Result at memory[0xFF]: %d\n", memory[0xFF]);
    return 0; // Should not reach here
}

// Define functions for each instruction

void add(unsigned char Rd, unsigned char Rs) {
    registers[Rd] += registers[Rs];
    debug("ADD", ADD, Rd, Rs, 0);
}

void sub(unsigned char Rd, unsigned char Rs) {
    registers[Rd] -= registers[Rs];
    debug("SUB", SUB, Rd, Rs, 0);
}

void load(unsigned char Rd, unsigned char Rs) {
    registers[Rd] = memory[registers[Rs]];
    debug("LOAD", LOAD, Rd, Rs, 0);
}

void store(unsigned char Rd, unsigned char Rs) {
    memory[registers[Rs]] = registers[Rd];
    debug("STORE", STORE, Rd, Rs, 0);
}

void jalr(unsigned char Rd, unsigned char Rs) {
    unsigned char temp = registers[Rs];
    registers[Rd] = PC + 1;
    PC = temp - 1; // Adjusted for zero-based indexing
    debug("JALR", JALR, Rd, Rs, 0);
}

void sli(unsigned char Rd, unsigned char imm) {
    registers[Rd] = (registers[Rd] & 0xF0) | (imm & 0x0F);
    debug("SLI", SLI, Rd, 0, imm);
}

void halt() {
    debug("HALT", HALT, 0, 0, 0);
}

void nand(unsigned char Rd, unsigned char Rs) {
    registers[Rd] = ~(registers[Rd] & registers[Rs]);
    debug("NAND", NAND, Rd, Rs, 0);
}

void inc(unsigned char Rd) {
    registers[Rd]++;
    debug("INC", INC, Rd, 0, 0);
}

void dec(unsigned char Rd) {
    registers[Rd]--;
    debug("DEC", DEC, Rd, 0, 0);
}

void in(unsigned char Rd) {
    char input;
    scanf(" %c", &input);
    registers[Rd] = (unsigned char)input;
    debug("IN", IN, Rd, 0, 0);
}

void out(unsigned char Rd) {
    printf("%c", registers[Rd]);
    debug("OUT", OUT, Rd, 0, 0);
}

void nop() {
    debug("NOP", NOP, 0, 0, 0);
}

void debug(const char* operation, unsigned char opcode, unsigned char Rd, unsigned char Rs, unsigned char imm) {
    if (debugflag) {
        printf("PC=%x, inst=%x, op=%s, A=%x, B=%x, C=%x, D=%x\n",
               PC, opcode, operation, registers[0], registers[1], registers[2], registers[3]);
    }
}

void skipz(unsigned char Rd) {
    if (registers[Rd] == 0) {
        PC++;
    }
    debug("SKIPZ", SKIP, Rd, 0, 0);
}

void skipnz(unsigned char Rd) {
    if (registers[Rd] != 0) {
        PC++;
    }
    debug("SKIPNZ", SKIP, Rd, 0, 0);
};