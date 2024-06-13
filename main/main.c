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
#include "console.h"

/**
 * @brief Initialize GPIO for Stepper Motor Driver
 *
 * This function configures the GPIO pins for the stepper motor driver.
 * It sets the pins to output mode, disables interrupts, and ensures that
 * pull-up and pull-down modes are disabled.
 *
 * @return
 *     - ESP_OK: Successfully configured the GPIO pins
 *     - ESP_ERR_INVALID_ARG: Parameter error
 *     - ESP_ERR_INVALID_STATE: GPIO driver is not initialized
 *     - ESP_FAIL: Other failures
 */
esp_err_t Initialize_GPIO_for_Stepper_Motor_Driver(void)
{
    // Configuration structure for GPIO settings
    gpio_config_t io_conf = {}; // Zero-initialize the config structure

    // Set up the GPIO configuration
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;  // Disable interrupt for the GPIO pins
    io_conf.mode = GPIO_MODE_OUTPUT;            // Set the GPIO pins to output mode
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; // Bit mask of the pins to be configured
    io_conf.pull_down_en = GPIO_PULLUP_DISABLE; // Disable pull-down mode for the GPIO pins
    io_conf.pull_up_en = GPIO_PULLDOWN_DISABLE; // Disable pull-up mode for the GPIO pins

    // Configure GPIO with the given settings and return the result
    return gpio_config(&io_conf);
}

/**
 * @brief Initialize PWM for Stepper Motor Driver
 *
 * This function configures the PWM timer and channel for the stepper motor driver.
 * It sets the timer properties, including speed mode, resolution, timer number,
 * and frequency. It also configures the PWM channel, assigning the GPIO pin,
 * speed mode, channel number, timer selection, duty cycle, and high point.
 * Additionally, it installs the fade function for smooth PWM transitions.
 *
 * @return
 *     - ESP_OK: Successfully installed the fade function
 *     - ESP_ERR_NO_MEM: Insufficient memory for fade function
 *     - ESP_ERR_INVALID_STATE: LEDC driver not initialized or other invalid state
 */
esp_err_t Initialize_PWM_for_Stepper_Motor_Driver(void)
{
    // Configuration structure for the PWM timer
    ledc_timer_config_t PWM_timer = {}; // Zero-initialize the config structure for the timer used for the PWM

    // Set up the PWM timer configuration
    PWM_timer.speed_mode = LEDC_LOW_SPEED_MODE;    // Use the low-speed mode for the PWM driver
    PWM_timer.duty_resolution = LEDC_TIMER_10_BIT; // Set the resolution of the timer to 10 bits
    PWM_timer.timer_num = LEDC_TIMER_0;            // Use timer 0
    PWM_timer.freq_hz = PWM_FREQUENCY_1KHZ;        // Set the PWM frequency to 1 kHz
    PWM_timer.clk_cfg = LEDC_AUTO_CLK;             // Auto-select the clock source

    // Configure the PWM timer with the specified settings
    ledc_timer_config(&PWM_timer);

    // Configuration structure for the PWM channel
    ledc_channel_config_t PWM_channel_for_stepper_motor = {}; // Zero-initialize the config structure for the PWM channel

    // Set up the PWM channel configuration
    PWM_channel_for_stepper_motor.gpio_num = STEPPER_MOTOR_PUL_PIN; // Assign the GPIO pin for the stepper motor pulse
    PWM_channel_for_stepper_motor.speed_mode = LEDC_LOW_SPEED_MODE; // Use the low-speed mode for the PWM channel
    PWM_channel_for_stepper_motor.channel = LEDC_CHANNEL_0;         // Use channel 0
    PWM_channel_for_stepper_motor.timer_sel = LEDC_TIMER_0;         // Select timer 0 for this channel
    PWM_channel_for_stepper_motor.duty = 0;                         // Initialize the duty cycle to 0
    PWM_channel_for_stepper_motor.hpoint = 0;                       // Set the high point to 0

    // Configure the PWM channel with the specified settings
    ledc_channel_config(&PWM_channel_for_stepper_motor);

    // Install the fade function, allowing smooth PWM transitions and return the result
    return ledc_fade_func_install(0);
}

void app_main(void)
{

    Initialize_GPIO_for_Stepper_Motor_Driver();
    Initialize_PWM_for_Stepper_Motor_Driver();

    /* Make the stop */
    gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_HIGH);
    gpio_set_level(STEPPER_MOTOR_DIR_PIN, SET_GPIO_LEVEL_HIGH);
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, PWM_DUTY_CYCLE_50, 0);

    initialize_console();

    const char *prompt = LOG_COLOR_I PROMPT_STR "> " LOG_RESET_COLOR; // Define the prompt string

    int probe_status = linenoiseProbe(); // Probe the terminal for capabilities

    if (probe_status) // Check if probe was unsuccessful
    {
        printf("\n"
               "Your terminal application does not support escape sequences.\n"
               "Line editing and history features are disabled.\n"
               "On Windows, try using Putty instead.\n");

        linenoiseSetDumbMode(1); // Disable advanced features if terminal does not support escape sequences
    }

    /* Main loop */
    while (true)
    {
        /* Get a line using linenoise.
         * The line is returned when ENTER is pressed.
         */
        char *line = linenoise(prompt); // Read a line of input from the console
        if (line == NULL)
        { // Break on EOF or error
            break;
        }
        /* Add the command to the history if not empty */
        if (strlen(line) > 0)
        {
            linenoiseHistoryAdd(line); // Add the command to history if it's not empty
        }

        /* Try to run the command */
        int ret;
        esp_err_t err = esp_console_run(line, &ret); // Execute the command
        if (err == ESP_ERR_NOT_FOUND)
        {
            printf("Unrecognized command\n"); // Print message if command is not recognized
        }
        else if (err == ESP_ERR_INVALID_ARG)
        {
            // Command was empty
        }
        else if (err == ESP_OK && ret != ESP_OK)
        {
            printf("Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(ret)); // Print error code if command failed
        }
        else if (err != ESP_OK)
        {
            printf("Internal error: %s\n", esp_err_to_name(err)); // Print internal error
        }

        /* linenoise allocates line buffer on the heap, so need to free it */
        linenoiseFree(line); // Free the allocated line buffer
    }
}
