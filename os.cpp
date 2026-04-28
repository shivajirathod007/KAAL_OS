#include "os.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <thread>
#include <chrono>

using namespace std;

OS::OS() {
    srand(time(0));
    is_debug_mode = false;
}

int OS::getIntFromMem(char arr[4]) {
    string s = "";
    for(int i=0;i<4;i++) if(isdigit(arr[i]) || arr[i]=='-') s += arr[i];
    if(s.empty() || s == "-") return 0;
    return stoi(s);
}

void OS::storeIntToMem(char arr[4], int val) {
    string s = to_string(val);
    while(s.length() < 4) s = "0" + s;
    if(s.length() > 4) s = s.substr(s.length()-4, 4);
    for(int i=0;i<4;i++) arr[i] = s[i];
}

void OS::INIT() {
    for (int i = 0; i < 300; i++) {
        for (int j = 0; j < 4; j++) {
            M[i][j] = ' ';
        }
    }
    for (int i = 0; i < 4; i++) {
        IR[i] = ' ';
        GPR[i] = ' ';
    }
    IC = 0;
    Toggle = false;
    SI = 0;
    PI = 0;
    TI = 0;
    PTR = -1;
    
    pcb.job_id = 0;
    pcb.TTL = 0;
    pcb.TLL = 0;
    pcb.TTC = 0;
    pcb.TLC = 0;
    pcb.state = NEW;

    allocated_blocks.clear();
}

int OS::allocateRandomBlock() {
    if (allocated_blocks.size() >= 30) return -1;
    int block;
    while(true) {
        block = rand() % 30;
        if(find(allocated_blocks.begin(), allocated_blocks.end(), block) == allocated_blocks.end()) {
            allocated_blocks.push_back(block);
            return block;
        }
    }
}

int OS::addressMap(int VA) {
    if (VA < 0 || VA > 99) {
        PI = 2; // Operand Error
        return -1;
    }
    int logical_block = VA / 10;
    int pte_address = (PTR * 10) + logical_block;
    string entry = "";
    for(int i = 0; i < 4; i++) {
        entry += M[pte_address][i];
    }
    
    if (entry[0] == '*' || entry[0] == ' ') { 
        PI = 3; // Page Fault
        return -1;
    }
    
    int physical_block = stoi(entry);
    return (physical_block * 10) + (VA % 10);
}

void OS::READ(int RA) {
    string line;
    if (getline(infile, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.substr(0, 4) == "$END") {
            TERMINATE(1); // Out of Data
            return;
        }
        int limit = line.length();
        if(limit > 40) limit = 40;
        
        int blockStart = (RA / 10) * 10;
        for(int i = 0; i < limit; i++) {
            M[blockStart + i / 4][i % 4] = line[i];
        }
    }
}

void OS::WRITE(int RA) {
    pcb.TLC++;
    if (pcb.TLC > pcb.TLL) {
        TI = 2; 
        TERMINATE(2); // Line Limit Exceeded
        return;
    }
    
    int blockStart = (RA / 10) * 10;
    string output = "";
    for (int i = 0; i < 10; i++) { 
        for (int j = 0; j < 4; j++) {
            output += M[blockStart + i][j];
        }
    }
    outfile << output << "\n";
}

void OS::TERMINATE(int error_code) {
    pcb.state = TERMINATED;
    outfile << "\nID: " << pcb.job_id << "\n";
    switch(error_code) {
        case 0: outfile << "NORMAL EXECUTION\n"; break;
        case 1: outfile << "OUT OF DATA\n"; break;
        case 2: outfile << "LINE LIMIT EXCEEDED\n"; break;
        case 3: outfile << "TIME LIMIT EXCEEDED\n"; break;
        case 4: outfile << "OPERATION CODE ERROR\n"; break;
        case 5: outfile << "OPERAND ERROR\n"; break;
        case 6: outfile << "INVALID PAGE FAULT\n"; break;
    }
    outfile << "IC: " << IC << "\nIR: ";
    for(int i=0; i<4; i++) outfile << IR[i];
    outfile << "\nTTC: " << pcb.TTC << "  TLC: " << pcb.TLC << "\n\n";
}

