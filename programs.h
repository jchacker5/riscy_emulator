#ifndef RISCY_PROGRAMS_H
#define RISCY_PROGRAMS_H

#include <stdint.h>

/* Course multiplication example, preserved from project_description.md.
   Multiplies memory[0xfe] by 3; stores the byte result at memory[0xff]. */
static const uint8_t multiply_program[] = {
    0xff, 0xfe, 0x23, 0x15, 0xc8, 0xcb, 0xcd, 0xcd, 0x49, 0x5f, 0x74,
    0xcc, 0xcd, 0x1b, 0xcc, 0xde, 0x5f, 0xff, 0xff, 0x37, 0x01
};

/* Reconstructed Fibonacci example (the original committed array was truncated).
   A=F(i), B=F(i+1), C=iterations remaining; D is the temporary/jump target.
   00: D=0xfe; C=memory[D]; B=1
   04: D=17; SKIPNZ C; JALR D,D   (exit when C==0)
   08: D=A; A=B; B+=D; C--
   14: D=4; JALR D,D             (loop)
   17: D=0xff; memory[D]=A; HALT */
static const uint8_t fibonacci_program[] = {
    0xff, 0xfe, 0x2b, 0xc5, 0xcd, 0xcd, 0x49, 0x5f, 0x1f, 0x7c, 0x10,
    0x71, 0x77, 0x89, 0xcc, 0xdc, 0x5f, 0xff, 0xff, 0x33, 0x01
};

#endif
