#include <stdint.h>

#define BOOTLOADER_SIZE (0x8000U)
#define FLASH_BASE (0x08000000U)
#define MAIN_APP_START_ADDR (FLASH_BASE + BOOTLOADER_SIZE)
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
    jump_to_app();
    return 0;
}