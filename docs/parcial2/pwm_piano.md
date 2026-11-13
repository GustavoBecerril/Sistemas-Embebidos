---
title: "Lab Log - PWM Piano"
course: "Embedded Systems I"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

## Goal

Our goal was to play one of eight notes on a buzzer when we pressed its button, and stop the sound when no button was pressed.

## Prediction

We expected the buzzer to play different notes when we pressed different buttons, like a piano.

## Setup

We connected the buzzer to GP15 and eight buttons to GP2-GP9. Our code turns off the internal pulls, so the button circuits need external pull-down resistors. We also started serial output, but our code does not use it to play notes.

## What I did

We checked the buttons in order and played the matching frequency on GP15 for the first pressed button. Our code used a 50 percent PWM level for each note and set the level to zero when no button was pressed.

## Evidence

<video src="../../img_videos_parcial2/pwm_piano.mp4" width="100%" controls></video>

## Predicted vs measured

We heard good notes. Each button changed the PWM frequency and played a note from the scale.

## What went wrong

At first, we had no external pull-down resistors, so electrical noise caused random sounds. After we added the resistors, the buzzer played only when we pressed a button.

## Code

```c

// Tema: 3. Modulación por Ancho de Pulso (PWM)

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define BUZZER_PIN 15 

const uint BUTTON_PINS[8] = {2, 3, 4, 5, 6, 7, 8, 9}; 

const float NOTES[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

#define TOP 65535

void play_tone(uint slice, uint chan, float freq) {
    if (freq == 0) {
        pwm_set_chan_level(slice, chan, 0);
    } else {
        float f_clk = 150000000.0f; 
        float div = f_clk / (freq * (TOP + 1));
        
        pwm_set_clkdiv(slice, div);
        pwm_set_wrap(slice, TOP);
        
        pwm_set_chan_level(slice, chan, TOP / 2);
    }
}

int main() {
    stdio_init_all();

    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint chan  = pwm_gpio_to_channel(BUZZER_PIN);
    
    pwm_set_enabled(slice, true);
    play_tone(slice, chan, 0); 

    for (int i = 0; i < 8; i++) {
        gpio_init(BUTTON_PINS[i]);
        gpio_set_dir(BUTTON_PINS[i], GPIO_IN);
        gpio_disable_pulls(BUTTON_PINS[i]); 
    }

    while (true) {
        bool note_played = false;

        for (int i = 0; i < 8; i++) {
            if (gpio_get(BUTTON_PINS[i])) { 
                play_tone(slice, chan, NOTES[i]); 
                note_played = true;
                break; 
            }
        }

        if (!note_played) {
            play_tone(slice, chan, 0);
        }

        sleep_ms(10); 
    }
}
```

## Open question

Can we change the code to play a song without pressing any buttons?
