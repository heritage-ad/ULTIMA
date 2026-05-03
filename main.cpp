/*Author: Inioluwa Oyatobo*/
#include "EncryptedIPC.h"
#include <iostream>
#include "Sched.h"
#include "Sema.h"
#include "IPC.h"
#include "MMU.h"
#include <fstream>

using namespace std;

// Log function
void logBoth(ofstream &logFile, string msg) {
    cout << msg;
    logFile << msg;
}

int main() {
    ofstream logFile("log.txt");

    // Create scheduler FIRST
    Scheduler sched;

    // Create semaphores
    Semaphore sem("Printer", 1, &sched);
    Semaphore core("Core Memory", 1, &sched);

    EncryptedIPC eipc(&sem);
    MMU mmu(1024, '.', 128, &core, &sched);


    sched.create_task("Task1");
    sched.create_task("Task2");
    sched.create_task("Task3");


    // PHASE 2 (IPC)


    logBoth(logFile, "\n--- SECURE IPC: Sending Messages ---\n");

    eipc.Message_Send(2, 1, "Hello from Task2");
    eipc.Message_Send(3, 1, "Secret Message");

    //ipc.dump();

    logBoth(logFile, "\n--- SECURE IPC: Receiving Messages ---\n");

    eipc.Message_Receive(1);
    eipc.Message_Receive(1);

    //ipc.dump();

    
    // PHASE 1 (Scheduler + Semaphore)
    

    logBoth(logFile, "\n--- Initial State ---\n");
    sched.dump();

    logBoth(logFile, "\n--- Task1 requesting resource ---\n");
    sem.down(1);
    sched.dump();
    sem.dump();

    logBoth(logFile, "\n[Scheduler] Task " + to_string(sched.getCurrentTask()) + " is yielding CPU...\n");
    sched.yield();
    sched.dump();

    logBoth(logFile, "\n--- Task2 requesting resource ---\n");
    sem.down(2);
    sched.dump();
    sem.dump();

    logBoth(logFile, "\n[Scheduler] Task " + to_string(sched.getCurrentTask()) + " is yielding CPU...\n");
    sched.yield();
    sched.dump();

    logBoth(logFile, "\n--- Task3 requesting resource ---\n");
    sem.down(3);
    sched.dump();
    sem.dump();

    logBoth(logFile, "\n--- Releasing resource ---\n");
    sem.up(1);
    sched.dump();
    sem.dump();

    
    // PHASE 3 (Memory)
    logBoth(logFile, "     PHASE 3: MEMORY        \n");

    // Allocate
    logBoth(logFile, "\n--- Memory Allocation ---\n");

    int h1 = mmu.Mem_Alloc(1, 100);
    logBoth(logFile, "Task1 allocated memory (Handle " + to_string(h1) + ")\n");

    int h2 = mmu.Mem_Alloc(2, 200);
    logBoth(logFile, "Task2 allocated memory (Handle " + to_string(h2) + ")\n");

    // Force failure
    int h3 = mmu.Mem_Alloc(3, 900);
    if (h3 == -1) {
        logBoth(logFile, "Task3 failed to allocate memory (Not enough memory)\n");
    }

    //Write
    logBoth(logFile, "\n--- Writing ---\n");
    mmu.Mem_Write(1, h1, 'A');
    mmu.Mem_Write(1, h1, 'B');
    mmu.Mem_Write(2, h2, 'X');

    //Read
    logBoth(logFile, "\n--- Reading ---\n");
    char ch;
    mmu.Mem_Read(1, h1, &ch);
    logBoth(logFile, string("Task1 read: ") + ch + "\n");

    //Free
    logBoth(logFile, "\n--- Freeing Memory ---\n");
    mmu.Mem_Free(1, h1);
    logBoth(logFile, "Task1 freed memory\n");

    //Reallocate
    logBoth(logFile, "\n--- Reallocation ---\n");
    h3 = mmu.Mem_Alloc(3, 100);
    if (h3 != -1) {
        logBoth(logFile, "Task3 successfully allocated memory after free\n");
    }

    //Dump
    logBoth(logFile, "\n--- Memory Dump ---\n");
    mmu.Mem_Dump(0, 256);

    logBoth(logFile, "\n--- Memory Block List ---\n");
    mmu.List_Dump();

    //Stats (bonus)
    logBoth(logFile, "\nMemory Left: " + to_string(mmu.Mem_Left()) + "\n");
    logBoth(logFile, "Largest Free Block: " + to_string(mmu.Mem_Largest()) + "\n");

    return 0;
}