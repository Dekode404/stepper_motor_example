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
void register_start_motor_cmd(void);
int start_motor(int argc, char **argv);

/** Arguments used for the stepper motor to run */
struct
{
    struct arg_int *Frequency; // Argument for Stepper motor frequency
    struct arg_int *Direction; // Argument for Stepper motor direction
    struct arg_int *Steps;     // Argument for Stepper motor Steps
    struct arg_int *RPM;       // Argument for Stepper motor RPM
    struct arg_end *end;       // End marker for argument table
} Stepper_motor_args;          // Structure to hold the arguments for the stepper_motor command

#endif // CONSOLE_H