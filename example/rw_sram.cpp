//
// Created by nirao on 04.04.17.
//
/* Read/ WRITE SRAM*/
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

    while (1) {

        NTAG_EnableSRAM(ntag);

        NTAG_ReadBytes(ntag, NTAG_MEM_START_ADDR_SRAM, rxbuffer,
                       NTAG_MEM_SIZE_SRAM);
        dbg_dump("SRAM", rxbuffer, 6);

        wait_ms(500);

//    /* Header of the NDEF Message
//     * record_len     = message to print + 3
//     * message_length = record_len + 4
//     */
        sram_buf[index++] = 0x03;
        sram_buf[index++] = 0x0D; // Message length
        sram_buf[index++] = 0xd1;

        sram_buf[index++] = 0x01; // Text Record
        sram_buf[index++] = 0x09; // Record length
        sram_buf[index++] = 0x54;
        sram_buf[index++] = 0x02;

        sram_buf[index++] = 0x65;
        sram_buf[index++] = 0x6e;

        sram_buf[index++] = 'U';
        sram_buf[index++] = 'B';
        sram_buf[index++] = 'I';
        sram_buf[index++] = 'R';
        sram_buf[index++] = 'C';
        sram_buf[index++] = 'H';

        sram_buf[index++] = 0xFE;
        sram_buf[index++] = 0x00;

        NTAG_WriteBytes(ntag, NTAG_MEM_START_ADDR_SRAM, sram_buf, index);
        dbg_dump("WRAM", sram_buf, index);
        wait_ms(500);

        NTAG_ReadBytes(ntag, NTAG_MEM_START_ADDR_SRAM, rxbuffer, 6);
        dbg_dump("RRAM", rxbuffer, 6);
        wait_ms(500);

        NTAG_SetSRAMMirrorLowerPageAddr(ntag, 0x01);

        NTAG_EnableSRAMMirrorMode(ntag);
        wait_ms(100);

        while (1) {

            NTAG_ReadBytes(ntag, NTAG_MEM_START_ADDR_SRAM, rxbuffer, 10);
            dbg_dump("RRAM", rxbuffer, 10);
            wait_ms(500);

        }
    }
}

#endif