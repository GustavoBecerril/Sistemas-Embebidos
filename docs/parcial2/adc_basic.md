---
title: "Lab Log - ADC Basic"
course: "Embedded Systems I"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

## Goal

Our goal was to read an analog voltage on ADC0 and print its raw reading and calculated voltage.

## Prediction

We expect the ADC reading and voltage to change when we change the input.

## Setup

We connected the analog signal to GP26 (ADC0). We used the Pico SDK serial output to see the readings.

## What I did

We started ADC0 and read it in a loop. Our code converted each reading to volts using a 3.3 V reference, printed both values, and waited 200 ms.

## Evidence

<video src="../../img_videos_parcial2/adc_basic.mp4" width="100%" controls></video>

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
 
int main() {
    stdio_init_all();
    adc_init();
    
    adc_gpio_init(26);
    adc_select_input(ADC_INPUT);
 
    while (true) {
        uint16_t adc = adc_read();
        
        float voltage = (adc * 3.3f) / 4095.0f;
        
        printf("ADC: %u | Voltaje: %.3f V\n", adc, voltage);
        
        sleep_ms(200);
    }
}
```

## Open question

How can we make small changes in the ADC reading easier to see?
