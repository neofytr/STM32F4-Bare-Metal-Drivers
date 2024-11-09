#include <stdint.h>
#include "../Include/comms.h"
#include "../Include/uart.h"

#define BOOTLOADER_SIZE (0x8000U)
#define FLASH_BASE_BOOTLOADER (0x08000000U)
#define MAIN_APP_START_ADDR (FLASH_BASE_BOOTLOADER + BOOTLOADER_SIZE)
#define MAIN_APP_RESET_VECTOR (MAIN_APP_START_ADDR + sizeof(uint32_t))

void jump_to_app(void)
{
    // main app ke vector table ke reset handler ko call krna h
    typedef void (*func)(void);
    func app_reset_handler = (func)(MAIN_APP_RESET_VECTOR);
    app_reset_handler();
}

int main(void)
{
    comms_setup();
    UART2_init();

    comms_packet_t packet = {.length = 0x5A, .data = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50}};
    packet.crc = 0xb1; 
    while (true)
    {
        // comms_write(&packet);
        comms_update();
    }
    jump_to_app();
    return 0;
}