
// Tema: 1. Interrupciones y Temporizadores (Timers & ISRs)

#include "pico/stdlib.h"
#include "pico/time.h"

#define TEST_PIN            15
#define CALLBACK_MARK_PIN   14
#define TIMER_DELAY_MS      10
#define WORK_US             20000
#define USE_NEGATIVE_DELAY  1

static repeating_timer_t timer;

static bool timer_callback(repeating_timer_t *rt) {
    static bool state = false;

    gpio_put(CALLBACK_MARK_PIN, 1);

    state = !state;
    gpio_put(TEST_PIN, state);

    // Intentional workload for the experiment.
    busy_wait_us_32(WORK_US);

    gpio_put(CALLBACK_MARK_PIN, 0);
    return true;
}

int main() {
    gpio_init(TEST_PIN);
    gpio_set_dir(TEST_PIN, GPIO_OUT);

    gpio_init(CALLBACK_MARK_PIN);
    gpio_set_dir(CALLBACK_MARK_PIN, GPIO_OUT);

    int32_t delay_ms = USE_NEGATIVE_DELAY ? -TIMER_DELAY_MS : TIMER_DELAY_MS;
    add_repeating_timer_ms(delay_ms, timer_callback, NULL, &timer);

    while (true) {
        tight_loop_contents();
    }
}