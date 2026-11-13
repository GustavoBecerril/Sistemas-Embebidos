---
title: "Lab Log — Exam 1: Stacker Game"
course: "Embedded Systems I (LIIE1214)"
student: "Becerril Villamil Gustavo; Bermudez Ruiz Uriel Onairam; Rios Garcia Cristian Ulises"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

# Exam 1 — Stacker Game on a 4×5 LED Matrix

**Team:**

- Becerril Villamil Gustavo
- Bermudez Ruiz Uriel Onairam
- Rios Garcia Cristian Ulises

**Goal (one sentence):** Our goal was to build a Stacker game with a moving LED block, a Stop button, and a Reset button.

**Prediction — written *before* running anything:**

We expected to build a working Stacker game with button interrupts and bit operations to stack the LEDs.

---

## Setup

We used 20 LEDs in four rows and five columns, plus two buttons. Each LED had its own GPIO pin. We connected Stop to GP0, Reset to GP1, and the LEDs to GP2-GP21. Our code turns off the internal pulls, so we used external pull-up resistors for the buttons. We used one 220-ohm resistor for each LED.

| Signal | GPIO | Direction | Pull | Purpose |
|--------|------|-----------|------|---------|
| BTN_STOP | 0 | IN | none (external) | Interrupt: stop the moving block |
| BTN_RESET | 1 | IN | none (external) | Interrupt: restart the game |
| Row 1 LEDs (bottom) | 2–6 | OUT | N/A | Display row 1 |
| Row 2 LEDs | 7–11 | OUT | N/A | Display row 2 |
| Row 3 LEDs | 12–16 | OUT | N/A | Display row 3 |
| Row 4 LEDs (top) | 17–21 | OUT | N/A | Display row 4 |

We stored the LED pin numbers in a 2D array, so our code could use row and column numbers.

```c
const uint LED_PINS[4][5] = {
    {2, 3, 4, 5, 6},
    {7, 8, 9, 10, 11},
    {12, 13, 14, 15, 16},
    {17, 18, 19, 20, 21}
};
```

## What I did

1. We connected the 4-by-5 LED matrix and two buttons.
2. We used an interrupt to stop the moving block and another to reset the game.
3. We used bit masks to keep the LEDs that matched the row below.

## How the game works

The player has a block of lit LEDs that slides left and right on the current
row. When the player presses **Stop**, the block freezes. Only the part of the
block that sits on top of the row below stays lit. Everything that hangs over
the edge is lost.

The block gets smaller as you go up, so the game gets harder. Win by reaching
the top. Lose if the block is cut down to zero LEDs.

## How the code works

### 1. The game state

The game is kept in a few global variables. They are marked `volatile` because
the interrupt changes them while the main loop is reading them. Without
`volatile` the compiler could keep an old copy in a register and miss the
change.

| Variable | Meaning |
|----------|---------|
| `grid[4]` | One byte per row. Each bit is one LED that was already locked in place |
| `current_row` | Which row the player is on now (0 is the bottom) |
| `moving_block` | The shape that slides, as bits. It starts at `0b111`, three LEDs |
| `pos` | How far to the right the block is shifted |
| `dir` | `+1` moving right, `-1` moving left |
| `game_over` / `won` | End of game flags |

### 2. The moving block

The block is drawn with a shift:

```c
row_data = (moving_block << pos);
```

If `moving_block` is `0b111` and `pos` is `1`, the result is `0b1110`. So the
same three LEDs appear one step to the right. Moving the block is just changing
one number.

The main loop changes `pos` on a timer:

```c
pos += dir;
```

When the block reaches an edge it stops and turns around by flipping `dir`:

```c
if (pos >= max_pos) { pos = max_pos; dir = -1; }
else if (pos <= 0)  { pos = 0;       dir = 1;  }
```

`max_pos` is `5 - width`, so the block never slides off the row.

### 3. The speed gets faster

Each row has its own delay:

```c
const uint32_t delay_times[4] = {300, 200, 120, 70};
```

Row 1 moves every 300 ms, row 4 every 70 ms. The higher you go, the harder it
is to stop the block where you want it.

The loop does **not** use `sleep_ms()` to wait for the step. It compares the
current time with the last step time:

```c
if (now - last_change >= delay_times[current_row]) { ... }
```

This is important. `sleep_ms()` would block the program, and the LEDs would
stop being refreshed. This way the display keeps updating while the block
waits for its next step.

### 4. The buttons use interrupts

Both buttons are connected to the same interrupt handler:

```c
gpio_set_irq_enabled_with_callback(BTN_STOP, GPIO_IRQ_EDGE_FALL, true, &button_isr);
gpio_set_irq_enabled(BTN_RESET, GPIO_IRQ_EDGE_FALL, true);
```

