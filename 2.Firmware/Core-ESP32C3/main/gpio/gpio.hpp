#pragma once

#include "driver/gpio.h"

void GPIO_init(gpio_num_t gpio_num, gpio_mode_t mode, gpio_pull_mode_t pull_mode, gpio_int_type_t intr_type);
void GPIO_init(gpio_num_t gpio_num, gpio_mode_t mode, gpio_pull_mode_t pull_mode);
void GPIO_init(uint64_t pin_bit_mask, gpio_mode_t mode, gpio_pull_mode_t pull_mode, gpio_int_type_t intr_type);
void GPIO_init(uint64_t pin_bit_mask, gpio_mode_t mode, gpio_pull_mode_t pull_mode);
void GPIO_set_level(gpio_num_t gpio_num, uint32_t level);
void GPIO_reset_pin(gpio_num_t gpio_num);
