// where is the LED connected?
// Port: A
// Pin: 5

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define PERIPH_BASE (0x40000000UL) // base for all the peripherals

#define AHB1_PERIPH_OFFSET (0x20000UL)
#define AHB1_BASE (PERIPH_BASE + AHB1_PERIPH_OFFSET) // base for all the peripherals on the AHB1 bus

#define GPIOA_AHB1_OFFSET (0x0UL)
#define GPIOA_BASE (AHB1_BASE + GPIOA_AHB1_OFFSET) // base for the GPIOA port configuration

#define RCC_AHB1_OFFSET (0x3800UL)
#define RCC_BASE (AHB1_BASE + RCC_AHB1_OFFSET) // base of the RCC peripheral configuration

// setting the bits [11:10] of the GPIOA Mode register to 01 puts the 5th pin of GPIOA into output mode

#define PIN5_OUT ((01UL) << 10)

#define GPIOA_EN (1UL << 0) // first bit in the AHB1 enable register is for enabling GPIOA
#define PIN5 (1UL << 5)     // 5th bit in the GPIOA output data register is for sending data to PIN5

#define __IO volatile

typedef struct
{
    __IO uint32_t MODER; // mode register
    __IO uint32_t DUMMY[4];
    __IO uint32_t ODR;
} GPIO_;

typedef struct
{
    __IO uint32_t DUMMY[12];
    __IO uint32_t AHB1ENR;
} RCC_;

#define RCC ((RCC_ *)RCC_BASE)
#define GPIOA ((GPIO_ *)GPIOA_BASE)

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
        GPIOA->ODR ^= PIN5; // XOR just inverts the bit number in OD_R that is set to 1 in PIN5
        for (volatile size_t i = 0; i < 1000000; i++)
            ;
    }
}
