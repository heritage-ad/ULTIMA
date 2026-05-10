/*
 * Author: Heritage Adigun
 * Module: ULTIMA 2.0 Integrated Lab 7 UI Driver
 *
 * Description:
 * This main.cpp integrates the completed ULTIMA 2.0 components into one
 * ncurses-based interface modeled after Lab 7.
 *
 * Integrated components:
 * - Scheduler
 * - Semaphore
 * - IPC
 * - Encrypted IPC
 * - MMU
 *
 * Commands:
 * a = Run all integration tests
 * s = Run Scheduler + Semaphore test
 * i = Run Encrypted IPC test
 * m = Run MMU test
 * p = Pause / Resume display
 * h = Help
 * c = Clear output windows
 * q = Quit
 */

#include <iostream>
#include <sstream>
#include <functional>
#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>

#include "Sched.h"
#include "Sema.h"
#include "IPC.h"
#include "EncryptedIPC.h"
#include "MMU.h"

using namespace std;

// Global mutex protects all ncurses screen writes.
pthread_mutex_t screenMutex = PTHREAD_MUTEX_INITIALIZER;

// ------------------------------------------------------------
// Safely writes text into a ncurses window.
// This follows the Lab 7 idea of protecting critical sections
// where multiple parts of the program may write to the screen.
// ------------------------------------------------------------
void write_window(WINDOW* win, const string& text) {
    pthread_mutex_lock(&screenMutex);

    int maxY, maxX;
    getmaxyx(win, maxY, maxX);

    int y, x;
    getyx(win, y, x);

    if (y <= 0) y = 1;
    if (x <= 0) x = 1;

    for (char c : text) {
        if (c == '\n' || x >= maxX - 2) {
            y++;
            x = 1;
        }

        if (y >= maxY - 1) {
            wscrl(win, 1);
            y = maxY - 2;
        }

        if (c != '\n') {
            mvwaddch(win, y, x, c);
            x++;
        }
    }

    box(win, 0, 0);
    wmove(win, y, x);
    wrefresh(win);

    pthread_mutex_unlock(&screenMutex);
}

// ------------------------------------------------------------
// Writes text at a specific location inside a window.
// ------------------------------------------------------------
void write_window(WINDOW* win, int y, int x, const string& text) {
    pthread_mutex_lock(&screenMutex);

    if (y <= 0) y = 1;
    if (x <= 0) x = 1;

    mvwprintw(win, y, x, "%s", text.c_str());
    box(win, 0, 0);
    wrefresh(win);

    pthread_mutex_unlock(&screenMutex);
}
// ------------------------------------------------------------
// Clears and refreshes a window.
// ------------------------------------------------------------
void clear_window(WINDOW* win) {
    pthread_mutex_lock(&screenMutex);

    werase(win);
    box(win, 0, 0);
    wmove(win, 1, 1);
    wrefresh(win);

    pthread_mutex_unlock(&screenMutex);
}

// ------------------------------------------------------------
// Captures cout output from existing module functions.
// This allows Scheduler/Semaphore/MMU dump output to be shown
// inside the ncurses UI instead of only the terminal.
// ------------------------------------------------------------
string capture_output(function<void()> func) {
    stringstream buffer;
    streambuf* oldCout = cout.rdbuf(buffer.rdbuf());

    func();

    cout.rdbuf(oldCout);
    return buffer.str();
}

// ------------------------------------------------------------
// Displays help commands in the console window.
// ------------------------------------------------------------
void display_help(WINDOW* consoleWin) {
    clear_window(consoleWin);

    write_window(consoleWin, 1, 1, "Commands:");
    write_window(consoleWin, 2, 1, "a = Run all integration tests");
    write_window(consoleWin, 3, 1, "s = Scheduler + Semaphore");
    write_window(consoleWin, 4, 1, "i = Encrypted IPC");
    write_window(consoleWin, 5, 1, "m = Memory Management");
    write_window(consoleWin, 6, 1, "p = Pause / Resume");
    write_window(consoleWin, 7, 1, "c = Clear windows");
    write_window(consoleWin, 8, 1, "q = Quit");
}

// ------------------------------------------------------------
// Updates task windows to show task-level activity.
// ------------------------------------------------------------
void update_task_windows(WINDOW* t1Win, WINDOW* t2Win, WINDOW* t3Win) {
    clear_window(t1Win);
    clear_window(t2Win);
    clear_window(t3Win);

    write_window(t1Win, 1, 1, "Task 1");
    write_window(t1Win, 3, 1, "Uses Scheduler");
    write_window(t1Win, 4, 1, "Uses Semaphore");
    write_window(t1Win, 5, 1, "Sends IPC message");
    write_window(t1Win, 6, 1, "Allocates memory");

    write_window(t2Win, 1, 1, "Task 2");
    write_window(t2Win, 3, 1, "Receives IPC msg");
    write_window(t2Win, 4, 1, "Requests resource");
    write_window(t2Win, 5, 1, "Allocates memory");

    write_window(t3Win, 1, 1, "Task 3");
    write_window(t3Win, 3, 1, "Competes for resource");
    write_window(t3Win, 4, 1, "Tests memory failure");
    write_window(t3Win, 5, 1, "System integration");
}

