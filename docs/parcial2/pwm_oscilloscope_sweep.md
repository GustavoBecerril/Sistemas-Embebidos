---
title: "Lab Log - PWM Oscilloscope Sweep"
course: "Embedded Systems I"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

## Goal

Our goal was to see how PWM settings change the signal on an oscilloscope.

## Prediction

We expected changing level to change the duty cycle. We expected changing TOP or div to affect the signal timing and frequency. We did not predict exact frequency values.

## Setup

We connected GP15 to an oscilloscope input and connected the grounds. The code shown below is our base code, with div = 10 and TOP = 4095. The five videos show tests with selected PWM settings changed.

## What I did

We used one base program for this practice and kept one code listing. The base code starts at level 3399, changes it by 8 every 10 ms, and reverses direction at the limits. For the videos, we changed selected parameter values between tests instead of making five copies of the code.

- TOP is the highest counter value before it returns to zero. With the clock and divider fixed, changing TOP changes the PWM period and the number of available duty steps.
- level is the compare value. It controls how long the PWM output stays high in each period, so it changes the duty cycle.
- div divides the system clock for the PWM counter. A larger div makes the counter run more slowly and lowers the PWM frequency.

## Evidence

**Video 1: TOP = 4095, level = 2252.** This is a duty-level test.

<video src="../../img_videos_parcial2/pwm_oscilloscope_sweep.mp4" width="100%" controls></video>

**Video 2: TOP = 4095, level = 819.** This is another duty-level test with a lower level value.

<video src="../../img_videos_parcial2/pwm_oscilloscope_sweep1.mp4" width="100%" controls></video>

**Video 3: TOP = 255.** This test changes the counter wrap value.

<video src="../../img_videos_parcial2/pwm_oscilloscope_sweep2.mp4" width="100%" controls></video>

**Video 4: div = 10, TOP = 1023.** This test uses these divider and wrap settings.

<video src="../../img_videos_parcial2/pwm_oscilloscope_sweep3.mp4" width="100%" controls></video>

**Video 5: div = 1, TOP = 1023.** TOP stays at 1023 while div is lower than in Video 4. With the same system clock, a lower divider makes the counter run faster and raises the PWM frequency.

<video src="../../img_videos_parcial2/pwm_oscilloscope_sweep4.mp4" width="100%" controls></video>

## Predicted vs measured

We saw the waveform change while we tested different PWM settings. The level tests changed the pulse width. We did not record numerical frequency measurements.

## What went wrong

It was hard to keep the wave steady on the oscilloscope. It moved around until we adjusted the trigger settings.

## Code

```c

// Tema: 3. Modulación por Ancho de Pulso (PWM)

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PWM_PIN 15

int main() {
    stdio_init_all();
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    
    uint slice = pwm_gpio_to_slice_num(PWM_PIN);
    uint chan = pwm_gpio_to_channel(PWM_PIN);

    // Valores que cambiaste en tus pruebas para el osciloscopio
    float div = 10.0f; 
    uint16_t top = 4095; 

    pwm_set_clkdiv(slice, div);
    pwm_set_wrap(slice, top);
    pwm_set_enabled(slice, true);

    int level = 3399;
    int step = 8;
    int dir = step;

    while (true) {
        pwm_set_chan_level(slice, chan, level);

        level += dir;
        
        if (level >= top) {
            level = top;
            dir = -step;
        } else if (level <= 0) {
            level = 0;
            dir = step;
        }
        
        sleep_ms(10);
    }
}
```

## Open question

Could we use this signal to fade an LED in and out?
