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

void UART2_TX_init(void);
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

// all of the following USART info is for UART single-buffer transmission / reception with
// no hardware flow control

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
8. A baud rate register (USART_BRR) - 12-bit mantissa and 4-bit fraction

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

For understanding the second case, see one of the comments below.

## Selecting the proper oversampling method

The receiver implements different user-configurable oversampling techniques (except in synchronous mode)
for data recovery by discriminating between valid incoming data and noise.

The oversampling method can be selected by programming the OVER8 bit in the USART_CR1 register
and can be either 16 or 8 times the baud rate clock.

Depending on the application:

1. select oversampling by 8 (OVER8 = 1) to achieve higher speed (upto f_pclk / 8). In this
case the maximum receiver tolerance to clock deviation is reduced.
2. select oversampling by 16 (OVER8 = 0) to increase the tolerance of the receiver to clock
deviations. In this case, the maximum speed is limited to maximum f_pclk / 16.

After the start bit evaluation, the rest of the bits are evaluated and sampled differently.
Programming the ONEBIT bit in the USART_CR3 register selects the method used to evaluate
the logic level. There are two options:

1. the majority vote of the three samples in the center of the received bit. In this case, when the
3 samples used for the majority vote are not equal, the NF bit is set.

2. a single sample in the center of the received bit

Depending on the application:

1. select the three sample's majority vote (ONEBIT = 0) when operating in a noisy environment
and reject the data when a noise is detected because this indicates that a glitch occured during the
sampling.

2. select the single sample method (ONEBIT = 1) when the line is noise-free to increase the receiver's
tolerance to clock deviations. In this case, the NF bit will never be set.

When noise is detected in a frame:

1. The NF bit is set at the rising edge of the RXNE bit.
2. The invalid data is transferred from the Shift register to the USART_DR register.
3. No interrupt is generated in case of single byte communication. However this bit rises
at the same time as the RXNE bit which itself generates an interrupt.

The NF bit is reset by a USART_SR register read operation followed by a USART_DR register read operation.

Framing Error

A framing error is detected when:

The stop bit is not recognized on reception at expected time, following either a de-synchronization
or excessive noise.

When the framing error is detected:

1. The FE bit is set by hardware
2. The invalid data is transferred from the shift register to the USART_DR register.
3. No interrupt is generated in case of single byte communication. However, this bit rises
at the same time as RXNE bit which itself generates an interrupt.

The FE bit is reset by a USART_SR register read operation followed by a USART_DR register
read operation.

The number of stop bits to be received can be configured through the control bits of Control
register 2 -  it can be either 1 or 2 in normal mode.

1. 1 stop bit: Sampling for 1 stop bit is done on the 8th, 9th and 10th (or 4th, 5th and 6th for x8) samples.
2. 2 stop bits: Sampling for 2 stop bits is done on the 8th, 9th and 10th samples (or 4th, 5th and 6th for x8) of the first stop
bit. If a framing error is detected during the first stop bit the FE flag will be set. The second
stop bit is not checked for framing error. The RXNE flag will be set at the end of the first stop bit.

*/

/*

# Fractional Baud Rate Generation

The baud rate for the receiver and transmitter (Rx and Tx) are both set to the same values as
programmed in the Mantissa and Fraction values of USARTDIV.

Baud rate = f_clk / (8 * (2 - OVER8) * USARTDIV)

USARTDIV is an unsigned fixed floating point number that is coded on the USART_BRR register.

1. When OVER8 = 0, the fractional part is coded on 4 bits and programmed by the DIV_fraction[3:0]
bits in the USART_BRR register.

2. When OVER8 = 1, the fractional part is coded on 3 bits and programmed by the DIV_fraction[2:0]
bits in the USART_BRR register, and bit DIV_fraction[3] must be kept cleared.

The baud counters are updated to the new value in the baud registers after a write operation
to USART_BRR. Hence the baud rate register value should not be changed during
communication.

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

Here’s a breakdown of how the two main oversampling options (8x and 16x) affect performance and tolerance:

Oversampling by 8 (OVER8 = 1):

The incoming signal is sampled 8 times per bit.
This approach is ideal for applications where higher speed is desired since it allows data to be received at up to f_pclk / 8.
However, because there are fewer samples per bit, this configuration reduces the tolerance to clock deviations or noise in the incoming signal. Small timing errors or clock mismatches may cause errors more easily than in 16x oversampling.
Oversampling by 16 (OVER8 = 0):

The signal is sampled 16 times per bit, doubling the sample count compared to the 8x configuration.
This configuration increases the USART's tolerance to clock deviation (timing errors) because there are more samples per bit to help the USART "lock onto" the correct timing.
The trade-off is that the maximum achievable speed is limited to f_pclk / 16, so it’s a bit slower than 8x oversampling but more robust against noise and clock inaccuracies.
Choosing the Right Option
Applications that prioritize speed (for example, when the data link is stable and reliable) may use oversampling by 8 to achieve higher throughput.
Applications where timing precision is critical, such as those sensitive to clock drift or with noisy communication lines, should choose oversampling by 16 to increase reliability at a slightly lower speed.
In summary, oversampling by 8 favors speed but sacrifices some tolerance to timing variations, while oversampling by 16 offers greater error tolerance at a lower maximum speed.

*/

