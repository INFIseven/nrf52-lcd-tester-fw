/**
 * @file syscalls.c
 * @brief Minimal syscall implementations for newlib
 */

#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

/**
 * @brief Exit implementation
 * Called by exit() to terminate the program
 */
void _exit(int status)
{
    (void)status;

    // Disable interrupts
    __asm volatile("cpsid i");

    // Infinite loop
    while(1) {
        __asm volatile("wfi"); // Wait for interrupt (low power)
    }
}

/**
 * @brief Kill process implementation (not used in bare-metal)
 */
int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

/**
 * @brief Get process ID (not used in bare-metal)
 */
int _getpid(void)
{
    return 1;
}
