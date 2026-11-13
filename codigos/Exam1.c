// BECERRIL VILLAMIL GUSTAVO
// BERMUDEZ RUIZ URIEL ONAIRAM
// RIOS GARCIA CRISTIAN ULISES

#include <stdio.h>
#include "pico/stdlib.h"
 
#define BTN_STOP  0
#define BTN_RESET 1
const uint LED_PINS[4][5] = {
    {2, 3, 4, 5, 6},  
    {7, 8, 9, 10, 11},  
    {12, 13, 14, 15, 16},
    {17, 18, 19, 20, 21}  
};
 
volatile uint32_t ultimo = 0;
volatile int current_row = 0;        
volatile uint8_t grid[4] = {0, 0, 0, 0};
volatile uint8_t moving_block = 0b111;  
volatile int pos = 0;              
volatile int dir = 1;              
volatile bool game_over = false;
volatile bool won = false;
 
static void button_isr(uint gpio, uint32_t events) {
    uint32_t now = to_ms_since_boot(get_absolute_time());
   
    if (now - ultimo > 200) {
        if (gpio == BTN_STOP && !game_over && !won) {
 
            uint8_t current_mask = moving_block << pos;
           
            if (current_row == 0) {
 
                grid[current_row] = current_mask;
            } else {
 
                grid[current_row] = current_mask & grid[current_row - 1];
            }
 
            int leds_left = 0;
            for (int c = 0; c < 5; c++) {
                if (grid[current_row] & (1 << c)) leds_left++;
            }
           
            if (leds_left == 0) {
                game_over = true;
            } else {
                current_row++;
               
                if (current_row >= 4) {
                    won = true;
                } else {
                    moving_block = (1 << leds_left) - 1;
                    pos = 0;
                    dir = 1;
                }
            }
        }
        else if (gpio == BTN_RESET) {
            current_row = 0;
            grid[0] = grid[1] = grid[2] = grid[3] = 0;
            moving_block = 0b111;
            pos = 0;
            dir = 1;
            game_over = false;
            won = false;
        }
        ultimo = now;
    }
    gpio_acknowledge_irq(gpio, events);
}
 
int main(void) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 5; c++) {
            gpio_init(LED_PINS[r][c]);
            gpio_set_dir(LED_PINS[r][c], GPIO_OUT);
        }
    }
 
    gpio_init(BTN_STOP);
    gpio_set_dir(BTN_STOP, GPIO_IN);
    gpio_disable_pulls(BTN_STOP);
 
    gpio_init(BTN_RESET);
    gpio_set_dir(BTN_RESET, GPIO_IN);
    gpio_disable_pulls(BTN_RESET);
 
    gpio_set_irq_enabled_with_callback(BTN_STOP, GPIO_IRQ_EDGE_FALL, true, &button_isr);
    gpio_set_irq_enabled(BTN_RESET, GPIO_IRQ_EDGE_FALL, true);
 
    const uint32_t delay_times[4] = {300, 200, 120, 70};
    uint32_t last_change = to_ms_since_boot(get_absolute_time());
 
    while (true) {
        uint32_t now = to_ms_since_boot(get_absolute_time());
 
        if (!game_over && !won) {
            if (now - last_change >= delay_times[current_row]) {
                last_change = now;
               
                pos += dir;
               
                int width = 0;
                for(int b = 0; b < 5; b++) { if (moving_block & (1 << b)) width++; }
               
                int max_pos = 5 - width;
               
                if (pos >= max_pos) {
                    pos = max_pos;
                    dir = -1;
                } else if (pos <= 0) {
                    pos = 0;
                    dir = 1;
                }
            }
        }
 
        for (int r = 0; r < 4; r++) {
            uint8_t row_data = grid[r];
 
            if (r == current_row && !game_over && !won) {
                row_data = (moving_block << pos);
            }
           
            for (int c = 0; c < 5; c++) {
                if (row_data & (1 << c)) {
                    gpio_put(LED_PINS[r][c], 1);
                } else {
                    gpio_put(LED_PINS[r][c], 0);
                }
            }
        }
 
        if (won == true) {
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 5; c++) {
                    gpio_put(LED_PINS[r][c], 1);
                }
            }
            sleep_ms(200);
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 5; c++) {
                    gpio_put(LED_PINS[r][c], 0);
                }
            }
            sleep_ms(200);
        }
        sleep_ms(2);
    }
}

/* 
 * ==========================================
 * PIN MAPEO
 * ==========================================
 * Señal    | GPIO | Dirección | Pull Config | Propósito
 * ---------|------|-----------|-------------|-------------------
 * BTN_STOP | 0    | IN        | Ninguno     | Interrupción de parada
 * BTN_RESET| 1    | IN        | Ninguno     | Interrupción de reinicio
 * 
 * LEDs Fila 1 (Inferior):
 * LED 1-5  | 2-6  | OUT       | N/A         | Pantalla Fila 1
 * 
 * LEDs Fila 2:
 * LED 6-10 | 7-11 | OUT       | N/A         | Pantalla Fila 2
 * 
 * LEDs Fila 3:
 * LED 11-15| 12-16| OUT       | N/A         | Pantalla Fila 3
 * 
 * LEDs Fila 4 (Superior):
 * LED 16-20| 17-21| OUT       | N/A         | Pantalla Fila 4
 * ==========================================
 * Patrón Inicial: Fila 1 inicia con el bloque 0b111 (3 LEDs encendidos).
 * Estructura física: 4 filas x 5 columnas (Un LED por GPIO, sin multiplexar).
 */

 // Límite de corriente: Resistencias de 220 ohm usadas por pin para mantener la corriente total < 120mA.