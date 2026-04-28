#include "../include/OS.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <ctime>

using namespace std;

// ─────────────────────────────────────────────────────────────
//  Utility helpers
// ─────────────────────────────────────────────────────────────
static string timestamp() {
    time_t t = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&t));
    return buf;
}

static string trimRight(const string& s) {
    size_t e = s.find_last_not_of(' ');
    return (e == string::npos) ? "" : s.substr(0, e + 1);
}

// ─────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────
OS::OS() : is_debug_mode(false) {
    srand(time(0));
}

// ─────────────────────────────────────────────────────────────
//  Internal log helper — writes coloured line to stdout + log buffer
// ─────────────────────────────────────────────────────────────
void OS::log(const string& colour, const string& tag, const string& msg) {
    string line = colour + "[" + tag + "] " + "\033[0m" + msg;
    cout << "  " << line << "\n";
    exec_log.push_back("[" + tag + "] " + msg);
}

// ─────────────────────────────────────────────────────────────
//  Virtual → Physical address translation
// ─────────────────────────────────────────────────────────────
int OS::addressMap(int VA) {
    if (VA < 0 || VA > 99) { cpu.PI = 2; return -1; }
    int lb  = VA / 10;
    int pte = cpu.PTR * 10 + lb;
    string entry = "";
    for (int i = 0; i < 4; i++) entry += mem.M[pte][i];
    if (entry[0] == '*' || entry[0] == ' ') { cpu.PI = 3; return -1; }
    int phys = stoi(entry);
    return phys * 10 + (VA % 10);
}

// ─────────────────────────────────────────────────────────────
//  System Calls
// ─────────────────────────────────────────────────────────────
void OS::READ(int RA) {
    string line;
    if (!getline(infile, line)) { TERMINATE(1); return; }
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line.substr(0, 4) == "$END") { TERMINATE(1); return; }
    int limit = min((int)line.length(), 40);
    int bs    = (RA / 10) * 10;
    for (int i = 0; i < limit; i++)
        mem.M[bs + i / 4][i % 4] = line[i];
    log("\033[1;34m", "GD", "Data loaded into block " + to_string(bs / 10) +
        " (phys rows " + to_string(bs) + "-" + to_string(bs + 9) + ")  data=\"" + trimRight(line) + "\"");
}

void OS::WRITE(int RA) {
    pcb.TLC++;
    if (pcb.TLC > pcb.TLL) {
        cpu.TI = 2;
        log("\033[1;31m", "LLE", "Line limit exceeded  TLC=" + to_string(pcb.TLC) + " TLL=" + to_string(pcb.TLL));
        TERMINATE(2); return;
    }
    int bs = (RA / 10) * 10;
    string out = "";
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 4; j++)
            out += mem.M[bs + i][j];
    string clean = trimRight(out);
    outfile << clean << "\n";
    log("\033[1;35m", "PD", "Printed block " + to_string(bs / 10) + "  output=\"" + clean + "\"");
}

