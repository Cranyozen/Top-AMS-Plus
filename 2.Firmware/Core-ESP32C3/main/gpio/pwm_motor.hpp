#pragma once

#include <stdint.h>

#include "pwm.hpp"

#define PWM_MOTOR_MAX_DUTY 10000

#define PWM_MOTOR_DEFAULT_SPEED_PERCENT    0
#define PWM_MOTOR_DEFAULT_REVERSE_POLARITY false
#define PWM_MOTOR_DEFAULT_REVERSE          false
#define PWM_MOTOR_DEFAULT_ENABLED          false
#define PWM_MOTOR_DEFAULT_MIN_PWM_DUTY     6000

typedef struct
{
    int gpio_num1;
    int gpio_num2;
    pwm_channel_t channel1;
    pwm_channel_t channel2;
    bool is_reverse_polarity; // If true, the motor will run in reverse direction
    bool is_reverse;
    uint16_t speed_percent; // Speed percentage (0-10000 for 0.00% to 100.00%)
    uint16_t min_pwm_duty;

    pwm_config_t _pwm_config1;
    pwm_config_t _pwm_config2;
    bool _is_enabled;

} pwm_motor_config_t;

void PWM_Motor_init(pwm_motor_config_t *config, gpio_num_t gpio_num1, gpio_num_t gpio_num2, pwm_channel_t channel1, pwm_channel_t channel2,
    uint16_t speed_percent, uint16_t min_pwm_duty, bool is_reverse_polarity, bool is_reverse, bool is_enabled);
void PWM_Motor_init(pwm_motor_config_t *config, gpio_num_t gpio_num1, gpio_num_t gpio_num2, pwm_channel_t channel1, pwm_channel_t channel2);
void PWM_Motor_set_speed(pwm_motor_config_t *config, uint16_t speed_percent);
void PWM_Motor_start(pwm_motor_config_t *config);
void PWM_Motor_stop(pwm_motor_config_t *config);
void PWM_Motor_reverse(pwm_motor_config_t *config);
void PWM_Motor_set_reverse_polarity(pwm_motor_config_t *config, bool is_reverse_polarity);
void PWM_Motor_set_min_pwm_duty(pwm_motor_config_t *config, uint16_t min_pwm_duty);