void OS::MOS(int VA) {
    if (TI == 0 && SI == 1 && PI == 0) { READ(addressMap(VA)); SI = 0;}
    else if (TI == 2 && SI == 1 && PI == 0) { TERMINATE(3); }
    else if (TI == 0 && SI == 2 && PI == 0) {
        int RA = addressMap(VA);
        if (PI == 0) { WRITE(RA); SI = 0; }
    }
    else if (TI == 2 && SI == 2 && PI == 0) { 
        int RA = addressMap(VA);
        if (PI == 0) { WRITE(RA); TERMINATE(3); }
        else if (PI == 3) { TERMINATE(3); }
    }
    else if (TI == 0 && SI == 3 && PI == 0) { TERMINATE(0); }
    else if (TI == 2 && SI == 3 && PI == 0) { TERMINATE(0); }
    
    // Page Fault handlers
    else if (TI == 0 && PI == 3 && (IR[0]=='G' && IR[1]=='D')) {
        int logical_block = VA / 10;
        int new_phys_block = allocateRandomBlock();
        int pte_address = (PTR * 10) + logical_block;
        string b_str = (new_phys_block < 10 ? "0" : "") + to_string(new_phys_block);
        M[pte_address][0] = '0';
        M[pte_address][1] = '0';
        M[pte_address][2] = b_str[0];
        M[pte_address][3] = b_str[1];
        
        PI = 0; SI = 0;
        int RA = addressMap(VA);
        READ(RA);
    } 
    else if (TI == 0 && PI == 3 && (IR[0]=='S' && IR[1]=='R')) {
        int logical_block = VA / 10;
        int new_phys_block = allocateRandomBlock();
        int pte_address = (PTR * 10) + logical_block;
        string b_str = (new_phys_block < 10 ? "0" : "") + to_string(new_phys_block);
        M[pte_address][0] = '0';
        M[pte_address][1] = '0';
        M[pte_address][2] = b_str[0];
        M[pte_address][3] = b_str[1];
        
        PI = 0; SI = 0;
        int RA = addressMap(VA);
        for(int i=0; i<4; i++) M[RA][i] = GPR[i];
    }
    else if (TI == 0 && PI == 3) { TERMINATE(6); } 
    else if (TI == 2 && PI == 3) { TERMINATE(3); } 
    
    // Other errors
    else if (TI == 0 && PI == 1) { TERMINATE(4); } 
    else if (TI == 2 && PI == 1) { TERMINATE(3); } 
    else if (TI == 0 && PI == 2) { TERMINATE(5); } 
    else if (TI == 2 && PI == 2) { TERMINATE(3); }
    else if (TI == 2 && PI == 0) { TERMINATE(3); } 
}

void OS::STARTEXECUTION() {
    IC = 0;
    pcb.state = READY;
    cout << "\n\033[1;36m[::] KAAL OS CPU TRACE INITIALIZED [::]\033[0m\n";
    EXECUTEUSERPROGRAM();
}

