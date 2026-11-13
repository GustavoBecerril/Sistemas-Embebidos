
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