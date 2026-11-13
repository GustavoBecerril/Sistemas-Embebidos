---
title: "Lab Log - ADC Temperature"
course: "Embedded Systems I"
board: "Raspberry Pi Pico 2 (RP2350)"
status: "published"
---

## Goal

Our goal was to estimate temperature from the voltage read on ADC0.

## Prediction

We expected the serial monitor to show a normal room temperature and go up when we touched the sensor with our fingers.

## Setup

We connected the analog temperature sensor to GP26 (ADC0). We used the Pico SDK serial output to see the voltage and temperature.

## What I did

We read ADC0 once each second. Our code converted the reading to volts and divided by 0.01 to estimate the temperature. We printed both values.

## Evidence

<video src="../../img_videos_parcial2/adc_temperature.mp4" width="100%" controls></video>

## Predicted vs measured

Our math worked perfectly. The screen showed around 25 C, and when we held the sensor, the temperature slowly increased to 30 C.

## What went wrong

At first, our math formula was a bit off, so the screen showed very high values like 80 C. We fixed the conversion formula in the code, and it worked fine.

## Code

```c

// Tema: 2. Convertidor Analógico-Digital (ADC)

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
 
int main() {
    stdio_init_all();
    adc_init();
    
    adc_gpio_init(26);
    adc_select_input(0);
 
    const float conversion_factor = 3.3f / (1 << 12);
 
    while (true) {
        uint16_t raw_reading = adc_read();
        
        float voltage = raw_reading * conversion_factor;
        
        float temperature_c = voltage / 0.01f;
 
        printf("Voltaje: %.3f V | Temperatura: %.2f C\n", voltage, temperature_c);
 
        sleep_ms(1000);
    }
 
    return 0;
}
```

## Open question

Can we use this temperature sensor to automatically turn on a cooling fan when it gets too hot?