void OS::EXECUTEUSERPROGRAM() {
    pcb.state = RUNNING;
    while (true) {
        int RA_IC = addressMap(IC);
        if (PI != 0) {
            MOS(-1);
            break;
        }

        for (int i = 0; i < 4; i++) {
            IR[i] = M[RA_IC][i];
        }
        
        cout << "\033[1;33m  [~] IC: \033[1;37m" << setfill('0') << setw(2) << IC 
             << " \033[1;33m| IR: \033[1;37m" << IR[0] << IR[1] << IR[2] << IR[3] 
             << " \033[1;33m| PTR: \033[1;37m" << PTR 
             << " \033[1;33m| TTC: \033[1;37m" << pcb.TTC 
             << "\033[0m\n";
             
        if (is_debug_mode) {
             cout << "\033[1;31m   [DEBUG] \033[1;36mPress ENTER to Step Over...\033[0m";
             char c;
             cin.get(c);
        } else {
             this_thread::sleep_for(chrono::milliseconds(150));
        }
        
        IC++;
        pcb.TTC++;
        
        if (pcb.TTC > pcb.TTL) {
            TI = 2; 
        }
        
        if (IR[0] == 'H') {
            SI = 3;
            MOS(-1);
            break;
        }
        
        int VA;
        if(isdigit(IR[2]) && isdigit(IR[3])) {
            VA = (IR[2] - '0') * 10 + (IR[3] - '0');
        } else {
            PI = 2; // Operand Error
            MOS(-1);
            break;
        }

        int RA;

        if (IR[0] == 'G' && IR[1] == 'D') {
            SI = 1;
            MOS(VA);
        } else if (IR[0] == 'P' && IR[1] == 'D') {
            SI = 2;
            MOS(VA);
        } else if (IR[0] == 'L' && IR[1] == 'R') {
            RA = addressMap(VA);
            if(PI == 0) {
                for (int i = 0; i < 4; i++) GPR[i] = M[RA][i];
            } else MOS(VA);
        } else if (IR[0] == 'S' && IR[1] == 'R') {
            RA = addressMap(VA);
            if(PI == 0) {
                for (int i = 0; i < 4; i++) M[RA][i] = GPR[i];
            } else MOS(VA);
        } else if (IR[0] == 'C' && IR[1] == 'R') {
            RA = addressMap(VA);
            if(PI == 0) {
                bool equal = true;
                for (int i = 0; i < 4; i++) {
                    if (GPR[i] != M[RA][i]) {
                        equal = false; break;
                    }
                }
                Toggle = equal;
            } else MOS(VA);
        } else if (IR[0] == 'B' && IR[1] == 'T') {
            if (Toggle) IC = VA;
        // Advanced ALU features
        } else if (IR[0] == 'A' && IR[1] == 'D') {
            RA = addressMap(VA);
            if(PI == 0) {
                int valGPR = getIntFromMem(GPR);
                int valMem = getIntFromMem(M[RA]);
                storeIntToMem(GPR, valGPR + valMem);
            } else MOS(VA);
        } else if (IR[0] == 'S' && IR[1] == 'B') {
            RA = addressMap(VA);
            if(PI == 0) {
                int valGPR = getIntFromMem(GPR);
                int valMem = getIntFromMem(M[RA]);
                storeIntToMem(GPR, valGPR - valMem);
            } else MOS(VA);
        } else if (IR[0] == 'M' && IR[1] == 'L') {
            RA = addressMap(VA);
            if(PI == 0) {
                int valGPR = getIntFromMem(GPR);
                int valMem = getIntFromMem(M[RA]);
                storeIntToMem(GPR, valGPR * valMem);
            } else MOS(VA);
        } else if (IR[0] == 'D' && IR[1] == 'V') {
            RA = addressMap(VA);
            if(PI == 0) {
                int valGPR = getIntFromMem(GPR);
                int valMem = getIntFromMem(M[RA]);
                if (valMem != 0) storeIntToMem(GPR, valGPR / valMem);
            } else MOS(VA);
        } else {
            PI = 1; // Operand Error / Opcode Error
            MOS(-1);
            break;
        }
        
        if (SI != 0 || PI != 0 || TI != 0) {
            if(SI==1 || SI==2 || SI==3) continue; 
            else break;
        }
    }
}

