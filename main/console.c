#include <console.h>

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