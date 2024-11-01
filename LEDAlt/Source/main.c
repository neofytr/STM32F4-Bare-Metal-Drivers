// where is the LED connected?
// Port: A
// Pin: 5

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../coresys/Includes/STM32F401.h"

#define GPIOA_EN (1UL << 0) // first bit in the AHB1 enable register is for enabling GPIOA
#define PIN5 (1UL << 5)     // 5th bit in the GPIOA output data register is for sending data to PIN5
#define PIN5_OUT (01UL << 10)

#define __IO volatile


int main(void)
{
    // 1. enable clock access to GPIOA
    RCC->AHB1ENR |= GPIOA_EN;
    // 2. set PA5 as an output pin
    GPIOA->MODER |= PIN5_OUT;

    while (true)
    {
        // 4. Toggle PA5
        // GPIOA_OD_R ^= PIN5
        // GPIOA->ODR ^= PIN5; // XOR just inverts the bit number in OD_R that is set to 1 in PIN5
        GPIOA->BSRR = PIN5;
        for (volatile size_t i = 0; i < 1000000; i++)
            ;

        GPIOA->BSRR = (1UL << 21);
        for (volatile size_t i = 0; i < 1000000; i++)
            ;
    }
}
