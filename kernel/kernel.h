#ifndef _KERNEL_H
#define _KERNEL_H

#include "configure.h"
#include "kernel/serial.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

typedef int (*startup_func) (void *ctx);
typedef int (*shutdown_func) (void *ctx);
typedef int (*run_func) (void *ctx);

extern unsigned int timer_ticks;

/* Function Definitions */

/* kernel.c */
int kernel_main (void);

/* task.c */
void* add_task (startup_func start, shutdown_func stop, run_func run, void *ctx);
int stop_task (void *task_handle);

/* adc.c */
void adc_startup (void);
#endif /* _KERNEL_H */
