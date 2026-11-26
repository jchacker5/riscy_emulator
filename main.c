#include "riscy.h"
#include "programs.h"

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *name)
{
    fprintf(stderr, "Usage: %s [-d] [--program multiply|fib | --rom FILE] "
            "[--input 0..255] [--max-steps N]\n", name);
}

static int number(const char *text, uintmax_t maximum, uintmax_t *value)
{
    char *end;
    uintmax_t parsed;
    if (*text < '0' || *text > '9') return 0;
    errno = 0;
    parsed = strtoumax(text, &end, 10);
    if (errno != 0 || *end != '\0' || parsed > maximum) return 0;
    *value = parsed;
    return 1;
}

int main(int argc, char **argv)
{
    Riscy cpu;
    const char *program = "multiply";
    const char *rom_path = NULL;
    uintmax_t input_value = 0, limit = 1000000;
    int input_set = 0, program_set = 0, debug = 0, i;
    for (i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (strcmp(arg, "-d") == 0 || strcmp(arg, "--debug") == 0 || strcmp(arg, "d") == 0) {
            debug = 1;
            continue;
        }
        if (strcmp(arg, "--help") == 0) { usage(argv[0]); return 0; }
        if (i + 1 >= argc) { usage(argv[0]); return 2; }
        if (strcmp(arg, "--program") == 0) {
            program = argv[++i];
            program_set = 1;
        } else if (strcmp(arg, "--rom") == 0) {
            rom_path = argv[++i];
        } else if (strcmp(arg, "--input") == 0) {
            if (!number(argv[++i], 255, &input_value)) { usage(argv[0]); return 2; }
            input_set = 1;
        } else if (strcmp(arg, "--max-steps") == 0) {
            if (!number(argv[++i], SIZE_MAX, &limit) || limit == 0) { usage(argv[0]); return 2; }
        } else { usage(argv[0]); return 2; }
    }
    if (rom_path != NULL) {
        uint8_t image[257];
        size_t size;
        FILE *file;
        if (program_set || input_set) { usage(argv[0]); return 2; }
        file = fopen(rom_path, "rb");
        if (file == NULL) { perror(rom_path); return 1; }
        size = fread(image, 1, sizeof image, file);
        if (ferror(file)) { fclose(file); fprintf(stderr, "Cannot read ROM\n"); return 1; }
        fclose(file);
        if (!riscy_load(&cpu, image, size)) {
            fprintf(stderr, "ROM must contain 1..256 bytes\n");
            return 1;
        }
    } else {
        if (strcmp(program, "multiply") == 0) {
            riscy_load(&cpu, multiply_program, sizeof multiply_program);
            if (!input_set) input_value = 2;
        } else if (strcmp(program, "fib") == 0) {
            riscy_load(&cpu, fibonacci_program, sizeof fibonacci_program);
            if (!input_set) input_value = 6;
        } else { usage(argv[0]); return 2; }
        cpu.memory[0xfe] = (uint8_t)input_value;
    }
    {
        const RiscyResult result = riscy_run(&cpu, (size_t)limit, stdin, stdout, debug ? stderr : NULL);
        if (fflush(stdout) == EOF) { fprintf(stderr, "Output error\n"); return 1; }
        if (result != RISCY_HALTED) {
            fprintf(stderr, "%s at PC=%02x after %zu instructions\n", riscy_result_name(result),
                    (unsigned)cpu.pc, cpu.instructions);
            return 1;
        }
    }
    printf("Result at memory[0xFF]: %u\n", (unsigned)cpu.memory[0xff]);
    return fflush(stdout) == EOF ? 1 : 0;
}
