#include "kernel/serial.h"
#define TOTAL_TASKS 24

#ifndef NULL
#define NULL 0
#endif /* NULL */

typedef int (*startup_func) (void *ctx);
typedef int (*shutdown_func) (void *ctx);
typedef int (*run_func) (void *ctx);

typedef struct task_s task_t;

typedef struct task_s {
	startup_func	startup;
	shutdown_func	shutdown;
	run_func		run;
	void			*ctx;
	int				running;
	int				started;
	int				paused;
	int				allocated;
	task_t			*next_task;
};

extern unsigned int timer_ticks;

/* Function Definitions */

/* kernel.c */
int kernel_main (void);

/* task.c */
int task_spin (void);
task_t *add_task (startup_func start, shutdown_func stop, run_func run, void *ctx);
int stop_task (task_t *task_handle);

/* adc.c */
void adc_startup (void);
