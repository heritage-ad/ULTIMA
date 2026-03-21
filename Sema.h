#ifndef SEMA_H
#define SEMA_H

#include <string>
#include <iostream>
#include "queue.h"
#include "Sched.h"

class Semaphore {
private:
    std::string resource_name;
    int sema_value;              // binary: 0 or 1
    int owner_task_id;           // -1 if nobody owns it
    Queue<int> sema_queue;       // waiting task IDs
    Scheduler* sched_ptr;        // scheduler controls states

public:
    Semaphore(const std::string& resourceName,
              int initialValue,
              Scheduler* schedulerPtr);

    ~Semaphore() = default;

    bool down(int task_id);      // true = acquired, false = blocked
    int up(int task_id);         // returns task id awakened, or -1
    void dump(int level = 1) const;

    std::string get_resource_name() const;
    int get_value() const;
    int get_owner() const;
    bool queue_empty() const;
};

#endif
