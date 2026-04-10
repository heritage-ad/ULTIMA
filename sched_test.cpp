#include <iostream>
#include "Sched.h"

using namespace std;

int main() {

    Scheduler sched;

    sched.create_task("Task1");
    sched.create_task("Task2");
    sched.create_task("Task3");

    cout << "\n--- Scheduler Test ---\n";

    for (int i = 0; i < 3; i++) {
        sched.dump();
        sched.yield();
    }

    return 0;
}
