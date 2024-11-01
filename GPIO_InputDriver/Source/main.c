// where is the LED connected?
// Port: A
// Pin: 5

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../coresys/Includes/STM32F401.h"

// User Button is on Port C Pin 13
// So we need to enable clock access to this Port C

// Port C is on AHB1 bus; We need to enable clock access to it in the RCC-AHB1ENR register

// Bit two of RCC-AHB1ENR (if set) enables the clock access to GPIOC

#define GPIOC_ENR (1UL << 2)


int main(void)
{
    RCC->AHB1ENR |= GPIOC_ENR;
}
