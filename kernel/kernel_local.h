#ifndef _KERNEL_LOCAL_H
#define _KERNEL_LOCAL_H
#include "kernel/kernel.h"

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

/* task.c */
int task_spin (void);

#endif /* _KERNEL_LOCAL_H */
