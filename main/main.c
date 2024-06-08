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
    io_conf.pull_down_en = 0;                   // disable pull-down mode
    io_conf.pull_up_en = 0;                     // disable pull-up mode

    gpio_config(&io_conf); // configure GPIO with the given settings

    while (1)
    {

        gpio_set_level(GPIO_OUTPUT_PIN_SEL, SET_GPIO_LEVEL_HIGH);

        vTaskDelay(1000 / portTICK_RATE_MS);

        gpio_set_level(GPIO_OUTPUT_PIN_SEL, SET_GPIO_LEVEL_HIGH);
    }
}
