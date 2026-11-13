//Codigo 1: LEDS Numero en aumento 
 
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

    for (int i = 0; i < 16; i++) {

        sio_hw->gpio_clr = Mask;            

        sio_hw->gpio_set = i;

        sleep_ms(300);

    }

   }

}
 