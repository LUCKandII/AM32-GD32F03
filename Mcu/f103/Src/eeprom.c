/**
 * eeprom.c
 *
 * Flash EEPROM emulation for settings storage
 */

#include "main.h"

#ifndef EEPROM_START_ADD
#define EEPROM_START_ADD  0x08000000 + (128 * 1024) - (4 * 1024)
#endif

void read_flash_bin(uint8_t* data, uint32_t add, int out_buff_len)
{
    for (int i = 0; i < out_buff_len; i++) {
        data[i] = *(uint8_t*)(add + i);
    }
}

void save_flash_nolib(uint8_t* data, int length, uint32_t add)
{
    /* Unlock Flash */
    fmc_unlock();

    /* Clear pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

    /* Erase the page if needed (must erase before programming) */
    fmc_page_erase(add);

    /* Wait until Flash operation is complete */
    while (fmc_bank0_state_get() != FMC_READY) {
    }

    /* Program the Flash word by word */
    uint32_t* data32 = (uint32_t*)data;
    int words = length / 4;

    for (int i = 0; i < words; i++) {
        fmc_word_program(add + (i * 4), data32[i]);
        while (fmc_bank0_state_get() != FMC_READY) {
        }
    }

    /* Lock Flash */
    fmc_lock();
}

void eeprom_read(uint32_t address, uint8_t *buffer, uint32_t length)
{
    read_flash_bin(buffer, EEPROM_START_ADD + address, length);
}

int eeprom_write(uint32_t address, uint8_t *data, uint32_t length)
{
    save_flash_nolib(data, length, EEPROM_START_ADD + address);
    return 0;
}

int eeprom_erase(void)
{
    /* Unlock Flash */
    fmc_unlock();

    /* Clear pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

    /* Erase the entire EEPROM page */
    fmc_page_erase(EEPROM_START_ADD);

    /* Wait until Flash operation is complete */
    while (fmc_bank0_state_get() != FMC_READY) {
    }

    /* Lock Flash */
    fmc_lock();

    return 0;
}
