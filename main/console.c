#include <console.h>
#include "main.h"

esp_err_t Rotate_Motor(int argc, char **argv)
{
    esp_err_t Function_Error = ESP_OK;

    int nerrors = arg_parse(argc, argv, (void **)&Rotate_motor_args); // Parse command line arguments

    if (nerrors != 0)
    {
        arg_print_errors(stderr, Rotate_motor_args.end, argv[0]); // Print errors if argument parsing fails
        return 1;
    }

    printf("FREQUENCY : '%d'\n", Rotate_motor_args.Frequency->ival[0]); // Print Frequency
    printf("DIRECTION : '%d'\n", Rotate_motor_args.Direction->ival[0]); // Print Direction
    printf("STEPS     : '%d'\n", Rotate_motor_args.Steps->ival[0]);     // Print Steps
    printf("Rotation  : '%d'\n", Rotate_motor_args.Rotation->ival[0]);  // Print Rotation

    return Function_Error;
}

/**
 * @brief Start the stepper motor based on command-line arguments.
 *
 * This function parses the command-line arguments for the start_motor command,
 * including frequency, direction, steps, and RPM. It then initializes the motor
 * based on these arguments, setting the appropriate GPIO levels, PWM frequency,
 * and duty cycle. The function also prints the parsed arguments for debugging.
 *
 * @return 0 if successful, 1 if argument parsing fails.
 */
esp_err_t Quick_Start_Motor(void)
{
    printf("FREQUENCY : 1Khz\n");    // Print Frequency
    printf("DIRECTION : FORWARD\n"); // Print Direction
    printf("DUTY CYCLE: 50\n");      // Print Duty cycle

    return Start_Stepper_Motor(MOTOR_DIRECTION_FORWARD, PWM_FREQUENCY_1KHZ, PWM_DUTY_CYCLE_50);
}

/**
 * @brief Start the stepper motor based on command-line arguments.
 *
 * This function parses the command-line arguments for the start_motor command,
 * including frequency, direction, steps, and RPM. It then initializes the motor
 * based on these arguments, setting the appropriate GPIO levels, PWM frequency,
 * and duty cycle. The function also prints the parsed arguments for debugging.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return 0 if successful, 1 if argument parsing fails.
 */
esp_err_t Start_Motor(int argc, char **argv)
{
    esp_err_t Function_Error = ESP_OK;

    int nerrors = arg_parse(argc, argv, (void **)&Start_motor_args); // Parse command line arguments

    if (nerrors != 0)
    {
        arg_print_errors(stderr, Start_motor_args.end, argv[0]); // Print errors if argument parsing fails
        return 1;
    }

    printf("FREQUENCY : '%d'\n", Start_motor_args.Frequency->ival[0]);  // Print Frequency
    printf("DIRECTION : '%d'\n", Start_motor_args.Direction->ival[0]);  // Print Direction
    printf("DUTY CYCLE: '%d'\n", Start_motor_args.Duty_cycle->ival[0]); // Print Steps

    Function_Error = Start_Stepper_Motor(Start_motor_args.Direction->ival[0], Start_motor_args.Frequency->ival[0], Start_motor_args.Duty_cycle->ival[0]);

    return Function_Error;
}

/**
 * @brief Register the start_motor command with the console
 *
 * This function defines the command-line arguments for the start_motor command,
 * creates a console command structure, and registers the command with the console.
 *
 * @return
 *     - ESP_OK: Successfully registered the command
 *     - ESP_ERR_NO_MEM: Insufficient memory to allocate command structure
 *     - ESP_ERR_INVALID_ARG: Invalid argument provided
 *     - ESP_ERR_INVALID_STATE: Console not initialized or other invalid state
 */
esp_err_t Register_Start_Motor_CMD(void)
{
    Start_motor_args.Frequency = arg_int0(NULL, "frq", "<t>", "Frequency of the PWM");           // Define timeout argument
    Start_motor_args.Direction = arg_int0(NULL, "dir", "<t>", "Direction of the stepper motor"); // Define SSID argument
    Start_motor_args.Duty_cycle = arg_int0(NULL, "duty", "<t>", "Duty cycle of the PWM");        // Define password argument
    Start_motor_args.end = arg_end(2);                                                           // Define end marker for argument table

    const esp_console_cmd_t join_cmd = {
        .command = "start_motor",                      // Command name
        .help = "Start moving the motor continuously", // Command description
        .hint = NULL,                                  // Command hint (optional)
        .func = &Start_Motor,                          // Command handler function
        .argtable = &Start_motor_args                  // Argument table
    };

    return esp_console_cmd_register(&join_cmd); // Register the 'join' command with the console
}

