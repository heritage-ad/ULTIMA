#ifndef SCHED_H
#define SCHED_H

#include <string>
#include <iostream>

enum TaskState { RUNNING, READY, BLOCKED, DEAD };

struct TCB {
    int task_id;
    std::string task_name;
    TaskState state;
    TCB* next;

    TCB(int id, const std::string& name)
        : task_id(id), task_name(name), state(READY), next(nullptr) {}
};

class Scheduler {
private:
    TCB* head;
    TCB* current;
    int next_task_id;

public:
    Scheduler();

    int create_task(const std::string& name);
    void yield();
    void kill_task(int task_id);
    void garbage_collect();

    void block_task(int task_id);
    void ready_task(int task_id);

    TCB* find_task(int task_id);
    void dump() const;
};

#endif
