#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <stdint.h>
//#include <unistd.h>
#include <time.h>
#include "stdatomic.h"
#include <metal/gpio.h>
#include "aes.h"
#include "helper.h"
//#include "uart.h"
#include "cpu.h"
#include "spi.h"
#include "led.h"
#include <metal/uart.h>
#include <metal/machine/platform.h>

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

//static uint8_t data[MAX_USER_DATA_LENGTH], cmp_data[MAX_USER_DATA_LENGTH];

struct metal_gpio *gpio0;

#define BUT0 15
#define BUT1 30
#define BUT2 31

static int mb85rs64v_access(uint8_t cmd, uint16_t addr, char *tx_buf,
		char *rx_buf, size_t len) {
	uint8_t access[3];

	char tx_temp[30] = { 0 };
	int tempLength = len;
	memmove(tx_temp + 3, tx_buf, len * sizeof(char));
	tx_temp[0] = cmd;
	tx_temp[1] = 0;
	tx_temp[2] = 0;
	if (cmd == MB85RS64V_WRITE_CMD || cmd == MB85RS64V_READ_CMD) {

		tx_temp[1] = (addr >> 8) & 0xFF;
		tx_temp[2] = addr & 0xFF;

		tempLength = tempLength + 3;

		if (cmd == MB85RS64V_READ_CMD) {

			spi_read_fram(tx_temp, rx_buf, tempLength);
			return 0;
		}
	} else {
	}
	spi_send_fram(tx_temp, rx_buf, tempLength);
	return 0;
}

static int mb85rs64v_read_id() {
	uint8_t id[5];
	int err;

	err = mb85rs64v_access(MB85RS64V_MANUFACTURER_ID_CMD, 0, NULL, &id, 5);
	if (err) {
		printf("%s\n", "Error during ID read\n");
		return -1;
	}

	if (id[1] != 0x04) {
		return -1;
	}

	if (id[2] != 0x7f) {
		return -1;
	}

	if (id[3] != 0x03) {
		return -1;
	}

	if (id[4] != 0x02) {
		return -1;
	}

	return 0;
}

static int write_bytes(uint16_t addr, char *tx_buf, char *rx_buf,
		uint32_t num_bytes) {
	int err;

	/* disable write protect */
	err = mb85rs64v_access(MB85RS64V_WRITE_ENABLE_CMD, 0, NULL, NULL, 1);
	if (err) {
		printf("unable to disable write protect\n");
		return -1;
	}

	/* write cmd */
	err = mb85rs64v_access(MB85RS64V_WRITE_CMD, addr, tx_buf, &rx_buf,
			num_bytes);
	if (err) {
		printf("Error during SPI write\n");
		return -1;
	}

	return 0;
}

static int read_bytes(uint16_t addr, char *tx_buf, char *rx_buf,
		uint32_t num_bytes) {
	int err;

	/* read cmd */
	err = mb85rs64v_access(MB85RS64V_READ_CMD, addr, tx_buf, rx_buf, num_bytes);
	if (err) {
		printf("Error during SPI read\n");
		return -1;
	}

	return 0;
}

static int AES_KEY_GEN(uint16_t *addr) {
	int err;
	uint8_t key[16] = { 0 };
	for (int i = 0; i < 5; i++) {
		createKey(&key);
		uint16_t temp = addr[i];
		err = write_bytes(addr[i], key, NULL, 16);
	}
	return 0;
}

