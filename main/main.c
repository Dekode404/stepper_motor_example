/*H**********************************************************************
 * FILENAME :        main.c             DESIGN REF: NA
 *
 * DESCRIPTION :
 *       main.c file for the stepper motor example.
 *
 * NOTES :
 *       These functions are a part of the FM suite;
 *       See IMS FM0121 for detailed description.
 *
 *       Copyright: All rights reserved.
 *
 * AUTHOR     :  Saurabh kadam.
 * START DATE :  08 Jun 2024
 *H*/

#include "main.h"

esp_err_t Initialize_GPIO_for_Stepper_Motor_Driver(void)
{
    gpio_config_t io_conf = {};                 // zero-initialize the config structure.
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;  // disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT;            // set as output mode
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pull_down_en = GPIO_PULLUP_DISABLE; // disable pull-down mode
    io_conf.pull_up_en = GPIO_PULLDOWN_DISABLE; // disable pull-up mode

    return (gpio_config(&io_conf)); // configure GPIO with the given settings
}

esp_err_t Initialize_PWM_for_Stepper_Motor_Driver(void)
{
    ledc_timer_config_t PWM_timer = {};            // zero-initialize the config structure for the timer used for the PWM.
    PWM_timer.speed_mode = LEDC_LOW_SPEED_MODE;    // Driver in low speed mode
    PWM_timer.duty_resolution = LEDC_TIMER_10_BIT; // Set the resolution of the timer to 10BIT
    PWM_timer.timer_num = LEDC_TIMER_0;            // Used timer zero
    PWM_timer.freq_hz = PWM_FREQUENCY_1KHZ;        // Set frequency to 1kHZ
    PWM_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&PWM_timer);

    ledc_channel_config_t PWM_channel_for_stepper_motor = {};
    PWM_channel_for_stepper_motor.gpio_num = STEPPER_MOTOR_PUL_PIN;
    PWM_channel_for_stepper_motor.speed_mode = LEDC_LOW_SPEED_MODE;
    PWM_channel_for_stepper_motor.channel = LEDC_CHANNEL_0;
    PWM_channel_for_stepper_motor.timer_sel = LEDC_TIMER_0;
    PWM_channel_for_stepper_motor.duty = 0;
    PWM_channel_for_stepper_motor.hpoint = 0;
    ledc_channel_config(&PWM_channel_for_stepper_motor);

    return (ledc_fade_func_install(0));
}

void app_main(void)
{

    Initialize_GPIO_for_Stepper_Motor_Driver();
    Initialize_PWM_for_Stepper_Motor_Driver();

    gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_HIGH);
    gpio_set_level(STEPPER_MOTOR_DIR_PIN, SET_GPIO_LEVEL_HIGH);
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, PWM_DUTY_CYCLE_50, 0);

    while (true)
    {
        gpio_set_level(STEPPER_MOTOR_DIR_PIN, SET_GPIO_LEVEL_HIGH);
        gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_LOW);
        vTaskDelay(1600 / portTICK_PERIOD_MS);
        gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        gpio_set_level(STEPPER_MOTOR_DIR_PIN, SET_GPIO_LEVEL_LOW);
        gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_LOW);
        vTaskDelay(1600 / portTICK_PERIOD_MS);
        gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
