#include "gpio.hpp"

void GPIO_init(gpio_num_t gpio_num, gpio_mode_t mode, gpio_pull_mode_t pull_mode, gpio_int_type_t intr_type)
{
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << gpio_num);
    io_conf.mode = mode;
    io_conf.pull_up_en = (pull_mode == GPIO_PULLUP_ONLY || pull_mode == GPIO_PULLUP_PULLDOWN) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = (pull_mode == GPIO_PULLDOWN_ONLY || pull_mode == GPIO_PULLUP_PULLDOWN) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = intr_type;
    gpio_config(&io_conf);
}

void GPIO_init(gpio_num_t gpio_num, gpio_mode_t mode, gpio_pull_mode_t pull_mode)
{
    GPIO_init(gpio_num, mode, pull_mode, GPIO_INTR_DISABLE);
}

void GPIO_init(uint64_t pin_bit_mask, gpio_mode_t mode, gpio_pull_mode_t pull_mode, gpio_int_type_t intr_type)
{
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = pin_bit_mask;
    io_conf.mode = mode;
    io_conf.pull_up_en = (pull_mode == GPIO_PULLUP_ONLY || pull_mode == GPIO_PULLUP_PULLDOWN) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = (pull_mode == GPIO_PULLDOWN_ONLY || pull_mode == GPIO_PULLUP_PULLDOWN) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = intr_type;
    gpio_config(&io_conf);
}

void GPIO_init(uint64_t pin_bit_mask, gpio_mode_t mode, gpio_pull_mode_t pull_mode)
{
    GPIO_init(pin_bit_mask, mode, pull_mode, GPIO_INTR_DISABLE);
}

void GPIO_set_level(gpio_num_t gpio_num, uint32_t level)
{
    gpio_set_level(gpio_num, level);
}

void GPIO_reset_pin(gpio_num_t gpio_num)
{
    gpio_reset_pin(gpio_num);
}
