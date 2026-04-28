#ifndef MEMORY_H
#define MEMORY_H

#include <vector>

using namespace std;

class Memory {
public:
    char M[300][4];
    vector<int> allocated_blocks;

    Memory();
    void init();
    int allocateRandomBlock();
    void printDump();
};

#endif