// ─────────────────────────────────────────────────────────────
//  Terminate job — write structured report to output.txt + stdout
// ─────────────────────────────────────────────────────────────
void OS::TERMINATE(int ec) {
    pcb.state = TERMINATED;
    static const char* msgs[] = {
        "NORMAL EXECUTION COMPLETED",
        "DATA OUT OF RANGE ($END hit during GD)",
        "LINE LIMIT EXCEEDED (TLC > TLL)",
        "TIME LIMIT EXCEEDED (TTC > TTL)",
        "OPERATION CODE ERROR (invalid opcode)",
        "OPERAND ERROR (invalid address)",
        "INVALID PAGE FAULT (unmapped data access)"
    };
    static const char* sev[] = {
        "\033[1;32m", "\033[1;33m", "\033[1;33m",
        "\033[1;33m", "\033[1;31m", "\033[1;31m", "\033[1;31m"
    };

    string irStr   = string(1,cpu.IR[0])  + cpu.IR[1]  + cpu.IR[2]  + cpu.IR[3];
    string gprTrim = trimRight(string(1,cpu.GPR[0]) + cpu.GPR[1] + cpu.GPR[2] + cpu.GPR[3]);
    string ttcStr  = to_string(pcb.TTC)+"/"+to_string(pcb.TTL);
    string tlcStr  = to_string(pcb.TLC)+"/"+to_string(pcb.TLL);
    string intStr  = "SI="+to_string(cpu.SI)+" PI="+to_string(cpu.PI)+" TI="+to_string(cpu.TI);

    // Fixed-width box helper: pads content to W chars then appends closing ║
    const int W = 46;
    string hbar(W+2, '=');
    auto boxLine = [&](const string& col, const string& content) {
        string padded = content;
        if ((int)padded.size() < W) padded += string(W - padded.size(), ' ');
        else padded = padded.substr(0, W);
        cout << "  " << col << "\u2551  " << padded << "\u2551\033[0m\n";
    };

    // ── stdout banner ──
    cout << "\n  " << sev[ec] << "\u2554" << string(W+2,'\u2550') << "\u2557\033[0m\n";
    boxLine(sev[ec], "JOB "+to_string(pcb.job_id)+"  TERMINATED  [CODE:"+to_string(ec)+"]");
    boxLine(sev[ec], string(msgs[ec]));
    cout << "  " << sev[ec] << "\u2560" << string(W+2,'\u2550') << "\u2563\033[0m\n";
    boxLine(sev[ec], "IC="+to_string(cpu.IC)+"  IR="+irStr+"  GPR=["+gprTrim+"]");
    boxLine(sev[ec], "TTC="+ttcStr+"  TLC="+tlcStr+"  PTR="+to_string(cpu.PTR));
    boxLine(sev[ec], intStr+"  Toggle="+(cpu.Toggle ? "T" : "F"));
    cout << "  " << sev[ec] << "\u255a" << string(W+2,'\u2550') << "\u255d\033[0m\n\n";

    // ── output.txt report ──
    outfile << "╔══════════════════════════════════════════╗\n";
    outfile << "║  JOB " << setw(4) << pcb.job_id << "  TERMINATED  CODE:" << ec << "\n";
    outfile << "║  STATUS : " << msgs[ec] << "\n";
    outfile << "╠══════════════════════════════════════════╣\n";
    outfile << "║  IC=" << cpu.IC << "  IR=" << irStr << "\n";
    outfile << "║  TTC=" << pcb.TTC << "/" << pcb.TTL
            << "  TLC=" << pcb.TLC << "/" << pcb.TLL << "\n";
    outfile << "║  SI=" << cpu.SI << " PI=" << cpu.PI << " TI=" << cpu.TI
            << "  Toggle=" << (cpu.Toggle ? "T" : "F") << "\n";
    outfile << "╠══════════════════════════════════════════╣\n";
    outfile << "║  EXECUTION LOG:\n";
    for (auto& l : exec_log) outfile << "║    " << l << "\n";
    outfile << "╚══════════════════════════════════════════╝\n\n";
}

