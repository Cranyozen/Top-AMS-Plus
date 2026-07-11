#include "pwm_motor.hpp"

void PWM_Motor_init(pwm_motor_config_t *config, gpio_num_t gpio_num1, gpio_num_t gpio_num2, pwm_channel_t channel1, pwm_channel_t channel2,
    uint16_t speed_percent, uint16_t min_pwm_duty, bool is_reverse_polarity, bool is_reverse, bool is_enabled)
{
    config->gpio_num1 = gpio_num1;
    config->gpio_num2 = gpio_num2;
    config->channel1 = channel1;
    config->channel2 = channel2;
    config->speed_percent = speed_percent;
    config->min_pwm_duty = min_pwm_duty;
    config->is_reverse_polarity = is_reverse_polarity;
    config->is_reverse = is_reverse;
    config->_is_enabled = is_enabled;

    PWM_init(&config->_pwm_config1, (pwm_channel_t)channel1, (gpio_num_t)gpio_num1, speed_percent);
    PWM_init(&config->_pwm_config2, (pwm_channel_t)channel2, (gpio_num_t)gpio_num2, 0);
}

void PWM_Motor_init(pwm_motor_config_t *config, gpio_num_t gpio_num1, gpio_num_t gpio_num2, pwm_channel_t channel1, pwm_channel_t channel2)
{
    PWM_Motor_init(config, gpio_num1, gpio_num2, channel1, channel2, PWM_MOTOR_DEFAULT_SPEED_PERCENT, PWM_MOTOR_DEFAULT_MIN_PWM_DUTY,
        PWM_MOTOR_DEFAULT_REVERSE_POLARITY, PWM_MOTOR_DEFAULT_REVERSE, PWM_MOTOR_DEFAULT_ENABLED);
}

void _PWM_Motor_update(pwm_motor_config_t *config)
{
    if (!config->_is_enabled) {
        PWM_set_duty(&config->_pwm_config1, 0);
        PWM_set_duty(&config->_pwm_config2, 0);
        PWM_stop(&config->_pwm_config1);
        PWM_stop(&config->_pwm_config2);
        return;
    }
    uint16_t effective_speed
        = (config->speed_percent / (float)PWM_MOTOR_MAX_DUTY) * (PWM_MOTOR_MAX_DUTY - config->min_pwm_duty) + config->min_pwm_duty;
    if (config->is_reverse ^ config->is_reverse_polarity) {
        /* ^: XOR operator:
        a, b, a ^ b
        0, 0, 0
        0, 1, 1
        1, 0, 1
        1, 1, 0
        */
        PWM_set_duty(&config->_pwm_config1, 0);
        PWM_set_duty(&config->_pwm_config2, effective_speed);
    } else {
        PWM_set_duty(&config->_pwm_config1, effective_speed);
        PWM_set_duty(&config->_pwm_config2, 0);
    }
    PWM_start(&config->_pwm_config1);
    PWM_start(&config->_pwm_config2);
}

void PWM_Motor_set_speed(pwm_motor_config_t *config, uint16_t speed_percent)
{
    config->speed_percent = speed_percent;
    _PWM_Motor_update(config);
}

void PWM_Motor_start(pwm_motor_config_t *config)
{
    config->_is_enabled = true;
    _PWM_Motor_update(config);
}

void PWM_Motor_stop(pwm_motor_config_t *config)
{
    config->_is_enabled = false;
    _PWM_Motor_update(config);
}

void PWM_Motor_reverse(pwm_motor_config_t *config)
{
    config->is_reverse = !config->is_reverse;
    _PWM_Motor_update(config);
}

void PWM_Motor_set_reverse_polarity(pwm_motor_config_t *config, bool is_reverse_polarity)
{
    config->is_reverse_polarity = is_reverse_polarity;
    _PWM_Motor_update(config);
}

void PWM_Motor_set_min_pwm_duty(pwm_motor_config_t *config, uint16_t min_pwm_duty)
{
    config->min_pwm_duty = min_pwm_duty;
    _PWM_Motor_update(config);
}
