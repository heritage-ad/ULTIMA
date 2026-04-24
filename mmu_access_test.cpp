#include "MMU.h"
#include "Sema.h"
#include "Sched.h"
#include <iostream>

/* Author: Heidi Ganim
 * Module: MMU Test (Memory Access)
 * Description:
 * Tests memory read, write, reset, and multi-byte operations
 * to verify correct tracking, ownership, and boundary handling.
 */

int main() {
    // Setup core components
    Scheduler sched;
    Semaphore core("Core", 1, &sched);

    MMU mmu(1024, '.', 64, &core, &sched);

    int task_id = 1;

    // Allocate memory block
    int handle = mmu.Mem_Alloc(task_id, 50);

    if (handle == -1) {
        std::cout << "Allocation failed\n";
        return 0;
    }

    std::cout << "Allocated handle: " << handle << "\n";

    // --------------------------
    // Single-byte write
    // --------------------------
    mmu.Mem_Write(task_id, handle, 'H');
    mmu.Mem_Write(task_id, handle, 'i');

    std::cout << "Wrote characters: H i\n";

    // --------------------------
    // Reset pointer
    // --------------------------
    mmu.Mem_Reset(task_id, handle);

    std::cout << "Pointer reset to start\n";

    // --------------------------
    // Single-byte read
    // --------------------------
    char ch;

    mmu.Mem_Read(task_id, handle, &ch);
    std::cout << "Read 1: " << ch << "\n";

    mmu.Mem_Read(task_id, handle, &ch);
    std::cout << "Read 2: " << ch << "\n";

    // --------------------------
    // Multi-byte write
    // --------------------------
    const char* msg = "Test";
    mmu.Mem_Write(task_id, handle, 0, 4, msg);

    // --------------------------
    // Multi-byte read
    // --------------------------
    char buffer[10];
    mmu.Mem_Read(task_id, handle, 0, 4, buffer);

    std::cout << "Multi-byte read: " << buffer << "\n";

    // --------------------------
    // Free memory
    // --------------------------
    mmu.Mem_Free(task_id, handle);

    std::cout << "Memory released\n";

    return 0;
}