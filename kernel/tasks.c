#include <avr/io.h>
#include "kernel.h"

/* Pool to take our tasks from */
task_t task_pool[TOTAL_TASKS];
int num_allocated_tasks = 0;
task_t *head_task = NULL;
task_t *tail_task = NULL;
int stop_task_flag = 0;

void
remove_old_tasks (void)
{
	task_t *previous_task = head_task;
	task_t *current_task;

	while (head_task != NULL && (head_task->running == 0 && head_task->started == 1)) {
		head_task->allocated = 0;
		head_task = head_task->next_task;
	}

	if (head_task == NULL) {
		return 0;
	}
	previous_task = head_task;

	current_task = previous_task->next_task;

	while (current_task != NULL) {
		if (current_task->running == 0 && current_task->started == 1) {
			previous_task->next_task = current_task->next_task;
			current_task = previous_task->next_task;
			current_task->allocated = 0;
		} else {
			previous_task = current_task;
			current_task = previous_task->next_task;
		}
	}
}

int
task_spin (void)
{
	int ret = 0;
	int r = 0;
	task_t *current_task = head_task;

	while (current_task != NULL) {
		r = 0;
		if (current_task->started == 0) {
			if (current_task->startup != NULL) {
				r = current_task->startup(current_task->ctx);
				if (r != 0) {
					ret = r;
				}
			}
			current_task->started = 1;
			current_task->running = 1;
		} else {
			if (current_task->running){
				if (current_task->run != NULL) {
					r = current_task->run(current_task->ctx);
				} else {
					stop_task(current_task);
				}
				if (r != 0) {
					ret = r;
				}
			}
		}
		current_task = current_task->next_task;
	}
	return ret;
}

task_t*
add_task (startup_func start, shutdown_func stop, run_func run, void *ctx)
{
	int i;
	task_t *this_task = NULL;
	if (num_allocated_tasks >= (TOTAL_TASKS - 1)) {
		printf("No remaining tasks\n");
		return NULL;
	}

	for (i = 0; i < TOTAL_TASKS; i++) {
		if (!task_pool[i].allocated) {
			this_task = &task_pool[i];
			this_task->allocated = 1;
			break;
		}
	}

	if (!this_task) {
		printf("add_task-anamoly\n");
		return NULL;
	}

	num_allocated_tasks++;

	/* add this task to the linked list */
	if (head_task == NULL) {
		head_task = this_task;
		tail_task = this_task;
	}

	/* Add this to the end of the linked list */
	tail_task->next_task = this_task;
	tail_task = this_task;
	tail_task->next_task = NULL;

	/* Assign this task variables */
	this_task->startup = start;
	this_task->shutdown = stop;
	this_task->run = run;
	this_task->ctx = ctx;

	this_task->started = 0;
	this_task->running = 0;
	this_task->paused = 0;

	return this_task;
}

int
stop_task(task_t* task_handle)
{
	int exists = 0;
	int i = 0;

	//TODO: check that the task exists and that it is allocates
	for (i = 0; i < TOTAL_TASKS; i++) {
		if (task_handle == &task_pool[i]) {
			if (!task_handle->allocated) {
				break;
			}
			exists = 1;
			break;
		}
	}

	if (!exists) {
		/* Someone passed something that isnt a task */
		return -1;
	}

	task_handle->running = 0;
	if (task_handle->shutdown != NULL) {
		task_handle->shutdown(task_handle->ctx);
	}

	return 0;
}
