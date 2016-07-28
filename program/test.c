#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "kernel/kernel.h"

#define BLINK_MS 200

typedef struct {
	int state;
	unsigned int last_timer;
	unsigned int last_blink_time;
	uint8_t duty; /* 0 - 100 */
} blink_ctx;

blink_ctx blink;

typedef struct {
	uint8_t max;
	uint8_t min;
	blink_ctx *blink;
	uint8_t counter;
} adc_ctx;

adc_ctx adc;

int led_on	 (int argc, char* argv[], void *ctx);
int led_off	 (int argc, char* argv[], void *ctx);
int read_adc (int argc, char* argv[], void *ctx);
	
/***** BLINK TEST ******/
int blink_startup (void *ctx)
{
	blink_ctx *blnk = ctx;

	printf("blink_startup()\n");

	blnk->state = 0;
	blnk->last_blink_time = timer_ticks;
	blnk->last_timer = timer_ticks;
	//enable d5 as output
	DDRD |= (1 << PD5);

	//add commands to turn the LED on and off
	add_command("ledon", led_on, ctx, "Turn on the LED");
	add_command("ledoff", led_off, ctx, "Turn off the LED");
	return 0;
}

int led_on (int argc, char* argv[], void *ctx)
{
	blink_ctx *blnk = ctx;
	//turn on led
	PORTD |= (1 << PD5);
	blnk->state = 1;
	return 0;
}

int led_off (int argc, char* argv[], void *ctx)
{
	blink_ctx *blnk = ctx;
	//turn off the LED
	PORTD &= ~(1 << PD5);
	blnk->state = 0;
	return 0;
}

int blink_run (void *ctx)
{
#if 0 //adc test
	blink_ctx *blnk = ctx;
	unsigned int elapsed_time;
	unsigned int current_time = timer_ticks;


	if (blnk->last_blink_time > current_time) {
		//handle clock overflow case
		elapsed_time = (65535 - blnk->last_blink_time) + current_time;
	} else {
		elapsed_time = current_time - blnk->last_blink_time;
	}

//	if (elapsed_time > (BLINK_MS * blnk->duty)) {
	if (elapsed_time > (blnk->duty)) {

		blnk->last_blink_time += (BLINK_MS * blnk->duty);
		if (blnk->state == 0) {
			//turn on the LED
			//turn on led
			PORTD |= (1 << PD5);
			blnk->state = 1;
		} else {
			//turn off the LED
			PORTD &= ~(1 << PD5);
			blnk->state = 0;
		}
	}	
#endif
	return 0;
}

int adc_test_startup (void *ctx) {
	adc_ctx *adc = (adc_ctx *) ctx;
	adc_startup();
	adc->max = 0;
	adc->min = 255;
	adc->counter = 0;
	add_command("readadc", read_adc, ctx, "Read the current ADC value");
	return 0;
}

int read_adc (int argc, char* argv[], void *ctx) {
	adc_ctx *adc = (adc_ctx *) ctx;
	uint8_t sample = adc_get();
	printf("%d\n", sample);
	return 0;
}

int adc_run (void *ctx) {
	uint8_t sample = adc_get();
	adc_ctx *adc = (adc_ctx *) ctx;

	if (adc->counter < 200) {
		//wait a bit before starting
		adc->counter++;
		return 0;
	}

	if (sample < adc->min) {
//		printf("Sample min updated from %d to ", adc->min);
		adc->min = sample;
//		printf("%d\n", adc->min);
	}
	if (sample > adc->max) {
//		printf("Sample max updated from %d to ", adc->max);
		adc->max = sample;
//		printf("%d\n", adc->max);
	}

	//scale
	adc->blink->duty = (((int)(sample - adc->min)) * 100) / (int)(adc->max - adc->min);
	return 0;
}

static int last_shift_time = 0;
static int current_pattern = 0;
   uint8_t led_pattern[8]={
                        0b10000001,
                        0b11000011,
                        0b11100111,
                        0b11111111,
                        0b01111110,
                        0b00111100,
                        0b00011000,
                        0b00000000,
                     };


int shift_run(void *ctx) {
#if 0
	unsigned int elapsed_time;
	unsigned int current_time = timer_ticks;


	if (last_shift_time > current_time) {
		//handle clock overflow case
		elapsed_time = (65535 - last_shift_time) + current_time;
	} else {
		elapsed_time = current_time - last_shift_time;
	}

	if (elapsed_time > (100)) {

		last_shift_time += 100;
		
		printf("HC595Write: %02x\n", led_pattern[current_pattern]);
		HC595Write(led_pattern[current_pattern]);
		current_pattern++;
		if (current_pattern >= 8) {
			current_pattern = 0;
		}
	}
	
#endif
	return 0;
}

uint8_t current_value = 0;
int shift_value(int argc, char* argv[], void *ctx)
{
	int i;
	uint8_t value = 0;
	char *p;
	if (argc < 2) {
		printf("Usage: shift 01001101\n");
		return 0;
	}

	if (argc == 2) {
		p = argv[1];

		for (i = 0; i < 8; i++) {
			if (*p == '\0') {
				printf("Usage: shift 01001101\n");
				return 0;
			}
			if (*p != '1' && *p != '0') {
				printf("Usage: shift 01001101\n");
				return 0;
			}

			if (*p == '1') {
				value |= (1 << (7-i));
			}
			p++;
		}
	} else {
		if (strncmp(argv[1], "bit", 3) == 0) {
			uint8_t bit = 0;
			bit = *argv[2] - '0';
			printf("BIT: %d\n", bit);
			if (bit < 0 || bit > 8) {
				printf("Bit out of range\n");
				return 0;
			}

			if (current_value & (1<<bit)) {
				value = current_value & ~(1 << bit);
			} else {
				value = current_value | (1 << bit);
			}
		}
	}
	

	printf("Shifting %02x into register\n", value);
	current_value = value;
	HC595Write(value);
	return 0;
}

int shift_startup (void *ctx) {
	HC595Init();
	last_shift_time = timer_ticks;
	add_command("shift", shift_value, NULL, "Play with the shift register");
	return 0;
}

void
startup_tests (void)
{
	add_task(blink_startup, NULL, blink_run, &blink);
	adc.blink = &blink;
	add_task(adc_test_startup, NULL, adc_run, &adc);
	add_task(shift_startup, NULL, shift_run, NULL);
}