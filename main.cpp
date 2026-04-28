#include "os.h"
#include <iostream>
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

void displayBanner() {
    clearScreen();
    cout << "\033[1;36m" << endl;
    cout << "  _  __      _         _       _           ___      _____  " << endl;
    cout << " | |/ /     / \\       / \\     | |         / _ \\    / ____| " << endl;
    cout << " | ' /     / ^ \\     / ^ \\    | |        | | | |  | (___   " << endl;
    cout << " |  <     / ___ \\   / ___ \\   | |___     | |_| |   \\___ \\  " << endl;
    cout << " |_|\\_\\  /_/   \\_\\ /_/   \\_\\  |_____|     \\___/   _____) | " << endl;
    cout << "                                                 |______/  " << endl;
    cout << "\033[0m" << endl;
    cout << "       \033[1;31mKAAL OS Simulator v2.0 Enterprise\033[0m" << endl;
    cout << "       \033[1;33m[-] built by Shivazz [-]\033[0m" << endl << endl;
}

void animateLoading() {
    cout << "\033[1;32mBooting KAAL OS Data Streams...\033[0m\n[";
    for(int i = 0; i < 30; i++) {
        cout << "#";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(30));
    }
    cout << "] 100%\n\n";
    this_thread::sleep_for(chrono::milliseconds(200));
}

void createInputFile(int jobType) {
    ofstream out("input.txt", ios::trunc);
    if(jobType == 1) {
        out << "$AMJ000100050005\nGD10PD10H   \n$DTA\nHello World! This is KAAL OS Phase 2.\n$END\n";
    } else if(jobType == 2) {
        out << "$AMJ000200050005\nXX10H       \n$DTA\nData\n$END\n";
    } else if(jobType == 3) {
        out << "$AMJ000300020005\nGD10PD10PD10H   \n$DTA\nTime Limit Test\n$END\n";
    } else if(jobType == 4) {
        out << "$AMJ000400050000\nGD10PD10H   \n$DTA\nLine Limit Test\n$END\n";
    } else if(jobType == 5) {
        out << "$AMJ000500050005\nPD10H       \n$DTA\nData\n$END\n";
    } else if(jobType == 6) {
        out << "$AMJ000600050005\nGDXXH       \n$DTA\nData\n$END\n";
    } else if(jobType == 7) {
        out << "$AMJ000100050005\nGD10PD10H   \n$DTA\nHello World! Phase 2 is awesome.\n$END\n";
        out << "$AMJ000200020005\nXX10H       \n$DTA\nTest Opcode Error\n$END\n";
        out << "$AMJ000400030005\nGD10PD10PD10H   \n$DTA\nTest Time Limit\n$END\n";
        out << "$AMJ000600050005\nGDXXH       \n$DTA\nTest Operand Error\n$END\n";
        out << "$AMJ000300050000\nPD10H       \n$DTA\nTest Line Limit\n$END\n";
        out << "$AMJ000800100005\nGD10LR10AD11SR12PD12H\n$DTA\n00500075\n$END\n";
    }
    out.close();
}

void promptReturn() {
    cout << "\n\033[1;36mPress Enter to return to main menu...\033[0m";
    cin.ignore();
    cin.get();
}

