#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/structs/sio.h"
 
 
#define PIN_A 0
#define PIN_B 1
#define PIN_C 2
#define PIN_D 3
 
int main(void)
{
    const uint32_t Mask =
        (1u << PIN_A) |
        (1u << PIN_B) |
        (1u << PIN_C) |
        (1u << PIN_D);
 
    gpio_init(PIN_A);
    gpio_init(PIN_B);
    gpio_init(PIN_C);
    gpio_init(PIN_D);
 
    sio_hw->gpio_oe_set = Mask;
 
    while (true) {
        sio_hw->gpio_clr = Mask; 
        sio_hw->gpio_set = 1;  
        sleep_ms(300);
        sio_hw->gpio_clr = Mask; 
        sio_hw->gpio_set = 3;  
        sleep_ms(300); 
        sio_hw->gpio_clr = Mask; 
        sio_hw->gpio_set = 7;  
        sleep_ms(300);
        sio_hw->gpio_clr = Mask; 
        sio_hw->gpio_set = 15; 
        sleep_ms(300);
 
        sio_hw->gpio_clr = Mask; 
        sio_hw->gpio_set = 14; 
        sleep_ms(300);
        sio_hw->gpio_clr = Mask;         
        sio_hw->gpio_set = 12; 
        sleep_ms(300); 
        sio_hw->gpio_clr = Mask; 
        sio_hw->gpio_set = 8;  
        sleep_ms(300); 
        sio_hw->gpio_clr = Mask; 
        sio_hw->gpio_set = 0;  
        sleep_ms(300);
    }
}