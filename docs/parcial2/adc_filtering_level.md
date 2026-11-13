---
title: "Lab Log - ADC Filtering Level"
course: "Embedded Systems I"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

## Goal

Our goal was to average five ADC0 readings and show the voltage as a level from 0 to 100 percent.

## Prediction

We expect the average of five readings to give a steadier level than one reading alone.

## Setup

We connected the analog signal to GP26 (ADC0). We used the Pico SDK serial output to see the average, voltage, and level.

## What I did

We took five readings, with a 100 ms wait between readings. Our code averaged them, changed the average to volts, mapped it between 0.150 V and 0.300 V, limited the result to 0-100 percent, and printed it.

## Evidence

<video src="../../img_videos_parcial2/adc_filtering_level.mp4" width="100%" controls></video>

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
 
#define ADC_INPUT 0
#define V_BASELINE 0.150f
#define V_MAX 0.300f
#define SAMPLES 5
 
int main() {
    stdio_init_all();
    adc_init();
    
    adc_gpio_init(26);
    adc_select_input(ADC_INPUT);
 
    while (true) {
        uint32_t adc_sum = 0;
        
        for (int i = 0; i < SAMPLES; i++) {
            adc_sum += adc_read();
            sleep_ms(100);
        }
        
        uint16_t adc_avg = adc_sum / SAMPLES;
        float voltage = (adc_avg * 3.3f) / 4095.0f;
        float percentage = ((voltage - V_BASELINE) / (V_MAX - V_BASELINE)) * 100.0f;
        
        if (percentage < 0.0f) {
            percentage = 0.0f;
        } else if (percentage > 100.0f) {
            percentage = 100.0f;
        }
        
        printf("Promedio ADC: %u | Voltaje: %.3f V | Nivel: %.1f%%\n", adc_avg, voltage, percentage);
    }
}
```

## Open question

What happens to the level if we average more readings?
