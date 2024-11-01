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

#define GPIOA_EN (1UL << 0)
#define PIN5 (1UL << 5)

#define GPIOC_EN (1UL << 2)
#define PIN13 (1UL << 13)

#define BTN_PIN PIN13
#define LED_PIN PIN5

int main(void)
{
    // enable clock access to GPIOA and GPIOC
    RCC->AHB1ENR |= GPIOC_EN;
    RCC->AHB1ENR |= GPIOA_EN;

    // set PA5 as output pin (can be done in MODE register of GPIOA)
    // for output: 01
    // for pin 5: bit 11 bit 10
    GPIOA->MODER |= (1UL << 10);    // sets the 10th bit to 1
    GPIOA->MODER &= (~(1UL << 11)); // sets the 11th bit to 0

    // set PC13 as input pin
    // for input: 00
    // for pin 13: bit 27 bit 26
    GPIOC->MODER &= (~(1UL << 26));
    GPIOC->MODER &= (~(1UL << 27));
    // we didn't really need to do this since the reset state of GPIOC MODE register is all zeroes

    
}
