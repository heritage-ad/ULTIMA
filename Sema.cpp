#include "Sema.h"
#include <stdexcept>

Semaphore::Semaphore(const std::string& resourceName,
                     int initialValue,
                     Scheduler* schedulerPtr)
    : resource_name(resourceName),
      sema_value(initialValue),
      owner_task_id(-1),
      sched_ptr(schedulerPtr) {

    if (initialValue != 0 && initialValue != 1) {
        throw std::invalid_argument("Binary semaphore initial value must be 0 or 1.");
    }

    if (sched_ptr == nullptr) {
        throw std::invalid_argument("Scheduler pointer cannot be null.");
    }
}

bool Semaphore::down(int task_id) {
    if (task_id < 0) {
        throw std::invalid_argument("Task ID must be non-negative.");
    }

    // Resource available
    if (sema_value == 1) {
        sema_value = 0;
        owner_task_id = task_id;

        std::cout << "[SEMA DOWN] Task " << task_id
                  << " acquired resource \"" << resource_name << "\"\n";
        return true;
    }

    // Resource busy = queue and block
    std::cout << "[SEMA DOWN] Task " << task_id
              << " blocked on resource \"" << resource_name << "\"\n";

    sema_queue.en_Q(task_id);
    sched_ptr->block_task(task_id);

    return false;
}

int Semaphore::up(int task_id) {
    if (task_id < 0) {
        throw std::invalid_argument("Task ID must be non-negative.");
    }

    // Ownership protection
    if (owner_task_id != -1 && owner_task_id != task_id) {
        std::cout << "[SEMA UP] Warning: Task " << task_id
                  << " attempted to release resource \"" << resource_name
                  << "\" but current owner is Task " << owner_task_id << ".\n";
        return -1;
    }

    // Nobody waiting: make resource free
    if (sema_queue.isEmpty()) {
        sema_value = 1;
        owner_task_id = -1;

        std::cout << "[SEMA UP] Task " << task_id
                  << " released resource \"" << resource_name
                  << "\". No waiting tasks.\n";
        return -1;
    }

    // Someone is waiting: wake one task
    int next_task = sema_queue.de_Q();

    // Keep semaphore logically unavailable because ownership transfers
    sema_value = 0;
    owner_task_id = next_task;

    sched_ptr->ready_task(next_task);

    std::cout << "[SEMA UP] Task " << task_id
              << " released resource \"" << resource_name
              << "\". Waking Task " << next_task << ".\n";

    return next_task;
}

void Semaphore::dump(int level) const {
    std::cout << "\n========== SEMAPHORE DUMP ==========\n";
    std::cout << "Resource:   " << resource_name << "\n";
    std::cout << "Sema Value: " << sema_value << "\n";
    std::cout << "Owner TID:  " << owner_task_id << "\n";
    std::cout << "Queue:      ";
    sema_queue.print();
    std::cout << "\n";

    if (level > 1) {
        std::cout << "Queue Size: " << sema_queue.size() << "\n";
    }

    std::cout << "==============\n";
}

std::string Semaphore::get_resource_name() const {
    return resource_name;
}

int Semaphore::get_value() const {
    return sema_value;
}

int Semaphore::get_owner() const {
    return owner_task_id;
}

bool Semaphore::queue_empty() const {
    return sema_queue.isEmpty();
}
