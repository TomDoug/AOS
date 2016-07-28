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