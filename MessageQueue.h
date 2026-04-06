#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <queue>
#include <iostream>
#include "Message.h"

/*
 * Author: Heritage Adigun
 * Module: MessageQueue
 * Description:
 * Implements a FIFO queue for storing messages.
 * Used by the IPC system to manage messages between tasks.
 */

class MessageQueue {
private:
    std::queue<Message> msg_queue;   // FIFO queue of messages

public:

    // Add a message to the queue
    void enqueue(const Message& msg) {
        msg_queue.push(msg);
    }

    // Remove and return the front message
    Message dequeue() {
        Message msg = msg_queue.front();
        msg_queue.pop();
        return msg;
    }

    // Check if queue is empty
    bool isEmpty() const {
        return msg_queue.empty();
    }

    // Debug: print queue size
    void dump() const {
        std::cout << "[MessageQueue] Size: " << msg_queue.size() << "\n";
    }
};

#endif
