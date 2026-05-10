/*Author: Inioluwa Oyatobo*/
#include "EncryptedIPC.h"
#include <iostream>
#include "Sched.h"
#include "Sema.h"
#include "IPC.h"
#include "MMU.h"
#include <fstream>
# include <pthread.h>
# include <assert.h>
# include <time.h>
# include <unistd.h>
# include <ncurses.h>
# include <stdarg.h>
# include <termios.h>
# include <fcntl.h>

using namespace std;

// Log function
void logBoth(ofstream &logFile, WINDOW* win, string msg) {

    wprintw(win, "%s", msg.c_str());

    wrefresh(win);

    logFile << msg;
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    clear();
    refresh();
    ofstream logFile("log.txt");

    WINDOW* headerWin = newwin(5, 120, 0, 0);
    WINDOW* logWin = newwin(25, 120, 5, 0);
    WINDOW* consoleWin = newwin(8, 120, 30, 0);

    box(headerWin, 0, 0);
    box(logWin, 0, 0);
    box(consoleWin, 0, 0);

    mvwprintw(headerWin, 1, 2,
    "ULTIMA OS - Secure IPC / Scheduler / MMU");

    mvwprintw(headerWin, 2, 2,
    "Phase 4 Integration Demo");

    wrefresh(headerWin);

    scrollok(logWin, TRUE);

    wrefresh(logWin);
    wrefresh(consoleWin);

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


    logBoth(logFile,logWin, "\n--- SECURE IPC: Sending Messages ---\n");

    eipc.Message_Send(2, 1, "Hello from Task2");
    eipc.Message_Send(3, 1, "Secret Message");

    //ipc.dump();

    logBoth(logFile,logWin, "\n--- SECURE IPC: Receiving Messages ---\n");

    eipc.Message_Receive(1);
    eipc.Message_Receive(1);

    //ipc.dump();

    
    // PHASE 1 (Scheduler + Semaphore)
    

    logBoth(logFile,logWin, "\n--- Initial State ---\n");
    //sched.dump();

    logBoth(logFile,logWin, "\n--- Task1 requesting resource ---\n");
    sem.down(1);
    //sched.dump();
    //sem.dump();

    logBoth(logFile, logWin,"\n[Scheduler] Task " + to_string(sched.getCurrentTask()) + " is yielding CPU...\n");
    sched.yield();
    //sched.dump();

    logBoth(logFile,logWin, "\n--- Task2 requesting resource ---\n");
    sem.down(2);
    //sched.dump();
    //sem.dump();

    logBoth(logFile,logWin, "\n[Scheduler] Task " + to_string(sched.getCurrentTask()) + " is yielding CPU...\n");
    sched.yield();
   // sched.dump();

    logBoth(logFile, logWin,"\n--- Task3 requesting resource ---\n");
    sem.down(3);
    //sched.dump();
    //em.dump();

    logBoth(logFile,logWin, "\n--- Releasing resource ---\n");
    sem.up(1);
    //sched.dump();
    //sem.dump();

    
    // PHASE 3 (Memory)
    logBoth(logFile, logWin,"     PHASE 3: MEMORY        \n");

    // Allocate
    logBoth(logFile, logWin,"\n--- Memory Allocation ---\n");

    int h1 = mmu.Mem_Alloc(1, 100);
    logBoth(logFile,logWin, "Task1 allocated memory (Handle " + to_string(h1) + ")\n");

    int h2 = mmu.Mem_Alloc(2, 200);
    logBoth(logFile,logWin, "Task2 allocated memory (Handle " + to_string(h2) + ")\n");

    // Force failure
    int h3 = mmu.Mem_Alloc(3, 900);
    if (h3 == -1) {
        logBoth(logFile,logWin, "Task3 failed to allocate memory (Not enough memory)\n");
    }

    //Write
    logBoth(logFile,logWin, "\n--- Writing ---\n");
    mmu.Mem_Write(1, h1, 'A');
    mmu.Mem_Write(1, h1, 'B');
    mmu.Mem_Write(2, h2, 'X');

    //Read
    logBoth(logFile, logWin,"\n--- Reading ---\n");
    char ch;
    mmu.Mem_Read(1, h1, &ch);
    logBoth(logFile,logWin, string("Task1 read: ") + ch + "\n");

    //Free
    logBoth(logFile, logWin,"\n--- Freeing Memory ---\n");
    mmu.Mem_Free(1, h1);
    logBoth(logFile, logWin,"Task1 freed memory\n");

    //Reallocate
    logBoth(logFile, logWin,"\n--- Reallocation ---\n");
    h3 = mmu.Mem_Alloc(3, 100);
    if (h3 != -1) {
        logBoth(logFile, logWin,"Task3 successfully allocated memory after free\n");
    }

    //Dump
    logBoth(logFile,logWin, "\n--- Memory Dump ---\n");
    //mmu.Mem_Dump(0, 256);

    logBoth(logFile, logWin,"\n--- Memory Block List ---\n");
    //mmu.List_Dump();

    //Stats (bonus)
    logBoth(logFile, logWin,"\nMemory Left: " + to_string(mmu.Mem_Left()) + "\n");
    logBoth(logFile, logWin,"Largest Free Block: " + to_string(mmu.Mem_Largest()) + "\n");
    mvwprintw(consoleWin, 1, 2,
    "Press any key to exit...");

    wrefresh(consoleWin);

    wgetch(consoleWin);

    endwin();
    return 0;
}