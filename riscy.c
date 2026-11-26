#include "riscy.h"

#include <string.h>

int riscy_load(Riscy *cpu, const uint8_t *program, size_t size)
{
    if (cpu == NULL || program == NULL || size == 0 || size > sizeof cpu->memory)
        return 0;
    memset(cpu, 0, sizeof *cpu);
    memcpy(cpu->memory, program, size);
    return 1;
}

RiscyResult riscy_step(Riscy *cpu, FILE *input, FILE *output, FILE *trace)
{
    const uint8_t pc = cpu->pc;
    const uint8_t instruction = cpu->memory[pc];
    const unsigned rd = (instruction >> 2) & 3;
    const unsigned rs = instruction & 3;
    const unsigned opcode = instruction >> 4;
    uint8_t next = (uint8_t)(pc + 1);
    const char *name = NULL;

    if (cpu->halted) return RISCY_HALTED;
    if (instruction == 0x01) {
        name = "HALT";
        cpu->halted = 1;
        next = pc;
    } else if (instruction == 0x00) {
        name = "NOP";
    } else if (opcode >= 0x0c) {
        /* SLI's immediate occupies bits 5:4 and 1:0, around rd in 3:2. */
        const unsigned imm = ((instruction >> 2) & 12) | (instruction & 3);
        cpu->registers[rd] = (uint8_t)((cpu->registers[rd] << 4) | imm);
        name = "SLI";
    } else {
        switch (opcode) {
        case 1:
            cpu->registers[rd] = (uint8_t)(cpu->registers[rd] - cpu->registers[rs]);
            name = "SUB";
            break;
        case 2:
            cpu->registers[rd] = cpu->memory[cpu->registers[rs]];
            name = "LOAD";
            break;
        case 3:
            cpu->memory[cpu->registers[rs]] = cpu->registers[rd];
            name = "STORE";
            break;
        case 4:
            if (rs > 1) return RISCY_INVALID_INSTRUCTION;
            if ((rs == 0 && cpu->registers[rd] == 0) ||
                (rs == 1 && cpu->registers[rd] != 0))
                next = (uint8_t)(next + 1);
            name = rs == 0 ? "SKIPZ" : "SKIPNZ";
            break;
        case 5: {
            /* Save the target first: JALR D,D must jump to the old D. */
            const uint8_t target = cpu->registers[rs];
            cpu->registers[rd] = next;
            next = target;
            name = "JALR";
            break;
        }
        case 6:
            cpu->registers[rd] = (uint8_t)~(cpu->registers[rd] & cpu->registers[rs]);
            name = "NAND";
            break;
        case 7:
            cpu->registers[rd] = (uint8_t)(cpu->registers[rd] + cpu->registers[rs]);
            name = "ADD";
            break;
        case 8:
            switch (rs) {
            case 0:
                cpu->registers[rd] = (uint8_t)(cpu->registers[rd] + 1);
                name = "INC";
                break;
            case 1:
                cpu->registers[rd] = (uint8_t)(cpu->registers[rd] - 1);
                name = "DEC";
                break;
            case 2:
                if (output == NULL || fputc(cpu->registers[rd], output) == EOF)
                    return RISCY_IO_ERROR;
                name = "OUT";
                break;
            case 3: {
                int byte;
                if (input == NULL) return RISCY_IO_ERROR;
                byte = fgetc(input);
                if (byte == EOF) return ferror(input) ? RISCY_IO_ERROR : RISCY_INPUT_EOF;
                cpu->registers[rd] = (uint8_t)byte;
                name = "IN";
                break;
            }
            }
            break;
        default:
            return RISCY_INVALID_INSTRUCTION;
        }
    }
    cpu->pc = next;
    ++cpu->instructions;
    if (trace != NULL) {
        fprintf(trace, "PC=%02x, inst=%02x, op=%s, A=%02x, B=%02x, C=%02x, D=%02x\n",
                (unsigned)pc, (unsigned)instruction, name,
                (unsigned)cpu->registers[0], (unsigned)cpu->registers[1],
                (unsigned)cpu->registers[2], (unsigned)cpu->registers[3]);
    }
    return cpu->halted ? RISCY_HALTED : RISCY_OK;
}

RiscyResult riscy_run(Riscy *cpu, size_t limit, FILE *input, FILE *output, FILE *trace)
{
    size_t step;
    if (cpu->halted) return RISCY_HALTED;
    for (step = 0; step < limit; ++step) {
        const RiscyResult result = riscy_step(cpu, input, output, trace);
        if (result != RISCY_OK) return result;
    }
    return RISCY_STEP_LIMIT;
}

const char *riscy_result_name(RiscyResult result)
{
    switch (result) {
    case RISCY_OK: return "running";
    case RISCY_HALTED: return "halted";
    case RISCY_INVALID_INSTRUCTION: return "invalid instruction";
    case RISCY_INPUT_EOF: return "input exhausted";
    case RISCY_IO_ERROR: return "I/O error";
    case RISCY_STEP_LIMIT: return "instruction limit reached";
    }
    return "unknown result";
}
