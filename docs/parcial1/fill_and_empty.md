---
title: "Lab Log — Fill and Empty Animation"
course: "Embedded Systems I (LIIE1214)"
student: "Becerril Villamil Gustavo; Bermudez Ruiz Uriel Onairam; Rios Garcia Cristian Ulises"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

# Fill and Empty Animation

**Goal (one sentence):** Our goal was to turn the four LEDs on one by one and then turn them off one by one.

**Prediction — written *before* running anything:**

We expected the LEDs to turn on one by one until all were on, and then turn off one by one.

---

## Setup

We connected four LEDs to GP0-GP3, with one 220-ohm resistor for each LED. We used the same circuit as the other LED exercises.

| Signal | GPIO | Direction | Bit | Purpose |
|--------|------|-----------|-----|---------|
| LED A | 0 | OUT | bit 0 | First to turn on, last to turn off |
| LED B | 1 | OUT | bit 1 | Animation step |
| LED C | 2 | OUT | bit 2 | Animation step |
| LED D | 3 | OUT | bit 3 | Last to turn on, first to turn off |

## What I did

1. We connected four LEDs to GP0-GP3.
2. We sent eight fixed values to the GPIO pins to fill and empty the row.
3. We waited 300 ms after each value.

## How it works

This program does not use a loop counter. It writes **eight fixed patterns**,
one after the other. Each pattern is a plain decimal number, but the hardware
reads it as four bits.

Every step is always the same two lines:

```c
sio_hw->gpio_clr = Mask;   // turn all four LEDs off
sio_hw->gpio_set = 7;      // write the new pattern
sleep_ms(300);             // wait so we can see it
```

Clearing with the mask first is what makes the pattern exact. Without it, the
`gpio_set` register can only add LEDs, never remove them, so the bar could
never empty.

### The filling half

The numbers 1, 3, 7 and 15 each add one more bit on the left:

| Step | Value | Binary | D | C | B | A |
|------|-------|--------|---|---|---|---|
| 1 | 1 | `0001` | off | off | off | **on** |
| 2 | 3 | `0011` | off | off | **on** | **on** |
| 3 | 7 | `0111` | off | **on** | **on** | **on** |
| 4 | 15 | `1111` | **on** | **on** | **on** | **on** |

Each of these numbers is `2^n - 1`, which in binary is always a row of ones.
That is why the bar grows one LED per step.

### The emptying half

The numbers 14, 12, 8 and 0 each remove one bit from the right:

| Step | Value | Binary | D | C | B | A |
|------|-------|--------|---|---|---|---|
| 5 | 14 | `1110` | **on** | **on** | **on** | off |
| 6 | 12 | `1100` | **on** | **on** | off | off |
| 7 | 8 | `1000` | **on** | off | off | off |
| 8 | 0 | `0000` | off | off | off | off |

So the bar fills from LED A towards LED D, and then empties from LED A towards
LED D as well. The LED that turns on first is also the one that turns off first.

One full animation is 8 steps, about `8 × 300 ms = 2.4 s`.

!!! tip "Same idea, shorter code"
    The filling half could also be written as a loop using
    `sio_hw->gpio_set = (1u << (i + 1)) - 1;`. Writing the eight numbers by hand
    makes each frame of the animation easy to read and easy to change.

## Evidence

No matching photo or demonstration video is available in the repository.

## Predicted vs measured

The animation played as planned. Our code used fixed values to show the eight LED patterns.

## What went wrong

- At first, we set the animation length incorrectly, so it stopped halfway. We fixed the code to show all eight steps.

## Code

`codigos/Exercise3FillAndEmptyAnimation.c`

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/structs/sio.h"


#define PIN_A 0
#define PIN_B 1
#define PIN_C 2
#define PIN_D 3

int main(void)
{
    const uint32_t Mask =
        (1u << PIN_A) |
        (1u << PIN_B) |
        (1u << PIN_C) |
        (1u << PIN_D);

    gpio_init(PIN_A);
    gpio_init(PIN_B);
    gpio_init(PIN_C);
    gpio_init(PIN_D);

    sio_hw->gpio_oe_set = Mask;

    while (true) {
        sio_hw->gpio_clr = Mask;
        sio_hw->gpio_set = 1;
        sleep_ms(300);
        sio_hw->gpio_clr = Mask;
        sio_hw->gpio_set = 3;
        sleep_ms(300);
        sio_hw->gpio_clr = Mask;
        sio_hw->gpio_set = 7;
        sleep_ms(300);
        sio_hw->gpio_clr = Mask;
        sio_hw->gpio_set = 15;
        sleep_ms(300);

        sio_hw->gpio_clr = Mask;
        sio_hw->gpio_set = 14;
        sleep_ms(300);
        sio_hw->gpio_clr = Mask;
        sio_hw->gpio_set = 12;
        sleep_ms(300);
        sio_hw->gpio_clr = Mask;
        sio_hw->gpio_set = 8;
        sleep_ms(300);
        sio_hw->gpio_clr = Mask;
        sio_hw->gpio_set = 0;
        sleep_ms(300);
    }
}
```

## Open question

- Could we add a button to pause the animation at any frame?
