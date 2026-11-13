
// Tema: 4. Protocolos de Comunicación (UART)

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

#define UART_ID uart0
#define BAUD_RATE 115200
#define TX_PIN 0
#define RX_PIN 1

int main() {
    stdio_init_all();

    gpio_set_function(TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RX_PIN, GPIO_FUNC_UART);

    uart_init(UART_ID, BAUD_RATE);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);

    printf("[MAIN] Sending messages every second...\n");

    int counter = 0;
    while (true) {
        char msg[64];
        sprintf(msg, "Message %d from MAIN\r\n", counter++);
        uart_puts(UART_ID, msg);
        sleep_ms(1000);
    }
}