#include "../Include/uart.h"

void UART2_write_string(const char *str)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        UART2_write_char(str[i]);
    }
}

static inline uint32_t compute_uart_div(uint32_t periph_clock, uint32_t baud_rate)
{
    return ((periph_clock + baud_rate / 2U) / baud_rate);
}

static inline void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t periph_clock, uint32_t baud_rate)
{
    USARTx->BRR = compute_uart_div(periph_clock, baud_rate);
}

void UART2_tx_init(void)
{
    // configuring UART2 GPIO TX pin
    // enable clock access to GPIOA
    SET_BIT(RCC->AHB1ENR, GPIOA_EN_BIT);
    // set PA2 to alternate function mode
    CLEAR_BIT(GPIOA->MODER, TX_PIN * 2);
    SET_BIT(GPIOA->MODER, TX_PIN * 2 + 1);
    // set PA2 alternate function type to AF07 (UART_TX); AF07 is 0111
    SET_BIT(GPIOA->AFR[0], 4 * TX_PIN);
    SET_BIT(GPIOA->AFR[0], 4 * TX_PIN + 1);
    SET_BIT(GPIOA->AFR[0], 4 * TX_PIN + 2);
    CLEAR_BIT(GPIOA->AFR[0], 4 * TX_PIN + 3);

    // configuring UART2 module
    // enable clock access to UART2
    SET_BIT(RCC->APB1ENR, USART2_EN_BIT);
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

void UART2_write_char(char chr)
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
        UART2_write_string("hello world\n\r");
        volatile size_t x = 0;
        for (volatile size_t i = 0; i < 100000; i++)
        {
            x += i;
        }
    }
}