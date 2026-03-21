#ifndef QUEUE_H
#define QUEUE_H

#include <deque>
#include <iostream>
#include <stdexcept>

template <typename T>
class Queue {
private:
    std::deque<T> data;

public:
    Queue() = default;
    ~Queue() = default;

    void en_Q(const T& value) {
        data.push_back(value);
    }

    T de_Q() {
        if (data.empty()) {
            throw std::runtime_error("Queue underflow: cannot dequeue from empty queue.");
        }
        T frontValue = data.front();
        data.pop_front();
        return frontValue;
    }

    bool isEmpty() const {
        return data.empty();
    }

    size_t size() const {
        return data.size();
    }

    T front() const {
        if (data.empty()) {
            throw std::runtime_error("Queue is empty: no front element.");
        }
        return data.front();
    }

    void clear() {
        data.clear();
    }

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
