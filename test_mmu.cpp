#include <iostream>
#include "MMU.h"
#include "Sema.h"
#include "Sched.h"

/*
 * Author: Heritage Adigun
 * Module: test_mmu
 * Description:
 * Test driver for Phase 3 Memory Management Unit.
 * Demonstrates allocation, writing, reading, freeing,
 * coalescing, and allocation failure.
 */

int main() {
    Scheduler sched;
    Semaphore coreMem("Core Memory", 1, &sched);

    // Block size = 64, total memory = 1024
    MMU mmu(1024, '.', 64, &coreMem, &sched);

    // Create tasks in scheduler
    int t1 = sched.create_task("Task1");
    int t2 = sched.create_task("Task2");
    int t3 = sched.create_task("Task3");

    std::cout << "\n===== TEST 1: BASIC ALLOCATION =====\n";
    int h1 = mmu.Mem_Alloc(t1, 35);   // should allocate 64 bytes
    int h2 = mmu.Mem_Alloc(t2, 100);  // should allocate 128 bytes
    mmu.List_Dump();
    mmu.Mem_Dump(0, 256);

    std::cout << "\n===== TEST 2: WRITE + READ =====\n";
    const char* text1 = "this is task one";
    mmu.Mem_Write(t1, h1, 0, 16, text1);

    char buffer[32];
    mmu.Mem_Read(t1, h1, 0, 16, buffer);
    std::cout << "Read back from Task1 memory: " << buffer << "\n";
    mmu.Mem_Dump(0, 256);

    std::cout << "\n===== TEST 3: INVALID ACCESS =====\n";
    // Task 2 tries to read Task 1's memory -> should fail
    if (mmu.Mem_Read(t2, h1, 0, 5, buffer) == -1) {
        std::cout << "Segmentation protection worked: Task2 cannot access Task1 memory.\n";
    }

    std::cout << "\n===== TEST 4: FREE + COALESCE =====\n";
    mmu.Mem_Free(t1, h1);
    mmu.List_Dump();
    mmu.Mem_Dump(0, 256);

    std::cout << "\n===== TEST 5: ALLOCATION FAILURE =====\n";
    int big1 = mmu.Mem_Alloc(t1, 900);
    int big2 = mmu.Mem_Alloc(t3, 900); // should likely fail
    std::cout << "Handle big1 = " << big1 << "\n";
    std::cout << "Handle big2 = " << big2 << " (should be -1 if insufficient memory)\n";

    std::cout << "\n===== TEST 6: MEMORY STATS =====\n";
    std::cout << "Memory Left: " << mmu.Mem_Left() << "\n";
    std::cout << "Largest Free Segment: " << mmu.Mem_Largest() << "\n";
    std::cout << "Smallest Free Segment: " << mmu.Mem_Smallest() << "\n";

    return 0;
}
