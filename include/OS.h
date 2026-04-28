#ifndef OS_H
#define OS_H

#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include "Memory.h"
#include "CPU.h"
#include "PCB.h"

using namespace std;

class OS {
private:
    Memory mem;
    CPU    cpu;
    PCB    pcb;
    vector<string> exec_log;

    ifstream infile;
    ofstream outfile;

    void log(const string& colour, const string& tag, const string& msg);
    int  addressMap(int VA);
    void READ(int RA);
    void WRITE(int RA);
    void TERMINATE(int ec);
    void MOS(int VA, bool is_syscall);
    void STARTEXECUTION();
    void EXECUTEUSERPROGRAM();

public:
    bool is_debug_mode;
    
    OS();
    void LOAD();
    void printMemoryDump();
    void printOutputFile();
    void printRegisters();
    void printPageTableExplorer();
};

#endif
