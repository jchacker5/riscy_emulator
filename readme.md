# RISCY Emulator Project

## Overview

This project is a software emulator for the RISCY instruction set, designed as part of the COMP406 / COMP590 course. It simulates a minimal RISCY machine with a focus on executing a defined subset of operations, including arithmetic, memory access, control flow, and special instructions.

## Getting Started

### Prerequisites

- A C compiler (e.g., GCC or Clang) is required to build the emulator.
- Basic knowledge of C programming and command-line tools is recommended.

### Installation

1. **Clone the repository:**
   ```
   git clone https://yourrepositoryurl.com
   ```
   
2. **Compile the emulator:**
   ```
   cd RISCY-Emulator
   gcc -o riscy_emulator main.c
   ```

3. **Run the emulator:**
   ```
   ./riscy_emulator
   ```

   To enable debugging output, use the `-d` flag:
   ```
   ./riscy_emulator -d
   ```

## Usage

The emulator reads RISCY binary machine code from an in-memory array and executes it. To test your own RISCY programs, modify the `memory` array in `main.c` with your machine code.

### Example Programs

Included are example RISCY programs for Fibonacci sequence calculation, memory printing, and Conway's Game of Life simulation. These programs demonstrate the emulator's capability to handle a variety of instructions and scenarios.

## Instruction Set

The emulator supports the following instructions:

- `ADD`, `SUB`: Arithmetic operations
- `LOAD`, `STORE`: Memory access
- `SKIPZ`, `SKIPNZ`: Conditional execution
- `JALR`: Jump and link register
- `SLI`: Shift left and insert
- `NAND`: Bitwise NAND
- `INC`, `DEC`: Increment and decrement
- `IN`, `OUT`: Input and output
- `NOP`: No operation
- `HALT`: Stop the emulator

Each instruction is implemented as a separate function in the emulator, ensuring modularity and clarity.

## Debugging

Debugging output can be enabled with the `-d` flag. This prints the state of the machine after each instruction execution, including the program counter, instruction opcode, register values, and more.

## Contributing

Contributions to the emulator are welcome. Please adhere to the project's coding standards and submit pull requests for review.

## License

This project is licensed under the MIT License - see the LICENSE.md file for details.

## Acknowledgments

- Course instructors and TAs for providing guidance and support throughout the project.
- Fellow students for collaboration and insights.

