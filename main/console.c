#include <console.h>
#include "main.h"

int start_motor(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&Stepper_motor_args); // Parse command line arguments

    if (nerrors != 0)
    {
        arg_print_errors(stderr, Stepper_motor_args.end, argv[0]); // Print errors if argument parsing fails
        return 1;
    }

    printf("FREQUENCY: '%d'\n", Stepper_motor_args.Frequency->ival[0]); // Print Frequency
    printf("DIRECTION: '%d'\n", Stepper_motor_args.Direction->ival[0]); // Print Direction
    printf("STEPS    : '%d'\n", Stepper_motor_args.Steps->ival[0]);     // Print Steps
    printf("RPM      : '%d'\n", Stepper_motor_args.RPM->ival[0]);       // Print RPM

    gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_LOW);

    if (Stepper_motor_args.Direction->ival[0] == 1)
    {
        gpio_set_level(STEPPER_MOTOR_DIR_PIN, SET_GPIO_LEVEL_HIGH);
    }
    else
    {
        gpio_set_level(STEPPER_MOTOR_DIR_PIN, SET_GPIO_LEVEL_LOW);
    }

    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, Stepper_motor_args.Frequency->ival[0], 0);

    return 0;
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
esp_err_t register_start_motor_cmd(void)
{
    Stepper_motor_args.Frequency = arg_int0(NULL, "F", "<t>", "Frequency of the PWM");           // Define timeout argument
    Stepper_motor_args.Direction = arg_int0(NULL, "D", "<t>", "Direction of the stepper motor"); // Define SSID argument
    Stepper_motor_args.Steps = arg_int0(NULL, "S", "<t>", "Steps of the motor");                 // Define password argument
    Stepper_motor_args.RPM = arg_int0(NULL, "R", "<t>", "RPM of the motor");                     // Define password argument
    Stepper_motor_args.end = arg_end(2);                                                         // Define end marker for argument table

    const esp_console_cmd_t join_cmd = {
        .command = "start_motor",                      // Command name
        .help = "Start moving the motor continuously", // Command description
        .hint = NULL,                                  // Command hint (optional)
        .func = &start_motor,                          // Command handler function
        .argtable = &Stepper_motor_args                // Argument table
    };

    return esp_console_cmd_register(&join_cmd); // Register the 'join' command with the console
}

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