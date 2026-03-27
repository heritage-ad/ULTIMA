#include <iostream>
#include "Sched.h"
#include "Sema.h"

using namespace std;

int main() {

    // Create scheduler
    Scheduler sched;

    // Create semaphore 
    Semaphore sem("Printer", 1, &sched);

    // Create tasks
    sched.create_task("Task1");
    sched.create_task("Task2");
    sched.create_task("Task3");

    cout << "\n--- Initial State ---\n";
    sched.dump();

    // Task1 runs and acquires resource
    cout << "\nTask1 requesting resource...\n";
    sem.down(1);  // assuming ID = 1
    sched.dump();
    sem.dump();

     // Switch to next task
    cout << "\n--- yield() ---\n";
    sched.yield();
    sched.dump();

    cout << "\n--- Task2 requesting resource ---\n";
    sem.down(2);

    sched.dump();
    sem.dump();

    // Switch again
    cout << "\n--- yield() ---\n";
    sched.yield();
    sched.dump();

    cout << "\n--- Task3 requesting resource ---\n";
    sem.down(3);

    sched.dump();
    sem.dump();

    // Release resource
    cout << "\n--- Releasing resource ---\n";
    sem.up(1);

    sched.dump();
    sem.dump();

    return 0;
}