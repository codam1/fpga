/*
 * helper.h
 *
 *  Created on: Apr 24, 2021
 *      Author: msu
 */

#ifndef HELPER_H_
#define HELPER_H_

#include <stdint.h>
#include <stdio.h>
#include <metal/gpio.h>
#include "aes.h"

void input(int BUT, struct metal_gpio *gpio);
void output(int LED, struct metal_gpio *gpio);
void toggle(int LED, struct metal_gpio *gpio);
void createKey(uint8_t *create);
uint8_t encryptKey(uint8_t *key, uint8_t *butVal);
uint8_t decryptKey(uint8_t *encryptValue, uint8_t *key);

#endif /* HELPER_H_ */
