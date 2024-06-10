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

void app_main(void)
{
    printf("Hello world \n");

    // zero-initialize the config structure.
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;  // disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT;            // set as output mode
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pull_down_en = GPIO_PULLUP_DISABLE; // disable pull-down mode
    io_conf.pull_up_en = GPIO_PULLDOWN_DISABLE; // disable pull-up mode

    gpio_config(&io_conf); // configure GPIO with the given settings

    ledc_timer_config_t timer = {};
    timer.speed_mode = LEDC_LOW_SPEED_MODE;
    timer.duty_resolution = LEDC_TIMER_10_BIT;
    timer.timer_num = LEDC_TIMER_0;
    timer.freq_hz = 5000;
    timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {};
    channel.gpio_num = GPIO_NUM_2;
    channel.speed_mode = LEDC_LOW_SPEED_MODE;
    channel.channel = LEDC_CHANNEL_0;
    channel.timer_sel = LEDC_TIMER_0;
    channel.duty = 0;
    channel.hpoint = 0;
    ledc_channel_config(&channel);

    ledc_fade_func_install(0);

    while (1)
    {

        for (int i = 0; i < 1024; i++)
        {
            ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i, 0);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_HIGH);
        gpio_set_level(STEPPER_MOTOR_DIR_PIN, SET_GPIO_LEVEL_HIGH);
        gpio_set_level(STEPPER_MOTOR_PUL_PIN, SET_GPIO_LEVEL_HIGH);

        vTaskDelay(1000 / portTICK_RATE_MS);

        gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_LOW);
        gpio_set_level(STEPPER_MOTOR_DIR_PIN, SET_GPIO_LEVEL_LOW);
        gpio_set_level(STEPPER_MOTOR_PUL_PIN, SET_GPIO_LEVEL_LOW);

        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
