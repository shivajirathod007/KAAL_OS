#ifndef CPU_H
#define CPU_H

#include "PCB.h"

class CPU {
public:
    char IR[4];
    char GPR[4];
    int IC;
    bool Toggle;
    
    int PTR;
    int SI;
    int PI;
    int TI;
    
    CPU();
    void init();
    void printRegisters(const PCB& pcb);
    int getIntFromMem(const char arr[4]);
    void storeIntToMem(char arr[4], int val);
};

#endif
