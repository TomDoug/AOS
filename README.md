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