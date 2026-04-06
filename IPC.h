#ifndef IPC_H
#define IPC_H

#include "MessageQueue.h"
#include "Sema.h"
#include "Sched.h"

/*
 * Author: Heritage Adigun
 * Module: IPC 
 * Description:
 * Provides message passing functionality between tasks.
 * Uses a message queue and semaphore to ensure safe access.
 */

class IPC {
private:
    MessageQueue queue;     // Message queue (mailbox)
    Semaphore* sema;        // Semaphore protecting the queue
    Scheduler* sched;       // Scheduler reference

public:
    // Constructor
    IPC(Semaphore* s, Scheduler* sc);

    // Send message from one task to another
    void Message_Send(int sender_id, int receiver_id, const std::string& data);

    // Receive message for a task
    void Message_Receive(int receiver_id);

    // Debug dump of queue
    void dump();
};

#endif
