---
title: "Lab Log — AND Gate"
course: "Embedded Systems I (LIIE1214)"
student: "Becerril Villamil Gustavo; Bermudez Ruiz Uriel Onairam; Rios Garcia Cristian Ulises"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

# AND Gate

**Goal:** Our goal was to turn the LED on only when both buttons were pressed.

**Prediction — written *before* running anything:**

We expected the LED to turn on only when we pressed both buttons at the same time.

---

## Setup

We connected an LED to GP0 and buttons to GP1 and GP2. Our code turns off the internal pulls, so we used external pull-down resistors.

| Signal | GPIO | Direction | Pull | Purpose |
|--------|------|-----------|------|---------|
| LED | 0 | OUT | N/A | Shows the gate output |
| Button 1 | 1 | IN | none (external) | Input A |
| Button 2 | 2 | IN | none (external) | Input B |

## What I did

1. We connected two buttons and one LED to the Pico and breadboard.
2. We wrote code to read both buttons and control the LED.
3. We tested the input cases and fixed problems.

## How it works

An AND gate gives a `1` only when **all** of its inputs are `1`. This program
does that in software, by reading two pins and lighting one LED.

### Setting the pin directions

The code builds one bit mask per pin with `1u << pin`, and then uses two
different registers:

- `sio_hw->gpio_oe_set = LED_BIT` — **sets** the output enable bit, so GP0
  becomes an **output**.
- `sio_hw->gpio_oe_clr = BTN_BIT` — **clears** the output enable bit, so GP1
  becomes an **input**. The same is done for GP2.

`oe` means *output enable*. `set` writes ones, `clr` writes zeros.

### Reading the buttons

`sio_hw->gpio_in` is a register that holds the live state of every input pin,
one bit per pin. To test a single pin we use the bitwise AND operator:

```c
sio_hw->gpio_in & BTN_BIT
```

This gives a non-zero value when that one bit is high, and zero when it is low.
In C any non-zero value counts as true.

### The decision

```c
if (sio_hw->gpio_in & BTN_BIT && (sio_hw->gpio_in & BTN2_BIT)) {
    sio_hw->gpio_set = LED_BIT;   // LED on
} else {
    sio_hw->gpio_clr = LED_BIT;   // LED off
}
```

The `&&` is the **logical AND**. Both sides must be true, so the LED turns on
only when both buttons are pressed. In every other case the `else` branch runs
and the LED turns off.

There is no `sleep_ms()` here. The `while (true)` loop runs as fast as the CPU
can, so the LED reacts to the buttons with no visible delay.

!!! note "Two different AND operators"
    `&` is the **bitwise** AND. It works on the bits of a number and is used to
    pick one pin out of the register.
    `&&` is the **logical** AND. It works on true/false values and is used to
    join the two conditions. The AND gate needs both of them.

### Truth table

| Button 1 | Button 2 | LED |
|----------|----------|-----|
| 0 | 0 | off |
| 0 | 1 | off |
| 1 | 0 | off |
| 1 | 1 | **on** |

## Evidence

| # | File | What it shows | Measured value |
|---|------|---------------|----------------|
| E1 | img/ANDGate.mp4 | The LED turns on only with both buttons pressed | Not recorded |

<video src="../../img/ANDGate.mp4" width="100%" controls></video>

## Predicted vs measured

It worked as expected. The LED turned on only when both inputs were 1.

## What went wrong

- Our input pins were floating because we had not added pull-down resistors. The LED blinked at random until we added them.

## Code

`codigos/ANDGate.c`

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
        if (sio_hw->gpio_in & BTN_BIT && (sio_hw->gpio_in & BTN2_BIT)) {
            sio_hw->gpio_set = LED_BIT;
        } else {
            sio_hw->gpio_clr = LED_BIT;
        }
    }
}
```

## Open question

- How can we make a 3-input AND gate?
