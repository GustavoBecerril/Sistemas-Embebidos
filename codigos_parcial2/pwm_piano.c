
// Tema: 3. Modulación por Ancho de Pulso (PWM)

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define BUZZER_PIN 15 

const uint BUTTON_PINS[8] = {2, 3, 4, 5, 6, 7, 8, 9}; 

const float NOTES[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

#define TOP 65535

void play_tone(uint slice, uint chan, float freq) {
    if (freq == 0) {
        pwm_set_chan_level(slice, chan, 0);
    } else {
        float f_clk = 150000000.0f; 
        float div = f_clk / (freq * (TOP + 1));
        
        pwm_set_clkdiv(slice, div);
        pwm_set_wrap(slice, TOP);
        
        pwm_set_chan_level(slice, chan, TOP / 2);
    }
}

int main() {
    stdio_init_all();

    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint chan  = pwm_gpio_to_channel(BUZZER_PIN);
    
    pwm_set_enabled(slice, true);
    play_tone(slice, chan, 0); 

    for (int i = 0; i < 8; i++) {
        gpio_init(BUTTON_PINS[i]);
        gpio_set_dir(BUTTON_PINS[i], GPIO_IN);
        gpio_disable_pulls(BUTTON_PINS[i]); 
    }

    while (true) {
        bool note_played = false;

        for (int i = 0; i < 8; i++) {
            if (gpio_get(BUTTON_PINS[i])) { 
                play_tone(slice, chan, NOTES[i]); 
                note_played = true;
                break; 
            }
        }

        if (!note_played) {
            play_tone(slice, chan, 0);
        }

        sleep_ms(10); 
    }
}