int BUTTON_INPUT(uint8_t ATTEMPTS_COUNT) {
	int exit = 0;
	char butState = 'A';

	uint8_t tempCount = ATTEMPTS_COUNT;
	uint8_t tempLimit = 0;

	input(BUT0, gpio0);
	input(BUT1, gpio0);
	input(BUT2, gpio0);

	int but_0 = 0, but_1 = 0, but_2 = 0;

	while (exit == 0) {
		but_0 = metal_gpio_get_input_pin(gpio0, BUT0);
		but_1 = metal_gpio_get_input_pin(gpio0, BUT1);
		but_2 = metal_gpio_get_input_pin(gpio0, BUT2);
		switch (butState) {
		case 'A':

			if (but_0) {
				delay(1000);
				while (metal_gpio_get_input_pin(gpio0, BUT0)) {
					metal_gpio_clear_pin(gpio0, BUT0);
				}
				butState = 'B';
			} else if (but_1 || but_2) {
				tempLimit++;

				butState = 'E';
			}

			break;

		case 'B':

			if (but_2) {
				delay(1000);
				while (metal_gpio_get_input_pin(gpio0, BUT2)) {
					metal_gpio_clear_pin(gpio0, BUT2);
				}

				butState = 'C';
			} else if (but_1 || but_0) {
				tempLimit++;
				butState = 'E';
			}

			break;

		case 'C':

			if (but_0) {
				delay(1000);
				while (metal_gpio_get_input_pin(gpio0, BUT0)) {
					metal_gpio_clear_pin(gpio0, BUT0);
				}

				butState = 'D';
			} else if (but_1 || but_2) {
				tempLimit++;
				butState = 'E';
			}

			break;

		case 'D':
			if (but_1) {
				delay(1000);
				while (metal_gpio_get_input_pin(gpio0, BUT1)) {
					metal_gpio_clear_pin(gpio0, BUT1);
				}

				butState = 'F';
			} else if (but_0 || but_2) {
				tempLimit++;
				butState = 'E';
			}

			break;

		case 'E':
			write_bytes(0x28, tempLimit, NULL, 1);
			metal_gpio_enable_output(gpio0, 1);
			metal_gpio_toggle_pin(gpio0, 1);
			delay(1000000);
			metal_gpio_toggle_pin(gpio0, 1);
			exit = -1;
			break;

		case 'F':
			metal_gpio_enable_output(gpio0, 2);
			metal_gpio_toggle_pin(gpio0, 2);
			delay(1000000);
			metal_gpio_toggle_pin(gpio0, 2);
			exit = 1;
			break;
		}
		but_0 = 0;
		but_1 = 0;
		but_2 = 0;

		delay(100000);

		metal_gpio_clear_pin(gpio0, BUT0);
		metal_gpio_clear_pin(gpio0, BUT1);
		metal_gpio_clear_pin(gpio0, BUT2);

	}
	return exit;
}

void UART_INPUT(uint16_t keyAddr[5]) {
	uint32_t i = 0;
	uint32_t *reg;

	char buffer[1] = { 0 };

	printf("uart-test start\n");

	reg = (uint32_t*) (METAL_SIFIVE_UART0_0_BASE_ADDRESS
			+ METAL_SIFIVE_UART0_TXCTRL);
	printf("reg txctrl: %x, %p\n", *reg, reg);
	reg = (uint32_t*) (METAL_SIFIVE_UART0_0_BASE_ADDRESS
			+ METAL_SIFIVE_UART0_RXCTRL);
	printf("reg rxctrl: %x, %p\n", *reg, reg);

	int uqsCount = 0;
	int uqsFlag = 0;
	int pickKey = 0;
	uint8_t encrypt[16] = { 0 };
	uint8_t key[16] = { 0 };
	uint16_t uqsAddr[5] = { 0xA0, 0xC8, 0xF0, 0x118, 0x140 };

	while (uqsCount < 5) {
		for (i = 0; i < sizeof(buffer); i++) {
			volatile uint32_t *reg =
					(volatile uint32_t*) (METAL_SIFIVE_UART0_0_BASE_ADDRESS
							+ METAL_SIFIVE_UART0_RXDATA);
			uint32_t regval = *reg;
			if (!(regval & (1 << 31))) {
				buffer[0] = (char) ((regval) & 0xff);
				if (isalpha(buffer[i])) {
					uqsFlag = 1;
					encrypt[0] = buffer[0];
					break;
				}
				break;
			} else {
				buffer[i] = 0x00;
				break;
			}
		}

		if (uqsFlag == 1) {

			read_bytes(keyAddr[uqsCount], NULL, &key, 16);
			encryptKey(key, encrypt);
			write_bytes(uqsAddr[uqsCount], encrypt, NULL, 16);
			uqsCount += 1;
			uqsFlag = 0;
			printf("ret: %c (%x)\n", buffer[0], buffer[0]);
		}
	}
}

