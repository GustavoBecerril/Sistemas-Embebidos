---
title: "Lab Log - ADC Potentiometer"
course: "Embedded Systems I"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

## Goal

Our goal was to read the potentiometer on ADC1 and show its value as an angle from 0 to 270 degrees.

## Prediction

We expect the reported angle to rise as we turn the potentiometer toward its high end.

## Setup

We connected the potentiometer signal to GP27 (ADC1). We used the Pico SDK serial output to see the reading, voltage, and angle.

## What I did

We read ADC1 on GP27 every 200 ms. Our code changed each reading to volts, mapped it to an angle from 0 to 270 degrees, and printed the values.

## Evidence

<video src="../../img_videos_parcial2/adc_potentiometer.mp4" width="100%" controls></video>

## Predicted vs measured

We did not record measured values for this practice, so we cannot compare them with our prediction.

## What went wrong

Our notes do not record a problem for this practice.

## Code

```c

// Tema: 2. Convertidor Analógico-Digital (ADC)

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
 
#define ADC_INPUT 1
#define MAX_ANGLE 270.0f
 
int main() {
    stdio_init_all();
    adc_init();
    
    adc_gpio_init(27);
    adc_select_input(ADC_INPUT);
 
    while (true) {
        int adc = adc_read();
        
        float voltage = (adc * 3.3f) / 4095.0f;
        float angle = ((float)adc / 4095.0f) * MAX_ANGLE;
        
        printf("ADC: %d | Voltaje: %.3f V | Angulo: %.1f°\n", adc, voltage, angle);
        
        sleep_ms(200);
    }
}
```

## Open question

How can we calibrate the reported angle to match our potentiometer?