// ─────────────────────────────────────────────────────────────
//  Master OS Service dispatcher
// ─────────────────────────────────────────────────────────────
void OS::MOS(int VA, bool is_syscall) {
    // ── Halt ──
    if (cpu.SI == 3) { TERMINATE(0); return; }

    // ── GD ──
    if (cpu.SI == 1 && cpu.PI == 0) {
        if (cpu.TI == 2) { TERMINATE(3); return; }
        // Dynamic page fault: allocate new block
        if (cpu.PI == 3) {
            int lb = VA / 10;
            int nb = mem.allocateRandomBlock();
            if (nb == -1) { cpu.PI = 3; TERMINATE(6); return; }
            string bs = (nb < 10 ? "0" : "") + to_string(nb);
            int pte = cpu.PTR * 10 + lb;
            mem.M[pte][0]='0'; mem.M[pte][1]='0';
            mem.M[pte][2]=bs[0]; mem.M[pte][3]=bs[1];
            cpu.PI = 0;
            log("\033[1;34m", "PF-GD", "Page fault resolved  Logical=" + to_string(lb) +
                " -> Physical=" + to_string(nb));
        }
        READ(addressMap(VA));
        cpu.SI = 0; return;
    }

    // ── PD ──
    if (cpu.SI == 2 && cpu.PI == 0) {
        if (cpu.TI == 2) { WRITE(addressMap(VA)); TERMINATE(3); return; }
        int RA = addressMap(VA);
        if (cpu.PI != 0) { TERMINATE(6); return; }   // page fault on read
        WRITE(RA);
        cpu.SI = 0; return;
    }

    // ── GD page fault path (pre-set before MOS call) ──
    if (cpu.PI == 3 && cpu.IR[0]=='G' && cpu.IR[1]=='D') {
        if (cpu.TI == 2) { TERMINATE(3); return; }
        int lb = VA / 10;
        int nb = mem.allocateRandomBlock();
        if (nb == -1) { TERMINATE(6); return; }
        string bs = (nb < 10 ? "0" : "") + to_string(nb);
        int pte = cpu.PTR * 10 + lb;
        mem.M[pte][0]='0'; mem.M[pte][1]='0';
        mem.M[pte][2]=bs[0]; mem.M[pte][3]=bs[1];
        log("\033[1;34m", "PF-GD", "Page fault resolved  Logical=" + to_string(lb) +
            " -> Physical=" + to_string(nb));
        cpu.PI = 0; cpu.SI = 1;
        READ(addressMap(VA));
        cpu.SI = 0; return;
    }

    // ── SR page fault path ──
    if (cpu.PI == 3 && cpu.IR[0]=='S' && cpu.IR[1]=='R') {
        if (cpu.TI == 2) { TERMINATE(3); return; }
        int lb = VA / 10;
        int nb = mem.allocateRandomBlock();
        if (nb == -1) { TERMINATE(6); return; }
        string bs = (nb < 10 ? "0" : "") + to_string(nb);
        int pte = cpu.PTR * 10 + lb;
        mem.M[pte][0]='0'; mem.M[pte][1]='0';
        mem.M[pte][2]=bs[0]; mem.M[pte][3]=bs[1];
        cpu.PI = 0;
        int RA = addressMap(VA);
        for (int i = 0; i < 4; i++) mem.M[RA][i] = cpu.GPR[i];
        return;
    }

    // ── Error dispatching ──
    if (cpu.TI == 2 && cpu.PI == 0) { TERMINATE(3); return; }
    if (cpu.PI == 1) { TERMINATE(4); return; }
    if (cpu.PI == 2) { TERMINATE(5); return; }
    if (cpu.PI == 3) { TERMINATE(6); return; }
    if (cpu.TI == 2) { TERMINATE(3); return; }
}

// ─────────────────────────────────────────────────────────────
//  Fetch–Decode–Execute cycle
// ─────────────────────────────────────────────────────────────
void OS::STARTEXECUTION() {
    cpu.IC = 0;
    pcb.state = READY;
    exec_log.clear();
    cout << "\n  \033[1;36m┌─────────────────────────────────────────────────────────────┐\033[0m\n";
    cout << "  \033[1;36m│  CPU EXECUTION START  Job=" << setw(4) << pcb.job_id
         << "  TTL=" << setw(5) << pcb.TTL << "  TLL=" << setw(5) << pcb.TLL
         << "  PTR=" << setw(2) << cpu.PTR << "  │\033[0m\n";
    cout << "  \033[1;36m└─────────────────────────────────────────────────────────────┘\033[0m\n\n";
    EXECUTEUSERPROGRAM();
}

