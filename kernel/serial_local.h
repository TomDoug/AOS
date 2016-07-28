#ifndef _SERIAL_LOCAL_H
#define _SERIAL_LOCAL_H

#include "kernel\serial.h"
typedef struct command_s command_t;
typedef struct command_s {
	const char	*command;
	cmd_func	run;
	void		*ctx;
	char		*help;
};

/* Funtions local to serial.h */
int print_help (int argc, char* argv[], void *ctx);

void USART_putchar (char c, FILE *stream);
char USART_getchar(FILE *stream);
char USART_kbhit (void);


#endif /* _SERIAL_LOCAL_H */