`GPIO_IRQ_EDGE_FALL` means the interrupt fires on a **falling edge**: the
moment the pin goes from high to low, which is when the button is pressed.

The callback is registered only once, with the first call. The second call just
adds the other pin to the same handler. Inside the handler, the `gpio` argument
tells which button was pressed.

### 5. Debounce in software

A real button does not make one clean edge. The metal contacts bounce and can
fire several interrupts in a few milliseconds. The code blocks that:

```c
uint32_t now = to_ms_since_boot(get_absolute_time());
if (now - ultimo > 200) {
    // ... handle the press ...
    ultimo = now;
}
```

Any press that arrives less than 200 ms after the last one is ignored. This is
a simple and very common debounce.

### 6. Cutting the block — the key line

This is the heart of the game:

```c
uint8_t current_mask = moving_block << pos;

if (current_row == 0) {
    grid[current_row] = current_mask;
} else {
    grid[current_row] = current_mask & grid[current_row - 1];
}
```

On the bottom row there is nothing below, so the whole block is saved.

On any other row the block is combined with the row below using the **bitwise
AND** (`&`). A bit survives only if it is `1` in both rows — that is, only where
the new block sits exactly on top of a locked LED. Every LED that hangs over the
edge becomes `0` and is lost.

Example on row 2:

| | Bits | LEDs |
|---|------|------|
| Row below (`grid[0]`) | `01110` | `. * * * .` |
| New block (`current_mask`) | `00111` | `. . * * *` |
| Result (`&`) | `00110` | `. . * * .` |

The player started with 3 LEDs and keeps only 2.

### 7. Counting what is left

After the AND, the code counts the surviving bits:

```c
int leds_left = 0;
for (int c = 0; c < 5; c++) {
    if (grid[current_row] & (1 << c)) leds_left++;
}
```

- If `leds_left == 0`, nothing matched and `game_over` becomes true.
- If not, the game moves up one row and the block is rebuilt with the new,
  smaller width:

  ```c
  moving_block = (1 << leds_left) - 1;
  ```

  `(1 << 2) - 1` is `0b11`, two LEDs. `(1 << 3) - 1` is `0b111`, three LEDs.
  This trick always gives a solid row of ones.

- If `current_row` reaches 4, the player has passed the top row and `won`
  becomes true.

### 8. Drawing the screen

Every pass of the main loop redraws all 20 LEDs:

```c
for (int r = 0; r < 4; r++) {
    uint8_t row_data = grid[r];
    if (r == current_row && !game_over && !won) {
        row_data = (moving_block << pos);   // the live block replaces the row
    }
    for (int c = 0; c < 5; c++) {
        gpio_put(LED_PINS[r][c], (row_data & (1 << c)) ? 1 : 0);
    }
}
```

Locked rows come from `grid`. The row the player is on shows the moving block
instead. The `sleep_ms(2)` at the end keeps the refresh rate around 500 Hz,
which the eye sees as steady light.

When `won` is true, the whole matrix blinks on and off every 200 ms.

### 9. The reset button

`BTN_RESET` puts every variable back to its starting value: row 0, empty grid,
a 3-LED block at position 0, and both end flags cleared. It works at any time,
even after the game is over.

## Evidence

| # | File | What it shows | Measured value |
|---|------|---------------|----------------|
| E1 | img/Exam.mp4 | A full game: block sliding, stopping, being cut, and the win blink | Not recorded |

<video src="../../img/Exam.mp4" width="100%" controls></video>

## Predicted vs measured

The game worked and was fun to play. We used `volatile` variables and hardware interrupts to stop the moving blocks.

## What went wrong

- We built a 4-row by 5-column matrix instead of a 5-row by 4-column matrix. We used a 2D array for the LED pin map and one byte for each row because of the time limit. We explained this choice in the oral exam.

## Code

`codigos/Exam1.c`

