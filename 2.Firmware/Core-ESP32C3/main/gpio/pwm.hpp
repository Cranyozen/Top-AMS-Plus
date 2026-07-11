#pragma once

#include "hal/ledc_types.h"
#include "soc/gpio_num.h"
#include <stdint.h>
#define PWM_TIMER                LEDC_TIMER_0
#define PWM_SPEED_MODE           LEDC_LOW_SPEED_MODE // Must be LEDC_LOW_SPEED_MODE for ESP32-C3
#define PWM_DUTY_RES             LEDC_TIMER_13_BIT   // 13-bit resolution
#define PWM_FREQ_HZ              5000                // 5 kHz frequency
#define PWM_CLK_CFG              LEDC_AUTO_CLK
#define PWM_DEFAULT_DUTY_PERCENT 0
#define PWM_DEFAULT_HPOINT       0

typedef enum
{
    PWM_CHANNEL_0 = LEDC_CHANNEL_0,
    PWM_CHANNEL_1 = LEDC_CHANNEL_1,
    PWM_CHANNEL_2 = LEDC_CHANNEL_2,
    PWM_CHANNEL_3 = LEDC_CHANNEL_3,
    PWM_CHANNEL_4 = LEDC_CHANNEL_4,
    PWM_CHANNEL_5 = LEDC_CHANNEL_5,
} pwm_channel_t;

typedef struct
{
    gpio_num_t gpio_num;
    pwm_channel_t channel;
    uint16_t duty_percent; // Duty cycle percentage (0-10000 for 0.00% to 100.00%)
} pwm_config_t;

void PWM_init(pwm_config_t *config, pwm_channel_t channel, gpio_num_t gpio_num, uint16_t duty_percent);
void PWM_init(pwm_config_t *config, pwm_channel_t channel, gpio_num_t gpio_num);
void PWM_set_duty(pwm_config_t *config, uint16_t duty_percent);
void PWM_stop(pwm_config_t *config);
void PWM_start(pwm_config_t *config);

void PWM_timer_pause();
void PWM_timer_resume();
