
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