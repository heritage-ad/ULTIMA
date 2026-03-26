// Author: Heritage Adigun
// Module: Binary Semaphore
// Description: Implements semaphore synchronization for shared resources

#ifndef QUEUE_H
#define QUEUE_H

#include <deque>
#include <iostream>
#include <stdexcept>

// Queue class
// This queue is used to store waiting task IDs in the semaphore
// It follows FIFO (First In First Out) behavior
template <typename T>
class Queue {
private:
    std::deque<T> data;  // underlying container

public:
    // Constructor
    Queue() = default;

    // Destructor
    ~Queue() = default;

    // Add element to the back of the queue
    void en_Q(const T& value) {
        data.push_back(value);
    }

    // Remove and return element from front of queue
    T de_Q() {
        if (data.empty()) {
            throw std::runtime_error("Queue underflow: cannot dequeue from empty queue.");
        }
        T frontValue = data.front();
        data.pop_front();
        return frontValue;
    }

    // Check if queue is empty
    bool isEmpty() const {
        return data.empty();
    }

    // Return number of elements
    size_t size() const {
        return data.size();
    }

    // Return front element without removing
    T front() const {
        if (data.empty()) {
            throw std::runtime_error("Queue is empty.");
        }
        return data.front();
    }

    // Clear all elements
    void clear() {
        data.clear();
    }

    // Print queue contents (for debugging / dump function)
    void print(std::ostream& os = std::cout) const {
        if (data.empty()) {
            os << "EMPTY";
            return;
        }

        for (size_t i = 0; i < data.size(); ++i) {
            os << data[i];
            if (i + 1 < data.size()) {
                os << " -> ";
            }
        }
    }
};

#endif
