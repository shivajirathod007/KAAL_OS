#ifndef OS_H
#define OS_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

enum ProcessState { NEW, READY, RUNNING, WAITING, TERMINATED };

struct PCB {
    int job_id;
    int TTL; // Total Time Limit
    int TLL; // Total Line Limit
    int TTC; // Total Time Counter
    int TLC; // Total Line Counter
    ProcessState state;
};

class OS {
private:
    char M[300][4]; // Expanded Memory
    char IR[4];     
    char GPR[4];      
    int IC;         
    bool Toggle;    
    int PTR;        // Page Table Register

    int SI;         // System Interrupt
    int PI;         // Program Interrupt
    int TI;         // Timer Interrupt

    PCB pcb;

    ifstream infile;
    ofstream outfile;
    
    // Phase 2 & 3 helpers
    vector<int> allocated_blocks;
    int allocateRandomBlock();
    int addressMap(int VA); // Virtual to Physical mapping
    
    // ALU Helpers
    int getIntFromMem(char arr[4]);
    void storeIntToMem(char arr[4], int val);
    
    void INIT();
    void STARTEXECUTION();
    void EXECUTEUSERPROGRAM();
    void READ(int RA); 
    void WRITE(int RA); 
    void TERMINATE(int error_code);
    void MOS(int VA);

public:
    OS();
    bool is_debug_mode;
    void LOAD();
    void printMemoryDump();
    void printOutputFile();
    void printRegisters();
    void printPageTableExplorer();
};

#endif
