---
title: "Lab Log - PWM Oscilloscope Sweep"
course: "Embedded Systems I"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

## Goal

Our goal was to change the PWM duty level on GP15 so we could see it on an oscilloscope.

## Prediction

We expected the PWM wave to get wider and narrower on the oscilloscope as the duty cycle changed.

## Setup

We connected GP15 to an oscilloscope input and connected the grounds. Our code sets the PWM clock divider to 10 and the wrap value to 4095.

## What I did

We set the PWM level to 3399 and changed it by 8 every 10 ms. When it reached either limit, our code changed direction so the level swept back and forth.

## Evidence

<video src="../../img_videos_parcial2/pwm_oscilloscope_sweep.mp4" width="100%" controls></video>
<video src="../../img_videos_parcial2/pwm_oscilloscope_sweep1.mp4" width="100%" controls></video>
<video src="../../img_videos_parcial2/pwm_oscilloscope_sweep2.mp4" width="100%" controls></video>
<video src="../../img_videos_parcial2/pwm_oscilloscope_sweep3.mp4" width="100%" controls></video>
<video src="../../img_videos_parcial2/pwm_oscilloscope_sweep4.mp4" width="100%" controls></video>

## Predicted vs measured

We saw the wave expand and contract smoothly. We recorded five videos to show it at different zoom levels and speeds.

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
