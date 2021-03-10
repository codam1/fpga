// See LICENSE for license details.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stdatomic.h"
#include <metal/gpio.h>

struct metal_gpio *gpio0;
struct metal_interrupt *cpu_intr, *tmr_intr, *buttonA_intr;
struct metal_interrupt *but0_ic, *but1_ic, *but2_ic, *but3_ic, *swch3_ic,
		*gpio_ic;

#define BUT0 15
#define BUT1 30
#define BUT2 31

/**
 *buttona callback function Clear the falling edge interrupt flag bit, set flag to 2
 */
void buttonA_isr(int id, void *data) {
	struct metal_gpio *gpio0 = metal_gpio_get_device(0);
	//gpio0 = metal_gpio_get_device(0);
	metal_gpio_enable_output(gpio0, 2);
	metal_gpio_toggle_pin(gpio0, 2);
	//set gpio 3 as input pin
	//metal_gpio_enable_input(gpio0, gpioNum);

	metal_gpio_clear_interrupt(gpio0, 11, METAL_GPIO_INT_FALLING);
	int *flag = (int*) data;
	*flag = 2;
}

/**
 *buttona is initialized as falling edge trigger interrupt
 */
void ButtonA_init(int *flag) {

	struct metal_gpio *gpio0; //Declare a gpio object
	int buttonA_id = 12; //According to the chip manual, the interrupt id of gpio11 is 12
	gpio0 = metal_gpio_get_device(0); //Instantiate the gpio object

	//Get the gpio controller, which is actually plic
	buttonA_intr = metal_gpio_interrupt_controller(gpio0);

	metal_interrupt_init(buttonA_intr);

	buttonA_id = metal_gpio_get_interrupt_id(gpio0, BUT0);

	//Register the callback function buttonA_isr is the function name of the callback function, and the last flag passed is a pointer, which is the parameter passed to the callback function
	metal_interrupt_register_handler(buttonA_intr, buttonA_id, buttonA_isr,
			flag);
	//Set the priority to 3
	metal_interrupt_set_priority(buttonA_intr, buttonA_id, 3);

	//enable gpio
	metal_gpio_disable_output(gpio0, BUT0);
	metal_gpio_enable_input(gpio0, BUT0);

	//Prohibit gpio function reuse
	metal_gpio_disable_pinmux(gpio0, BUT0);

	//Close the interrupt first
	metal_gpio_config_interrupt(gpio0, BUT0, METAL_GPIO_INT_DISABLE);

	//Clear all the flags of gpio interrupt
	metal_gpio_clear_interrupt(gpio0, BUT0, METAL_GPIO_INT_MAX);

	//Configure as falling edge interrupt
	metal_gpio_config_interrupt(gpio0, BUT0, METAL_GPIO_INT_HIGH);

	//Enable gpio11
	metal_interrupt_enable(buttonA_intr, buttonA_id);
}

int main() {
	printf("\n");
	printf("SIFIVE, INC.\n!!\n");
	printf("\n");
	printf("Coreplex IP Eval Kit 'local-interrupt' Example.\n\n");
	printf(
			"Buttons 0-3 and Switch 3 are enabled as local interrupt sources.\n");
	printf("A 1s debounce timer is used between these interrupts.\n");
	printf(
			"Pressing Buttons 0-2 toggle LEDs, while turn ON Switch 3 to exit.\n");
	printf("\n");
	struct metal_gpio *gpio0;
	int flag;
	gpio0 = metal_gpio_get_device(0);

	metal_gpio_disable_output(gpio0, BUT0);
	metal_gpio_enable_input(gpio0, BUT0);

	metal_gpio_disable_output(gpio0, BUT1);
	metal_gpio_enable_input(gpio0, BUT1);

	metal_gpio_disable_output(gpio0, BUT2);
	metal_gpio_enable_input(gpio0, BUT2);



	while (1) {
		int value = metal_gpio_get_input_pin(gpio0, BUT0);
		int value1 = metal_gpio_get_input_pin(gpio0, BUT1);
		int value2 = metal_gpio_get_input_pin(gpio0, BUT2);


		//but0 will turn on green led 0
		if (value) {
			printf("button 0 was pressed \n");
			metal_gpio_enable_output(gpio0, 2);
			metal_gpio_toggle_pin(gpio0, 2);
		}

		//but1 will turn on blue led 0
		if (value1) {
			printf("button 1 was pressed \n");
			metal_gpio_enable_output(gpio0, 3);
			metal_gpio_toggle_pin(gpio0, 3);
		}

		//but2 will turn on red led 0
		if (value2) {
			printf("button 2 was pressed \n");
			metal_gpio_enable_output(gpio0, 1);
			metal_gpio_toggle_pin(gpio0, 1);
		}
	}
}
