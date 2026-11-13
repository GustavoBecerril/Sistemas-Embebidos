
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