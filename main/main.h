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
#include "driver/ledc.h"

#define SET_GPIO_LEVEL_HIGH 0x01
#define SET_GPIO_LEVEL_LOW 0x00

#define STEPPER_MOTOR_EN_PIN GPIO_NUM_0
#define STEPPER_MOTOR_DIR_PIN GPIO_NUM_4
#define STEPPER_MOTOR_PUL_PIN GPIO_NUM_2

#define GPIO_OUTPUT_PIN_SEL ((1ULL << STEPPER_MOTOR_EN_PIN) | (1ULL << STEPPER_MOTOR_DIR_PIN))

#define PWM_FREQUENCY_1KHZ 1000
#define PWM_DUTY_CYCLE_50 512
#define PWM_DUTY_CYCLE_00 00

esp_err_t Initialize_GPIO_for_Stepper_Motor_Driver(void);
esp_err_t Initialize_PWM_for_Stepper_Motor_Driver(void);

#endif // HEADER_NAME_H