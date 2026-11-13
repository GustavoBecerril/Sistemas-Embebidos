---
title: "Lab Log — Fill From the Outside Inward"
course: "Embedded Systems I (LIIE1214)"
student: "Becerril Villamil Gustavo; Bermudez Ruiz Uriel Onairam; Rios Garcia Cristian Ulises"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

# Fill From the Outside Inward

**Goal (one sentence):** Our goal was to turn on the outside LEDs first, then move the pattern toward the middle.

**Prediction — written *before* running anything:**

We expected the outer LEDs to turn on first, then the inner ones, creating a closing effect.

---

## Setup

We connected four LEDs to GP0-GP3, with one 220-ohm resistor for each LED. We used the same circuit as the other LED exercises.

| Signal | GPIO | Direction | Position | Purpose |
|--------|------|-----------|----------|---------|
| LED A | 0 | OUT | left end | Outer LED |
| LED B | 1 | OUT | inner left | Middle LED |
| LED C | 2 | OUT | inner right | Middle LED |
| LED D | 3 | OUT | right end | Outer LED |

## What I did

1. We connected four LEDs to GP0-GP3.
2. We sent the values 0, 9, 3, and 4 to the GPIO pins.
3. We waited 300 ms after each value.

## How it works

Like the fill and empty exercise, this program writes **fixed patterns** to the
GPIO registers. Here there are only four steps.

Each step clears the mask first and then writes the new value:

```c
sio_hw->gpio_clr = Mask;   // all LEDs off
sio_hw->gpio_set = 9;      // 9 = 0b1001 = the two outer LEDs
sleep_ms(300);
```

The four patterns are 0, 9, 3 and 4:

| Step | Value | Binary | D | C | B | A | What you see |
|------|-------|--------|---|---|---|---|--------------|
| 1 | 0 | `0000` | off | off | off | off | Blank frame, the animation restarts |
| 2 | 9 | `1001` | **on** | off | off | **on** | Both ends light up |
| 3 | 3 | `0011` | off | off | **on** | **on** | The two left LEDs |
| 4 | 4 | `0100` | off | **on** | off | off | One middle LED |

The interesting number is **9**. In binary it is `1001`, which is bit 3 and
bit 0 — exactly the two LEDs at the ends. That one value is what gives the
exercise its name.

One full animation is 4 steps, about `4 × 300 ms = 1.2 s`.




## Evidence

No matching photo or demonstration video is available in the repository.

## Predicted vs measured

The first pattern showed the outside LEDs, but the later patterns did not close symmetrically. Our code used the values 3 and 4.

## What went wrong

- We used the wrong values for the last patterns. Our code uses 3 and then 4, but the two middle LEDs need the value 6.

## Code

`codigos/Exercise4_FillFromTheoutsideInward.c`

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
        sio_hw->gpio_set = 0;
        sleep_ms(300);
        sio_hw->gpio_clr = Mask;
        sio_hw->gpio_set =9;
        sleep_ms(300);
        sio_hw->gpio_clr = Mask;
        sio_hw->gpio_set = 3;
        sleep_ms(300);
        sio_hw->gpio_clr = Mask;
        sio_hw->gpio_set = 4;
        sleep_ms(300);
    }
}
```

## Open question

- Can we make this same effect with 8 LEDs instead of 4?
