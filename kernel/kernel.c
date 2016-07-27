#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "kernel.h"
#include "serial.h"

#define MS_PER_TIMER 10L
#define CLOCKS_PER_MS 16000L
#define CLOCK_PER_TICK MS_PER_TIMER * CLOCKS_PER_MS

//TODO:
//		-Add a module for handling buttons

//32 bits?
unsigned long clock_ticks;
int tick = 0;
unsigned int timer_ticks = 0;

static void
timer_tick()
{
	//loop through all of the timers and decrement their clocks. Fire
	//them if they are 0
}

/* Setup the timer functions */
static void
timer_setup ()
{
	cli(); //disable inturrupts
	TCCR1A = 0;
	TCCR1B = 0;

	// enable Timer0 overflow inturrupt
	TIMSK1 = (1 << TOIE1);

	// run timer at clock speed
	TCCR1B |= (1 << CS10);

	sei(); //re-enable inturrupts
}

int
clock_update (void) {
	cli(); //disable the timer temporarily
	if (clock_ticks >= CLOCK_PER_TICK) {
		clock_ticks -= CLOCK_PER_TICK;
		tick = 1;
	}
	sei();
}

int
time_update (void) {
	if (tick) {
		//do some timer stuff here
		timer_ticks++;
		timer_tick();

		//do this last in case we get inturrupted and tick gets reset		
		tick = 0;
	}
}
   
int
kernel_main (void) {

	//attach stdin and stdout
	serial_startup();

	printf("*****************************************\n");
	printf("kernel_main()\n");

	//any setup functions that need to be called?
	timer_setup();

	/* The main processing loop */
	while (1) {
		serial_poll();
		clock_update();
		time_update();
		
		if (task_spin() < 0) {
			//print_func("task error. Restarting\n");
		}
	}
}

//65535 clocks have passed
ISR(TIMER1_OVF_vect)
{
	clock_ticks += 65535;
}