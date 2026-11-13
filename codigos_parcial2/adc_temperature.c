
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