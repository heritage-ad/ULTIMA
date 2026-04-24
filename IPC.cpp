#include "IPC.h"
#include <iostream>

/*
 * Author: Heritage Adigun
 * Module: IPC 
 * Description:
 * Provides message passing functionality between tasks.
 * Uses a message queue and semaphore to ensure safe access.
 */


/*
 * Constructor initializes IPC system with semaphore and scheduler
 */
IPC::IPC(Semaphore* s, Scheduler* sc) {
    sema = s;
    sched = sc;
}

/*
 * Message_Send:
 * Sends a message from sender to receiver.
 * Uses semaphore to safely access shared message queue.
 */
void IPC::Message_Send(int sender_id, int receiver_id, const std::string& data) {

    /* Author: Heidi Ganim
     * Extra Credit: Secure IPC
     * Description:
     * Adds validation checks for safe communication
     */
    if (sender_id <= 0 || receiver_id <= 0) {
        std::cout << "[SECURITY] Invalid sender/receiver ID\n";
        return;
    }

    if (sender_id == receiver_id) {
        std::cout << "[SECURITY] Task cannot send message to itself\n";
        return;
    }

    // Acquire access to message queue
    sema->down(sender_id);

    // Create message
    Message msg(sender_id, receiver_id, data);

    // Add message to queue
    queue.enqueue(msg);

    std::cout << "[SEND] Task " << sender_id
              << " -> Task " << receiver_id
              << " : " << data << "\n";

    // Release queue
    sema->up(sender_id);
}

/*
 * Message_Receive:
 * Retrieves a message for the given task.
 */
void IPC::Message_Receive(int receiver_id) {

    // Heidi: validates receiver ID to prevent invalid access to the message queue
    if (receiver_id <= 0) {
    std::cout << "[SECURITY] Invalid receiver ID\n";
    return;
}

    // Acquire access to queue
    sema->down(receiver_id);

    if (!queue.isEmpty()) {

        Message msg = queue.dequeue();

        std::cout << "[RECEIVE] Task " << receiver_id
                  << " received from Task " << msg.sender_id
                  << " : " << msg.data << "\n";

    } else {
        std::cout << "[RECEIVE] No messages available\n";
    }

    // Release queue
    sema->up(receiver_id);
}

/*
 * dump:
 * Prints current message queue state
 */
void IPC::dump() {
    queue.dump();
}
