#include <stdio.h>

typedef struct command_s command_t;
typedef int (*cmd_func) (int argc, char* argv[], void *ctx);

typedef struct command_s {
	const char	*command;
	cmd_func	run;
	void		*ctx;
};



int print_help (int argc, char* argv[], void *ctx);

command_t* add_command(char* command, cmd_func run, void* ctx);


void USART_putchar (char c, FILE *stream);
char USART_getchar(FILE *stream);

int serial_startup (void);

void serial_poll (void);

char USART_kbhit (void);

char kbhit (void);




//extern FILE uart_output;
//extern FILE uart_input;