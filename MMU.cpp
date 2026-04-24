#include "MMU.h"

/*Author: Heritage Adigun
 * Constructor:
 * - Allocates simulated RAM
 * - Fills memory with default character ('.')
 * - Creates one large free block covering all memory
 */
MMU::MMU(int size, char init_value, int blk_size, Semaphore* sema, Scheduler* sched) {
    total_size = size;
    default_value = init_value;
    block_size = blk_size;
    next_handle = 1;
    core_sema = sema;
    sched_ptr = sched;

    // Allocate raw memory
    memory = new unsigned char[total_size];

    // Fill entire memory with default value
    for (int i = 0; i < total_size; i++) {
        memory[i] = default_value;
    }

    // Initially one large free block
    head = new MemBlock(true, -1, 0, total_size - 1, total_size, -1);
}

/*
 * Destructor:
 * Frees memory array and linked list nodes
 */
MMU::~MMU() {
    delete[] memory;

    MemBlock* temp = head;
    while (temp != nullptr) {
        MemBlock* del = temp;
        temp = temp->next;
        delete del;
    }
}

/*
 * Rounds requested size up to nearest block size.
 * Example: if block_size = 64 and request = 35, allocate 64.
 */
int MMU::round_up_to_block_size(int requested_size) {
    if (requested_size % block_size == 0) {
        return requested_size;
    }
    return ((requested_size / block_size) + 1) * block_size;
}

/*
 * Finds a free block using first-fit strategy.
 */
