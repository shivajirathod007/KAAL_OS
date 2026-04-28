#include "../include/OS.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void animateLoading() {
    cout << "\033[1;32mBooting KAAL OS...\033[0m\n[";
    for (int i = 0; i < 30; i++) {
        cout << "#"; cout.flush();
        this_thread::sleep_for(chrono::milliseconds(25));
    }
    cout << "] 100%\n\n";
    this_thread::sleep_for(chrono::milliseconds(200));
}

void displayBanner(bool debug_on) {
    clearScreen();
    cout << "\033[1;36m";
    cout << "  ██╗  ██╗ █████╗  █████╗ ██╗          ██████╗ ███████╗\n";
    cout << "  ██║ ██╔╝██╔══██╗██╔══██╗██║         ██╔═══██╗██╔════╝\n";
    cout << "  █████╔╝ ███████║███████║██║         ██║   ██║███████╗\n";
    cout << "  ██╔═██╗ ██╔══██║██╔══██║██║         ██║   ██║╚════██║\n";
    cout << "  ██║  ██╗██║  ██║██║  ██║███████╗    ╚██████╔╝███████║\n";
    cout << "  ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝     ╚═════╝ ╚══════╝\n";
    cout << "\033[0m";
    cout << "\n        \033[1;31mKAAL OS Simulator  v2.0  Enterprise\033[0m\n";
    cout << "          \033[1;33m[-] Built by Shivazz [-]\033[0m\n";
    if (debug_on) cout << "\n          \033[1;31m[ DEBUG MODE: ON ]\033[0m\n";
    cout << "\n";
}

void createInputFile(int jobType) {
    ofstream out("input.txt", ios::trunc);
    switch(jobType) {
        case 1: out << "$AMJ000100500100\nGD10PD10H   \n$DTA\nHello from KAAL OS Phase 2! This is the happy path.\n$END\n"; break;
        case 2: out << "$AMJ000200500100\nXX10H       \n$DTA\nData\n$END\n"; break;
        case 3: out << "$AMJ000300020100\nGD10PD10PD10GD10H\n$DTA\nTime Limit Test Data\n$END\n"; break;
        case 4: out << "$AMJ000400500000\nGD10PD10H   \n$DTA\nLine Limit Test\n$END\n"; break;
        case 5: out << "$AMJ000500500100\nPD10H       \n$DTA\nData\n$END\n"; break;
        case 6: out << "$AMJ000600500100\nGDXXH       \n$DTA\nData\n$END\n"; break;
        case 7: out << "$AMJ000800100100\nGD10LR10AD11SR12PD12H\n$DTA\n00500075\n$END\n"; break;
        case 8:
            out << "$AMJ000100500100\nGD10PD10H   \n$DTA\nJob1 Happy Path\n$END\n";
            out << "$AMJ000200500100\nXX10H       \n$DTA\nJob2 Opcode Error\n$END\n";
            out << "$AMJ000300020100\nGD10PD10PD10H\n$DTA\nJob3 TLE\n$END\n";
            out << "$AMJ000400500000\nGD10PD10H   \n$DTA\nJob4 LLE\n$END\n";
            out << "$AMJ000500500100\nPD10H       \n$DTA\nJob5 Page Fault\n$END\n";
            out << "$AMJ000600500100\nGDXXH       \n$DTA\nJob6 Operand Err\n$END\n";
            out << "$AMJ000700100100\nGD10LR10AD11SR12PD12H\n$DTA\n01200033\n$END\n";
            break;
    }
    out.close();
}

void promptReturn() {
    cout << "\n\033[1;36mPress Enter to return to menu...\033[0m";
    cin.ignore(); cin.get();
}

