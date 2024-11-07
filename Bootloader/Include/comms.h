#ifndef FE21EE1A_0B0A_4546_9BD6_FA0425C87443
#define FE21EE1A_0B0A_4546_9BD6_FA0425C87443

#include <stdint.h>
#include <stdbool.h>

#define PACKET_DATA_LENGTH (16)
#define PACKET_LENGTH_BYTES (1)
#define PACKET_CRC_BYTES (1)
#define PAKCET_CRC_INPUT_LENGTH (PACKET_DATA_LENGTH + PACKET_LENGTH_BYTES)
#define PACKET_LENGTH (PACKET_DATA_LENGTH + PACKET_LENGTH_BYTES + PACKET_CRC_BYTES)

typedef struct comms_packet_
{
    uint8_t length;
    uint8_t data[PACKET_DATA_LENGTH];
    uint8_t crc;
} comms_packet_t;

void comms_setup(void);
void comms_update(void);

bool comms_packet_available(void);
void comms_write(comms_packet_t *packet);
void comms_read(comms_packet_t *packet);

#endif /* FE21EE1A_0B0A_4546_9BD6_FA0425C87443 */
