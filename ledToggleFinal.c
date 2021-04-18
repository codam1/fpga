// See LICENSE for license details.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stdatomic.h"
#include <metal/gpio.h>

struct metal_gpio *gpio0;

#define BUT0 15
#define BUT1 30
#define BUT2 31

//enable input for button
void input (int BUT, struct metal_gpio *gpio){

		metal_gpio_disable_output(gpio, BUT);
		metal_gpio_enable_input(gpio, BUT);
}


//enable output of LED
void output (int LED, struct metal_gpio *gpio){

	metal_gpio_enable_output(gpio, LED);
}


// pass in value of pin(int x)
// choose which LED to turn on
//choose button we want to use

void toggle(int LED, struct metal_gpio *gpio){
					printf("button %d was pressed \n" , LED - 1);
					metal_gpio_toggle_pin(gpio, LED);
		}



int main() {
	printf("\n");
	printf("SIFIVE, INC.\n!!\n");
	printf("\n");
	printf(
			"Buttons 0-3 and Switch 3 are enabled as local interrupt sources.\n");
	printf(
			"Pressing Buttons 0-2 toggle LEDs, while turn ON Switch 3 to exit.\n");
	printf("\n");

	struct metal_gpio *gpio0;
	gpio0 = metal_gpio_get_device(0);

	input(BUT0, gpio0);
	input(BUT1,gpio0);
	input(BUT2,gpio0);

	while (1) {
		int val0 = metal_gpio_get_input_pin(gpio0, BUT0);
		int val1 = metal_gpio_get_input_pin(gpio0, BUT1);
		int val2 = metal_gpio_get_input_pin(gpio0, BUT2);

		output(1,gpio0);
		output(2,gpio0);
		output(3,gpio0);

		if (val0){
			toggle(1,gpio0);
		}

		else if (val1){
			toggle(2,gpio0);
		}

		else if (val2){
			toggle(3,gpio0);
		}
	}

}


