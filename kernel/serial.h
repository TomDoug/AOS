#ifndef _SERIAL_H
#define _SERIAL_H

#include <stdio.h>
typedef int (*cmd_func) (int argc, char* argv[], void *ctx);

/* Serial API */
void* add_command(char* command, cmd_func run, void* ctx, char* help);
int serial_startup (void);
void serial_poll (void);
char kbhit (void);

#endif /* _SERIAL_H */
