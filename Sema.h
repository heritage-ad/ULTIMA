// Author: Heritage Adigun
// Module: Binary Semaphore
// Description: Implements semaphore synchronization for shared resources

#ifndef SEMA_H
#define SEMA_H

#include <string>
#include <iostream>
#include "queue.h"
#include "Sched.h"

// Semaphore class
// Implements a binary semaphore (0 or 1)
// Used to control access to a shared resource
class Semaphore {
private:
    std::string resource_name;   // name of resource (e.g., screen, printer)
    int sema_value;              // 1 = available, 0 = busy
    int owner_task_id;           // current task holding resource (-1 if none)
    Queue<int> sema_queue;       // queue of blocked tasks
    Scheduler* sched_ptr;        // pointer to scheduler (controls task states)

public:
    // Constructor
    Semaphore(const std::string& resourceName,
              int initialValue,
              Scheduler* schedulerPtr);

    // Destructor
    ~Semaphore() = default;

    // DOWN operation (P operation)
    // Attempts to acquire resource
    // Returns true if acquired, false if blocked
    bool down(int task_id);

    // UP operation (V operation)
    // Releases resource and wakes next task (if any)
    // Returns task id of awakened task, or -1 if none
    int up(int task_id);

    // Debug function to display semaphore state
    void dump(int level = 1) const;

    // Getter functions
    std::string get_resource_name() const;
    int get_value() const;
    int get_owner() const;
    bool queue_empty() const;
};

#endif
