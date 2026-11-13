---
title: "Lab Log — Bouncing Light"
course: "Embedded Systems I (LIIE1214)"
student: "Becerril Villamil Gustavo; Bermudez Ruiz Uriel Onairam; Rios Garcia Cristian Ulises"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

# Bouncing Light

**Goal (one sentence):** Our goal was to move one lit LED from one end of the row to the other and back again.

**Prediction — written *before* running anything:**

We expected one light to move from left to right and bounce back, like a scanning laser.

---

## Setup

We connected four LEDs to GP0-GP3, with one 220-ohm resistor for each LED. We used the same circuit as the binary counter.

| Signal | GPIO | Direction | Position | Purpose |
|--------|------|-----------|----------|---------|
| LED A | 0 | OUT | 1st (left end) | Animation step |
| LED B | 1 | OUT | 2nd | Animation step |
| LED C | 2 | OUT | 3rd | Animation step |
| LED D | 3 | OUT | 4th (right end) | Animation step |

## What I did

1. We connected the four LEDs to GP0-GP3.
2. We used two loops to move one lit LED to the right and then back to the left.
3. We waited 300 ms at each step.

## How it works

Only **one LED is on at a time**. The code moves that single LED up and then
back down.

- The mask `0b1111` covers the four pins, and `sio_hw->gpio_oe_set = Mask`
  makes all four of them outputs.
- `1u << i` builds a number with only **one** bit set. When `i = 2` this gives
  `0b0100`, which is only LED C.
- Every step does two things in order:
    1. `sio_hw->gpio_clr = Mask` turns all four LEDs off.
    2. `sio_hw->gpio_set = (1u << i)` turns on only the LED for this step.

  Without step 1 the LEDs would stay on and the light would look like it is
  filling up instead of moving.

### The two loops

The first loop goes **up**:

```c
for (int i = 0; i < 4; i++)   // i = 0, 1, 2, 3
```

The second loop goes **down**:

```c
for (int i = 2; i > 0; i--)   // i = 2, 1
```

The second loop starts at `2` and not at `3`, and stops at `1` and not at `0`.
This is on purpose. LED D (`i = 3`) was already the last step of the up loop,
and LED A (`i = 0`) will be the first step of the next up loop. If the down loop
also used 3 and 0, those two LEDs would stay on for 600 ms instead of 300 ms and
the bounce would look like it pauses at the ends.

### One full cycle

| Step | `i` | Pattern | LED on |
|------|-----|---------|--------|
| 1 | 0 | `0001` | A |
| 2 | 1 | `0010` | B |
| 3 | 2 | `0100` | C |
| 4 | 3 | `1000` | D |
| 5 | 2 | `0100` | C |
| 6 | 1 | `0010` | B |
| → repeat from step 1 | | | |

A full bounce is 6 steps, so it takes about `6 × 300 ms = 1.8 s`.

## Evidence

No matching photo or demonstration video is available in the repository.

## Predicted vs measured

The light bounced as expected. Our code used `1u << i` to choose each LED.

## What went wrong

- At first, we set the limits incorrectly, so the light went out of bounds and disappeared.

## Code

`codigos/Exercise2BouncingLight.c`

!!! info "Note"
    The includes and the `#define` lines are the same as in the other LED
    exercises. They are shown here so the code can be built as it is.

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
        for (int i = 0; i < 4; i++) {
            sio_hw->gpio_clr = Mask;
            sio_hw->gpio_set = (1u << i);
            sleep_ms(300);
        }
        for (int i = 2; i > 0; i--) {
            sio_hw->gpio_clr = Mask;
            sio_hw->gpio_set = (1u << i);
            sleep_ms(300);
        }
    }

}
```

## Open question

- How can we make the light bounce faster every time it hits an edge?
