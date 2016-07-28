#AOS
Simple AVR C "operating system" for the Arduino board. AOS is task based, each "user program"
defines startup, shutdown, and run functions. The kernel will process the tasks as quickly as
possible. AOS also provides timing with a global variable that increments (approx.) once every
millisecond.

AOS Also provides access to some chip specific functions. The serial USART has been made available
via stdio. This means you can use functions such as printf, scanf, etc. In addition AOS provides
access to the ADC's of the chip (currently only ADC0).

AOS provides a command interface. Commands can be entered into the serial console from a PC and
will be parsed by the kernel.

#Serial API
The serial module handle all of the USART processing. This module is respnsable for setting up
the USART, attaching it to stdin and stdout, and parsing the commands.

```
	typedef int (*cmd_func) (int argc, char* argv[], void *ctx);
```

```
void* add_command(char* command, cmd_func run, void* ctx, char* help);
	Add a command to the command parser
	inputs:
		char* command - Name of the command to match against ("help")
		cmd_func run - Callback to be run when this command is matched
		void* ctx - Optional context that can be passed into the run callback
		char* help - A small one line description to be placed in the output of the "help" command

	returns:
		void* - Handle reserved for future use (removing commands)

```

```
int serial_startup (void);
	Startup the USART and attach the stdio. This should only need to be called from within the kernel.

	inputs:
		void
	returns:
		int - always 0

```

```
void serial_poll (void);
	Poll the serial module. This should only be called by the kernel.

	inputs:
		void
	returns:
		void
```

```
char kbhit (void);
	Check to see if a character has beed pressed. (Note: user space input handling will likely 
	interfere with command processing).

	input:
		void
	returns: 
		char - 1 if an unhandled key has been pressed. 0 otherwise.
```
	

#Feature Roadmap/TODO
- Add shift register support for both outputs and muliplexed inputs
- Add enc28j60 ethernet support
- Make help print look nicer
- Add ability to turn on/off options
- Add more sample tasks
- Better define the timer operation
- Add software timers (callbacks after x ticks)
- Generalize pin support (set pinstates with function calls, etc.)
- Add ability to attach to button inturrupts? (this could mesh nicely with the muxed input)
- Generalize ADC support
- Add ability to use PWM pins
- Add 10-bit ADC mode