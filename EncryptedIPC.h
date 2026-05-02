#ifndef ENCRYPTEDIPC_H
#define ENCRYPTEDIPC_H

#include <string>
#include "MessageQueue.h"
#include "Sema.h"

/*
 * Author: Heritage Adigun
 * Module: Encrypted IPC
 * Description:
 * Extends the IPC system by adding
 * encryption and decryption support
 * for secure message transmission.
 */

class EncryptedIPC {
private:
    MessageQueue queue;
    Semaphore* sema;

    // Helper encryption functions
    std::string encrypt(const std::string& text);
    std::string decrypt(const std::string& text);

public:
    EncryptedIPC(Semaphore* s);

    void Message_Send(int sender_id,
                      int receiver_id,
                      const std::string& data);

    void Message_Receive(int receiver_id);
};

#endif
