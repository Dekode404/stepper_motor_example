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

esp_err_t Rotate_Stepper_Motor(uint PWM_frequency, uint8_t Motor_Direction, float Motor_run_time)
{
    esp_err_t Function_Error = ESP_OK;

    gpio_set_level(STEPPER_MOTOR_DIR_PIN, (Motor_Direction == MOTOR_DIRECTION_FORWARD) ? SET_GPIO_LEVEL_HIGH : SET_GPIO_LEVEL_LOW);

    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, PWM_DUTY_CYCLE_50, 0);

    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, PWM_frequency); // Set the PWM frequency

    // Calculate the delay in ticks for a 1000 microsecond (1 millisecond) delay
    TickType_t delay_ticks = ((Motor_run_time * 1000) * configTICK_RATE_HZ) / 1000000;

    gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_LOW);  // Enable the Motor driver
    vTaskDelay(delay_ticks);                                   // Delay for the calculated number of ticks
    gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_HIGH); // Disable the Motor driver

    Stop_Stepper_Motor();

    return Function_Error;
}

/**
 * @brief Stop the stepper motor
 *
 * This function sets the GPIO levels to high, stopping the stepper motor,
 * and sets the PWM duty cycle to 0.
 *
 * @return
 *     - Sum of all ESP return values
 */
esp_err_t Stop_Stepper_Motor(void)
{
    esp_err_t Function_Error = ESP_OK;

    Function_Error += gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_HIGH);
    Function_Error += gpio_set_level(STEPPER_MOTOR_DIR_PIN, SET_GPIO_LEVEL_HIGH);
    Function_Error += ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, PWM_DUTY_CYCLE_00, 0);

    return Function_Error;
}

/**
 * @brief Start the stepper motor with specified parameters.
 *
 * This function enables the motor driver, sets the motor direction,
 * configures the PWM frequency, and sets the PWM duty cycle.
 *
 * @param Motor_Direction The direction of the motor (0 for low, 1 for high).
 * @param PWM_frequency The PWM frequency for motor control.
 * @param PWM_Duty_Cycle The PWM duty cycle for motor control.
 * @return ESP_OK if successful, or an error code if any operation fails.
 */
esp_err_t Start_Stepper_Motor(uint8_t Motor_Direction, uint PWM_frequency, uint PWM_Duty_Cycle)
{
    esp_err_t Function_Error = ESP_OK;

    // Enable the Motor driver
    Function_Error += gpio_set_level(STEPPER_MOTOR_EN_PIN, SET_GPIO_LEVEL_LOW);

    Function_Error += gpio_set_level(STEPPER_MOTOR_DIR_PIN, (Motor_Direction == MOTOR_DIRECTION_FORWARD) ? SET_GPIO_LEVEL_HIGH : SET_GPIO_LEVEL_LOW);

    // Set PWM duty cycle for motor
    Function_Error += ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, PWM_Duty_Cycle, 0);

    // Loop to gradually increase frequency from PWM_START_FREQ to target_frequency
    for (uint32_t freq = PWM_START_FREQ; freq <= PWM_frequency; freq += PWM_INCREMENT)
    {
        Function_Error = ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq); // Set the PWM frequency

        if (Function_Error != ESP_OK) // Check for errors while setting the frequency
        {
            printf("Error setting frequency: %dHZ\n", freq);

            break; // Exit the loop if an error occurs
        }

        vTaskDelay(pdMS_TO_TICKS(PWM_DELAY_MS)); // Delay to allow the motor to adapt to the new frequency
    }

    return Function_Error;
}

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
 *     - Sum of all ESP return values
 */
esp_err_t Initialize_PWM_for_Stepper_Motor_Driver(void)
{
    esp_err_t Function_Error = ESP_OK;

    // Configuration structure for the PWM timer
    ledc_timer_config_t PWM_timer = {}; // Zero-initialize the config structure for the timer used for the PWM

    // Set up the PWM timer configuration
    PWM_timer.speed_mode = LEDC_LOW_SPEED_MODE;    // Use the low-speed mode for the PWM driver
    PWM_timer.duty_resolution = LEDC_TIMER_10_BIT; // Set the resolution of the timer to 10 bits
    PWM_timer.timer_num = LEDC_TIMER_0;            // Use timer 0
    PWM_timer.freq_hz = PWM_FREQUENCY_1KHZ;        // Set the PWM frequency to 1 kHz
    PWM_timer.clk_cfg = LEDC_AUTO_CLK;             // Auto-select the clock source

    // Configure the PWM timer with the specified settings
    Function_Error += ledc_timer_config(&PWM_timer);

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
    Function_Error += ledc_channel_config(&PWM_channel_for_stepper_motor);

    // Install the fade function, allowing smooth PWM transitions and return the result
    Function_Error += ledc_fade_func_install(0);

    // Return the sum of all ESP return values
    return Function_Error;
}

void app_main(void)
{
    ESP_ERROR_CHECK(Initialize_GPIO_for_Stepper_Motor_Driver());

    ESP_ERROR_CHECK(Initialize_PWM_for_Stepper_Motor_Driver());

    /* Stop the stepper motor */
    ESP_ERROR_CHECK(Stop_Stepper_Motor());

    initialize_console();

    ESP_ERROR_CHECK(esp_console_register_help_command());

    ESP_ERROR_CHECK(Register_Start_Motor_CMD());
    ESP_ERROR_CHECK(Register_Stop_Motor_CMD());
    ESP_ERROR_CHECK(Register_Quick_Start_Motor_CMD());
    ESP_ERROR_CHECK(Register_Rotate_Motor_CMD());
    ESP_ERROR_CHECK(Register_Rotate_Angle_CMD());

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