esp_err_t Register_Stop_Motor_CMD(void)
{
    const esp_console_cmd_t join_cmd = {
        .command = "stop_motor",                                 // Command name
        .help = "Stop motor by disabling and also stop the PWM", // Command description
        .hint = NULL,                                            // Command hint (optional)
        .func = &Stop_Stepper_Motor,                             // Command handler function
    };

    return esp_console_cmd_register(&join_cmd); // Register the 'join' command with the console
}

esp_err_t Register_Quick_Start_Motor_CMD(void)
{
    const esp_console_cmd_t join_cmd = {
        .command = "quick_motor_start",            // Command name
        .help = "start_motor_with_default_values", // Command description
        .hint = NULL,                              // Command hint (optional)
        .func = &Quick_Start_Motor,                // Command handler function
    };

    return esp_console_cmd_register(&join_cmd); // Register the 'join' command with the console
}

esp_err_t Register_Rotate_Motor_CMD(void)
{
    Rotate_motor_args.Frequency = arg_int0(NULL, "frq", "<t>", "Frequency of the PWM signal");        // Define frequency of the PWM
    Rotate_motor_args.Direction = arg_int0(NULL, "dir", "<t>", "Direction of the stepper motor");     // Define direction of the motor
    Rotate_motor_args.Steps = arg_int0(NULL, "step", "<t>", "Micro steps of the motor driver");       // Define steps of the motor
    Rotate_motor_args.Rotation = arg_int0(NULL, "rotation", "<t>", "Number of rotation to be taken"); // Define rotation of the motor
    Rotate_motor_args.end = arg_end(2);                                                               // Define end marker for argument table

    const esp_console_cmd_t join_cmd = {
        .command = "rotate_motor",                 // Command name
        .help = "Moving the motor in fixed angle", // Command description
        .hint = NULL,                              // Command hint (optional)
        .func = &Rotate_Motor,                     // Command handler function
        .argtable = &Rotate_motor_args             // Argument table
    };

    return esp_console_cmd_register(&join_cmd); // Register the 'join' command with the console
}

/**
 * @brief Initialize the console for UART communication and command-line interface.
 *
 * This function configures the UART for console communication, sets up line endings,
 * configures the UART parameters, installs the UART driver, initializes the console,
 * configures the linenoise library for command completion and history, and sets other
 * necessary configurations for the console operation.
 */
void initialize_console(void)
{
    /* Drain stdout before reconfiguring it */
    fflush(stdout);        // Flush stdout to ensure all output is written
    fsync(fileno(stdout)); // Synchronize the file descriptor for stdout

    /* Disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0); // Disable buffering for stdin

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_uart_port_set_rx_line_endings(CONSOLE_UART_0, ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_port_set_tx_line_endings(CONSOLE_UART_0, ESP_LINE_ENDINGS_CRLF);

    /* Configure UART. Note that REF_TICK is used so that the baud rate remains
     * correct while APB frequency is changing in light sleep mode.
     */
    const uart_config_t uart_config = {
        .baud_rate = CONSOLE_UART_BAUDRATE_115200, // Set baud rate
        .data_bits = UART_DATA_8_BITS,             // Set data bits to 8
        .parity = UART_PARITY_DISABLE,             // Disable parity check
        .stop_bits = UART_STOP_BITS_1,             // Set stop bits to 1
        .source_clk = UART_SCLK_REF_TICK,          // Use reference clock
    };
    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK(uart_driver_install(CONSOLE_UART_0,
                                        256, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(CONSOLE_UART_0, &uart_config)); // Configure UART parameters

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(CONSOLE_UART_0); // Use UART driver for VFS

    /* Initialize the console */
    esp_console_config_t console_config = {
        .max_cmdline_args = 8,     // Maximum number of command line arguments
        .max_cmdline_length = 256, // Maximum command line length
    };
    ESP_ERROR_CHECK(esp_console_init(&console_config)); // Initialize the console with the specified configuration

    /* Configure linenoise line completion library */
    linenoiseSetMultiLine(1); // Enable multiline editing

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);                // Set completion callback
    linenoiseSetHintsCallback((linenoiseHintsCallback *)&esp_console_get_hint); // Set hints callback

    /* Set command history size */
    linenoiseHistorySetMaxLen(100); // Set the maximum length of command history

    /* Don't return empty lines */
    linenoiseAllowEmpty(false); // Disallow empty lines
}