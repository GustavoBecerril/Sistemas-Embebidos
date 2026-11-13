#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/structs/sio.h"
 
#define Led_Pin 0
#define Button_PIN 1 
#define Button_PIN2 2
 
int main(void)
{
    const uint32_t LED_BIT = 1u << Led_Pin;
    const uint32_t BTN_BIT = 1u << Button_PIN;
    const uint32_t BTN2_BIT = 1u << Button_PIN2;
 
    gpio_init(Led_Pin);
    gpio_init(Button_PIN);
    gpio_init(Button_PIN2);
 
    sio_hw->gpio_oe_set = LED_BIT;
    sio_hw->gpio_oe_clr = BTN_BIT;
    sio_hw->gpio_oe_clr = BTN2_BIT;
 
    gpio_disable_pulls(Button_PIN);
    gpio_disable_pulls(Button_PIN2);
 
    while (true) {
        if (sio_hw->gpio_in & BTN_BIT || (sio_hw->gpio_in & BTN2_BIT)) {
            sio_hw->gpio_set = LED_BIT;
        } else {
            sio_hw->gpio_clr = LED_BIT;
        }
    }
}