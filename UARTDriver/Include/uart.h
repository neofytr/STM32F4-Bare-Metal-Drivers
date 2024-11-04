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

/*

Universal Synchronous Asynchronous Receiver-Transmitter (USART)

Transfer Detection Flags:

1. Receive buffer full
2. Transmit buffer empty
3. End of transmission flags

Parity Control:

1. Transmits partity bit
2. Checks parity of received data byte

Four error detection flags:

1. Overrun error
2. Noise detection
3. Frame error
4. Parity error

Ten interrupt sources with flags:

1. CTS changes
2. LIN break detection
3. Transmit data register empty
4. Transmission complete
5. Receive data register full
6. Idle line received
7. Overrun error
8. Framing error
9. Noise error
10. Parity error

The interface is externally connected to another device by three pins. Any USART bidirectional
communication requires a minimum of two pins: Receive Data In (RX) and Transmit Data Out (TX):

RX: Receive Data Input is the serial data input. Oversampling techniques are used for data recovery
by discriminating between valid incoming data and noise.

TX: Transmit Data Output. When the transmitter is disabled, the output pin returns to it's I/O port configuration.
When the transmitter is enabled and nothing is to be transmitted, the TX pin is at a high level. In single-wire and smartcard modes,
this I/O is used to transmit and receive the data (at USART level, data are then received on SW_RX).

Through these pins, serial data is transmitted and received in normal USART mode as frames comprising:

1. An idle line prior to transmission or reception
2. A start bit
3. A data word (8 or 9 bits); least significant bit first
4. 0.5, 1, 1.5, 2 stop bits indicating that the frame is complete
5. This interface uses a fractional baud rate generator - with a 12-bit mantissa and 4-bit fraction
6. A status register (USART_SR)
7. Data register (USART_DR)
8. A baud rate register (USART_BRR) - 12-bit mantissa and 4-bit fractionuu

Word length may be selected as being either 8 bits or 9 bits by programming the M bit in the
USART_CR1 register.

The TX is in low state during start bit. It is in high state during the stop bit.

An Idle character is interpreted as an entire frame full of 1's followed by the start bit of the
next frame which contains data (The number of 1's will include the number of stop bits).

A Break character is interpreted on receiving 0s for a frame period. At the end of the break frame
the transmitter inserts either 1 or 2 stop bits (logic 1) to acknowledge the start bit.

Transmission and reception are driven by a common baud rate generator, the clock for each
is generated when the enable bit is set respectively for the transmitter and receiver.

When UART lines are idle (no data being transmitted), their default states are:

TX line (Transmit):

Held HIGH (logic 1)
This is the idle/rest state
The start bit (logic 0) will indicate beginning of transmission

RX line (Receive):

Should be pulled HIGH (logic 1)
This is why we use a pull-up resistor on RX

That's why:

Start bit is always 0 (falling edge from idle HIGH)
Stop bit is always 1 (returns line to idle HIGH)
No clock signal needed - receiver synchronizes on start bit's falling edge
Missing pull-up on RX can cause garbage data (floating input)

The constant logic high is NOT an idle frame as that would require a start bit after a frame of 1s.

# Transmitter

The transmitter can send data of words either 8 or 9 bits depending on the M bit status.
When the Transmit enable bit (TE) is set, the data in the transmit shift register is output on the
TX pin and the corresponding clock pulses are output on the CK pin.

During an USART transmission, data shifts out least significant bit first on the TX pin. In this
mode, the USART_DR register consists of a buffer (TDR) between the internal bus and the transmit shift
register.

Every character is preceded by a start bit which is a logic low for one bit period. The character is terminated
by a configurable number of stop bits.

The following stop bits are supported by USART: 0.5, 1, 1.5 and 2 stop bits.

The TE bit should not be reset during transmission of data. Resetting the TE bit during the
transmission will corrupt the data on the TX pin as the baud rate counters will get frozen. The
current data being transmitted will be lost.

An idle frame will be sent after the TE bit is enabled.

The number of stop bits to be transmitted with every character can be programmed in Control
register 2, bits 13, 12

1 stop bit is the default value.

An idle frame transmission will include the stop bits. A break transmission will be 10 low bits
followed by the configured number of stop bits (when m = 0) and 11 low bits followed by the configured
number of stop bits (when m = 1). It is not possible to transmit long breaks (break of length greater than 10/11 low bits).

Procedure:

1. Enable the USART by writing the UE bit in USART_CR1 register to 1
2. Program the M bit in USART_CR1 to define the word length
3. Program the number of stop bits in USART_CR2
4. Select the desired baud rate using the USART_BRR register
5. Set the TE bit in USART_CR1 to send an idle frame as first transmission.
6. Write the data to send in the USART_DR register (this clears the TXE bit). Repeat this
for each data to be transmitted in case of single buffer.
7. After writing the last data into the USART_DR register, wait until TC = 1. This indicates
that the transmission of the last frame is complete. This is required for instance when
the USART is disabled or enters the Halt mode to avoid corrupting the last transmission.

## Single-byte communication

Clearing the TXE bit is always performed by a write to the data register.

The TXE bit is set by hardware and it indicates:

1. The data has been moved from TDR to the shift register and the data transmission has started.
2. The TDR register is empty.
3. The next data can be written in the USART_DR register without overwriting the previous data.

This flag generated an interrupt if the TXEIE bit is set.

When a transmission is taking place, a write instruction to the USART_DR register stores the
current data in the TDR register and which is copied in the shift register at the end of the current
transmission.

When no transmission is taking place, a write instruction to the USART_DR register places
the data directly in the shift register, the data transmission starts, and the TXE bit is
immediately set.

If a frame is completely transmitted (after the stop bit) and the TXE bit is set, the TC bit goes high.
An interrupt is generated if the TCIE bit is set in the USART_CR1 register.

After writing the last data into the USART_DR register, it is mandatory to wait for TC = 1
before disabling the USART or causing the microcontroller to enter the low-power mode.

The TC bit is cleared by the following software sequence:

1. A read from the USART_SR register
2. A write to the USART_DR register

The TC bit can also be cleared by writing a ‘0 to it. This clearing sequence is recommended
only for Multibuffer communication.

### Break characters

Setting the SBK bit transmits a break character. The break frame length depends on the M bit.

If the SBK bit is set to 1, a break character is sent on the TX line after completing the current
character transmission. This bit is reset by hardware when the break character is completed
(during the stop bit of the break character). The USART inserts a logic 1 bit at the end of the last
break frame to guarantee the recognition of the start bit of the next frame.

If the software resets the SBK bit before the commencement of break transmission, the
break character will not be transmitted. For two consecutive breaks, the SBK bit should be
set after the stop bit of the previous break.

### Idle characters

Setting the TE bit drives the USART to send an idle frame before the first data frame.

# Receiver

The USART can receive data words of either 8 or 9 bits depending on the M bit in the USART_CR1 register

The start bit detection sequence is the same when oversampling by 16 or by 8.

In the USART, the start bit is detected when a specific sequence of samples is recognized.
This sequence is: 1 1 1 0 X 0 X 0 X 0 0 0 0.

The 1110 is falling edge detection; 0 is the one-bit time starting, the bit time is:

0 X S X S X S S S S X X X X X X

S indicates a sample point; X indicates a non-sample point.

If the sequence is not completed, the start bit detection aborts and the receiver returns
to the idle state (no flag is set) where it waits for a falling edge.

The start bit is confirmed (RXNE flag set, interrupt generated if RXNEIE = 1) if the 3 sampled
points are at zero (first sampling on the 3rd, 5th and 7th points finds the three bits at zero and second
sampling on the 8th, 9th and 10th points also finds the points at 0).

The start bit is validated (RXNE flag set, interrupt generated if RXNEIE = 1) but the NE noise
flag is set if, for both samplings, at least 2 out of the 3 sampled points are at 0. If this
condition is not met, the start detection aborts and the receiver returns to the idle state
(no flag is set).

If, for one of the samplings, 2 out of the 3 points are found at zero, the start bit is validated (RXNE flag set, interrupt generated if RXNEIE = 1)
but the NE noise flag bit is set.

## Character Reception

During an USART reception, data shifts in least significant bit first through the RX pin.
In this mode, the USART_DR register consists of a buffer (RDR) between the internal bus and the
received shift register.

Procedure:

1. Enable the USART by writing the UE bit in USART_CR1 register to 1.
2. Program the M bit in USART_CR1 to define the word length.
3. Program the number of stop bits in USART_CR2.
4. Select the desired baud rate using the baud rate register USART_BRR.
5. Set the RE bit in USART_CR1. This enables the receiver which begins searching for a
start bit.

When a character is received:

1. The RXNE bit is set. It indicates that the contents of the shift register is transferred
to the RDR. In other words, data has been received and can be read (as well as it's associated
error flags).
2. An interrupt is generated if the RXNEIE bit is set.
3. The error flags can be set if a frame error, noise or an overrun error has been detected
during reception.
4. In single buffer mode, clearing the RXNE bit is performed by a software read to the
USART_DR register. The RXNE flag can also be cleared by writing a zero to it. The RXNE bit must be
cleared before the end of the reception of the next character to avoid an overrun error.

The RE bit should not be reset while receiving data. If the RE bit is disabled during
reception, the reception of the current byte will be aborted.

Break character

When a break character is received, the USART handles it as a framing error.

Idle character

When an idle frame is detected, there is the same procedure as a data received character
plus an interrupt if the IDLEIE bit is set.

Overrun error

When a frame (starting with the start bit) is detected on the RX line, the USART begins
to receive bits, one at a time, into the Shift register. The shift register collects the
bits until it has a complete data byte, including any parity and stop bits if configured.

Once the shift register holds a complete frame, it checks if the Received Data Register (RDR)
is empty. If the RDR is empty, the byte in the shift register moves into the RDR making
it available for the software to read. The USART then raises the RXNE (Received Data Register
Not Empty) flag (and an interrupt if RXNEIE is set), signalling that data is ready to be read.

If the RDR is already holding unread data when a new byte is ready in the shift register, the
transfer fails, and the USART sets the Overrun Error (ORE) flag (and an interrupt is generated
if the RXNEIE flag is set).

The USART discards the byte in the shift register that could not be moved to the RDR. The RDR
still contains the previously received byte, which remains unchanged until it is read. The ORE flag alerts the software that one or more incoming bytes were lost due to the RDR being full, signaling that the data was not processed quickly enough.

An overrun error typically means that at least one byte was lost. This might not be a problem if no data is expected after the missed byte, but if a continuous data stream is being received, this can lead to missed or corrupted data.

An overrun error occurs when a character is received when RXNE has not been reset. Data can not be
transmitted from the shift register to the RDR register until the RXNE bit is cleared.

The RXNE flag is set after every byte received. An overrun error occurs if RXNE flag is set
when the next data is received. When an overrun error occurs:

1. The ORE bit is set
2. The RDR content will not be lost. The previous data is available when a read to USART_DR is
performed.
3. The shift register will be overwritten. After that point, any data received during overrun
is lost.
4. An interrupt is generated if RXNEIE flag is set.
5. The ORE bit is reset by a read to the USART_SR register followed by a USART_DR register
read operation.

The ORE bit, when set, indicates that atleast 1 byte of data has been lost. There are two possibilities:

1. if RXNE = 1, then the last valid data is stored in the RDR and can be read.
2. if RXNE = 0, then it means that the last valid data has already been read and thus
there is nothing to be read in RDR. This can can occur when the last valid data is read in
the RDR at the same time as the new (and lost) data is received. It may also
occur when the new data is received during the reading sequence (between the USART_SR register
read access and the USART_DR read access).

*/

/*

# Oversampling


Oversampling in USART is a technique used to improve the accuracy and stability of data
transmission, particularly in noisy environments or when the baud rate clock isn't perfectly synchronized with the incoming signal.
It works by sampling each bit of the data multiple times, rather than just once, and then using these multiple samples to make a decision
about whether the bit is a 1 or a 0.

When oversampling is enabled, the USART samples each bit at a higher rate than the baud rate. For example:

16x oversampling: The USART samples each bit 16 times within the duration of that bit.
8x oversampling: The USART samples each bit 8 times within the duration of that bit.

The USART then uses these samples to determine the actual bit value.
This improves reliability, as it helps to mitigate timing errors or transient noise spikes that might affect only one or two of the sampled values.

*/