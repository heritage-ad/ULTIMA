#include <iostream>
#include "Sched.h"
#include "Sema.h"
#include "IPC.h"
#include <fstream>

using namespace std;
void logBoth(ofstream &logFile, string msg) {
    cout << msg;
    logFile << msg;
}
int main() {
   ofstream logFile("log.txt");
    // Create scheduler
    Scheduler sched;

    // Create semaphore 
    Semaphore sem("Printer", 1, &sched);

    //Create IPC object
    IPC ipc(&sem, &sched);

    // Create tasks
    sched.create_task("Task1");
    sched.create_task("Task2");
    sched.create_task("Task3");

    logBoth(logFile, "\n--- IPC: Sending Messages ---\n");

    ipc.Message_Send(2, 1, "Hello from Task2");
    ipc.Message_Send(3, 1, "Hello from Task3");

    ipc.dump();

    logBoth(logFile, "\n--- IPC: Receiving Messages ---\n");

    ipc.Message_Receive(1);
    ipc.Message_Receive(1);

    ipc.dump();
    logBoth(logFile, "\n--- Initial State ---\n");
    sched.dump();

    // Task1 runs and acquires resource
    logBoth(logFile, "\n--- Task1 requesting resource ---\n");
    sem.down(1);  // assuming ID = 1
    sched.dump();
    sem.dump();

     // Switch to next task
    cout << "\n--- yield() ---\n";
    // Yield with explanation
    int current = sched.getCurrentTask();
    logBoth(logFile, "\n[Scheduler] Task " + to_string(current) + " is yielding CPU...\n");
    sched.yield(); 
    sched.dump();

    logBoth(logFile, "\n--- Task2 requesting resource ---\n");
    sem.down(2);

    sched.dump();
    sem.dump();

    // Switch again
    cout << "\n--- yield() ---\n";
    // Yield with explanation
    logBoth(logFile, "\n[Scheduler] Task " + to_string(current) + " is yielding CPU...\n");
    sched.yield();   
    sched.dump();


    logBoth(logFile, "\n--- Task3 requesting resource ---\n");
    sem.down(3);

    sched.dump();
    sem.dump();

    // Release resource
    logBoth(logFile, "\n--- Releasing resource ---\n");
    sem.up(1);

    sched.dump();
    sem.dump();

    return 0;
}