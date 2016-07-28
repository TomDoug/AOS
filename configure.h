#ifndef _CONFIGURE_H
#define _CONFIGURE_H

#undef ADD_USART
#undef ADD_STDIO
#undef ADD_COMMANDS
#undef ADD_STDIO
#undef ADD_STDIO
#undef ADD_STDIO

/* Configure various memory spaces */
#define TOTAL_TASKS 24
#define NUM_COMMANDS 24
#define COMMAND_BUFFER_SIZE 64

/* Configure the USART port */
#define BAUD 19200

#endif /* _CONFIGURE_H */
