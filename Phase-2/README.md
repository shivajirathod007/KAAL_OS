# KAAL OS - Phase 2: Error Handling and Memory Mapping (Paging)

## Project Overview
This represents Phase 2 of the KAAL OS (Operation Simulation) project, written in C++. It expands upon Phase 1 by introducing complexity found in actual OS environments: error handling for erroneous jobs and dynamic page mapping for memory placement.

## 1. Error Handling Exceptions
Unlike Phase 1 which assumed perfect files, Phase 2 must catch and react to the following errors:
- **Wrong Opcode Error:** Instruction name doesn't exist (e.g., `ED 10`).
- **Wrong Operand Error:** Invalid memory address.
- **Time Limit Exceeded (TLE):** Program executed more instructions than the `TTL` permitted in the `$AMJ` card.
- **Line Limit Exceeded (LLE):** Program tried to write to output file more times than the `TLL` permitted.
- **Page Fault Error:** Program attempts to read/access memory that hasn't been logically assigned yet.
- **Data Out of Range:** Program attempts to execute `GD` to fetch data but runs into the `$END` card instead of valid data.

## 2. Memory Mapping and Paging
- **Expanded RAM:** The memory size is increased to 300 rows (30 blocks). Index 0 to 299.
- **Random Storage:** Instructions are no longer sequentially loaded starting at index 0. Program cards and Data are loaded into random, empty memory blocks to simulate dynamic memory allocation.
- **Page Table Register (PTR):** A randomly assigned memory block acts as the Page Table mapping logical blocks to physical random memory blocks.

## 3. Dynamic Execution Flow
1. **Initialize Memory:** Clear all 300 blocks.
2. **Page Table Allocation:** Generate a random block for the Page Table itself.
3. **Program Card Allocation:** Generate random blocks for all Program Cards. Insert mapping into the Page Table.
4. **Execution & Dynamic Data Loading:** Operations like `GD 10` check the Page Table to see if the logical block is assigned. If not assigned, the OS dynamically generates a new random block, updates the Page Table, and loads the data into the physical block.
5. **Validation:** Operations like `PD` check the Page Table. If the memory block hasn't been mapped, a Page Fault error occurs.