/*

# RXNE Clear when Overrun Occurs


### Key Points in Overrun and RXNE Behavior

An **overrun error (ORE)** generally occurs when the **Receive Data Register (RDR)** holds unread data (meaning **RXNE = 1**) and a new byte arrives in the shift register. In this case:

1. **RXNE must be set** (indicating unread data in the RDR) for a traditional overrun error to occur because the USART couldn’t transfer new data from the shift register to RDR.
2. The **ORE flag is set** because the new data in the shift register cannot move to RDR, so it’s discarded. This missed data signals an overrun condition.

However, there is a subtle case where **RXNE could be 0 when ORE is set**:

### Special Case: RXNE = 0 When ORE is Set

1. **Simultaneous Reading of RDR and Shift Register Update**:
   - When the software reads the RDR just as new data arrives, **RXNE can clear** as the data register is read.
   - However, if new data arrives *during* this process, the shift register data cannot move to RDR because the USART’s internal processes are updating flags.
   - This overlap can lead to an **ORE flag set with RXNE cleared** since the last byte was read right as the new byte was arriving.

2. **Timing Issue**:
   - This scenario is rare and depends on precise timing between data arrival and RDR reading. It might occur if software reads USART_SR (status register) and immediately reads USART_DR (data register) right as new data arrives.

### Why Does This Happen?

USART is designed to prioritize maintaining the last valid data. If software reads **USART_DR** right when new data arrives, the USART could discard the incoming byte rather than overwrite the existing data in RDR.

### Practical Consideration

While it’s uncommon, this behavior is documented to help developers understand cases where **ORE might be set while RXNE is unexpectedly clear**. In practice, this edge case is mostly handled by ensuring that software reads data from RDR promptly, minimizing the risk of overlap that could lead to a missed byte.

**In summary**: The typical condition for ORE is indeed RXNE = 1, but in a rare timing overlap, RXNE can be 0 when ORE is set if a read occurs exactly as new data arrives.

*/

/*

# USART Interrupts

The USART interrupt events are connected to the same interrupt vector.

1. During transmission: Transmission complete, Clear to Send or Transmit Data Register empty interrupt
2. While receiving: Idle line detection, Overrun error, Receive data register not empty,
Parity error.

These events generate an interrupt if the corresponding Enable Control Bit is set.

Interrupt event         Event flag          Enable Control Bit

Transmit Data
Register Empty          TXE                 TXEIE

Transmission
Complete                TC                  TCIE

Received Data
Ready to be Read        RXNE                RXNEIE
/ Overrun Error
Detected                ORE                 RXNEIE

Idle line detected      IDLE                IDLEIE

Parity Error            PE                  PEIE

Break flag              LBD                 LBDIE

 */

/*

# Parity Control

Parity control (generation of parity bit in transmission and parity checking in reception)
can be enabled by setting the PCE bit in the USART_CR1 register. Depending on the frame
length defined by the M bit, the possible USART frame formats are as listed:

M bit    PCE bit            USART Frame
0        0                  | SB | 8 bit data | STB |
0        1                  | SB | 7 bit data | PB | STB |
1        0                  | SB | 9 bit data | STB |
1        1                  | SB | 8 bit data | PB | STB |

## Even parity

The parity bit is calculated to obtain an even number of 1's inside the frame made of the
7 or 8 LSB data bits (depending on whether M is equal to 0 or 1) and the parity bit.

E.g.: data=00110101; 4 bits set => parity bit will be 0 if even parity is selected (PS bit in
USART_CR1 = 0).

## Odd parity

The parity bit is calculated to obtain an odd number of “1s” inside the frame made of the 7 or
8 LSB bits (depending on whether M is equal to 0 or 1) and the parity bit.
E.g.: data=00110101; 4 bits set => parity bit will be 1 if odd parity is selected (PS bit in
USART_CR1 = 1).

## Parity checking in reception

If the parity check fails, the PE flag is set in the USART_SR register and an interrupt is
generated if PEIE is set in the USART_CR1 register. The PE flag is cleared by a software
sequence (a read from the status register followed by a read or write access to the
USART_DR data register).

## Parity generation in transmission

If the PCE bit is set in USART_CR1, then the MSB bit of the data written in the data register
is transmitted but is changed by the parity bit (even number of “1s” if even parity is selected
(PS=0) or an odd number of “1s” if odd parity is selected (PS=1)).

The software routine that manages the transmission can activate the software sequence
which clears the PE flag (a read from the status register followed by a read or write access
to the data register). When operating in half-duplex mode, depending on the software, this
can cause the PE flag to be unexpectedly cleared.

*/