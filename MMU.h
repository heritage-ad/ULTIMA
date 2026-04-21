#ifndef MMU_H
#define MMU_H

#include <iostream>
#include <cstring>
#include "MemBlock.h"
#include "Sema.h"
#include "Sched.h"

/*
 * Author: Heritage Adigun
 * Module: MMU
 * Description:
 * Simulates a Memory Management Unit for ULTIMA 2.0.
 * Supports allocation, freeing, reading, writing, coalescing,
 * and dumping of a fixed 1024-byte memory space.
 */

class MMU {
private:
    unsigned char* memory;      // simulated RAM array
    int total_size;             // total memory size (1024)
    char default_value;         // default initialization character ('.')
    int block_size;             // allocation block size (e.g. 64 or 128)
    int next_handle;            // generates unique memory handles

    MemBlock* head;             // head of linked list of memory blocks
    Semaphore* core_sema;       // semaphore to protect allocation/free operations
    Scheduler* sched_ptr;       // optional scheduler reference for integration

    // Private helper functions
    MemBlock* find_block_by_handle(int mem_handle);
    MemBlock* find_free_block_first_fit(int needed_size);
    int round_up_to_block_size(int requested_size);

public:
    // Constructor / Destructor
    MMU(int size, char init_value, int blk_size, Semaphore* sema, Scheduler* sched);
    ~MMU();

    // Required public operations
    int Mem_Alloc(int task_id, int size);
    int Mem_Free(int task_id, int mem_handle);

    int Mem_Read(int task_id, int mem_handle, char* ch);
    int Mem_Write(int task_id, int mem_handle, char ch);

    int Mem_Read(int task_id, int mem_handle, int offset_from_beg, int text_size, char* text);
    int Mem_Write(int task_id, int mem_handle, int offset_from_beg, int text_size, const char* text);

    // Debugging / status functions
    int Mem_Left() const;
    int Mem_Largest() const;
    int Mem_Smallest() const;
    int Mem_Coalesce();
    int Mem_Dump(int starting_from, int num_bytes) const;
    void List_Dump() const;
};

#endif
