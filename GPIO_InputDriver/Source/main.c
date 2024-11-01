#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../coresys/Includes/STM32F401.h"

// Pin definitions
#define GPIOA_EN_BIT 0
#define GPIOC_EN_BIT 2
#define LED_PIN 5
#define BTN_PIN 13

// Utility macros
#define SET_BIT(reg, bit) ((reg) |= (1UL << (bit)))
#define CLEAR_BIT(reg, bit) ((reg) &= ~(1UL << (bit)))
#define READ_BIT(reg, bit) ((reg) & (1UL << (bit)))

int main(void)
{
    // Enable clock access to GPIOA and GPIOC
    SET_BIT(RCC->AHB1ENR, GPIOA_EN_BIT); // Enable GPIOA clock
    SET_BIT(RCC->AHB1ENR, GPIOC_EN_BIT); // Enable GPIOC clock

    // Configure PA5 (LED) as output
    // Clear bits first then set required bit
    CLEAR_BIT(GPIOA->MODER, (LED_PIN * 2 + 1)); // Clear bit 11
    SET_BIT(GPIOA->MODER, (LED_PIN * 2));       // Set bit 10

    // Configure PC13 (Button) as input
    // Input mode is 00, so just clear both bits
    CLEAR_BIT(GPIOC->MODER, (BTN_PIN * 2));     // Clear bit 26
    CLEAR_BIT(GPIOC->MODER, (BTN_PIN * 2 + 1)); // Clear bit 27

    while (true)
    {
        // Button is active low
        if (READ_BIT(GPIOC->IDR, BTN_PIN))
        {
            // Turn LED on by setting bit in BSRR lower half
            SET_BIT(GPIOA->BSRR, LED_PIN);
        }
        else
        {
            // Turn LED off by setting bit in BSRR upper half
            SET_BIT(GPIOA->BSRR, (LED_PIN + 16));
        }
    }
}