void OS::EXECUTEUSERPROGRAM() {
    pcb.state = RUNNING;
    bool terminated = false;

    while (!terminated) {
        // ── Fetch ──
        cpu.PI = 0;                         // ← clear stale PI before each fetch
        int RA_IC = addressMap(cpu.IC);
        if (cpu.PI != 0) {
            // Instruction fetch itself caused a page fault or operand error
            MOS(-1, false);
            break;
        }
        for (int i = 0; i < 4; i++) cpu.IR[i] = mem.M[RA_IC][i];

        // ── Trace ──
        string gprDisp = string(1,cpu.GPR[0])+cpu.GPR[1]+cpu.GPR[2]+cpu.GPR[3];
        cout << "  \033[1;33m▶ IC:\033[1;37m" << setfill('0') << setw(2) << cpu.IC
             << "\033[1;33m  IR:\033[1;37m" << cpu.IR[0]<<cpu.IR[1]<<cpu.IR[2]<<cpu.IR[3]
             << "\033[1;33m  GPR:\033[1;37m[" << trimRight(gprDisp) << "]"
             << "\033[1;33m  Tgl:\033[1;37m" << (cpu.Toggle ? "\033[1;32mT" : "\033[1;31mF")
             << "\033[1;33m  TTC:\033[1;37m" << pcb.TTC
             << "\033[1;33m  SI:\033[1;37m" << cpu.SI
             << "\033[0m\n";

        if (is_debug_mode) {
            cout << "  \033[1;31m[DEBUG] Press ENTER to step (q+ENTER to quit debug)...\033[0m";
            string dbg; getline(cin, dbg);
            if (dbg == "q") { is_debug_mode = false; cout << "  \033[1;33m[~] Debug mode OFF\033[0m\n"; }
        } else {
            this_thread::sleep_for(chrono::milliseconds(80));
        }

        // ── Tick ──
        cpu.IC++;
        pcb.TTC++;
        if (pcb.TTC > pcb.TTL) cpu.TI = 2;

        // ── Decode & Execute ──
        if (cpu.IR[0] == 'H') {
            log("\033[1;32m", " H ", "Halt instruction encountered");
            cpu.SI = 3;
            TERMINATE(0);
            terminated = true; break;
        }

        // Parse operand
        int VA;
        if (isdigit(cpu.IR[2]) && isdigit(cpu.IR[3])) {
            VA = (cpu.IR[2] - '0') * 10 + (cpu.IR[3] - '0');
        } else {
            log("\033[1;31m", "ERR", string("Bad operand: ") + cpu.IR[2] + cpu.IR[3]);
            cpu.PI = 2; MOS(-1, false); terminated = true; break;
        }

        int RA;
        bool stop = false;

        if (cpu.IR[0]=='G' && cpu.IR[1]=='D') {
            log("\033[1;34m", " GD", "Get Data -> VA=" + to_string(VA));
            cpu.SI = 1; cpu.PI = 0;
            RA = addressMap(VA);           // may set PI=3
            MOS(VA, true);
            if (pcb.state == TERMINATED) { stop = true; }
            cpu.SI = 0;

        } else if (cpu.IR[0]=='P' && cpu.IR[1]=='D') {
            log("\033[1;35m", " PD", "Print Data -> VA=" + to_string(VA));
            cpu.SI = 2; cpu.PI = 0;
            MOS(VA, true);
            if (pcb.state == TERMINATED) { stop = true; }
            cpu.SI = 0;

        } else if (cpu.IR[0]=='L' && cpu.IR[1]=='R') {
            RA = addressMap(VA);
            if (cpu.PI != 0) { MOS(VA, false); stop = true; }
            else {
                for (int i = 0; i < 4; i++) cpu.GPR[i] = mem.M[RA][i];
                log("\033[1;36m", " LR", "GPR <- M[" + to_string(VA) + "] = [" + trimRight(string(mem.M[RA],4)) + "]");
            }

        } else if (cpu.IR[0]=='S' && cpu.IR[1]=='R') {
            cpu.PI = 0; RA = addressMap(VA);
            if (cpu.PI == 3) { MOS(VA, false); if (pcb.state == TERMINATED) stop = true; }
            else if (cpu.PI != 0) { MOS(VA, false); stop = true; }
            else {
                for (int i = 0; i < 4; i++) mem.M[RA][i] = cpu.GPR[i];
                log("\033[1;36m", " SR", "M[" + to_string(VA) + "] <- GPR = [" + trimRight(string(cpu.GPR,4)) + "]");
            }

        } else if (cpu.IR[0]=='C' && cpu.IR[1]=='R') {
            RA = addressMap(VA);
            if (cpu.PI != 0) { MOS(VA, false); stop = true; }
            else {
                bool eq = true;
                for (int i = 0; i < 4; i++) if (cpu.GPR[i] != mem.M[RA][i]) { eq = false; break; }
                cpu.Toggle = eq;
                log("\033[1;36m", " CR", "Compare GPR vs M[" + to_string(VA) + "]  Toggle=" + (eq?"TRUE":"FALSE"));
            }

        } else if (cpu.IR[0]=='B' && cpu.IR[1]=='T') {
            if (cpu.Toggle) {
                log("\033[1;33m", " BT", "Branch taken  IC=" + to_string(cpu.IC) + " -> " + to_string(VA));
                cpu.IC = VA;
            } else {
                log("\033[1;33m", " BT", "Branch NOT taken (Toggle=F)");
            }

        } else if (cpu.IR[0]=='A' && cpu.IR[1]=='D') {
            RA = addressMap(VA);
            if (cpu.PI != 0) { MOS(VA,false); stop=true; }
            else {
                int a = cpu.getIntFromMem(cpu.GPR), b = cpu.getIntFromMem(mem.M[RA]);
                cpu.storeIntToMem(cpu.GPR, a + b);
                log("\033[1;36m", " AD", "GPR=" + to_string(a) + " + M[" + to_string(VA) + "]=" + to_string(b) + " = " + to_string(a+b));
            }

        } else if (cpu.IR[0]=='S' && cpu.IR[1]=='B') {
            RA = addressMap(VA);
            if (cpu.PI != 0) { MOS(VA,false); stop=true; }
            else {
                int a = cpu.getIntFromMem(cpu.GPR), b = cpu.getIntFromMem(mem.M[RA]);
                cpu.storeIntToMem(cpu.GPR, a - b);
                log("\033[1;36m", " SB", "GPR=" + to_string(a) + " - M[" + to_string(VA) + "]=" + to_string(b) + " = " + to_string(a-b));
            }

        } else if (cpu.IR[0]=='M' && cpu.IR[1]=='L') {
            RA = addressMap(VA);
            if (cpu.PI != 0) { MOS(VA,false); stop=true; }
            else {
                int a = cpu.getIntFromMem(cpu.GPR), b = cpu.getIntFromMem(mem.M[RA]);
                cpu.storeIntToMem(cpu.GPR, a * b);
                log("\033[1;36m", " ML", "GPR=" + to_string(a) + " * M[" + to_string(VA) + "]=" + to_string(b) + " = " + to_string(a*b));
            }

        } else if (cpu.IR[0]=='D' && cpu.IR[1]=='V') {
            RA = addressMap(VA);
            if (cpu.PI != 0) { MOS(VA,false); stop=true; }
            else {
                int a = cpu.getIntFromMem(cpu.GPR), b = cpu.getIntFromMem(mem.M[RA]);
                if (b == 0) { log("\033[1;31m","ERR","Division by zero"); cpu.PI=1; MOS(-1,false); stop=true; }
                else {
                    cpu.storeIntToMem(cpu.GPR, a / b);
                    log("\033[1;36m", " DV", "GPR=" + to_string(a) + " / M[" + to_string(VA) + "]=" + to_string(b) + " = " + to_string(a/b));
                }
            }

        } else {
            string bad = string(1,cpu.IR[0]) + cpu.IR[1];
            log("\033[1;31m", "ERR", "Unknown opcode: '" + bad + "'");
            cpu.PI = 1; MOS(-1, false); stop = true;
        }

        // TLE boundary check after instruction
        if (!stop && cpu.TI == 2) {
            log("\033[1;33m", "TLE", "Time limit exceeded TTC=" + to_string(pcb.TTC));
            MOS(-1, false); stop = true;
        }

        terminated = stop;
    }
}

