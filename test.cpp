#include <iostream>
#include "Sema.h"
#include "Sched.h"

// Test driver for semaphore behavior
int main() {

    // Create scheduler stub
    Scheduler sched;

    // Create semaphore for "screen" resource
    Semaphore screen("screen", 1, &sched);

    // Initial state
    screen.dump();

    // Simulate tasks requesting resource
    screen.down(1);   // should acquire
    screen.down(2);   // should block
    screen.down(3);   // should block

    screen.dump();

    // Release resource step by step
    screen.up(1);     // should wake task 2
    screen.dump();

    screen.up(2);     // should wake task 3
    screen.dump();

    screen.up(3);     // should free resource
    screen.dump();

    return 0;
}