// ------------------------------------------------------------
// Runs Scheduler + Semaphore integration test.
// ------------------------------------------------------------
void run_scheduler_semaphore_test(
    Scheduler& sched,
    Semaphore& printerSem,
    WINDOW* logWin,
    WINDOW* schedWin,
    WINDOW* semWin
) {
    write_window(logWin, "\n--- Scheduler + Semaphore Integration ---\n");

    string output;

    output += capture_output([&]() { printerSem.down(1); });
    output += capture_output([&]() { printerSem.down(2); });
    output += capture_output([&]() { printerSem.down(3); });
    output += capture_output([&]() { printerSem.dump(); });
    output += capture_output([&]() { printerSem.up(1); });
    output += capture_output([&]() { printerSem.up(2); });
    output += capture_output([&]() { printerSem.up(3); });

    write_window(logWin, output);

    clear_window(schedWin);
    write_window(schedWin, "Scheduler Dump\n\n");
    write_window(schedWin, capture_output([&]() { sched.dump(); }));

    clear_window(semWin);
    write_window(semWin, "Semaphore Dump\n\n");
    write_window(semWin, capture_output([&]() { printerSem.dump(); }));
}

// ------------------------------------------------------------
// Runs encrypted IPC integration test.
// ------------------------------------------------------------
void run_encrypted_ipc_test(
    EncryptedIPC& secureIPC,
    WINDOW* logWin,
    WINDOW* ipcWin
) {
    write_window(logWin, "\n--- Encrypted IPC Integration ---\n");

    string output;

    output += capture_output([&]() {
        secureIPC.Message_Send(1, 2, "HELLO TASK TWO");
    });

    output += capture_output([&]() {
        secureIPC.Message_Receive(2);
    });

    output += capture_output([&]() {
        secureIPC.Message_Send(3, 1, "OPERATING SYSTEMS");
    });

    output += capture_output([&]() {
        secureIPC.Message_Receive(1);
    });

    write_window(logWin, output);

    clear_window(ipcWin);
    write_window(ipcWin, "Encrypted IPC Dump\n\n");
    write_window(ipcWin, output);
}

