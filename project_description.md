Problem 2:  Write a program that emulates this instruction set and runs your program from problem 1.  You may use any language you want, but I *strongly* recommend C or C++ so you can use the unsigned char datatype.

I recommend you build this emulator as follows:

Step 1:  Setup the variables

You will need global variables to hold the PC, A, B, C, D registers and the memory.  I recommend that you make them all unsigned char, a datatype that only store 8-bits, which means you don't have to worry about what happens when a program adds 250+10.   Otherwise you will need to make sure that the value in all registers and memory never exceeds 255 and is never less than 0.

You should make memory an array with 256 elements.  Also consider making the registers an array unsigned char registers[4] rather than defining them seperately.

In main() set all the registers to 0.

Step 2:  Load the test program

In main(), use a for loop to copy this test program into your memory array, starting at 0.

unsigned char program[21] = {0xff,0xfe,0x23,0x15,0xc8,0xcb,0xcd,0xcd,0x49,0x5f,0x74,0xcc,0xcd,0x1b,0xcc,0xde,0x5f,0xff,0xff,0x37,0x01};

Then write memory[0xfe] = 2;

At the end of main, print out memory[0xff].  Once you get your emulator working correctly, it should print out 6 at the end.

The test program multiplies 2 * 3.  The code for it and my emulator printout is at the end of the assignment.

Step 3:  Read an instruction and increment PC

Do this in main, before you print memory[0xff].  Make a temporary instruction variable and read from memory[PC].

Step 4:  Extract the fields

Make some temporary variables to hold rs, rd, imm, opcode, and extract those values from your instruction variable.  The & and >> operations (logical AND, bit shift-right) will be essential here.  For example, since rd is bits 3 and 2 of the instruction, you would write:
unsigned char rd = (instruction >> 2) & 3;

Step 5:  Write an if / else if for each instruction

NOTE: be careful with JALR.  JALR D,D should jump to the current value of D (PC=D) while saving the return address in D (D=PC+1).  You'll need a temporary variable.

Step 6:  Put a debugging statement after your series of if's

I used printf("PC=%x, inst=%x, op=%s, A=%x, B=%x, C=%x, D=%x\n",PC,instruction,iname,register[0],register[1],register[2],register[3]);  I also make a string iname variable and set it to "ADD", "SLI", etc based on the instruction.

Step 7:  Put everything from Steps 3-6 in a for loop

Technically your emulator should loop forever, but for debugging you'll only want to run 10 or 20 instructions until everything looks correct.

Step 8:  Debug!!!

Compare each line of your printout with my solution printout at the end of this document.    Make sure register content is identical!

Step 9:  Put in your fibonacci program

Then set memory[0xfe] to 6.  Do you get 8 at the end?

