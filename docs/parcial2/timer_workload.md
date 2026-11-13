---
title: "Lab Log - Timer Workload"
course: "Embedded Systems I"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

## Goal

Our goal was to show timer callback activity on two GPIO outputs.

## Prediction

We expected the timer to run a task at set times without stopping the main loop.

## Setup

We connected GP15 and GP14 to separate oscilloscope or logic analyzer inputs and shared the ground. We used GP15 as the test output and GP14 to mark the callback work.

## What I did

We set up a repeating timer with a -10 ms delay. In each callback, we raised GP14, changed the state of GP15, waited 20 ms, and then lowered GP14.

## Evidence

<video src="../../img_videos_parcial2/timer_workload.mp4" width="100%" controls></video>

## Predicted vs measured

Our code keeps GP14 high during a 20 ms wait, but the timer uses a 10 ms start-to-start interval. The callback takes longer than that interval, and the main loop cannot run while the callback is active.

## What went wrong

At first, we put `sleep_ms` inside the timer callback, and the system froze. We learned to keep timer callbacks short.

## Code

```c

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
```

## Open question

What happens if two hardware timers trigger at the same time?
