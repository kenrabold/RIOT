/*
 * Copyright (C) 2017
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        nanostubs.c
 * @brief       Implementation of the Newlib-nano for SiFive FE310
 *
 * @author
 * @}
 */


#include <reent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "arch/thread_arch.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"
#include "cpu.h"
#include "uart_stdio.h"


extern char _heap_start;			//	Heap markers from fe310.ld file
extern char _heap_end;
char *heap_top = &_heap_start + 4;


/**
 * @brief Initialize the Newlib-nano functions (also forces inclusion of stubs for linking)
 */
void nanostubs_init(void)
{
#if defined(MODULE_UART_STDIO)
	uart_stdio_init();
#endif
}

void _init(void)
{
}

void _fini(void)
{
}


void * _sbrk(ptrdiff_t incr)
{
    unsigned int state = irq_disable();
    void *res = heap_top;

    //	Allocate memory from heap
    if ((heap_top + incr > &_heap_end) || (heap_top + incr < &_heap_start)) {
        errno = ENOMEM;
        res = (void *)-1;
    }
    else {
        heap_top += incr;
    }

    irq_restore(state);
    return res;
}


int _open(const char *name, int flags, int mode)
{
    (void) name;
    (void) flags;
    (void) mode;
    errno = ENODEV;
    return -1;
}


_ssize_t _read(int fd, void *buffer, size_t count)
{
#if defined(MODULE_UART_STDIO)
    if (fd == STDIN_FILENO) {
        return uart_stdio_read(buffer, count);
    }
    errno = EBADF;
    return -1;
#else
    (void)fd;
    (void)buffer;
    (void)count;
    errno = ENODEV;
    return -1;
#endif
}

_ssize_t _write(int fd, const void *data, size_t count)
{
#if defined(MODULE_UART_STDIO)
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        return uart_stdio_write(data, count);
    }
    errno = EBADF;
    return -1;
#else
    (void)fd;
    (void)data;
    (void)count;
    errno = ENODEV;
    return -1;
#endif
}

int _close(int fd)
{
    errno = ENODEV;
    return -1;
}


int _isatty(int fd)
{
    errno = 0;

    if(fd == STDIN_FILENO || fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        return 1;
    }

    return 0;
}

int _fstat(int fd, struct stat *st)
{
    (void) fd;
    (void) st;
    errno = ENODEV;
    return -1;
}


_off_t _lseek(int fd, _off_t pos, int dir)
{
    (void) fd;
    (void) pos;
    (void) dir;
    errno = ENODEV;
    return -1;
}


