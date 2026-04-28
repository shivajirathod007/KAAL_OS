#include "../include/CPU.h"
#include <iostream>
#include <string>

using namespace std;

CPU::CPU() {
    init();
}

void CPU::init() {
    for (int i = 0; i < 4; i++) { IR[i] = ' '; GPR[i] = ' '; }
    IC = 0; Toggle = false; PTR = -1; SI = 0; PI = 0; TI = 0;
}

int CPU::getIntFromMem(const char arr[4]) {
    string s = "";
    for (int i = 0; i < 4; i++)
        if (isdigit(arr[i]) || arr[i] == '-') s += arr[i];
    if (s.empty() || s == "-") return 0;
    return stoi(s);
}

void CPU::storeIntToMem(char arr[4], int val) {
    string s = to_string(val);
    while ((int)s.length() < 4) s = "0" + s;
    if ((int)s.length() > 4) s = s.substr(s.length() - 4, 4);
    for (int i = 0; i < 4; i++) arr[i] = s[i];
}

void CPU::printRegisters(const PCB& pcb) {
    static const string states[] = { "NEW", "READY", "RUNNING", "WAITING", "TERMINATED" };
    cout << "\n\033[1;36m[::] KAAL OS CPU REGISTER DUMP [::]\033[0m\n\n";
    cout << "\033[1;33m  IC  (Instruction Counter)  : \033[1;37m" << IC << "\033[0m\n";
    cout << "\033[1;33m  IR  (Instruction Register) : \033[1;37m" << IR[0] << IR[1] << IR[2] << IR[3] << "\033[0m\n";
    cout << "\033[1;33m  GPR (General Purpose Reg)  : \033[1;37m" << GPR[0] << GPR[1] << GPR[2] << GPR[3] << "\033[0m\n";
    cout << "\033[1;33m  Toggle (Compare Flag)      : \033[1;37m" << (Toggle ? "TRUE" : "FALSE") << "\033[0m\n";
    cout << "\033[1;33m  PTR (Page Table Reg Block) : \033[1;37m" << (PTR == -1 ? "NULL" : to_string(PTR)) << "\033[0m\n";
    cout << "\n\033[1;35m  [PROCESS CONTROL BLOCK]\033[0m\n";
    cout << "  Job ID      : " << pcb.job_id << "\n";
    cout << "  State       : \033[1;32m" << states[pcb.state] << "\033[0m\n";
    cout << "  Time Used   : " << pcb.TTC << " / " << pcb.TTL << "\n";
    cout << "  Lines Used  : " << pcb.TLC << " / " << pcb.TLL << "\n";
    cout << "\n\033[1;35m  [INTERRUPTS]\033[0m\n";
    cout << "  SI (System) : " << SI  << "\n";
    cout << "  PI (Program): " << PI << "\n";
    cout << "  TI (Timer)  : " << TI << "\n";
}
