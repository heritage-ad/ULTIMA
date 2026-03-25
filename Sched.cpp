#include "Sched.h"

/*
 * Initializes an empty scheduler.
 * No tasks exist yet, so both head and current are null.
 */
Scheduler::Scheduler() {
    head = nullptr;
    current = nullptr;
    next_task_id = 1;
}

/*
 * Adds a new task to the end of the list.
 * First task runs immediately, others wait as READY.
 */
int Scheduler::create_task(const std::string& name) {

    TCB* newTask = new TCB(next_task_id, name);

    if (head == nullptr) {
        head = newTask;
        current = newTask;
        newTask->state = RUNNING;
    } else {
        TCB* temp = head;
        while (temp->next != nullptr)
            temp = temp->next;

        temp->next = newTask;
        newTask->state = READY;
    }

    return next_task_id++;
}

/*
 * Finds a task in the list using its ID.
 */
TCB* Scheduler::find_task(int task_id) {
    TCB* temp = head;
    while (temp != nullptr) {
        if (temp->task_id == task_id)
            return temp;
        temp = temp->next;
    }
    return nullptr;
}

/*
 * Called when a task cannot access a resource.
 * Marks it BLOCKED and removes it from running.
 */
void Scheduler::block_task(int task_id) {
    TCB* t = find_task(task_id);
    if (t != nullptr) {
        t->state = BLOCKED;
        if (current == t) current = nullptr;
    }
}

/*
 * Called when a resource is released.
 * Moves task back to READY so it can run again.
 */
void Scheduler::ready_task(int task_id) {
    TCB* t = find_task(task_id);
    if (t != nullptr) {
        t->state = READY;
    }
}

/*
 * Switches execution to the next READY task.
 * Skips BLOCKED and DEAD tasks.
 */
void Scheduler::yield() {

    if (head == nullptr) return;

    if (current != nullptr && current->state == RUNNING)
        current->state = READY;

    TCB* temp = (current && current->next) ? current->next : head;

    while (temp->state != READY) {
        temp = temp->next ? temp->next : head;
        if (temp == current) return;
    }

    current = temp;
    current->state = RUNNING;
}

/*
 * Marks task as finished.
 */
void Scheduler::kill_task(int task_id) {
    TCB* t = find_task(task_id);
    if (t != nullptr) {
        t->state = DEAD;
    }
}

/*
 * Removes DEAD tasks from the list and frees memory.
 */
void Scheduler::garbage_collect() {

    TCB* temp = head;
    TCB* prev = nullptr;

    while (temp != nullptr) {

        if (temp->state == DEAD) {

            if (prev == nullptr)
                head = temp->next;
            else
                prev->next = temp->next;

            TCB* del = temp;
            temp = temp->next;
            delete del;

        } else {
            prev = temp;
            temp = temp->next;
        }
    }
}

/*
 * Prints all tasks and their states.
 */
void Scheduler::dump() const {

    std::cout << "\nProcess Table\n";

    TCB* temp = head;

    while (temp != nullptr) {

        std::cout << "Task " << temp->task_id << " : ";

        if (temp->state == RUNNING) std::cout << "RUNNING";
        else if (temp->state == READY) std::cout << "READY";
        else if (temp->state == BLOCKED) std::cout << "BLOCKED";
        else if (temp->state == DEAD) std::cout << "DEAD";

        std::cout << "\n";
        temp = temp->next;
    }
}