```c
// BECERRIL VILLAMIL GUSTAVO
// BERMUDEZ RUIZ URIEL ONAIRAM
// RIOS GARCIA CRISTIAN ULISES

#include <stdio.h>
#include "pico/stdlib.h"

#define BTN_STOP  0
#define BTN_RESET 1
const uint LED_PINS[4][5] = {
    {2, 3, 4, 5, 6},
    {7, 8, 9, 10, 11},
    {12, 13, 14, 15, 16},
    {17, 18, 19, 20, 21}
};

volatile uint32_t ultimo = 0;
volatile int current_row = 0;
volatile uint8_t grid[4] = {0, 0, 0, 0};
volatile uint8_t moving_block = 0b111;
volatile int pos = 0;
volatile int dir = 1;
volatile bool game_over = false;
volatile bool won = false;

static void button_isr(uint gpio, uint32_t events) {
    uint32_t now = to_ms_since_boot(get_absolute_time());

    if (now - ultimo > 200) {
        if (gpio == BTN_STOP && !game_over && !won) {

            uint8_t current_mask = moving_block << pos;

            if (current_row == 0) {

                grid[current_row] = current_mask;
            } else {

                grid[current_row] = current_mask & grid[current_row - 1];
            }

            int leds_left = 0;
            for (int c = 0; c < 5; c++) {
                if (grid[current_row] & (1 << c)) leds_left++;
            }

            if (leds_left == 0) {
                game_over = true;
            } else {
                current_row++;

                if (current_row >= 4) {
                    won = true;
                } else {
                    moving_block = (1 << leds_left) - 1;
                    pos = 0;
                    dir = 1;
                }
            }
        }
        else if (gpio == BTN_RESET) {
            current_row = 0;
            grid[0] = grid[1] = grid[2] = grid[3] = 0;
            moving_block = 0b111;
            pos = 0;
            dir = 1;
            game_over = false;
            won = false;
        }
        ultimo = now;
    }
    gpio_acknowledge_irq(gpio, events);
}

int main(void) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 5; c++) {
            gpio_init(LED_PINS[r][c]);
            gpio_set_dir(LED_PINS[r][c], GPIO_OUT);
        }
    }

    gpio_init(BTN_STOP);
    gpio_set_dir(BTN_STOP, GPIO_IN);
    gpio_disable_pulls(BTN_STOP);

    gpio_init(BTN_RESET);
    gpio_set_dir(BTN_RESET, GPIO_IN);
    gpio_disable_pulls(BTN_RESET);

    gpio_set_irq_enabled_with_callback(BTN_STOP, GPIO_IRQ_EDGE_FALL, true, &button_isr);
    gpio_set_irq_enabled(BTN_RESET, GPIO_IRQ_EDGE_FALL, true);

    const uint32_t delay_times[4] = {300, 200, 120, 70};
    uint32_t last_change = to_ms_since_boot(get_absolute_time());

    while (true) {
        uint32_t now = to_ms_since_boot(get_absolute_time());

        if (!game_over && !won) {
            if (now - last_change >= delay_times[current_row]) {
                last_change = now;

                pos += dir;

                int width = 0;
                for(int b = 0; b < 5; b++) { if (moving_block & (1 << b)) width++; }

                int max_pos = 5 - width;

                if (pos >= max_pos) {
                    pos = max_pos;
                    dir = -1;
                } else if (pos <= 0) {
                    pos = 0;
                    dir = 1;
                }
            }
        }

        for (int r = 0; r < 4; r++) {
            uint8_t row_data = grid[r];

            if (r == current_row && !game_over && !won) {
                row_data = (moving_block << pos);
            }

            for (int c = 0; c < 5; c++) {
                if (row_data & (1 << c)) {
                    gpio_put(LED_PINS[r][c], 1);
                } else {
                    gpio_put(LED_PINS[r][c], 0);
                }
            }
        }

        if (won == true) {
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 5; c++) {
                    gpio_put(LED_PINS[r][c], 1);
                }
            }
            sleep_ms(200);
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 5; c++) {
                    gpio_put(LED_PINS[r][c], 0);
                }
            }
            sleep_ms(200);
        }
        sleep_ms(2);
    }
}
```

### Pin map (from the comment block in the source file)

```text
==========================================
PIN MAP
==========================================
Signal    | GPIO | Direction | Pull config | Purpose
----------|------|-----------|-------------|-------------------
BTN_STOP  | 0    | IN        | None        | Stop interrupt
BTN_RESET | 1    | IN        | None        | Reset interrupt

Row 1 LEDs (bottom):
LED 1-5   | 2-6  | OUT       | N/A         | Display row 1

Row 2 LEDs:
LED 6-10  | 7-11 | OUT       | N/A         | Display row 2

Row 3 LEDs:
LED 11-15 | 12-16| OUT       | N/A         | Display row 3

Row 4 LEDs (top):
LED 16-20 | 17-21| OUT       | N/A         | Display row 4
==========================================
Starting pattern: row 1 starts with the block 0b111 (3 LEDs on).
Physical structure: 4 rows x 5 columns (one LED per GPIO, no multiplexing).
Current limit: 220 ohm resistors per pin to keep the total current under 120 mA.
```

## Open question

- How could we add a small speaker to play a sound when the player wins or loses?