void OS::LOAD() {
    infile.open("input.txt");
    outfile.open("output.txt", ios::trunc); 
    
    if (!infile.is_open()) {
        cout << "\033[1;31m[-] Error: Could not open input.txt file!\033[0m\n";
        return;
    }

    string line;
    int program_card_index = 0;

    while (getline(infile, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (line.substr(0, 4) == "$AMJ") {
            cout << "\033[1;32m[+] $AMJ Detected: Initializing Phase 3 KAAL OS System...\033[0m\n";
            INIT();
            pcb.job_id = stoi(line.substr(4, 4));
            pcb.TTL = stoi(line.substr(8, 4));
            pcb.TLL = stoi(line.substr(12, 4));
            
            PTR = allocateRandomBlock();
            for(int i=0; i<10; i++){
               for(int j=0; j<4; j++){
                   M[PTR*10 + i][j] = '*';
               }
            }
            program_card_index = 0;

        } else if (line.substr(0, 4) == "$DTA") {
            cout << "\033[1;32m[+] $DTA Detected: Executing Ready Queue [Job " << pcb.job_id << "]\033[0m\n";
            if (is_debug_mode) cin.ignore(); // Wait out existing buffer for debug stepping
            STARTEXECUTION();
        } else if (line.substr(0, 4) == "$END") {
            cout << "\033[1;32m[+] $END Detected: Job Finished Successfully.\033[0m\n";
        } else {
            int limit = line.length();
            if (limit > 40) limit = 40;
            
            int phys_block = allocateRandomBlock();
            int pte_address = (PTR * 10) + program_card_index;
            string b_str = (phys_block < 10 ? "0" : "") + to_string(phys_block);
            M[pte_address][0] = '0';
            M[pte_address][1] = '0';
            M[pte_address][2] = b_str[0];
            M[pte_address][3] = b_str[1];
            
            for (int i = 0; i < limit; i++) {
                M[phys_block*10 + i / 4][i % 4] = line[i];
            }
            cout << "\033[1;34m[~] Spooling Program Block: Logical " << program_card_index << " -> Physical " << phys_block << "\033[0m\n";
            program_card_index++;
        }
    }

    infile.close();
    outfile.close();
    cout << "\n\033[1;33m[*] Advanced Phase 3 Execution Completed! Process Terminated.\033[0m\n";
}

void OS::printMemoryDump() {
    cout << "\n     \033[1;36m[::] KAAL OS Memory Dump (300 Blocks) [::]\033[0m\n\n";
    for(int i = 0; i < 300; i++) {
        cout << "\033[1;36mM[" << setfill('0') << setw(3) << i << "]\033[0m: ";
        for(int j = 0; j < 4; j++) {
            cout << M[i][j];
        }
        if((i+1) % 5 == 0) cout << endl;
        else cout << "  |  ";
    }
    cout << "\n";
}

void OS::printOutputFile() {
    ifstream read_out("output.txt");
    if(!read_out.is_open()) {
        cout << "\033[1;31m[-] Error: output.txt not found! (Run simulation first)\033[0m\n";
        return;
    }
    cout << "\n     \033[1;35m[::] KAAL OS Output Log [::]\033[0m\n\n";
    string line;
    while(getline(read_out, line)) {
        cout << "  " << line << "\n";
    }
    read_out.close();
}

void OS::printRegisters() {
    string states[] = {"NEW", "READY", "RUNNING", "WAITING", "TERMINATED"};

    cout << "\n\033[1;36m[::] KAAL OS CPU REGISTERS [::]\033[0m\n\n";
    cout << "\033[1;33m  [~] IC (Instruction Counter) : \033[1;37m" << IC << "\033[0m\n";
    cout << "\033[1;33m  [~] IR (Instruction Register): \033[1;37m" << IR[0] << IR[1] << IR[2] << IR[3] << "\033[0m\n";
    cout << "\033[1;33m  [~] GPR (General Purpose)    : \033[1;37m" << GPR[0] << GPR[1] << GPR[2] << GPR[3] << "\033[0m\n";
    cout << "\033[1;33m  [~] Toggle Register (C)      : \033[1;37m" << (Toggle ? "TRUE" : "FALSE") << "\033[0m\n";
    cout << "\033[1;33m  [~] PTR (Page Table Reg)     : \033[1;37m" << (PTR == -1 ? "NULL" : to_string(PTR)) << "\033[0m\n\n";
    cout << "\033[1;35m  [PROCESS CONTROL BLOCK]\033[0m\n";
    cout << "  Job ID       : " << pcb.job_id << "\n";
    cout << "  State        : \033[1;32m" << states[pcb.state] << "\033[0m\n";
    cout << "  Timers       : TTC " << pcb.TTC << " / " << pcb.TTL << " (Max)\n";
    cout << "  Line Writer  : TLC " << pcb.TLC << " / " << pcb.TLL << " (Max)\n\n";
    cout << "\033[1;35m  [INTERRUPTS]\033[0m\n";
    cout << "  SI (System)  : " << SI << "\n";
    cout << "  PI (Program) : " << PI << "\n";
    cout << "  TI (Timer)   : " << TI << "\n";
}

void OS::printPageTableExplorer() {
    if (PTR == -1) {
        cout << "\033[1;31m[-] Page Table currently unassigned. Run a job first!\033[0m\n";
        return;
    }
    cout << "\n\033[1;36m[::] KAAL OS PAGE TABLE EXPLORER [::]\033[0m\n\n";
    cout << "\033[1;35m  Page Table Register (PTR): \033[1;37m" << PTR << "\033[0m\n";
    cout << "  ------------------------------------------------\n";
    cout << "   Logical Block | Physical Block | Record Status \n";
    cout << "  ------------------------------------------------\n";
    for (int i = 0; i < 10; i++) {
        int pte_address = (PTR * 10) + i;
        string entry = "";
        for(int j = 0; j < 4; j++) entry += M[pte_address][j];
        
        bool valid = (entry[0] != '*' && entry[0] != ' ');
        string phys = valid ? to_string(stoi(entry)) : "---";
        string status = valid ? "\033[1;32mVALID\033[0m" : "\033[1;31mINVALID\033[0m";
        
        cout << "        " << setfill('0') << setw(2) << i << "       |       " 
             << (valid ? setfill('0') : setfill(' ')) << setw(2) << phys << "       |  " << status << "\n";
    }
    cout << "  ------------------------------------------------\n";
}
