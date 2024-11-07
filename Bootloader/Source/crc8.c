#ifndef A50225AA_9409_4502_8D4B_7D8F5846EA46
#define A50225AA_9409_4502_8D4B_7D8F5846EA46

#include "../Include/crc8.h"

uint8_t calculate_crc8(uint8_t *data, uint8_t length)
{
    uint8_t crc = 0x00;

    for (uint8_t i = 0; i < length; i++)
    {
        crc ^= data[i];

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ 0x07;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}

#endif /* A50225AA_9409_4502_8D4B_7D8F5846EA46 */