void DRIVE_BUTTON(uint16_t uqsAddr[5], uint16_t keyAddr[5]) {
	uint8_t decrypt[16] = { 0 };

	metal_gpio_enable_output(gpio0, 2);
	metal_gpio_set_pin(gpio0, 2, 1);
	delay(5000000);
	metal_gpio_toggle_pin(gpio0, 2);

	uint8_t key[16] = { 0 };

	while (1) {
		input(BUT0, gpio0);
		int driveBUT = metal_gpio_get_input_pin(gpio0, BUT0);

		if (driveBUT) {
			for (int i = 0; i < 5; i++) {
				read_bytes(uqsAddr[i], NULL, &decrypt, 16);
				read_bytes(keyAddr[i], NULL, &key, 16);
				decryptKey(decrypt, key);
				printf("%s %c\n", "Decrypted value : ", decrypt[0]);

				switch (decrypt[0]) {
				case 0x61:
					metal_gpio_enable_output(gpio0, 10);
					metal_gpio_set_pin(gpio0, 10, 1);
					delay(10000000);
					metal_gpio_toggle_pin(gpio0, 10);
					break;

				case 0x62:
					metal_gpio_enable_output(gpio0, 11);
					metal_gpio_set_pin(gpio0, 11, 1);
					delay(10000000);
					metal_gpio_toggle_pin(gpio0, 11);
					break;

				case 0x63:
					metal_gpio_enable_output(gpio0, 12);
					metal_gpio_set_pin(gpio0, 12, 1);
					delay(10000000);
					metal_gpio_toggle_pin(gpio0, 12);
					break;

				case 0x64:
					metal_gpio_enable_output(gpio0, 13);
					metal_gpio_set_pin(gpio0, 13, 1);
					delay(10000000);
					metal_gpio_toggle_pin(gpio0, 13);
					break;

				case 0x65:
					metal_gpio_enable_output(gpio0, 2);
					metal_gpio_set_pin(gpio0, 2, 1);
					delay(10000000);
					metal_gpio_toggle_pin(gpio0, 2);
					break;
				}
			}
			exit(0);

		}
	}
}

int main() {

	spi_init(800000);
	gpio0 = metal_gpio_get_device(0);
	int err;
	err = mb85rs64v_read_id();
	srand(time(0));

	//RESTRICT ATTEMPTS
	uint8_t ATTEMPTS_COUNT[5] = { 0 };
	uint8_t ATTEMPTS_LIMIT = { 0x03 };
	write_bytes(0x28, 0x00, NULL, 1);
	read_bytes(0x28, NULL, ATTEMPTS_COUNT, 1);
	if (ATTEMPTS_COUNT[0] > ATTEMPTS_LIMIT) {
		metal_gpio_enable_output(gpio0, 1);
		metal_gpio_toggle_pin(gpio0, 1);
	}

	//AES KEY GEN MODE
	uint16_t keyAddr[5] = { 0x3C, 0x50, 0x64, 0x78, 0x8C };
	AES_KEY_GEN(keyAddr);

	//BUTTON INPUT MODE
	printf("%s\n", "Awaiting 4 Button sequence...\n");
	if (BUTTON_INPUT(ATTEMPTS_COUNT) == -1) {
		exit(0);
	}

	metal_gpio_disable_output(gpio0, 3);

	uint8_t key[16] = { 0 };
	uint16_t uqsAddr[5] = { 0xA0, 0xC8, 0xF0, 0x118, 0x140 };

	//UART INPUT MODE
	UART_INPUT(keyAddr);

	fflush(stdout);
	printf("%s\n", "Entering Final Drive...\n");

	//DRIVE_BUTTON MODE
	DRIVE_BUTTON(uqsAddr, keyAddr);
	return 0;
}