int main() {
    displayBanner(false);
    animateLoading();
    OS kaalOS;

    while (true) {
        displayBanner(kaalOS.is_debug_mode);
        cout << "\033[1;31m[::] Select A Option For You [::]\033[0m\n\n";
        cout << "  \033[1;36m--- SIMULATION ---\033[0m\n";
        cout << "    [\033[1;33m01\033[0m] Run Simulation (Happy Path)\n";
        cout << "  \033[1;36m--- ERROR TESTS ---\033[0m\n";
        cout << "    [\033[1;33m02\033[0m] Test: Wrong Opcode Error\n";
        cout << "    [\033[1;33m03\033[0m] Test: Time Limit Exceeded (TLE)\n";
        cout << "    [\033[1;33m04\033[0m] Test: Line Limit Exceeded (LLE)\n";
        cout << "    [\033[1;33m05\033[0m] Test: Invalid Page Fault\n";
        cout << "    [\033[1;33m06\033[0m] Test: Operand Error\n";
        cout << "  \033[1;36m--- ADVANCED ---\033[0m\n";
        cout << "    [\033[1;33m07\033[0m] Test: ALU Arithmetic (Load, Add, Store, Print)\n";
        cout << "    [\033[1;33m08\033[0m] Full System Verification (All Jobs)\n";
        cout << "  \033[1;36m--- INSPECTION ---\033[0m\n";
        cout << "    [\033[1;33m09\033[0m] View Memory Dump\n";
        cout << "    [\033[1;33m10\033[0m] View Output Log\n";
        cout << "    [\033[1;33m11\033[0m] View CPU Registers & PCB\n";
        cout << "    [\033[1;33m12\033[0m] View Page Table Explorer\n";
        cout << "  \033[1;36m--- SETTINGS ---\033[0m\n";
        cout << "    [\033[1;33m13\033[0m] Toggle Debug Stepping Mode [" 
             << (kaalOS.is_debug_mode ? "\033[1;32mON\033[0m" : "\033[1;31mOFF\033[0m") << "]\n";
        cout << "    [\033[1;33m99\033[0m] About KAAL OS\n";
        cout << "    [\033[1;33m00\033[0m] Exit System\n\n";

        cout << "\033[1;32m[-] Select an option : \033[0m";
        string ch; cin >> ch;

        if      (ch=="01"||ch=="1") { createInputFile(1); kaalOS.LOAD(); promptReturn(); }
        else if (ch=="02"||ch=="2") { createInputFile(2); kaalOS.LOAD(); promptReturn(); }
        else if (ch=="03"||ch=="3") { createInputFile(3); kaalOS.LOAD(); promptReturn(); }
        else if (ch=="04"||ch=="4") { createInputFile(4); kaalOS.LOAD(); promptReturn(); }
        else if (ch=="05"||ch=="5") { createInputFile(5); kaalOS.LOAD(); promptReturn(); }
        else if (ch=="06"||ch=="6") { createInputFile(6); kaalOS.LOAD(); promptReturn(); }
        else if (ch=="07"||ch=="7") { createInputFile(7); kaalOS.LOAD(); promptReturn(); }
        else if (ch=="08"||ch=="8") { createInputFile(8); kaalOS.LOAD(); promptReturn(); }
        else if (ch=="09"||ch=="9") { kaalOS.printMemoryDump(); promptReturn(); }
        else if (ch=="10") { kaalOS.printOutputFile(); promptReturn(); }
        else if (ch=="11") { kaalOS.printRegisters(); promptReturn(); }
        else if (ch=="12") { kaalOS.printPageTableExplorer(); promptReturn(); }
        else if (ch=="13") {
            kaalOS.is_debug_mode = !kaalOS.is_debug_mode;
            cout << "\n\033[1;36m[*] Debug stepping is now " << (kaalOS.is_debug_mode ? "\033[1;32mON" : "\033[1;31mOFF") << "\033[0m\n";
            promptReturn();
        }
        else if (ch=="99") {
            cout << "\n\033[1;33m[ KAAL OS v2.0 Enterprise ]\033[0m\n";
            cout << "  Built by : Shivazz\n";
            cout << "  Language : C++\n";
            cout << "  Principles: SOLID Design | Decoupled Modules\n";
            cout << "  Features :  ISA (GD/PD/LR/SR/CR/BT/AD/SB/ML/DV)\n";
            cout << "              PCB State Machine | Dynamic Paging\n";
            cout << "              Phase Fault Recovery | 6 Error Types\n";
            cout << "              ALU Arithmetic | Debug Stepper\n";
            cout << "              Page Table Explorer | Memory Dump\n";
            promptReturn();
        }
        else if (ch=="00"||ch=="0") {
            cout << "\n\033[1;31m[-] System Shutting Down... Goodbye!\033[0m\n";
            this_thread::sleep_for(chrono::milliseconds(500));
            break;
        }
        else {
            cout << "\n\033[1;31m[!] Invalid Option!\033[0m\n";
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
    return 0;
}
