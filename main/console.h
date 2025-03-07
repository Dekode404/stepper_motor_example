/*H**********************************************************************
 * FILENAME :        console.h             DESIGN REF: NA
 *
 * DESCRIPTION :
 *       console.h file for the stepper motor example.
 *
 * NOTES :
 *       These functions are a part of the FM suite;
 *       See IMS FM0121 for detailed description.
 *
 *       Copyright: All rights reserved.
 *
 * AUTHOR     :  Saurabh kadam.
 * START DATE :  13 Jun 2024
 *H*/

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "esp_vfs_fat.h"
#include "nvs.h"
#include "nvs_flash.h"

#define PROMPT_STR CONFIG_IDF_TARGET // Prompt string for the console
#define CONSOLE_UART_0 0x00
#define CONSOLE_UART_BAUDRATE_115200 115200

void initialize_console(void);

esp_err_t Register_Start_Motor_CMD(void);
esp_err_t Register_Stop_Motor_CMD(void);
esp_err_t Register_Quick_Start_Motor_CMD(void);
esp_err_t Register_Rotate_Motor_CMD(void);
esp_err_t Register_Rotate_Angle_CMD(void);

esp_err_t Start_Motor(int argc, char **argv);
esp_err_t Quick_Start_Motor(void);
esp_err_t Rotate_Motor(int argc, char **argv);
esp_err_t Rotate_Angle(int argc, char **argv);

/** Arguments used for the stepper motor to run */
struct
{
    struct arg_int *Frequency;  // Argument for Stepper motor PWM frequency
    struct arg_int *Direction;  // Argument for Stepper motor direction
    struct arg_int *Duty_cycle; // Argument for Stepper motor PWM Duty cycle
    struct arg_end *end;        // End marker for argument table
} Start_motor_args;             // Structure to hold the arguments for the stepper_motor command

struct
{
    struct arg_int *Frequency; // Argument for Stepper motor PWM frequency
    struct arg_int *Direction; // Argument for Stepper motor direction
    struct arg_int *Steps;     // Argument for the number of steps per rotation
    struct arg_int *Rotation;  // Argument for the number of rotations to rotate the motor
    struct arg_end *end;       // End marker for argument table
} Rotate_motor_args;           // Structure to hold the arguments for the stepper_motor command

struct
{
    struct arg_int *Frequency; // Argument for Stepper motor PWM frequency
    struct arg_int *Direction; // Argument for Stepper motor direction
    struct arg_int *Steps;     // Argument for the number of steps per rotation
    struct arg_dbl *Angle;     // Argument for the angle to rotate the motor
    struct arg_end *end;       // End marker for argument table
} Rotate_angle_args;           // Structure to hold the arguments for the rotate_motor_angle command

#endif // CONSOLE_H