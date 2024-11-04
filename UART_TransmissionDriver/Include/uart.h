#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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
// GPIOA is connected to the AHB1 bus
#define GPIOA_EN_BIT 0

#define SYS_CLOCK 16000000 // the default system clock (if clock tree not configured) on stm32 is 16MHz
// In the clock tree, the system clock is taken and then divided by a value; and then what is derived after this division
// is used as the clock for the other buses like AHB, APB; in the default state (if clock three is not configured), the divider for the
// buses is one.
// Thus, the peripheral clock for APB1, which our UART is connected to, has the same frequency as our system
#define APB1_CLOCK SYS_CLOCK

#define UART_BAUD_RATE 115200

#define TX_PIN 2

#define WORD_LENGTH_BIT 12
#define PARITY_CONTROL_BIT 10
/*

The parity control bit sets the hardware parity control (generation and detection)/
When the parity control is enabled, the computed parity is inserted at the MSB position (9th if word bit is 1, and 8th if its 0)
and parity is checked on the received data */

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
void UART2_write_char(char chr);
void UART2_write_string(const char *str);

#endif


/*  

We use a baud rate of 115200; this means 115200 bits are transferred per second;
This count includes all the start, stop and parity bits;
Since we use 1 start and 1 stop bit and no parity bit, we have a total of 10 bits for each
byte of data. Hence, our actual data transfer rate is 11520 bytes per second, which is about
11.5 Kilobytes per second.

*/