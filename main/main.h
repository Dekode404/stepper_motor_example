/*H**********************************************************************
 * FILENAME :        main.h             DESIGN REF: NA
 *
 * DESCRIPTION :
 *       main.h file for the stepper motor example.
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

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define SET_GPIO_LEVEL_HIGH 0x01
#define SET_GPIO_LEVEL_LOW 0x00

#define STEPPER_MOTOR_EN_PIN GPIO_NUM_15
#define STEPPER_MOTOR_DIR_PIN GPIO_NUM_2
#define STEPPER_MOTOR_PUL_PIN GPIO_NUM_0

#define GPIO_OUTPUT_PIN_SEL ((1ULL << STEPPER_MOTOR_EN_PIN) | (1ULL << STEPPER_MOTOR_DIR_PIN) | (1ULL << STEPPER_MOTOR_PUL_PIN))

#endif // HEADER_NAME_H