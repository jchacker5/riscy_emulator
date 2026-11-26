#include "riscy.h"
#include "programs.h"

#include <stdlib.h>
#include <string.h>

static unsigned checks;
#define CHECK(condition) do { ++checks; if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); exit(1); \
} } while (0)

static Riscy machine(uint8_t instruction)
{
    Riscy cpu;
    CHECK(riscy_load(&cpu, &instruction, 1));
    return cpu;
}

static void test_sli(void)
{
    unsigned rd, imm, value;
    for (rd = 0; rd < 4; ++rd)
        for (imm = 0; imm < 16; ++imm)
            for (value = 0; value < 256; ++value) {
                const uint8_t code = (uint8_t)(0xc0 | ((imm & 12) << 2) | (rd << 2) | (imm & 3));
                Riscy cpu = machine(code);
                cpu.registers[rd] = (uint8_t)value;
                CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
                CHECK(cpu.registers[rd] == (uint8_t)((value << 4) | imm));
                CHECK(cpu.pc == 1);
            }
}

static void test_arithmetic(void)
{
    Riscy cpu = machine(0x71); /* ADD A,B */
    cpu.registers[0] = 250; cpu.registers[1] = 10;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.registers[0] == 4);
    cpu = machine(0x11); /* SUB A,B */
    cpu.registers[0] = 0; cpu.registers[1] = 1;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.registers[0] == 255);
    cpu = machine(0x61); /* NAND A,B */
    cpu.registers[0] = 0xf0; cpu.registers[1] = 0xaa;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.registers[0] == 0x5f);
    cpu = machine(0x80); cpu.registers[0] = 255;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.registers[0] == 0);
    cpu.memory[1] = 0x81;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.registers[0] == 255);
}

static void test_control_flow(void)
{
    Riscy cpu = machine(0x01);
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_HALTED);
    CHECK(cpu.pc == 0 && cpu.instructions == 1);
    CHECK(riscy_run(&cpu, 0, NULL, NULL, NULL) == RISCY_HALTED);
    CHECK(cpu.instructions == 1);
    cpu = machine(0x5f); cpu.registers[3] = 0;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.pc == 0 && cpu.registers[3] == 1);
    cpu = machine(0x00); cpu.pc = 255; cpu.memory[255] = 0x00;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.pc == 0);
    cpu.memory[0] = 0x40;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.pc == 2);
    cpu.memory[2] = 0x41;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.pc == 3);
    cpu.memory[3] = 0x41; cpu.registers[0] = 7;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.pc == 5);
    cpu.pc = 255; cpu.memory[255] = 0x41;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.pc == 1);
    cpu = machine(0x00);
    CHECK(riscy_run(&cpu, 513, NULL, NULL, NULL) == RISCY_STEP_LIMIT);
    CHECK(cpu.instructions == 513 && cpu.pc == 1);
}

static void test_memory(void)
{
    uint8_t image[257];
    Riscy cpu, before;
    memset(image, 0xa5, sizeof image);
    CHECK(riscy_load(&cpu, image, 256));
    CHECK(memcmp(cpu.memory, image, 256) == 0);
    before = cpu;
    CHECK(!riscy_load(&cpu, image, 257));
    CHECK(memcmp(&cpu, &before, sizeof cpu) == 0);
    CHECK(!riscy_load(&cpu, image, 0));
    CHECK(!riscy_load(&cpu, NULL, 1));
    cpu = machine(0x23); cpu.registers[3] = 255; cpu.memory[255] = 98;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.registers[0] == 98);
    cpu.memory[1] = 0x33; cpu.registers[0] = 45;
    CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_OK);
    CHECK(cpu.memory[255] == 45);
}

static void test_io_and_errors(void)
{
    const unsigned char bytes[] = {' ', '\n', 0, 255};
    FILE *input = tmpfile(), *output = tmpfile(), *trace = tmpfile();
    Riscy cpu;
    size_t i;
    char line[128];
    CHECK(input != NULL && output != NULL && trace != NULL);
    CHECK(fwrite(bytes, 1, sizeof bytes, input) == sizeof bytes);
    rewind(input);
    for (i = 0; i < sizeof bytes; ++i) {
        cpu = machine(0x83); /* IN A */
        CHECK(riscy_step(&cpu, input, output, trace) == RISCY_OK);
        CHECK(cpu.registers[0] == bytes[i]);
        cpu.memory[1] = 0x82; /* OUT A */
        CHECK(riscy_step(&cpu, input, output, trace) == RISCY_OK);
    }
    rewind(output);
    for (i = 0; i < sizeof bytes; ++i) CHECK(fgetc(output) == bytes[i]);
    CHECK(fgetc(output) == EOF); /* IN must not echo input or add CR/LF bytes. */
    cpu = machine(0x83); cpu.registers[0] = 77;
    CHECK(riscy_step(&cpu, input, output, NULL) == RISCY_INPUT_EOF);
    CHECK(cpu.pc == 0 && cpu.registers[0] == 77 && cpu.instructions == 0);
    CHECK(riscy_step(&cpu, NULL, output, NULL) == RISCY_IO_ERROR);
    cpu = machine(0x82);
    CHECK(riscy_step(&cpu, input, NULL, NULL) == RISCY_IO_ERROR);
    rewind(trace);
    CHECK(fgets(line, sizeof line, trace) != NULL);
    CHECK(strstr(line, "PC=00, inst=83, op=IN") != NULL);
    fclose(input); fclose(output); fclose(trace);
    {
        const uint8_t invalid[] = {0x02, 0x42, 0x43, 0x90, 0xa0, 0xb0};
        for (i = 0; i < sizeof invalid; ++i) {
            cpu = machine(invalid[i]);
            CHECK(riscy_step(&cpu, NULL, NULL, NULL) == RISCY_INVALID_INSTRUCTION);
            CHECK(cpu.pc == 0 && cpu.instructions == 0);
        }
    }
}

static void test_programs(void)
{
    Riscy cpu;
    unsigned n, previous = 0, current = 1;
    for (n = 0; n <= 255; ++n) {
        CHECK(riscy_load(&cpu, multiply_program, sizeof multiply_program));
        cpu.memory[0xfe] = (uint8_t)n;
        CHECK(riscy_run(&cpu, 100, NULL, NULL, NULL) == RISCY_HALTED);
        CHECK(cpu.memory[0xff] == (uint8_t)(n * 3));
        CHECK(riscy_load(&cpu, fibonacci_program, sizeof fibonacci_program));
        cpu.memory[0xfe] = (uint8_t)n;
        CHECK(riscy_run(&cpu, 10000, NULL, NULL, NULL) == RISCY_HALTED);
        CHECK(cpu.memory[0xff] == previous);
        {
            const unsigned next = (previous + current) & 255;
            previous = current;
            current = next;
        }
    }
}

int main(void)
{
    test_sli(); test_arithmetic(); test_control_flow(); test_memory();
    test_io_and_errors(); test_programs();
    printf("RISCY core: %u checks passed\n", checks);
    return 0;
}