// ─────────────────────────────────────────────────────────────
//  Job Loader
// ─────────────────────────────────────────────────────────────
void OS::LOAD() {
    infile.open("input.txt");
    outfile.open("output.txt", ios::trunc);
    if (!infile.is_open()) {
        cout << "\033[1;31m  [-] Error: input.txt not found!\033[0m\n"; return;
    }

    string line;
    int prog_idx = 0, job_count = 0;
    while (getline(infile, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (line.substr(0, 4) == "$AMJ") {
            job_count++;
            mem.init(); cpu.init(); pcb = {0,0,0,0,0,NEW};
            pcb.job_id = stoi(line.substr(4,4));
            pcb.TTL    = stoi(line.substr(8,4));
            pcb.TLL    = stoi(line.substr(12,4));
            cpu.PTR    = mem.allocateRandomBlock();
            // Mark PTR block as page table (sentinel)
            for (int i=0; i<10; i++) for (int j=0; j<4; j++) mem.M[cpu.PTR*10+i][j]='*';
            prog_idx = 0;
            cout << "\n  \033[1;32m┌─ $AMJ  Job=" << pcb.job_id
                 << "  TTL=" << pcb.TTL << "  TLL=" << pcb.TLL
                 << "  PTR=Block" << cpu.PTR << " ─\033[0m\n";
            outfile << "══════════════════════════════════════════════\n";
            outfile << "JOB " << pcb.job_id << "  TTL=" << pcb.TTL << "  TLL=" << pcb.TLL << "\n";

        } else if (line.substr(0, 4) == "$DTA") {
            cout << "  \033[1;32m├─ $DTA  Dispatching Job " << pcb.job_id << " to CPU\033[0m\n";
            if (is_debug_mode) {
                cout << "  \033[1;31m[DEBUG] Press ENTER to begin execution...\033[0m";
                cin.ignore(); string tmp; getline(cin, tmp);
            }
            STARTEXECUTION();

        } else if (line.substr(0, 4) == "$END") {
            cout << "  \033[1;32m└─ $END  Job " << pcb.job_id << " complete\033[0m\n";

        } else {
            // Program card — spool to random physical block
            int limit = min((int)line.length(), 40);
            int phys  = mem.allocateRandomBlock();
            int pte   = cpu.PTR * 10 + prog_idx;
            string bs = (phys < 10 ? "0" : "") + to_string(phys);
            mem.M[pte][0]='0'; mem.M[pte][1]='0';
            mem.M[pte][2]=bs[0]; mem.M[pte][3]=bs[1];
            for (int i = 0; i < limit; i++) mem.M[phys*10 + i/4][i%4] = line[i];
            cout << "  \033[1;34m│  Spool: LogBlock " << prog_idx
                 << " -> PhysBlock " << phys
                 << "  [" << trimRight(line.substr(0, min(20, (int)line.size()))) << "]\033[0m\n";
            prog_idx++;
        }
    }
    infile.close(); outfile.close();
    cout << "\n  \033[1;33m[✓] " << job_count << " job(s) processed.  Output → output.txt\033[0m\n";
}

// ─────────────────────────────────────────────────────────────
//  Inspection utilities
// ─────────────────────────────────────────────────────────────
void OS::printMemoryDump() { mem.printDump(); }

void OS::printOutputFile() {
    ifstream f("output.txt");
    if (!f.is_open()) { cout << "\033[1;31m  [-] No output.txt (run a job first)\033[0m\n"; return; }
    cout << "\n\033[1;35m  ╔══════════════════════════════╗\033[0m\n";
    cout << "\033[1;35m  ║    KAAL OS  OUTPUT LOG       ║\033[0m\n";
    cout << "\033[1;35m  ╚══════════════════════════════╝\033[0m\n\n";
    int ln = 1;
    for (string l; getline(f, l);)
        cout << "  \033[0;36m" << setw(3) << ln++ << " │\033[0m " << l << "\n";
}

void OS::printRegisters() { cpu.printRegisters(pcb); }

void OS::printPageTableExplorer() {
    if (cpu.PTR == -1) { cout << "\033[1;31m  [-] No PTR: run a job first\033[0m\n"; return; }
    cout << "\n\033[1;36m  ╔══════════════ PAGE TABLE EXPLORER ══════════════╗\033[0m\n";
    cout << "\033[1;36m  ║  PTR Phys Block: " << setw(2) << cpu.PTR
         << "   (PTR rows " << cpu.PTR*10 << "-" << cpu.PTR*10+9
         << ")                   ║\033[0m\n";
    cout << "\033[1;36m  ╠════════════╦═══════════════╦════════════╦════════╣\033[0m\n";
    cout << "\033[1;36m  ║  LogBlock  ║  PhysBlock    ║  PhysRows  ║ Status ║\033[0m\n";
    cout << "\033[1;36m  ╠════════════╬═══════════════╬════════════╬════════╣\033[0m\n";
    for (int i = 0; i < 10; i++) {
        int  pte = cpu.PTR * 10 + i;
        string e = "";
        for (int j = 0; j < 4; j++) e += mem.M[pte][j];
        bool valid = (e[0] != '*' && e[0] != ' ');
        int  phys  = valid ? stoi(e) : -1;
        string srows = valid ? to_string(phys*10) + "-" + to_string(phys*10+9) : "---";
        string stat  = valid ? "\033[1;32mVALID \033[0m" : "\033[1;31mFAULT \033[0m";
        cout << "\033[1;36m  ║\033[0m    " << setw(6) << i
             << "\033[1;36m  ║\033[0m    " << setw(9) << (valid ? to_string(phys) : "---")
             << "\033[1;36m  ║\033[0m    " << setw(7) << srows
             << "\033[1;36m  ║\033[0m " << stat << "\033[1;36m║\033[0m\n";
    }
    cout << "\033[1;36m  ╚════════════╩═══════════════╩════════════╩════════╝\033[0m\n\n";
    cout << "  \033[1;33mAllocated blocks: \033[1;37m";
    for (int b : mem.allocated_blocks) cout << b << " ";
    cout << "\033[0m\n";
}
