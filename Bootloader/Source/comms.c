#include "../Include/comms.h"
#include "../Include/uart.h"
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
static comms_packet_t ack_packet = {0};
static comms_packet_t last_transmitted_packet = {0};

#define PACKET_BUFFER_SIZE (16)
#define PACKET_BUFFER_MASK (PACKET_BUFFER_SIZE - 1)

static comms_packet_t packet_buffer[PACKET_BUFFER_SIZE];
static uint8_t packet_buffer_read_index = 0;
static uint8_t packet_buffer_write_index = 0;

static void
comms_packet_copy(const comms_packet_t *source, comms_packet_t *dest)
{
    dest->length = source->length;
    for (uint8_t i = 0; i < PACKET_DATA_LENGTH; i++)
    {
        dest->data[i] = source->data[i];
    }
    dest->crc = source->crc;
}

static bool comms_is_ack_packet(const comms_packet_t *packet)
{
    if (packet->length != PACKET_ACK_DATA_LENGTH)
    {
        return false;
    }

    if (packet->data[0] != PACKET_ACK_DATA0)
    {
        return false;
    }

    for (uint8_t i = 1; i < PACKET_DATA_LENGTH; i++)
    {
        if (packet->data[i] != 0xff)
        {
            return false;
        }
    }

    return true;
}

static bool comms_is_retx_packet(const comms_packet_t *packet)
{
    if (packet->length != PACKET_RETX_DATA_LENGTH)
    {
        return false;
    }

    if (packet->data[0] != PACKET_RETX_DATA0)
    {
        return false;
    }

    for (uint8_t i = 1; i < PACKET_DATA_LENGTH; i++)
    {
        if (packet->data[i] != 0xff)
        {
            return false;
        }
    }

    return true;
}

uint8_t comms_compute_crc(comms_packet_t *packet)
{
    return calculate_crc8((uint8_t *)(packet), PACKET_CRC_INPUT_LENGTH);
}

void comms_setup(void)
{
    retx_packet.length = PACKET_RETX_DATA_LENGTH;
    retx_packet.data[0] = PACKET_RETX_DATA0;
    for (uint8_t i = PACKET_RETX_DATA_LENGTH; i < PACKET_DATA_LENGTH; i++)
    {
        retx_packet.data[i] = 0xFF;
    }
    retx_packet.crc = comms_compute_crc(&retx_packet);

    ack_packet.length = PACKET_ACK_DATA_LENGTH;
    ack_packet.data[0] = PACKET_ACK_DATA0;
    for (uint8_t i = PACKET_ACK_DATA_LENGTH; i < PACKET_DATA_LENGTH; i++)
    {
        ack_packet.data[i] = 0xFF;
    }
    ack_packet.crc = comms_compute_crc(&ack_packet);
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
            if (data_byte_count == PACKET_DATA_LENGTH)
            {
                data_byte_count = 0;
                state = CommsState_CRC;
            }
            break;
        }

        case CommsState_CRC:
        {
            UART2_read_byte(&(temporary_packet.crc));

            if (temporary_packet.crc != comms_compute_crc(&temporary_packet))
            {
                comms_write(&retx_packet);
                state = CommsState_Length;
                break;
            }

            if (comms_is_retx_packet(&temporary_packet))
            {
                comms_write(&last_transmitted_packet);
                state = CommsState_Length;
                break;
            }

            if (comms_is_ack_packet(&temporary_packet))
            {
                state = CommsState_Length;
                break;
            }

            comms_packet_copy(&temporary_packet, &packet_buffer[packet_buffer_write_index]);
            packet_buffer_write_index = (packet_buffer_write_index + 1) & PACKET_BUFFER_MASK;
            comms_write(&ack_packet);

            state = CommsState_Length;

            break;
        }

        default:
        {
            state = CommsState_Length;
            break;
        }
        }
    }
}

bool comms_packet_available(void)
{
    return (packet_buffer_read_index != packet_buffer_write_index);
}

void comms_write(comms_packet_t *packet)
{
    UART2_write((uint8_t *)packet, PACKET_LENGTH);
    comms_packet_copy(packet, &last_transmitted_packet);
}

void comms_read(comms_packet_t *packet)
{
    comms_packet_copy(&(packet_buffer[packet_buffer_read_index]), packet);
    packet_buffer_read_index = (packet_buffer_read_index + 1) & PACKET_BUFFER_MASK;
}