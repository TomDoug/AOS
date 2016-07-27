#include <avr/io.h>
#include <util/delay.h>
#include "kernel/kernel.h"

#define BLINK_MS 200



int main (void) {
	//run startup stuff
	startup_tests();
	kernel_main();
	
	return 0;
}

