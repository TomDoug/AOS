#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "kernel/kernel.h"
#include "kernel/serial_local.h"
#include <util/setbaud.h>


#undef DEBUG_COMMAND_PARSE

/* These are the streams that will be used for stdin and stdout */
//FILE uart_output;
//FILE uart_input;

char command_buffer[COMMAND_BUFFER_SIZE];
uint8_t command_buffer_pointer = 0;

command_t commands[NUM_COMMANDS];

//note: save this pointer. as of now the only way to diable a command
//form being run is to null its run function
void*
add_command(char* command, cmd_func run, void* ctx, char *help) {
	int i = 0;

	/* Command 0 is always help */
	for (i = 1; i < NUM_COMMANDS; i++) {
		if (commands[i].command == NULL) {
			commands[i].command = command;
			commands[i].run = run;
			commands[i].ctx = ctx;
			commands[i].help = help;
#ifdef DEBUG_COMMAND_PARSE
			printf("Command %s added\n", command);
#endif /* DEBUG_COMMAND_PARSE */
			return &commands[i];
		}
	}
	printf("Could not add command, all spots full\n");
	return NULL;
}

void
init_commands (void)
{
	int i = 0;

	//startup the help command
	commands[i].command = "help";
	commands[i].run = print_help;
	commands[i].ctx = NULL;
	commands[i].help = "Print all available commands";

	for (i = 1; i < NUM_COMMANDS; i++) {
		commands[i].command = NULL;
		commands[i].run = NULL;
		commands[i].ctx = NULL;
		commands[i].help = NULL;
	}
}

int
print_help (int argc, char* argv[], void *ctx)
{
	command_t *current_command;
	char i = 0;
	current_command = &commands[i];
	printf("Available commands: \n");

	while (current_command->command != NULL) {
		printf("\t%s", current_command->command);
		if (current_command->help != NULL) {
			printf(" - %s", commands[i].help);
		}
		printf("\n");
		current_command = &commands[++i];
	}
	
	return 0;
}


char
serial_match_strings (char *st1, char *st2)
{
	char *st1p = st1;
	char *st2p = st2;
	char st1c;
	char st2c;
	char match = 1;
	if (!st1p || !st2p || *st1p == '\0' || *st2p == '\0') {
#ifdef DEBUG_COMMAND_PARSE
		printf("Error on input\n");
#endif /* DEBUG_COMMAND_PARSE */
		return 0;
	}

	while (*st1p != '\0') {
		st1c = *st1p;
		st2c = *st2p;

#ifdef DEBUG_COMMAND_PARSE
		printf("-- %c --\n", *st1p);
#endif /* DEBUG_COMMAND_PARSE */

		if (st1c >= 97 && st1c <= 122) {
#ifdef DEBUG_COMMAND_PARSE
			printf("in: %c ", st1c);
#endif /* DEBUG_COMMAND_PARSE */
			st1c -= 32;
#ifdef DEBUG_COMMAND_PARSE
			printf("out: %c \n", st1c);
#endif /* DEBUG_COMMAND_PARSE */
		}

		if (st2c >= 97 && st2c <= 122) {
#ifdef DEBUG_COMMAND_PARSE
			printf("in: %c ", st2c);
#endif /* DEBUG_COMMAND_PARSE */
			st2c -= 32;
#ifdef DEBUG_COMMAND_PARSE
			printf("out: %c \n", st2c);
#endif /* DEBUG_COMMAND_PARSE */
		}

		if (*st2p == '\0') {
#ifdef DEBUG_COMMAND_PARSE
			printf("Match got to the end of the second string\n");
#endif /* DEBUG_COMMAND_PARSE */
			return match;
		}		

		if (st1c != st2c) {
			//match failed
#ifdef DEBUG_COMMAND_PARSE
			printf("Match failed\n");
#endif /* DEBUG_COMMAND_PARSE */
			return 0;
		}
		
		st1p++;
		st2p++;
	}

#ifdef DEBUG_COMMAND_PARSE
	printf("Match got to end of first string\n");
#endif /* DEBUG_COMMAND_PARSE */
	return match;
}

