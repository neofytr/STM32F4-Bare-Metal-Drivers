#include "../Include/uart.h"

// USART2 uses PA2 configured in AF07 as it's TX pin

#define UE_BIT 13
#define M_BIT 12
#define STOP_BIT 12

#define TX_PIN 2
#define RX_PIN 3

#define GPIOA_EN 0
#define USART2_EN 17
#define OVER8 15
#define ONEBIT 11
#define PCE 10
#define TXEIE 7
#define TCIE 6
#define TE 3
#define TXE 7
#define TC 6

#define BUFFER_SIZE 128

volatile uint8_t buffer[BUFFER_SIZE];
volatile uint8_t buffer_index = 0;
volatile uint8_t buffer_length = 0;
volatile bool buffer_busy = false;

void USART2_Handler(void)
{
    if (IS_SET(USART2->SR, TXE)) // TXE is cleared by a write to the USART_DR
    {
        if (buffer_index < buffer_length)
        {
            USART2->DR = buffer[buffer_index++];
        }
        else
        {
            buffer_index = 0;
            buffer_length = 0;
            CLEAR_BIT(USART2->CR1, TXEIE);
            buffer_busy = false;
        }
    }
}

uint8_t UART2_read(char *str, uint8_t len)
{
    if (buffer_busy)
    {
        return 0;
    }

    if (len == 0 || !str || len > BUFFER_SIZE)
    {
        return 0;
    }

    buffer_busy = true;

    if (len > buffer_length)
    {
        buffer_busy = false;
        return 0;
    }

    for (uint8_t i = 0; i < len; i++)
    {
        str[i] = buffer[buffer_length - len + i];
    }

    buffer_busy = false;
    return len;
}

uint8_t UART2_write(const char *str, uint8_t len)
{
    if (buffer_busy)
    {
        return 0;
    }

    if (len == 0 || !str || len > BUFFER_SIZE)
    {
        return 0;
    }

    buffer_busy = true;
    buffer_length = len;

    for (uint8_t i = 0; i < len; i++)
    {
        buffer[i] = (uint8_t)str[i];
    }

    SET_BIT(USART2->CR1, TXEIE);

    return len;
}

void UART2_init(void)
{
    // GPIOA config

    // enable GPIOA
    SET_BIT(RCC->AHB1ENR, GPIOA_EN);

    // set the TX_PIN into AF mode
    CLEAR_BIT(GPIOA->MODER, TX_PIN * 2);
    SET_BIT(GPIOA->MODER, TX_PIN * 2 + 1);

    // set the TX_PIN into AF07
    SET_BIT(GPIOA->AFR[0], TX_PIN * 4);
    SET_BIT(GPIOA->AFR[0], TX_PIN * 4 + 1);
    SET_BIT(GPIOA->AFR[0], TX_PIN * 4 + 2);
    CLEAR_BIT(GPIOA->AFR[0], TX_PIN * 4 + 3);

    // USART2 config

    // enable the clock to the bus connecting USART2
    SET_BIT(RCC->APB1ENR, USART2_EN);

    // set 8 data bits
    CLEAR_BIT(USART2->CR1, M_BIT);

    // set one stop bit
    CLEAR_BIT(USART2->CR2, STOP_BIT);
    CLEAR_BIT(USART2->CR2, STOP_BIT + 1);

    // set oversampling to x16
    CLEAR_BIT(USART2->CR1, OVER8);

    // set three sample majority vote as the method of evaluation
    CLEAR_BIT(USART2->CR3, ONEBIT);

    // set baud rate to 115200
    // (16 Mhz) / (2 * 8  * 115200) = 8.6806
    // mantissa  = 8; frac = 0.6806
    // coded on 4 bits; 0.6806 * 16 = 10.8896; frac = 11

    USART2->BRR = (8 << 4) | (11);

    // turn off parity control
    CLEAR_BIT(USART2->CR1, PCE);

    // Turning on interrupts

    // for TXE
    // setting this will immediately generate an interrupt since
    // the TXE register is empty; so we set it when we get a transmission
    // request in the transmit function; and then clear it when all data is transferred
    // SET_BIT(USART2->CR1, TXEIE);

    // for TC
    // we'll set TC in the transmission function too
    // SET_BIT(USART2->CR1, TCIE); // we clear this too when all data is transferred; we set
    // it again in the transmission function

    // enable interrupts for the USART2 peripheral in the NVIC

    NVIC_EnableIRQ(USART2_IRQn);

    // enable the transmitter
    SET_BIT(USART2->CR1, TE);

    // all the other features will be turned off since the reset
    // state of their control bits are zero
    // enable the USART2 module
    SET_BIT(USART2->CR1, UE_BIT);
}

#define PIN5 5
#define LED_PIN PIN5

int main(void)
{
    UART2_TX_init();

    // set LED pin as output pin
    SET_BIT(GPIOA->MODER, 2 * LED_PIN);
    CLEAR_BIT(GPIOA->MODER, 2 * LED_PIN + 1);

    while (true)
    {
        if (!UART2_write("hello\r\n", 8))
        {
            SET_BIT(GPIOA->ODR, LED_PIN);
        }
        else
        {
            CLEAR_BIT(GPIOA->ODR, LED_PIN);
        }
    }

    return 0;
}