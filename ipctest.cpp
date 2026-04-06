#include <iostream>
#include "IPC.h"
#include "Sched.h"
#include "Sema.h"

/*
 * Test driver for IPC system
 */

int main() {

    // Create scheduler
    Scheduler sched;

    // Create semaphore for message queue protection
    Semaphore msg_sema("msg_queue", 1, &sched);

    // Create IPC system
    IPC ipc(&msg_sema, &sched);

    // Create tasks
    int t1 = sched.create_task("Task1");
    int t2 = sched.create_task("Task2");
    int t3 = sched.create_task("Task3");

    std::cout << "\n===== TEST 1: Basic Send/Receive =====\n";
    ipc.Message_Send(t1, t2, "Hello from Task 1");
    ipc.Message_Receive(t2);

    std::cout << "\n===== TEST 2: Multiple Messages =====\n";
    ipc.Message_Send(t1, t2, "First message");
    ipc.Message_Send(t3, t2, "Second message");

    ipc.Message_Receive(t2);
    ipc.Message_Receive(t2);

    std::cout << "\n===== TEST 3: Empty Queue =====\n";
    ipc.Message_Receive(t2);

    return 0;
}