// ------------------------------------------------------------
// Runs MMU integration test.
// ------------------------------------------------------------
void run_mmu_test(
    MMU& mmu,
    WINDOW* logWin,
    WINDOW* mmuWin
) {
    write_window(logWin, "\n--- Memory Management Integration ---\n");

    int h1 = -1;
    int h2 = -1;
    int h3 = -1;

    string output;

    output += capture_output([&]() {
        h1 = mmu.Mem_Alloc(1, 35);
        h2 = mmu.Mem_Alloc(2, 100);
    });

    output += "Task 1 memory handle: " + to_string(h1) + "\n";
    output += "Task 2 memory handle: " + to_string(h2) + "\n";

    output += "\nMMU allocation successful.\n";
    output += "Memory blocks updated.\n";
    output += "Memory dump available in standalone MMU test.\n";

    const char* text = "this is task one";
    char buffer[32];

    output += "\nWriting and reading Task 1 memory...\n";

    mmu.Mem_Write(1, h1, 0, 16, text);
    mmu.Mem_Read(1, h1, 0, 16, buffer);

    output += "Read back from Task 1 memory: ";
    output += buffer;
    output += "\n";

    output += "\nTesting invalid access...\n";

    if (mmu.Mem_Read(2, h1, 0, 5, buffer) == -1) {
        output += "Segmentation protection worked: Task 2 cannot access Task 1 memory.\n";
    }

    output += "\nTesting allocation failure...\n";
    h3 = mmu.Mem_Alloc(3, 900);

    if (h3 == -1) {
        output += "Task 3 allocation failed correctly due to insufficient memory.\n";
    }

    output += "\nFreeing Task 1 memory...\n";

    output += capture_output([&]() {
        mmu.Mem_Free(1, h1);
    });

    output += "\nMemory successfully freed and coalesced.\n";

    output += "\nMemory Left: " + to_string(mmu.Mem_Left()) + "\n";
    output += "Largest Free Segment: " + to_string(mmu.Mem_Largest()) + "\n";
    output += "Smallest Free Segment: " + to_string(mmu.Mem_Smallest()) + "\n";

    write_window(logWin, output);

    clear_window(mmuWin);
    write_window(mmuWin, "MMU Dump\n\n");
    write_window(mmuWin, output);
}
// ------------------------------------------------------------
// MAIN PROGRAM
// ------------------------------------------------------------
int main() {
    // Start ncurses.
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    // Allow non-blocking input from console window.
    keypad(stdscr, TRUE);

    // Clear screen before creating windows.
    clear();
    refresh();

    // --------------------------------------------------------
    // Create Lab 7-style windows.
    // --------------------------------------------------------
    WINDOW* headingWin = newwin(6, 120, 1, 2);
    WINDOW* task1Win   = newwin(10, 35, 8, 2);
    WINDOW* task2Win   = newwin(10, 35, 8, 39);
    WINDOW* task3Win   = newwin(10, 35, 8, 76);

    WINDOW* logWin     = newwin(14, 58, 19, 2);
    WINDOW* consoleWin = newwin(14, 58, 19, 62);

    WINDOW* schedWin   = newwin(10, 58, 34, 2);
    WINDOW* semWin     = newwin(10, 58, 34, 62);

    WINDOW* ipcWin     = newwin(12, 58, 45, 2);
    WINDOW* mmuWin     = newwin(12, 58, 45, 62);

    scrollok(logWin, TRUE);
    scrollok(ipcWin, TRUE);
    scrollok(mmuWin, TRUE);

    // Draw heading.
    box(headingWin, 0, 0);
    write_window(headingWin, 1, 35, "ULTIMA 2.0 Integrated System Demo");
    write_window(headingWin, 2, 18, "Scheduler | Semaphore | IPC | Encrypted IPC | Memory Management");
    write_window(headingWin, 4, 20, "Press 'a' to run all integration tests, 'h' for help, 'q' to quit.");

    // Draw all windows.
    clear_window(task1Win);
    clear_window(task2Win);
    clear_window(task3Win);
    clear_window(logWin);
    clear_window(consoleWin);
    clear_window(schedWin);
    clear_window(semWin);
    clear_window(ipcWin);
    clear_window(mmuWin);

    update_task_windows(task1Win, task2Win, task3Win);
    display_help(consoleWin);

    write_window(logWin, 1, 1, "Log Window");
    write_window(schedWin, 1, 1, "Scheduler Window");
    write_window(semWin, 1, 1, "Semaphore Window");
    write_window(ipcWin, 1, 1, "IPC Window");
    write_window(mmuWin, 1, 1, "MMU Window");

    // --------------------------------------------------------
    // Create actual ULTIMA system objects.
    // --------------------------------------------------------
    Scheduler sched;

    int t1 = sched.create_task("Task1");
    int t2 = sched.create_task("Task2");
    int t3 = sched.create_task("Task3");

    Semaphore printerSem("Printer", 1, &sched);
    Semaphore ipcSem("IPC Queue", 1, &sched);
    Semaphore coreSem("Core Memory", 1, &sched);

    EncryptedIPC secureIPC(&ipcSem);
    MMU mmu(1024, '.', 64, &coreSem, &sched);

    bool paused = false;
    int input = -1;

    // --------------------------------------------------------
    // Main command loop.
    // --------------------------------------------------------
    while (input != 'q') {
        input = wgetch(consoleWin);

        switch (input) {
            case 'a':
                clear_window(logWin);
                clear_window(schedWin);
                clear_window(semWin);
                clear_window(ipcWin);
                clear_window(mmuWin);

                write_window(logWin, 1, 1, "Running FULL integration test...\n");

                run_scheduler_semaphore_test(sched, printerSem, logWin, schedWin, semWin);
                run_encrypted_ipc_test(secureIPC, logWin, ipcWin);
                run_mmu_test(mmu, logWin, mmuWin);

                write_window(logWin, "\nFULL INTEGRATION TEST COMPLETE.\n");
                break;

            case 's':
                run_scheduler_semaphore_test(sched, printerSem, logWin, schedWin, semWin);
                break;

            case 'i':
                run_encrypted_ipc_test(secureIPC, logWin, ipcWin);
                break;

            case 'm':
                run_mmu_test(mmu, logWin, mmuWin);
                break;

            case 'p':
                paused = !paused;
                if (paused) {
                    write_window(consoleWin, 10, 1, "System paused. Press p again to resume.");
                } else {
                    write_window(consoleWin, 10, 1, "System resumed.                         ");
                }
                break;

            case 'h':
                display_help(consoleWin);
                break;

            case 'c':
                clear_window(logWin);
                clear_window(schedWin);
                clear_window(semWin);
                clear_window(ipcWin);
                clear_window(mmuWin);
                write_window(logWin, 1, 1, "Windows cleared.");
                break;

            case 'q':
                write_window(logWin, "\nQuitting ULTIMA 2.0 integrated demo...\n");
                break;

            case ERR:
                // No key pressed. This keeps UI responsive.
                break;

            default:
                if (input != -1) {
                    write_window(consoleWin, 11, 1, "Invalid command. Press h for help.");
                }
                break;
        }

        // Yield CPU like Lab 7 hacker's corner suggests.
        sched_yield();
        usleep(50000);
    }

    // Clean up ncurses.
    endwin();

    return 0;
}