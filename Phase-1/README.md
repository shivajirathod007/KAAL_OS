# KAAL OS - Phase 1: Basic Execution and Memory Management

## Project Overview
This represents Phase 1 of the KAAL OS (Operation Simulation) project, written in C++. The goal is to simulate basic Operating System execution using fixed-language instructions. This phase handles standard sequential loading of instructions from non-volatile memory (an `input.txt` file) directly into volatile memory (simulated as RAM using character arrays).

## 1. Input File Structure (`input.txt`)
The input is formatted into "Jobs," containing control cards, program cards, and data cards.
- **Control Cards:**
  - `$AMJ`: Marks the start of a job. Includes Job ID, Total Time Limit (TTL - max instructions), and Total Line Limit (TLL - max output lines).
  - `$DTA`: Marks the start of the Data Cards.
  - `$END`: Marks the end of a Job.
- **Program Cards:** Hold the actual instructions (max 40 bytes or 10 instructions per line).
- **Data Cards:** Sit below `$DTA` and provide data for instructions to process (max 40 bytes per line).

## 2. Hardware Variables/Registers
- **Instruction Counter (IC):** Integer, increments as instructions are executed.
- **Instruction Register (IR):** 4-byte character array holding the currently executing instruction.
- **General Purpose Register (GPR):** 4-byte character array used for temporary data storage.
- **Toggle Register:** Boolean value set during compare instructions (`CR`) and used for branching (`BT`).

## 3. Memory Structure (Simulated RAM)
- Phase 1 memory is a 2D Character Array of size 100 rows x 4 columns (Indices 0 to 99).
- **Word:** 1 Row (4 bytes).
- **Block:** 10 contiguous Words/Rows (e.g., Rows 0-9, 10-19). Total of 10 blocks (400 bytes).

## 4. Instruction Set Architecture (ISA)
Format: `Opcode` (name) + `Operand` (location/address)
- `GD`: Get Data. Reads from input file, loads into memory block.
- `PD`: Print Data. Reads from memory block, prints to output file.
- `H`: Halt. Exits the current job.
- `LR`: Load Register. Loads data from memory word into GPR.
- `SR`: Store Register. Stores data from GPR into memory word.
- `CR`: Compare Register. Compares GPR to memory word. Sets Toggle Register if they match.
- `BT`: Branch if True. Jumps to execution index if Toggle Register is True.

## 5. System Calls & Interrupts
`GD`, `PD`, and `H` are OS kernel system calls triggering System Interrupts (SI).
- `SI = 1`: `GD`
- `SI = 2`: `PD`
- `SI = 3`: `H`