MemBlock* MMU::find_free_block_first_fit(int needed_size) {
    MemBlock* temp = head;
    while (temp != nullptr) {
        if (temp->is_free && temp->size >= needed_size) {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}

/*
 * Finds a memory block by handle.
 */
MemBlock* MMU::find_block_by_handle(int mem_handle) {
    MemBlock* temp = head;
    while (temp != nullptr) {
        if (temp->mem_handle == mem_handle) {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}

/*
 * Mem_Alloc:
 * - Protects memory using semaphore
 * - Uses first-fit allocation
 * - Splits free block if needed
 * - Returns unique memory handle
 */
int MMU::Mem_Alloc(int task_id, int size) {
    // Acquire access to core memory allocation structures
    core_sema->down(task_id);

    int alloc_size = round_up_to_block_size(size);
    MemBlock* free_block = find_free_block_first_fit(alloc_size);

    // No free block large enough
    if (free_block == nullptr) {
        core_sema->up(task_id);
        return -1;
    }

    int new_handle = next_handle++;

    // Exact fit case
    if (free_block->size == alloc_size) {
        free_block->is_free = false;
        free_block->mem_handle = new_handle;
        free_block->task_id = task_id;
        free_block->current_location = free_block->start;

        core_sema->up(task_id);
        return new_handle;
    }

    // Split block
    MemBlock* used_block = new MemBlock(
        false,
        new_handle,
        free_block->start,
        free_block->start + alloc_size - 1,
        alloc_size,
        task_id
    );

    used_block->current_location = used_block->start;

    // Adjust free block to represent remaining free space
    free_block->start = used_block->end + 1;
    free_block->size = free_block->end - free_block->start + 1;
    free_block->current_location = free_block->start;

    // Insert used block before free block in list
    if (head == free_block) {
        used_block->next = free_block;
        head = used_block;
    } else {
        MemBlock* prev = head;
        while (prev != nullptr && prev->next != free_block) {
            prev = prev->next;
        }
        if (prev != nullptr) {
            used_block->next = free_block;
            prev->next = used_block;
        }
    }

    core_sema->up(task_id);
    return new_handle;
}

/*
 * Mem_Free:
 * - Validates ownership
 * - Marks memory region with '#'
 * - Marks block free
 * - Coalesces adjacent free blocks
 */
int MMU::Mem_Free(int task_id, int mem_handle) {
    core_sema->down(task_id);

    MemBlock* block = find_block_by_handle(mem_handle);

    // Invalid handle or not owner
    if (block == nullptr || block->is_free || block->task_id != task_id) {
        core_sema->up(task_id);
        return -1;
    }

    // Mark bytes with '#' to show freed memory before coalescing
    for (int i = block->start; i <= block->end; i++) {
        memory[i] = '#';
    }

    block->is_free = true;
    block->mem_handle = -1;
    block->task_id = -1;
    block->current_location = block->start;

    // Coalesce after freeing
    Mem_Coalesce();

    core_sema->up(task_id);
    return 1;
}

/*
 * Mem_Read (single char):
 * Reads one character from current_location and advances pointer.
 */
int MMU::Mem_Read(int task_id, int mem_handle, char* ch) {
    MemBlock* block = find_block_by_handle(mem_handle);

    // Validate ownership and bounds
    if (block == nullptr || block->is_free || block->task_id != task_id) {
        return -1;
    }

    if (block->current_location > block->end) {
        return -1;
    }

    *ch = memory[block->current_location];
    block->current_location++;
    return 1;
}

/*
 * Mem_Write (single char):
 * Writes one character at current_location and advances pointer.
 */
int MMU::Mem_Write(int task_id, int mem_handle, char ch) {
    MemBlock* block = find_block_by_handle(mem_handle);

    // Validate ownership and bounds
    if (block == nullptr || block->is_free || block->task_id != task_id) {
        return -1;
    }

    if (block->current_location > block->end) {
        return -1;
    }

    memory[block->current_location] = ch;
    block->current_location++;
    return 1;
}

/*
 * Multi-byte read:
 * Reads text_size chars starting from block->start + offset_from_beg
 */
int MMU::Mem_Read(int task_id, int mem_handle, int offset_from_beg, int text_size, char* text) {
    MemBlock* block = find_block_by_handle(mem_handle);

    if (block == nullptr || block->is_free || block->task_id != task_id) {
        return -1;
    }

    int start_pos = block->start + offset_from_beg;
    int end_pos = start_pos + text_size - 1;

    // Bounds protection
    if (start_pos < block->start || end_pos > block->end) {
        return -1;
    }

    for (int i = 0; i < text_size; i++) {
        text[i] = memory[start_pos + i];
    }

    text[text_size] = '\0';
    return 1;
}

/*
 * Multi-byte write:
 * Writes text_size chars starting from block->start + offset_from_beg
 */
int MMU::Mem_Write(int task_id, int mem_handle, int offset_from_beg, int text_size, const char* text) {
    MemBlock* block = find_block_by_handle(mem_handle);

    if (block == nullptr || block->is_free || block->task_id != task_id) {
        return -1;
    }

    int start_pos = block->start + offset_from_beg;
    int end_pos = start_pos + text_size - 1;

    // Bounds protection
    if (start_pos < block->start || end_pos > block->end) {
        return -1;
    }

    for (int i = 0; i < text_size; i++) {
        memory[start_pos + i] = text[i];
    }

    return 1;
}

/*
 * Returns amount of free memory left.
 */
int MMU::Mem_Left() const {
    int total_free = 0;
    MemBlock* temp = head;
    while (temp != nullptr) {
        if (temp->is_free) {
            total_free += temp->size;
        }
        temp = temp->next;
    }
    return total_free;
}

/*
 * Returns largest free segment size.
 */
int MMU::Mem_Largest() const {
    int largest = 0;
    MemBlock* temp = head;
    while (temp != nullptr) {
        if (temp->is_free && temp->size > largest) {
            largest = temp->size;
        }
        temp = temp->next;
    }
    return largest;
}

/*
 * Returns smallest free segment size.
 */
int MMU::Mem_Smallest() const {
    int smallest = total_size + 1;
    bool found = false;

    MemBlock* temp = head;
    while (temp != nullptr) {
        if (temp->is_free) {
            found = true;
            if (temp->size < smallest) {
                smallest = temp->size;
            }
        }
        temp = temp->next;
    }

    return found ? smallest : 0;
}

/*
 * Coalesces adjacent free blocks and replaces '#' with '.'
 */
int MMU::Mem_Coalesce() {
    int merges = 0;
    MemBlock* temp = head;

    while (temp != nullptr && temp->next != nullptr) {
        if (temp->is_free && temp->next->is_free) {
            MemBlock* next_block = temp->next;

            // Expand current free block
            temp->end = next_block->end;
            temp->size = temp->end - temp->start + 1;
            temp->next = next_block->next;

            // Replace freed '#' region with default '.'
            for (int i = temp->start; i <= temp->end; i++) {
                memory[i] = default_value;
            }

            delete next_block;
            merges++;
        } else {
            temp = temp->next;
        }
    }

    return merges;
}

/*
 * Dumps a range of memory contents.
 */
int MMU::Mem_Dump(int starting_from, int num_bytes) const {
    if (starting_from < 0 || starting_from >= total_size) {
        return -1;
    }

    int end = starting_from + num_bytes;
    if (end > total_size) {
        end = total_size;
    }

    std::cout << "\n===== MEMORY CORE DUMP =====\n";
    for (int i = starting_from; i < end; i++) {
        std::cout << memory[i];
        // Add line breaks every 64 chars for readability
        if ((i - starting_from + 1) % 64 == 0) {
            std::cout << "\n";
        }
    }
    return 1;
}

/*
 * Dumps linked list of memory blocks.
 */
void MMU::List_Dump() const {
    std::cout << "\n===== MEMORY BLOCK LIST =====\n";
    printf("%-10s %-15s %-18s %-16s %-12s %-18s %-10s\n",
           "Status", "Memory Handle", "Starting Location",
           "Ending Location", "Size/Bytes", "Current Location", "Task-ID");

    MemBlock* temp = head;
       while (temp != nullptr) {
    std::string currentLoc = temp->is_free ? "NA" : std::to_string(temp->current_location);
        std::string taskId     = temp->is_free ? "MMU" : std::to_string(temp->task_id);

        printf("%-10s %-15d %-18d %-16d %-12d %-18s %-10s\n",
               (temp->is_free ? "Free" : "Used"),
               temp->mem_handle,
               temp->start,
               temp->end,
               temp->size,
               currentLoc.c_str(),
               taskId.c_str());
        temp = temp->next;
    }

}
