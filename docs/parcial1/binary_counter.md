---
title: "Lab Log — Binary Counter"
course: "Embedded Systems I (LIIE1214)"
student: "Becerril Villamil Gustavo; Bermudez Ruiz Uriel Onairam; Rios Garcia Cristian Ulises"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

# Binary Counter — LEDs Counting Up

**Goal (one sentence):** Our goal was to show the numbers 0 to 15 on four LEDs, with each LED showing one bit of the binary number.

**Prediction — written *before* running anything:**

We expected the 4 LEDs to count from 0 to 15 in binary, lighting up in a specific pattern.

---

## Setup

We connected four LEDs to GP0-GP3. We used one 220-ohm resistor for each LED and kept the board at its default clock.

| Signal | GPIO | Direction | Weight | Purpose |
|--------|------|-----------|--------|---------|
| LED A | 0 | OUT | 1 | Bit 0  |
| LED B | 1 | OUT | 2 | Bit 1 |
| LED C | 2 | OUT | 4 | Bit 2 |
| LED D | 3 | OUT | 8 | Bit 3  |

## What I did

1. We connected four LEDs to GP0-GP3.
2. We sent each number from 0 to 15 to the GPIO pins as a bit pattern.
3. We repeated the count in a loop.

## How it works

This program is very short because it uses one good idea: **the number itself is
the pattern**.

- First the code builds a **mask**. A mask is a number where a bit is `1` for
  every pin we want to touch:

  ```text
  (1u << PIN_A) | (1u << PIN_B) | (1u << PIN_C) | (1u << PIN_D)
  = 0b0001 | 0b0010 | 0b0100 | 0b1000
  = 0b1111
  ```

- `gpio_init()` turns each pin into a normal GPIO pin.
- `sio_hw->gpio_oe_set = Mask` sets the **output enable** bit for those four
  pins. `oe` means output enable, so now the four pins are outputs.
- Inside the loop, `sio_hw->gpio_clr = Mask` turns **all four LEDs off**. This
  is important: it clears the old pattern before writing the new one.
- Then `sio_hw->gpio_set = i` turns on the LEDs. Here `i` is just the counter
  number, but the hardware reads it as a bit pattern. So when `i = 5`, the
  hardware sees `0b0101` and turns on GP0 and GP2.
- `sleep_ms(300)` waits 300 ms so the human eye can follow the count.
- When `i` reaches 16 the `for` loop ends, the `while (true)` loop starts it
  again from 0, and the counter repeats forever.

One full count takes about `16 × 300 ms = 4.8 s`.

| `i` | Binary | LED D | LED C | LED B | LED A |
|-----|--------|-------|-------|-------|-------|
| 0 | `0000` | off | off | off | off |
| 1 | `0001` | off | off | off | **on** |
| 2 | `0010` | off | off | **on** | off |
| 3 | `0011` | off | off | **on** | **on** |
| 5 | `0101` | off | **on** | off | **on** |
| 10 | `1010` | **on** | off | **on** | off |
| 15 | `1111` | **on** | **on** | **on** | **on** |

!!! note "Why write to registers and not `gpio_put()`?"
    `gpio_put()` changes one pin at a time. Writing to `sio_hw->gpio_set`
    changes **all four pins in the same clock cycle**. So the four LEDs change
    together and never show a wrong value in between.

## Evidence

| # | File | What it shows | Measured value |
|---|------|---------------|----------------|
| E1 | img/ContadorBinario.mp4 | The LEDs counting from 0 to 15 | Not recorded |

<video src="../../img/ContadorBinario.mp4" width="100%" controls></video>

## Predicted vs measured

It worked exactly as expected. We saw the LEDs counting in binary perfectly.

## What went wrong

- At first, we tried to turn on each LED one by one. We learned to use masks to control all four LEDs at the same time.

## Code

`codigos/LedsNumeroEnAumento.c`

!!! info "Note"
    The includes and the `#define` lines are the same as in the other LED
    exercises. They are shown here so the code can be built as it is.

```c
// Codigo 1: LEDS Numero en aumento

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
        for (int i = 0; i < 16; i++) {
            sio_hw->gpio_clr = Mask;
            sio_hw->gpio_set = i;
            sleep_ms(300);
        }
    }
}
```

## Open question

- Can we make it count backwards from 15 to 0?
