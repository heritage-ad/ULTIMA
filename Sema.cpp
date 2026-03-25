#include "Sema.h"
#include <stdexcept>

// Constructor
// Initializes semaphore with resource name, value, and scheduler pointer
Semaphore::Semaphore(const std::string& resourceName,
                     int initialValue,
                     Scheduler* schedulerPtr)
    : resource_name(resourceName),
      sema_value(initialValue),
      owner_task_id(-1),
      sched_ptr(schedulerPtr) {

    // Ensure valid binary value
    if (initialValue != 0 && initialValue != 1) {
        throw std::invalid_argument("Semaphore value must be 0 or 1.");
    }

    // Ensure scheduler pointer is valid
    if (sched_ptr == nullptr) {
        throw std::invalid_argument("Scheduler pointer cannot be null.");
    }
}

// DOWN 
bool Semaphore::down(int task_id) {

    // If resource is available
    if (sema_value == 1) {
        sema_value = 0;              // lock resource
        owner_task_id = task_id;     // assign ownership

        std::cout << "[DOWN] Task " << task_id
                  << " acquired resource: " << resource_name << "\n";

        return true;
    }

    // Resource is busy = block task
    std::cout << "[DOWN] Task " << task_id
              << " blocked (resource busy)\n";

    sema_queue.en_Q(task_id);         // add task to queue
    sched_ptr->block_task(task_id);   // scheduler marks it BLOCKED
    sched_ptr->yield();

    return false;
}

// UP 
int Semaphore::up(int task_id) {

    // Ownership: ensure only owner releases
    if (owner_task_id != -1 && owner_task_id != task_id) {
        std::cout << "[UP] Warning: Task " << task_id
                  << " is not owner of resource\n";
        return -1;
    }

    // If no tasks waiting
    if (sema_queue.isEmpty()) {
        sema_value = 1;        // resource becomes free
        owner_task_id = -1;

        std::cout << "[UP] Resource released, no waiting tasks\n";
        return -1;
    }

    // Wake next task in queue
    int next_task = sema_queue.de_Q();

    owner_task_id = next_task;  // transfer ownership

    // Tell scheduler to make task READY
    sched_ptr->ready_task(next_task);
    sched_ptr->yield();

    std::cout << "[UP] Task " << next_task
              << " awakened and given resource\n";

    return next_task;
}

// Dump function for debugging
void Semaphore::dump(int level) const {

    std::cout << "\n===== SEMAPHORE DUMP =====\n";
    std::cout << "Resource: " << resource_name << "\n";
    std::cout << "Value: " << sema_value << "\n";
    std::cout << "Owner: " << owner_task_id << "\n";

    std::cout << "Queue: ";
    sema_queue.print();
    std::cout << "\n";

    if (level > 1) {
        std::cout << "Queue size: " << sema_queue.size() << "\n";
    }

    std::cout << "====================\n";
}

// Getter methods
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
