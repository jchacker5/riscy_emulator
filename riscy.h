#ifndef RISCY_H
#define RISCY_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t pc;
    uint8_t registers[4];
    uint8_t memory[256];
    int halted;
    size_t instructions;
} Riscy;

typedef enum {
    RISCY_OK,
    RISCY_HALTED,
    RISCY_INVALID_INSTRUCTION,
    RISCY_INPUT_EOF,
    RISCY_IO_ERROR,
    RISCY_STEP_LIMIT
} RiscyResult;

/* Loading always resets the machine; a rejected image leaves it unchanged. */
int riscy_load(Riscy *cpu, const uint8_t *program, size_t size);
RiscyResult riscy_step(Riscy *cpu, FILE *input, FILE *output, FILE *trace);
RiscyResult riscy_run(Riscy *cpu, size_t limit, FILE *input, FILE *output, FILE *trace);
const char *riscy_result_name(RiscyResult result);

#endif
