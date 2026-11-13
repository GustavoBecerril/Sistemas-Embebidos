---
title: "Lab Log - UART Bidirectional"
course: "Embedded Systems I"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

## Goal

Our goal was to send button changes to another UART device and use received letters to control two LEDs.

## Prediction

We expected a two-way UART link: button presses would send letters to another device, and received letters would control the Pico LEDs.

## Setup

We connected UART0 TX on GP0 and RX on GP1 to another UART device. We crossed TX and RX and shared the ground. Our code uses 115200 baud, 8 data bits, no parity, and 1 stop bit. We connected buttons to GP14 and GP15, with internal pull-ups, and LEDs to GP16 and GP17.

## What I did

We used the received letters A/a and B/b to turn the two LEDs on and off. When a button changed state, we sent its matching letter over UART and waited 20 ms before checking again.

## Evidence

<video src="../../img_videos_parcial2/uart_bidirectional.mp4" width="100%" controls></video>

## Predicted vs measured

The UART link worked as expected. Our buttons sent letters to the other device, and received letters controlled the LEDs. This code does not print a text reply to the USB serial monitor.

## What went wrong

Our baud rate did not match the other device at first, so we saw strange symbols. We set both sides to 115200 baud and fixed the problem.

## Code

```c
// Tema: 4. Protocolos de Comunicación (UART)

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

#define UART_ID uart0
#define BAUD_RATE 115200
#define TX_PIN 0
#define RX_PIN 1

#define BTN1_PIN 14
#define BTN2_PIN 15
#define LED1_PIN 16
#define LED2_PIN 17

int main() {
    stdio_init_all();

    gpio_set_function(TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RX_PIN, GPIO_FUNC_UART);
    uart_init(UART_ID, BAUD_RATE);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(UART_ID, true);

    gpio_init(LED1_PIN);
    gpio_set_dir(LED1_PIN, GPIO_OUT);
    gpio_put(LED1_PIN, 0);

    gpio_init(LED2_PIN);
    gpio_set_dir(LED2_PIN, GPIO_OUT);
    gpio_put(LED2_PIN, 0);

    gpio_init(BTN1_PIN);
    gpio_set_dir(BTN1_PIN, GPIO_IN);
    gpio_pull_up(BTN1_PIN);

    gpio_init(BTN2_PIN);
    gpio_set_dir(BTN2_PIN, GPIO_IN);
    gpio_pull_up(BTN2_PIN);

    bool last_btn1 = gpio_get(BTN1_PIN);
    bool last_btn2 = gpio_get(BTN2_PIN);

    while (true) {
        while (uart_is_readable(UART_ID)) {
            char cmd = uart_getc(UART_ID);
            switch (cmd) {
                case 'A': // Encender LED 1
                    gpio_put(LED1_PIN, 1);
                    break;
                case 'a': // Apagar LED 1
                    gpio_put(LED1_PIN, 0);
                    break;
                case 'B': // Encender LED 2
                    gpio_put(LED2_PIN, 1);
                    break;
                case 'b': // Apagar LED 2
                    gpio_put(LED2_PIN, 0);
                    break;
                default:
                    break;
            }
        }

        bool current_btn1 = gpio_get(BTN1_PIN);
        if (current_btn1 != last_btn1) {
            if (!current_btn1) {
                uart_putc(UART_ID, 'A');
            } else {
                uart_putc(UART_ID, 'a');
            }
            last_btn1 = current_btn1;
        }

        bool current_btn2 = gpio_get(BTN2_PIN);
        if (current_btn2 != last_btn2) {
            if (!current_btn2) {
                uart_putc(UART_ID, 'B');
            } else {
                uart_putc(UART_ID, 'b');
            }
            last_btn2 = current_btn2;
        }

        sleep_ms(20);
    }

    return 0;
}
```

## Open question

Can we use UART to make two Raspberry Pi Picos talk to each other?
