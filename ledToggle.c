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

// pass in value of pin(int x)
//choose button we want to use
void toggle0(int x, int BUT, struct metal_gpio *gpio){

		 x = metal_gpio_get_input_pin(gpio, BUT);
			if (x) {
					printf("button 0 was pressed \n");
					metal_gpio_enable_output(gpio, 1);
					metal_gpio_toggle_pin(gpio, 1);
				}
		}


void toggle1(int x, int BUT, struct metal_gpio *gpio){

		 x = metal_gpio_get_input_pin(gpio, BUT);
			if (x) {
					printf("button 1 was pressed \n");
					metal_gpio_enable_output(gpio, 2);
					metal_gpio_toggle_pin(gpio, 2);
				}
		}

void toggle2(int x, int BUT, struct metal_gpio *gpio){

		 x = metal_gpio_get_input_pin(gpio, BUT);
			if (x) {
					printf("button 2 was pressed \n");
					metal_gpio_enable_output(gpio, 3);
					metal_gpio_toggle_pin(gpio, 3);
				}
		}


//void toggleALL (int x1, int BUT_1, int x2, int BUT_2, int x3,int BUT_3){
//
//	while (1) {
//		 x1 = metal_gpio_get_input_pin(gpio0, BUT_1);
//		 x2 = metal_gpio_get_input_pin(gpio0, BUT_2);
//		 x3 = metal_gpio_get_input_pin(gpio0, BUT_3);
//
//			if (x1) {
//					printf("button 0 was pressed \n");
//					metal_gpio_enable_output(gpio0, 1);
//					metal_gpio_toggle_pin(gpio0, 1);
//				}
//			else if (x2) {
//						printf("button 1 was pressed \n");
//						metal_gpio_enable_output(gpio0, 2);
//						metal_gpio_toggle_pin(gpio0, 2);
//					}
//			else if (x3) {
//						printf("button 2 was pressed \n");
//						metal_gpio_enable_output(gpio0, 3);
//						metal_gpio_toggle_pin(gpio0, 3);
//					}
//
//
//}
//}

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

		toggle0(val0,BUT0,gpio0);
		toggle1(val1,BUT1,gpio0);
		toggle2(val2,BUT2,gpio0);
	}

}


