#ifndef MEMBLOCK_H
#define MEMBLOCK_H

/*
 * Author: Heritage Adigun
 * Module: MemBlock
 * Description:
 * Represents a block of memory in the simulated 1024-byte RAM.
 * Each block tracks whether it is free or used, the owning task,
 * the memory handle, size, address range, and current position
 * for single-byte reads/writes.
 */

struct MemBlock {
    bool is_free;            // true if this block is free, false if allocated
    int mem_handle;          // unique memory handle for this block
    int start;               // starting index in memory array
    int end;                 // ending index in memory array
    int size;                // size of block in bytes
    int current_location;    // current read/write position for single-byte access
    int task_id;             // owner task id (-1 if free)
    MemBlock* next;          // pointer to next block in linked list

    // Constructor
    MemBlock(bool free_status, int handle, int s, int e, int block_size, int owner)
        : is_free(free_status),
          mem_handle(handle),
          start(s),
          end(e),
          size(block_size),
          current_location(s),
          task_id(owner),
          next(nullptr) {}
};

#endif
