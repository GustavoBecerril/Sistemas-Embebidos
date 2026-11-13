
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