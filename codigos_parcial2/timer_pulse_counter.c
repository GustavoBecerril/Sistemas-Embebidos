
// Tema: 1. Interrupciones y Temporizadores (Timers & ISRs)

#include <stdio.h>
#include "pico/stdlib.h"
 
#define TEST_PIN    15
#define BUTTON_PIN  14
#define TOGGLE_MS   2000
 
static volatile uint32_t count = 0;
static repeating_timer_t timer;
 
static bool timer_callback(repeating_timer_t *rt) {
    gpio_put(TEST_PIN, !gpio_get(TEST_PIN));
    printf("Pulsaciones: %lu\n", count);
    count = 0;
    return true;
}
 
int main() {
    stdio_init_all();
    gpio_init(TEST_PIN);
    gpio_set_dir(TEST_PIN, GPIO_OUT);
    gpio_init(BUTTON_PIN);
    gpio_pull_up(BUTTON_PIN);
 
    add_repeating_timer_ms(-TOGGLE_MS, timer_callback, NULL, &timer);
 
    bool prev = true;
    while (true) {
        bool curr = gpio_get(BUTTON_PIN);
        if (prev && !curr) count++;
        prev = curr;
        tight_loop_contents();
    }
}