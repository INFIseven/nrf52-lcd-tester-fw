/**
 ********************************************************************************
 * @file    syscalls.c
 * @brief   Minimal syscall implementations for newlib on bare-metal
 *
 * This is free and unencumbered software released into the public domain.
 * For more information, please refer to <http://unlicense.org/>
 ********************************************************************************
 */

#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

/**
 * @brief Exit implementation for program termination
 * @param status Exit status code (unused in bare-metal)
 *
 * Called by exit() to terminate the program. Disables interrupts and
 * enters an infinite low-power loop.
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
 * @brief Kill process implementation (stub for bare-metal)
 * @param pid Process ID (unused)
 * @param sig Signal number (unused)
 * @return Always returns -1 with errno set to EINVAL
 */
int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

/**
 * @brief Get process ID (stub for bare-metal)
 * @return Always returns 1 (single process system)
 */
int _getpid(void)
{
    return 1;
}
