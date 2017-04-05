//
// Created by nirao on 04.04.17.
//
#if 1
#include "mbed.h"
#include "../source/inc/ntag_defines.h"
#include "../source/inc/ntag_driver.h"
#include "../source/inc/HAL_timer_driver.h"
#include "../source/inc/HAL_I2C_driver.h"
#include "../source/inc/ntag_bridge.h"

DigitalInOut led1(LED1);
DigitalOut extPower(PTC8);


void dbg_dump(const char *prefix, const uint8_t *b, size_t size) {
    for (int i = 0; i < size; i += 16) {
        if (prefix && strlen(prefix) > 0) printf("%s %06x: ", prefix, i);
        for (int j = 0; j < 16; j++) {
            if ((i + j) < size) printf("%02x", b[i + j]); else printf("  ");
            if ((j + 1) % 2 == 0) putchar(' ');
        }
        putchar(' ');
        for (int j = 0; j < 16 && (i + j) < size; j++) {
            putchar(b[i + j] >= 0x20 && b[i + j] <= 0x7E ? b[i + j] : '.');
        }
        printf("\r\n");
    }
}

void led_thread(void const *args) {
    while (true) {
        led1 = !led1;
        Thread::wait(1000);
    }
}

osThreadDef(led_thread, osPriorityNormal, DEFAULT_STACK_SIZE);

int main() {

    osThreadCreate(osThread(led_thread), NULL);

    printf("Lets write to the NFC\r\n");

    extPower.write(1);

    NTAG_HANDLE_T *ntag = new NTAG_HANDLE_T;

    uint8_t sram_buf[NTAG_MEM_SIZE_SRAM];
    memset(sram_buf, 0, NTAG_MEM_SIZE_SRAM);

    uint8_t rxbuffer[4 * NTAG_BLOCK_SIZE];
    memset(rxbuffer, 0, 4 * NTAG_BLOCK_SIZE);
    uint16_t index = 0;
    uint8_t reg = 0;

    NTAG_DisableSRAM(ntag);

    NTAG_EnableSRAMMirrorMode(ntag);

    /*
     * write to sram stuff goes here
     *
     * Enable Passthrough Mode RF->I2C
     */
    while (1) {

        // Enable Passthrough Mode RF->I2C
        NTAG_SetPassThroughRFtoI2C(ntag);
        NTAG_EnableSRAM(ntag);

        // wait for RF Write in the SRAM terminator page
        while (NTAG_WaitForEvent(ntag, NTAG_EVENT_RF_WROTE_SRAM,
                                 SRAM_TIMEOUT)) {

            // check if PT is off(happens when NTAG is not in the field anymore)
            // and switch it back on
            NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NC_REG, &reg);
            if (!(reg & NTAG_NC_REG_MASK_TR_SRAM_ON_OFF)) {
                NTAG_SetPassThroughRFtoI2C_withEn(ntag);
            }
        }

        // get the SRAM Data
        memset(sram_buf, 0, NTAG_MEM_SIZE_SRAM);
        NTAG_ReadBytes(ntag, NTAG_MEM_START_ADDR_SRAM, sram_buf,
                       NTAG_MEM_SIZE_SRAM);

        dbg_dump("NFCM", sram_buf, 10);
        wait(3);

    }

    /*
     * read from sram stuff goes here
     * get the SRAM Data
     */
//    NTAG_SetPassThroughI2CtoRF(ntag);

    memset(sram_buf, 0, NTAG_MEM_SIZE_SRAM);
    NTAG_ReadBytes(ntag, NTAG_MEM_START_ADDR_SRAM, sram_buf,
                   NTAG_MEM_SIZE_SRAM);


//    dbg_dump("NTAG", sram_buf, NTAG_MEM_SIZE_SRAM);
}

#endif