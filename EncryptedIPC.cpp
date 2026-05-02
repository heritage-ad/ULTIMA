/*Author: Heritage Adigun*/

#include "EncryptedIPC.h"
#include <iostream>

/*
 * Constructor
 */
EncryptedIPC::EncryptedIPC(Semaphore* s) {
    sema = s;
}

/*
 * Caesar cipher encryption (+3)
 */
std::string EncryptedIPC::encrypt(const std::string& text) {
    std::string result = text;

    for (char& c : result) {
        c = c + 3;
    }

    return result;
}

/*
 * Caesar cipher decryption (-3)
 */
std::string EncryptedIPC::decrypt(const std::string& text) {
    std::string result = text;

    for (char& c : result) {
        c = c - 3;
    }

    return result;
}

/*
 * Sends encrypted message
 */
void EncryptedIPC::Message_Send(int sender_id,
                                int receiver_id,
                                const std::string& data) {

    // Acquire semaphore
    sema->down(sender_id);

    // Encrypt message before storing
    std::string encrypted_text = encrypt(data);

    Message msg(sender_id, receiver_id, encrypted_text);

    queue.enqueue(msg);

    std::cout << "\n[SEND]\n";
    std::cout << "Original Message : " << data << "\n";
    std::cout << "Encrypted Message: " << encrypted_text << "\n";

    // Release semaphore
    sema->up(sender_id);
}

/*
 * Receives and decrypts message
 */
void EncryptedIPC::Message_Receive(int receiver_id) {

    sema->down(receiver_id);

    if (queue.isEmpty()) {
        std::cout << "\n[RECEIVE] No messages available\n";
        sema->up(receiver_id);
        return;
    }

    Message msg = queue.dequeue();

    // Decrypt message
    std::string decrypted_text = decrypt(msg.data);

    std::cout << "\n[RECEIVE]\n";
    std::cout << "Encrypted Message: " << msg.data << "\n";
    std::cout << "Decrypted Message: " << decrypted_text << "\n";

    sema->up(receiver_id);
}
