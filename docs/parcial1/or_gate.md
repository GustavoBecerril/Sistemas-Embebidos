---
title: "Lab Log — OR Gate"
course: "Embedded Systems I (LIIE1214)"
student: "Becerril Villamil Gustavo; Bermudez Ruiz Uriel Onairam; Rios Garcia Cristian Ulises"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

# OR Gate

**Goal (one sentence):** Our goal was to turn the LED on when either button or both buttons were pressed.

**Prediction — written *before* running anything:**

We expected the LED to turn on if we pressed at least one of the two buttons.

---

## Setup

We connected an LED to GP0 and buttons to GP1 and GP2. Our code turns off the internal pulls, so we used external pull-down resistors. We used the same circuit as the AND gate.

| Signal | GPIO | Direction | Pull | Purpose |
|--------|------|-----------|------|---------|
| LED | 0 | OUT | N/A | Shows the gate output |
| Button 1 | 1 | IN | none (external) | Input A |
| Button 2 | 2 | IN | none (external) | Input B |

## What I did

1. We connected two buttons and one LED to the Pico and breadboard.
2. We read both buttons and used logical OR to control the LED.
3. We tested each button and both buttons together.

## How it works

An OR gate gives a `1` when **at least one** input is `1`. It gives `0` only
when every input is `0`.

The setup part is identical to the AND gate:

- `sio_hw->gpio_oe_set = LED_BIT` makes GP0 an output.
- `sio_hw->gpio_oe_clr` makes GP1 and GP2 inputs.
- `gpio_in & BTN_BIT` reads one button bit out of the input register.

**Only one character is different from the AND gate program:**

```c
if (sio_hw->gpio_in & BTN_BIT || (sio_hw->gpio_in & BTN2_BIT)) {
    sio_hw->gpio_set = LED_BIT;   // LED on
} else {
    sio_hw->gpio_clr = LED_BIT;   // LED off
}
```

The `&&` became `||`, the **logical OR**. Now the condition is true if the left
side is true, or the right side is true, or both.

This is a good example of an important idea: the hardware did not change at
all. The same two buttons and the same LED behave like a different logic gate
just because one operator in the software changed.

!!! tip "Short-circuit evaluation"
    C stops checking `||` as soon as it finds a true value. So if Button 1 is
    pressed, the program never even reads Button 2 in that pass of the loop.
    This does not change the result here, but it is worth knowing.

### Truth table

| Button 1 | Button 2 | LED |
|----------|----------|-----|
| 0 | 0 | off |
| 0 | 1 | **on** |
| 1 | 0 | **on** |
| 1 | 1 | **on** |

### AND compared with OR

| Button 1 | Button 2 | AND | OR |
|----------|----------|-----|-----|
| 0 | 0 | off | off |
| 0 | 1 | off | **on** |
| 1 | 0 | off | **on** |
| 1 | 1 | **on** | **on** |

The two gates only disagree in the middle two rows, when exactly one button is
pressed.

## Evidence

| # | File | What it shows | Measured value |
|---|------|---------------|----------------|
| E1 | img/ORGate.mp4 | The LED turns on with either button | Not recorded |

<video src="../../img/ORGate.mp4" width="100%" controls></video>

## Predicted vs measured

It worked as expected. Pressing button A, button B, or both turned the LED on.

## What went wrong

- At first, our button condition used AND instead of OR. We changed it to logical OR (`||`), and the LED worked.

## Code

`codigos/OrGate.c`

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
        if (sio_hw->gpio_in & BTN_BIT || (sio_hw->gpio_in & BTN2_BIT)) {
            sio_hw->gpio_set = LED_BIT;
        } else {
            sio_hw->gpio_clr = LED_BIT;
        }
    }
}
```

## Open question

- What is the difference between bitwise OR (`|`) and logical OR (`||`) in C?
