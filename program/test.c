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
	add_command("ledon", led_on, ctx);
	add_command("ledoff", led_off, ctx);
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
	add_command("readadc", read_adc, ctx);
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


void
startup_tests (void)
{
	add_task(blink_startup, NULL, blink_run, &blink);
	adc.blink = &blink;
	add_task(adc_test_startup, NULL, adc_run, &adc);
}