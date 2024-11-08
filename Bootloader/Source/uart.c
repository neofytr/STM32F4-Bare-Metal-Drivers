#include "../Include/uart.h"

// pin and bit definitions
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
#define RE 2
#define TXE 7
#define RXNE 5
#define RXNEIE 5
#define TC 6

// buffer configurations
#define TX_BUFFER_SIZE 128
#define RX_BUFFER_SIZE 128

// TX Buffer structure
typedef struct
{
    volatile uint8_t data[TX_BUFFER_SIZE];
    volatile uint8_t write_index;
    volatile uint8_t read_index;
} TxBuffer;

// RX Buffer structure
typedef struct
{
    volatile uint8_t data[RX_BUFFER_SIZE];
    volatile uint8_t write_index;
    volatile uint8_t read_index;
} RxBuffer;

// global buffer instances
static TxBuffer tx_buffer = {0};
static RxBuffer rx_buffer = {0};

// buffer management functions
static bool tx_buffer_is_full(void)
{
    uint8_t next_write = (tx_buffer.write_index + 1) & (TX_BUFFER_SIZE - 1);
    return (tx_buffer.read_index == next_write);
}

static uint8_t tx_buffer_write(const uint8_t *str, uint8_t len)
{
    if (!str || len == 0 || len > TX_BUFFER_SIZE)
    {
        return 0;
    }

    uint8_t actual_written = 0;
    uint8_t current_write_index;

    while (!tx_buffer_is_full() && actual_written < len) // we constantly use the new values to check since data may be going out of the buffer as we are adding to it
    {
        current_write_index = tx_buffer.write_index;
        tx_buffer.data[current_write_index] = (uint8_t)str[actual_written];
        actual_written++;
        tx_buffer.write_index = (current_write_index + 1) & (TX_BUFFER_SIZE - 1);
    }

    return actual_written;
}

bool is_data_available(void)
{
    return rx_buffer.read_index != rx_buffer.write_index;
}

static bool rx_buffer_is_full(void)
{
    uint8_t next_write = (rx_buffer.write_index + 1) & (RX_BUFFER_SIZE - 1);
    return next_write == rx_buffer.read_index;
}

static bool rx_buffer_write(uint8_t data)
{
    if (rx_buffer_is_full())
    {
        return false;
    }

    rx_buffer.data[rx_buffer.write_index] = data;
    rx_buffer.write_index = (rx_buffer.write_index + 1) & (RX_BUFFER_SIZE - 1);
    return true;
}

static bool rx_buffer_read(uint8_t *data)
{
    if (!is_data_available())
    {
        return false;
    }

    *data = rx_buffer.data[rx_buffer.read_index];
    rx_buffer.read_index = (rx_buffer.read_index + 1) & (RX_BUFFER_SIZE - 1);
    return true;
}

void USART2_Handler(void)
{
    if (IS_SET(USART2->SR, TXE))
    {
        if (tx_buffer.read_index != tx_buffer.write_index)
        {
            uint8_t current_read_index = tx_buffer.read_index;
            USART2->DR = tx_buffer.data[current_read_index];
            tx_buffer.read_index = (current_read_index + 1) & (TX_BUFFER_SIZE - 1);
        }
        else
        {
            while (!IS_SET(USART2->SR, TC))
            {
                // wait for transmission complete
            }

            CLEAR_BIT(USART2->CR1, TXEIE);
            CLEAR_BIT(USART2->CR1, TCIE);
        }
    }

    if (IS_SET(USART2->SR, RXNE))
    {
        uint8_t received_data = USART2->DR; // i dont check the receive errors since this is a general driver and i dont have any specific thing in mind according to the application which will force me to do certain things when certain errors arise
        if (!rx_buffer_write(received_data))
        {
            // buffer full - data is discarded
        }
    }
}

uint8_t UART2_write(const uint8_t *str, uint8_t len)
{
    uint8_t return_val = tx_buffer_write(str, len);
    // Enable TX interrupts
    SET_BIT(USART2->CR1, TXEIE);
    SET_BIT(USART2->CR1, TCIE);
    return return_val;
}

bool UART2_write_byte(const uint8_t *str)
{
    return UART2_write(str, 1);
}

bool UART2_read_byte(uint8_t *data)
{
    return rx_buffer_read(data);
}

uint8_t UART2_read(uint8_t *data, uint8_t len)
{
    uint8_t actual_read = 0;
    for (uint8_t i = 0; i < len; i++)
    {
        if (UART2_read_byte(&(data[actual_read])))
        {
            actual_read++;
        }
    }

    return actual_read;
}

void UART2_init(void)
{
    // GPIOA configuration
    SET_BIT(RCC->AHB1ENR, GPIOA_EN);

    // Configure TX pin
    CLEAR_BIT(GPIOA->MODER, TX_PIN * 2);
    SET_BIT(GPIOA->MODER, TX_PIN * 2 + 1);
    SET_BIT(GPIOA->AFR[0], TX_PIN * 4);
    SET_BIT(GPIOA->AFR[0], TX_PIN * 4 + 1);
    SET_BIT(GPIOA->AFR[0], TX_PIN * 4 + 2);
    CLEAR_BIT(GPIOA->AFR[0], TX_PIN * 4 + 3);

    // Configure RX pin
    CLEAR_BIT(GPIOA->MODER, RX_PIN * 2);
    SET_BIT(GPIOA->MODER, RX_PIN * 2 + 1);
    SET_BIT(GPIOA->AFR[0], RX_PIN * 4);
    SET_BIT(GPIOA->AFR[0], RX_PIN * 4 + 1);
    SET_BIT(GPIOA->AFR[0], RX_PIN * 4 + 2);
    CLEAR_BIT(GPIOA->AFR[0], RX_PIN * 4 + 3);
    SET_BIT(GPIOA->PUPDR, RX_PIN * 2);
    CLEAR_BIT(GPIOA->PUPDR, RX_PIN * 2 + 1);

    // USART2 configuration
    SET_BIT(RCC->APB1ENR, USART2_EN);
    CLEAR_BIT(USART2->CR1, M_BIT);
    CLEAR_BIT(USART2->CR2, STOP_BIT);
    CLEAR_BIT(USART2->CR2, STOP_BIT + 1);
    CLEAR_BIT(USART2->CR1, OVER8);
    CLEAR_BIT(USART2->CR3, ONEBIT);
    CLEAR_BIT(USART2->CR1, PCE);

    // Set baud rate to 115200
    USART2->BRR = (8 << 4) | (11);

    // Enable RX interrupt
    SET_BIT(USART2->CR1, RXNEIE);

    // Enable USART2 interrupts in NVIC
    NVIC_EnableIRQ(USART2_IRQn);

    // Enable transmitter and receiver
    SET_BIT(USART2->CR1, TE);
    SET_BIT(USART2->CR1, RE);

    // Enable USART2 module
    SET_BIT(USART2->CR1, UE_BIT);
}
