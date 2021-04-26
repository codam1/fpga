/*
 * helper.c
 *
 *  Created on: Apr 24, 2021
 *      Author: msu
 */

#include "helper.h"

#include <stdint.h>
#include <stdio.h>
#include <metal/gpio.h>
#include "aes.h"

#define DELAY           	20000000
#define BAUDRATE_115200 	115200
#define SPICLOCK_80KHZ  	80000
#define SPICLOCK_100KHZ  	100000
#define STR_LEN         	1024
#define BUF_LEN         	4096

#define MB85RS64V_MANUFACTURER_ID_CMD 0x9f
#define MB85RS64V_WRITE_ENABLE_CMD 0x06
#define MB85RS64V_READ_CMD 0x03
#define MB85RS64V_WRITE_CMD 0x02
#define MAX_USER_DATA_LENGTH 1024




void input(int BUT, struct metal_gpio *gpio) {

	metal_gpio_disable_output(gpio, BUT);
	metal_gpio_enable_input(gpio, BUT);
}

void output(int LED, struct metal_gpio *gpio) {

	metal_gpio_enable_output(gpio, LED);
}

void toggle(int LED, struct metal_gpio *gpio) {
	printf("button %d was pressed \n", LED - 1);
	metal_gpio_toggle_pin(gpio, LED);
}


void createKey(uint8_t *create) {

	for (int i = 0; i < 15; i++) {
		create[i] = (rand() % (15));
	}
	//return 1;
}


uint8_t encryptKey(uint8_t *key, uint8_t *butVal) {
	struct AES_ctx ctx;
	uint8_t encrypted[16];
	AES_init_ctx(&ctx, key);
	AES_ECB_encrypt(&ctx, butVal);
	return 1;
}

uint8_t decryptKey(uint8_t *encryptValue, uint8_t *key) {

	struct AES_ctx ctx;
	//uint8_t decrypted[16];
	AES_init_ctx(&ctx, key);
	AES_ECB_decrypt(&ctx, encryptValue);
	return 1;
}
