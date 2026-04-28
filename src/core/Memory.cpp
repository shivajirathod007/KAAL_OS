#include "../include/Memory.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <algorithm>

using namespace std;

Memory::Memory() {
    init();
}

void Memory::init() {
    for (int i = 0; i < 300; i++)
        for (int j = 0; j < 4; j++)
            M[i][j] = ' ';
    allocated_blocks.clear();
}

int Memory::allocateRandomBlock() {
    if ((int)allocated_blocks.size() >= 30) return -1;
    int block;
    while (true) {
        block = rand() % 30;
        if (find(allocated_blocks.begin(), allocated_blocks.end(), block) == allocated_blocks.end()) {
            allocated_blocks.push_back(block);
            return block;
        }
    }
}

void Memory::printDump() {
    cout << "\n     \033[1;36m[::] KAAL OS Memory Dump (300 Words) [::]\033[0m\n\n";
    for (int i = 0; i < 300; i++) {
        cout << "\033[1;36mM[" << setfill('0') << setw(3) << i << "]\033[0m: ";
        for (int j = 0; j < 4; j++) cout << M[i][j];
        if ((i + 1) % 5 == 0) cout << "\n";
        else cout << "  |  ";
    }
    cout << "\n";
}