/* Unused */
uint8_t
serial_count_arguments (char* input)
{
	char *p = input;
	uint8_t count = 1;
	uint8_t sp = 0;
	
	while (*p != '\0') {
		if (*p == ' ') {
			sp = 1;
		} else {
			if (sp) {
				/* The previous char was space and this is not */
				count++;
				sp = 0;
			}
		}
		p++;
	}
	return count;
}

/* Note: this is destructive to input, argv is considered an output */
uint8_t
serial_split_args (char* input, char* argv[])
{
	char *p = input;
	uint8_t count = 1;
	uint8_t sp = 0;
	
	argv[count-1] = input;
	while (*p != '\0') {
		if (*p == ' ') {
			sp = 1;
			//replce the space with a null terminator
			*p = '\0';
		} else {
			if (sp) {
				/* The previous char was space and this is not */
				count++;
				argv[count-1] = p;
				sp = 0;
			}
		}
		p++;
	}
	return count;
}

void
serial_find_command (void)
{
	command_t *current_command;
	int i = 0;
	uint8_t argc;
	//up to 10 arguments
	char *argv[10];

	current_command = &commands[i];

	while ((current_command->command) != NULL) {
		if (serial_match_strings(current_command->command, command_buffer)) {
#ifdef DEBUG_COMMAND_PARSE
			printf("Found command %s\n", current_command->command);
#endif /* DEBUG_COMMAND_PARSE */

			argc = serial_split_args(command_buffer, argv);

#ifdef DEBUG_COMMAND_PARSE
			for (i = 0; i < argc; i++) {
				printf("%d: %s\n", i, argv[i]);
			}
			printf("Command has %d arguments\n", argc);
#endif /* DEBUG_COMMAND_PARSE */

			if (current_command->run) {
				current_command->run(argc, argv, current_command->ctx);
			}
			break;
		}
		i++;
		current_command = &commands[i];
	}
}

void
serial_process_command (void)
{
#ifdef DEBUG_COMMAND_PARSE
	printf("command was %s\n", command_buffer);
#endif /* DEBUG_COMMAND_PARSE */
	serial_find_command();
	command_buffer_pointer = 0;
	command_buffer[0] = '\0';
}

static void
USART_init (void)
{
	cli();
	/* UART init */
	UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

	/* 8-bit data */
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

	/* Enable RX, RX int, and TX */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	sei();

	/* Startup the command parser */
	init_commands();
}

void
USART_putchar (char c, FILE *stream)
{
	if (c == '\n') {
        USART_putchar('\r', stream);
    }
	loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}

char
USART_getchar(FILE *stream)
{
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
    return UDR0;
}

char
USART_kbhit (void)
{
	if (UCSR0A & (1 << RXC0)) {
		return 1;
	} else {
		return 0;
	}
}

char
kbhit (void)
{
	return USART_kbhit();
}

FILE uart_output = FDEV_SETUP_STREAM(USART_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_getchar, _FDEV_SETUP_READ);

int
serial_startup (void)
{
	/* Startup the USART and attach it to stdin and stdout */

	USART_init();

	stdout = &uart_output;
	stdin  = &uart_input;

	command_buffer[0] = '\0';

	return 0;
}

void
serial_poll (void)
{
	//check for keypress
	if (kbhit()) {
		char key = getc(stdin);
		printf("%c", key);
		if (key == '\r') {
			printf("\n");
			serial_process_command();
		} else if (key == 0x08) {
			command_buffer[command_buffer_pointer] = '\0';
			command_buffer_pointer--;
		} else {
			if (command_buffer_pointer < (COMMAND_BUFFER_SIZE - 1)) {
				command_buffer[command_buffer_pointer++] = key;
				command_buffer[command_buffer_pointer] = '\0';
			}
		}
	}
}

#if 0
int
main (void)
{

	FILE uart_output = FDEV_SETUP_STREAM(USART_putchar, NULL, _FDEV_SETUP_WRITE);
	FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_getchar, _FDEV_SETUP_READ);

	USART_init();

	stdout = &uart_output;
    stdin  = &uart_input;
	
	DDRD |= (1 << PD5);	

	while(1) {
		_delay_ms(500);
		//turn on led
		PORTD |= (1 << PD5);
		printf("On\n");
		_delay_ms(500);
		//turn off the LED
		PORTD &= ~(1 << PD5);
		printf("Off\n");
   }
}
#endif
