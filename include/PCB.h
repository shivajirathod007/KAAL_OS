#ifndef PCB_H
#define PCB_H

enum ProcessState { NEW, READY, RUNNING, WAITING, TERMINATED };

struct PCB {
    int job_id;
    int TTL; 
    int TLL; 
    int TTC; 
    int TLC; 
    ProcessState state;
};

#endif
