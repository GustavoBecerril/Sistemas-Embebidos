
// Tema: 1. Interrupciones y Temporizadores (Timers & ISRs)

#include "pico/stdlib.h"
#include "hardware/gpio.h"
 
#define LED_PIN 4  
#define BTN_PIN 0   
 
static void button_isr(uint gpio, uint32_t events) {
    if (gpio == BTN_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
        gpio_xor_mask(1u << LED_PIN);
    }
    gpio_acknowledge_irq(gpio, events);  
}
 
int main(void) {
    stdio_init_all();
 
    // LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
 
    // Button 
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_disable_pulls(BTN_PIN);  
 
    // Interrupt on falling edge
    gpio_set_irq_enabled_with_callback(BTN_PIN,
                                       GPIO_IRQ_EDGE_FALL,
                                       true,
                                       &button_isr);
 
    while (true) {
        tight_loop_contents();
    }
}