---
title: "Lab Log — XOR Gate"
course: "Embedded Systems I (LIIE1214)"
student: "Becerril Villamil Gustavo; Bermudez Ruiz Uriel Onairam; Rios Garcia Cristian Ulises"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

# XOR Gate

**Goal (one sentence):** Our goal was to turn the LED on when exactly one button was pressed, and keep it off in the other cases.

**Prediction — written *before* running anything:**

We expected the LED to turn on only if we pressed one button, but not both at the same time.

---

## Setup

We connected an LED to GP0 and buttons to GP1 and GP2. Our code turns off the internal pulls, so we used external pull-down resistors. We used the same circuit as the AND and OR gates.

| Signal | GPIO | Direction | Pull | Purpose |
|--------|------|-----------|------|---------|
| LED | 0 | OUT | N/A | Shows the gate output |
| Button 1 | 1 | IN | none (external) | Input A |
| Button 2 | 2 | IN | none (external) | Input B |

## What I did

1. We read both button pins in a loop.
2. We turned the LED on when the button states were different.
3. We tested both buttons released, one pressed, and both pressed.

## How it works

XOR means **exclusive OR**. It gives a `1` when the two inputs are
**different**, and a `0` when they are the same. People also call it the
"difference detector".

The pin setup is the same as the AND and OR gates. What changes is the
decision, and this time it needs **two conditions instead of one**:

```c
if ((sio_hw->gpio_in & BTN_BIT) && !(sio_hw->gpio_in & BTN2_BIT)) {
    sio_hw->gpio_set = LED_BIT;        // only Button 1 pressed
} else if (!(sio_hw->gpio_in & BTN_BIT) && (sio_hw->gpio_in & BTN2_BIT)) {
    sio_hw->gpio_set = LED_BIT;        // only Button 2 pressed
} else {
    sio_hw->gpio_clr = LED_BIT;        // both pressed or both released
}
```

### Reading it step by step

- The **first** `if` is true when Button 1 is pressed **and** Button 2 is
  **not** pressed.
- The **`else if`** is true when Button 1 is **not** pressed **and** Button 2
  is pressed.
- Anything else falls to the `else`, which covers both remaining cases: both
  buttons pressed, and no button pressed.

The new operator here is `!`, the **logical NOT**. It flips true into false and
false into true. `!(gpio_in & BTN2_BIT)` means "Button 2 is *not* pressed".

The brackets around `(sio_hw->gpio_in & BTN_BIT)` matter. `&` has lower
priority than `!` in C, so without them the code would not read the pin the way
we want.

!!! tip "A shorter way to write XOR"
    Because XOR is true when the inputs are different, the same logic can be
    written in one line with the `!=` operator:

    ```c
    bool a = sio_hw->gpio_in & BTN_BIT;
    bool b = sio_hw->gpio_in & BTN2_BIT;
    if (a != b) { /* LED on */ }
    ```

    The version with two branches is documented here because it is the one that
    was built and tested, and it shows each case clearly.

### Truth table

| Button 1 | Button 2 | LED |
|----------|----------|-----|
| 0 | 0 | off |
| 0 | 1 | **on** |
| 1 | 0 | **on** |
| 1 | 1 | off |

### The three gates side by side

| Button 1 | Button 2 | AND | OR | XOR |
|----------|----------|-----|-----|-----|
| 0 | 0 | off | off | off |
| 0 | 1 | off | **on** | **on** |
| 1 | 0 | off | **on** | **on** |
| 1 | 1 | **on** | **on** | off |

XOR behaves like OR, except in the last row. That last row is the whole
difference, and it is the row to check carefully in the video.

## Evidence

| # | File | What it shows | Measured value |
|---|------|---------------|----------------|
| E1 | img/XORGate.mp4 | The LED turns off again when both buttons are pressed | Not recorded |

<video src="../../img/XORGate.mp4" width="100%" controls></video>

## Predicted vs measured

It worked as expected. Pressing both buttons turned the LED off.

## What went wrong

- At first, we were confused by the logic table. We wrote the cases on paper, and this helped us understand XOR.

## Code

`codigos/XORGate.c`

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/structs/sio.h"

#define Led_Pin 0
#define Button_PIN 1
#define Button_PIN2 2

int main(void)
{
    const uint32_t LED_BIT = 1u << Led_Pin;
    const uint32_t BTN_BIT = 1u << Button_PIN;
    const uint32_t BTN2_BIT = 1u << Button_PIN2;

    gpio_init(Led_Pin);
    gpio_init(Button_PIN);
    gpio_init(Button_PIN2);

    sio_hw->gpio_oe_set = LED_BIT;
    sio_hw->gpio_oe_clr = BTN_BIT;
    sio_hw->gpio_oe_clr = BTN2_BIT;

    gpio_disable_pulls(Button_PIN);
    gpio_disable_pulls(Button_PIN2);

    while (true) {
        if ((sio_hw->gpio_in & BTN_BIT) && !(sio_hw->gpio_in & BTN2_BIT)) {
            sio_hw->gpio_set = LED_BIT;
        } else if (!(sio_hw->gpio_in & BTN_BIT) && (sio_hw->gpio_in & BTN2_BIT)) {
            sio_hw->gpio_set = LED_BIT;
        } else {
            sio_hw->gpio_clr = LED_BIT;
        }
    }
}
```

## Open question

- Why is the XOR gate important for binary addition?
