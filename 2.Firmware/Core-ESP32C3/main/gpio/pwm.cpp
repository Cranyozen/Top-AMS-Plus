#include "driver/ledc.h"

#include "pwm.hpp"

void PWM_init(pwm_config_t *config, pwm_channel_t channel, gpio_num_t gpio_num, uint16_t duty_percent)
{
    config->channel = channel;
    config->gpio_num = gpio_num;
    config->duty_percent = duty_percent;
    ledc_timer_config_t timer_conf = {};
    timer_conf.speed_mode = PWM_SPEED_MODE;
    timer_conf.timer_num = PWM_TIMER;
    timer_conf.duty_resolution = PWM_DUTY_RES;
    timer_conf.freq_hz = PWM_FREQ_HZ;
    timer_conf.clk_cfg = PWM_CLK_CFG;
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {};
    channel_conf.speed_mode = PWM_SPEED_MODE;
    channel_conf.channel = (ledc_channel_t)config->channel;
    channel_conf.timer_sel = PWM_TIMER;
    channel_conf.intr_type = LEDC_INTR_DISABLE;
    channel_conf.gpio_num = config->gpio_num;
    channel_conf.duty = config->duty_percent * (1 << PWM_DUTY_RES) / 10000; // Convert percentage to duty value
    ledc_channel_config(&channel_conf);
}

void PWM_init(pwm_config_t *config, pwm_channel_t channel, gpio_num_t gpio_num)
{
    PWM_init(config, channel, gpio_num, PWM_DEFAULT_DUTY_PERCENT);
}

void PWM_set_duty(pwm_config_t *config, uint16_t duty_percent)
{
    config->duty_percent = duty_percent;
}

void PWM_stop(pwm_config_t *config)
{
    ledc_stop(PWM_SPEED_MODE, (ledc_channel_t)config->channel, 0);
}

void PWM_start(pwm_config_t *config)
{
    ledc_set_duty(PWM_SPEED_MODE, (ledc_channel_t)config->channel, config->duty_percent * (1 << PWM_DUTY_RES) / 10000);
    ledc_update_duty(PWM_SPEED_MODE, (ledc_channel_t)config->channel);
}

void PWM_timer_pause()
{
    ledc_timer_pause(PWM_SPEED_MODE, PWM_TIMER);
}

void PWM_timer_resume()
{
    ledc_timer_resume(PWM_SPEED_MODE, PWM_TIMER);
}
