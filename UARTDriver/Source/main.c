#define PIN5 5
#define LED_PIN PIN5

#include "../Include/uart.h"

int main(void)
{
    UART2_init();

    // configure LED pin
    SET_BIT(GPIOA->MODER, 2 * LED_PIN);
    CLEAR_BIT(GPIOA->MODER, 2 * LED_PIN + 1);

    uint8_t received_byte;
    while (true)
    {
        if (UART2_read(&received_byte, 1) == 1)
        {
            if (received_byte == '1')
            {
                TOGGLE_PIN(GPIOA->ODR, LED_PIN);
                const uint8_t str[] = "\r\nLED Toggled!\r\n";
                while (!UART2_write(str, (uint8_t)strlen(str)))
                    ;
            }
            else
            {
                uint8_t echo_byte = received_byte + 1;

                // try to echo until successful
                while (!UART2_write_byte(&echo_byte))
                    ;
            }
        }
    }

    return 0;
}