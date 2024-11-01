#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../coresys/Includes/STM32F401.h"

/*

The ST-Link also has the capability to act as a bridge between the microcontroller and your computer's USB port.
This means that it can convert USB signals from your computer into USART signals for the microcontroller and vice versa.
When you send data from your computer (using a terminal program), it goes through the USB connection to the ST-Link, which translates it into USART2 signals and sends it to the STM32.
Similarly, data sent from the USART2 of STM32 can travel back to the computer through the ST-Link.

*/

// USART2 is connected to the APB1 bus
#define USART2_EN_BIT 17
// GPIOD is connected to the AHB1 bus
#define GPIOD_EN_BIT 3

#define SYS_CLOCK 16000000 // the default system clock (if clock tree not configured) on stm32 is 16MHz
// In the clock tree, the system clock is taken and then divided by a value; and then what is derived after this division
// is used as the clock for the other buses like AHB, APB; in the default state (if clock three is not configured), the divider for the
// buses is one.
// Thus, the peripheral clock for APB1, which our UART is connected to, has the same frequency as our system
#define APB1_CLOCK SYS_CLOCK

#define UART_BAUD_RATE 115200

#define TX_PIN 5

#define WORD_LENGTH_BIT 12
#define PARITY_CONTROL_BIT 10
/*

The parity control bit sets the hardware parity control (generation and detection)/
When the parity control is enabled, the computed parity is inserted at the MSB position (9th if word bit is 1, and 8th if its 0)
and parity is checked on the received data

*/

#define TRANSMITTER_ENABLE_BIT 3

#define STOP_BIT 12

#define USART_ENABLE_BIT 13

#define TXE_BIT 7 // transmit data register empty
// it is one if the transmit data register is empty and zero if it's not

// USART2 uses PD5 as it's TX when PD5 is configured on Alternate function 7 (AF07)

#define SET_BIT(reg, bit) ((reg) |= (1UL << (bit)))
#define CLEAR_BIT(reg, bit) ((reg) &= ~(1UL << (bit)))
#define ALL_CLEAR(reg) (reg &= 0x0)
#define READ_BIT(reg, bit) ((reg) & (1UL << (bit)))

void UART2_tx_init(void);
void UART2_write(char chr);

static inline void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t periph_clock, uint32_t baud_rate)
{
    USARTx->BRR = compute_uart_div(periph_clock, baud_rate);
}

static inline uint32_t compute_uart_div(uint32_t periph_clock, uint32_t baud_rate)
{
    return ((periph_clock + baud_rate / 2U) / baud_rate);
}

void UART2_tx_init(void)
{
    // configuring UART2 GPIO TX pin
    // enable clock access to GPIOD
    SET_BIT(RCC->AHB1ENR, GPIOD_EN_BIT);
    // set PD5 to alternate function mode
    CLEAR_BIT(GPIOD->MODER, TX_PIN * 2);
    SET_BIT(GPIOD->MODER, TX_PIN * 2 + 1);
    // set PD5 alternate function type to AF07 (UART_TX); AF07 is 0111
    SET_BIT(GPIOD->AFR[0], 4 * TX_PIN);
    SET_BIT(GPIOD->AFR[0], 4 * TX_PIN + 1);
    SET_BIT(GPIOD->AFR[0], 4 * TX_PIN + 2);
    CLEAR_BIT(GPIOD->AFR[0], 4 * TX_PIN + 3);

    // configuring UART2 module
    // enable clock access to UART2
    SET_BIT(RCC->AHB1ENR, USART2_EN_BIT);
    // configure baud rate
    uart_set_baudrate(USART2, APB1_CLOCK, UART_BAUD_RATE);
    // configure the transfer direction
    ALL_CLEAR(USART2->CR1);                       // for getting the default values for parameters we are not configuring; the reset value is all clear by default so we didn't need to do this
    CLEAR_BIT(USART2->CR1, WORD_LENGTH_BIT);      // 1 start bit, 8 data bits, n stop bits; n determined by CR2
    CLEAR_BIT(USART2->CR1, PARITY_CONTROL_BIT);   // disable parity control
    SET_BIT(USART2->CR1, TRANSMITTER_ENABLE_BIT); // enable the transmitter

    // 00 for 1 stop bit
    // we didn't need to do this since the reset value for CR2 is just all zeroes; we want that since we want default configuration
    CLEAR_BIT(USART2->CR2, STOP_BIT);
    CLEAR_BIT(USART2->CR2, STOP_BIT + 1);

    // enable the UART2 module
    SET_BIT(USART2->CR1, USART_ENABLE_BIT);
}

void UART2_write(char chr)
{
    // make sure the transmit data register is empty; wait until it is
    while (!READ_BIT(USART2->SR, TXE_BIT))
    {
    }

    // write to the transmit data register
    USART2->DR = chr;
}

int main(void)
{
    UART2_tx_init();
    while (true)
    {
        UART2_write('y');
    }
}