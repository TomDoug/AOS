#include <stdio.h>

typedef struct command_s command_t;
typedef int (*cmd_func) (void);

typedef struct command_s {
	const char		*command;
	cmd_func	run;
};



int print_help (void);




void USART_putchar (char c, FILE *stream);
char USART_getchar(FILE *stream);

int serial_startup (void);

void serial_poll (void);

char USART_kbhit (void);

char kbhit (void);




//extern FILE uart_output;
//extern FILE uart_input;