int main() {
    displayBanner();
    animateLoading();
    OS kaalOS;

    while(true) {
        displayBanner();
        cout << "\033[1;31m[::] KAAL OS Master Options [::]\033[0m\n\n";
        cout << "    [\033[1;33m01\033[0m] Start Simulation (Happy Path)\n";
        cout << "    [\033[1;33m02\033[0m] Test: Opcode Error\n";
        cout << "    [\033[1;33m03\033[0m] Test: Time Limit Exceeded\n";
        cout << "    [\033[1;33m04\033[0m] Test: Line Limit Exceeded\n";
        cout << "    [\033[1;33m05\033[0m] Test: Invalid Page Fault\n";
        cout << "    [\033[1;33m06\033[0m] Test: Operand Error\n";
        cout << "    [\033[1;33m07\033[0m] Test: Run Full System Verification\n";
        cout << "    [\033[1;33m08\033[0m] View Memory Dump\n";
        cout << "    [\033[1;33m09\033[0m] View Output Log\n";
        cout << "    [\033[1;33m10\033[0m] View Internal CPU Registers & PCB\n";
        cout << "    [\033[1;33m11\033[0m] View Global Page Table Directory\n";
        cout << "    [\033[1;33m12\033[0m] Toggle Iterative CPU Debug Mode [ " 
             << (kaalOS.is_debug_mode ? "\033[1;32mON\033[0m" : "\033[1;31mOFF\033[0m") << " ]\n";
        cout << "    [\033[1;33m99\033[0m] About KAAL OS\n";
        cout << "    [\033[1;33m00\033[0m] Exit System\n\n";
        
        cout << "\033[1;32m[-] Select an option : \033[0m";
        string choice;
        cin >> choice;

        if(choice == "01" || choice == "1") {
            cout << "\n\033[1;36m[*] Loading Happy Path Job...\033[0m\n";
            createInputFile(1); kaalOS.LOAD(); promptReturn();
        } else if(choice == "02" || choice == "2") {
            cout << "\n\033[1;36m[*] Loading Opcode Error Job...\033[0m\n";
            createInputFile(2); kaalOS.LOAD(); promptReturn();
        } else if(choice == "03" || choice == "3") {
            cout << "\n\033[1;36m[*] Loading Time Limit Exceeded Job...\033[0m\n";
            createInputFile(3); kaalOS.LOAD(); promptReturn();
        } else if(choice == "04" || choice == "4") {
            cout << "\n\033[1;36m[*] Loading Line Limit Exceeded Job...\033[0m\n";
            createInputFile(4); kaalOS.LOAD(); promptReturn();
        } else if(choice == "05" || choice == "5") {
            cout << "\n\033[1;36m[*] Loading Invalid Page Fault Job...\033[0m\n";
            createInputFile(5); kaalOS.LOAD(); promptReturn();
        } else if(choice == "06" || choice == "6") {
            cout << "\n\033[1;36m[*] Loading Operand Error Job...\033[0m\n";
            createInputFile(6); kaalOS.LOAD(); promptReturn();
        } else if(choice == "07" || choice == "7") {
            cout << "\n\033[1;36m[*] Loading Full System Verification Multi-Job sequence...\033[0m\n";
            createInputFile(7); kaalOS.LOAD(); promptReturn();
        } else if(choice == "08" || choice == "8") {
            kaalOS.printMemoryDump(); promptReturn();
        } else if(choice == "09" || choice == "9") {
            kaalOS.printOutputFile(); promptReturn();
        } else if(choice == "10") {
            kaalOS.printRegisters(); promptReturn();
        } else if(choice == "11") {
            kaalOS.printPageTableExplorer(); promptReturn();
        } else if(choice == "12") {
            kaalOS.is_debug_mode = !kaalOS.is_debug_mode;
            cout << "\n\033[1;36m[*] CPU Interactive Iteration Stepping is now \033[1;37m" 
                 << (kaalOS.is_debug_mode ? "ENABLED" : "DISABLED") << "\033[0m\n";
            promptReturn();
        } else if(choice == "99") {
            cout << "\n\033[1;33m[ About KAAL OS ]\033[0m\n";
            cout << "KAAL OS is an Operation Simulation project built by Shivazz.\n";
            cout << "Phase 3 implements Extended Arithmetic Instructions (AD, SB, ML, DV),\n";
            cout << "Interactive Page Table Navigation, and Debug Stepping Modes.\n";
            promptReturn();
        } else if(choice == "00" || choice == "0") {
            cout << "\n\033[1;31m[-] System Shutting Down... Goodbye!\033[0m\n";
            this_thread::sleep_for(chrono::milliseconds(500));
            break;
        } else {
            cout << "\n\033[1;31m[!] Invalid Option! Try again.\033[0m\n";
            this_thread::sleep_for(chrono::seconds(1));
        }
        
        // Handle input buffer if a choice leaves a leftover
        if(choice == "01" || choice == "1" || choice == "02" || choice == "2" || choice == "03" || choice == "3" || 
           choice == "04" || choice == "4" || choice == "05" || choice == "5" || choice == "06" || choice == "6" || 
           choice == "07" || choice == "7" || choice == "08" || choice == "8" || choice == "09" || choice == "9" || 
           choice == "10" || choice == "99") {
            // Already handled by promptReturn
        }
    }
    return 0;
}
