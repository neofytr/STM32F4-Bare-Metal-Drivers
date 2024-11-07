#include "../Include/comms.h"
#include "../../UARTDriver/Include/uart.h"
#include "../Include/crc8.h"

typedef enum comms_state_
{
    CommsState_Length,
    CommsState_Data,
    CommsState_CRC,
} comms_state_t;

static comms_state_t state = CommsState_Length;
static uint8_t data_byte_count = 0;
static comms_packet_t temporary_packet = {0};
static comms_packet_t retx_packet = {0};

comms_setup(void)
{
    
}
void comms_update(void)
{
    while (is_data_available())
    {
        switch ((state))
        {
        case CommsState_Length:
        {
            UART2_read_byte(&(temporary_packet.length));
            state = CommsState_Data;
            break;
        }

        case CommsState_Data:
        {
            UART2_read_byte(&(temporary_packet.data[data_byte_count++]));
            if (data_byte_count == temporary_packet.length)
            {
                for (uint8_t i = data_byte_count; i < PACKET_DATA_LENGTH; i++)
                {
                    temporary_packet.data[i] = 0xFF;
                }
                data_byte_count = 0;
                state = CommsState_CRC;
            }
            break;
        }

        case CommsState_CRC:
        {
            UART2_read_byte(&(temporary_packet.crc));
            break;

            uint8_t computed_crc = crc8((uint8_t *)&(temporary_packet), PAKCET_CRC_INPUT_LENGTH);

            if (temporary_packet.crc != computed_crc)
            {
            }
        }

        default:
        {
            state = CommsState_Length;
            break;
        }
        }
    }
}

bool comms_packet_available(void);
void comms_write(comms_packet_t *packet);
void comms_read(comms_packet_t *packet);