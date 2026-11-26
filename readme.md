# RISCY Emulator

[![C11 emulator tests](https://github.com/jchacker5/riscy_emulator/actions/workflows/ci.yml/badge.svg)](https://github.com/jchacker5/riscy_emulator/actions/workflows/ci.yml)

An educational emulator for the COMP406 / COMP590 RISCY machine: four 8-bit registers,
a wrapping 8-bit program counter, and 256 bytes of unified program/data memory. Arithmetic
wraps modulo 256. This project implements the course instruction set, not RISC-V.

## Build and run

Requires a C11 compiler and Make; tests also require Python 3.

```sh
git clone https://github.com/jchacker5/riscy_emulator.git
cd riscy_emulator
make
./riscy_emulator                       # course multiply example: 2 * 3 = 6
./riscy_emulator --program fib         # Fibonacci(6) = 8
./riscy_emulator --program fib --input 10 -d
make test
make sanitize
```

Without Make: `cc -std=c11 -Wall -Wextra -Werror -pedantic main.c riscy.c -o riscy_emulator`.

The default program is the complete multiplication bytecode preserved in
[the course assignment](project_description.md). `programs.h` also contains a documented,
reconstructed Fibonacci program. The originally committed Fibonacci, memory-printing and
Conway arrays were truncated in every historical version; those incomplete arrays are not
advertised as working examples. The broken duplicate `main1.c` implementation has been
consolidated into the single tested core in `riscy.c`.

## CLI

```text
riscy_emulator [-d] [--program multiply|fib | --rom FILE] [--input 0..255] [--max-steps N]
```

- `--input` sets memory `0xfe` for a bundled example (default: 2 for multiply, 6 for fib).
- `--rom` loads an exact binary image of 1–256 bytes at address zero. Unused memory is zeroed.
  It cannot be combined with `--program` or `--input`; a full image may use all 256 bytes.
- `--max-steps` sets a positive instruction budget (default: 1,000,000). Exhausting it is an
  error, not a successful halt. A jump to zero or PC wrap is valid and does not itself stop execution.
- `-d`, `--debug` and the legacy `d` emit the fetched PC, full instruction byte, mnemonic and
  updated registers to stderr. Program byte output and the final memory result go to stdout.
- `IN` reads one raw byte from stdin, including whitespace and zero bytes; it does not echo.
  EOF is reported as an error. `OUT` writes one raw byte with no newline conversion.

Exit status is 0 after HALT (or `--help`), 1 for loading/execution/I/O failures and 2 for invalid
arguments. The result at memory `0xff` is printed after a successful halt.

## Instruction encoding

For two-register instructions, `rd` is bits 3:2 and `rs` is bits 1:0.

| Encoding | Operation |
|---|---|
| `0x00` / `0x01` | NOP / HALT (whole-byte encodings) |
| `0x1?` | SUB: `rd = rd - rs` |
| `0x2?` | LOAD: `rd = memory[rs]` |
| `0x3?` | STORE: `memory[rs] = rd` |
| `0x4?`, `rs=0/1` | SKIPZ / SKIPNZ: skip the next byte if rd is zero/nonzero |
| `0x5?` | JALR: save `PC+1` in rd and jump to the old rs value |
| `0x6?` | NAND: `rd = ~(rd & rs)` |
| `0x7?` | ADD: `rd = rd + rs` |
| `0x8?`, `rs=0/1/2/3` | INC / DEC / OUT / IN on rd |
| `0xC?`–`0xF?` | SLI: shift rd left four bits and insert immediate bits 5:4,1:0 |

Other encodings are rejected. JALR reads its target before writing the link, including when
`rd == rs`. Memory is writable, so STORE may intentionally change program bytes.

## Verification

`make test` checks the instruction core and CLI. Coverage includes all 16,384 combinations
of SLI destination, immediate and prior byte value; arithmetic/PC wrapping; same-register
JALR; conditional skips; full image loading; byte I/O and EOF; invalid encodings; instruction
budgets; and both example programs for every 8-bit input. CI runs strict GCC and Clang builds
and AddressSanitizer/UndefinedBehaviorSanitizer on pull requests and main.

## License and credit

[MIT](license.md). Developed for COMP406 / COMP590 with guidance from the course instructors
and TAs. The multiplication example and assignment text are course-provided.
