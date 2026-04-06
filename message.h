#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

/*
 * Author: Heritage Adigun
 * Module: IPC
 * Description:
 * Represents a message sent between tasks in the IPC system.
 * Each message contains sender, receiver, and message content.
 */

struct Message {
    int sender_id;       // ID of the sending task
    int receiver_id;     // ID of the receiving task
    std::string data;    // Message content

    // Constructor to initialize message
    Message(int s, int r, const std::string& d)
        : sender_id(s), receiver_id(r), data(d) {}
};

#endif
