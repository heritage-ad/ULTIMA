#include <iostream>
#include "EncryptedIPC.h"
#include "Sched.h"
#include "Sema.h"

/*
 * Test driver for encrypted IPC
 */

int main() {

    Scheduler sched;

    Semaphore ipc_sema("IPC Queue", 1, &sched);

    EncryptedIPC ipc(&ipc_sema);

    // Create tasks
    int t1 = sched.create_task("Sender");
    int t2 = sched.create_task("Receiver");

    std::cout << "\n===== TEST 1: SECURE MESSAGE =====\n";

    ipc.Message_Send(t1, t2, "HELLO TASK TWO");

    ipc.Message_Receive(t2);

    std::cout << "\n===== TEST 2: ANOTHER MESSAGE =====\n";

    ipc.Message_Send(t1, t2, "OPERATING SYSTEMS");

    ipc.Message_Receive(t2);

    return 0;
}
