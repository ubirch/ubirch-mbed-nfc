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


    /*
     * write to sram stuff goes here
     *
     * Enable Passthrough Mode RF->I2C
     */
    while (1) {


//        sram_buf[index++] = 'T';
//        sram_buf[index++] = 'B';
//        sram_buf[index++] = 'I';
//        sram_buf[index++] = 'R';
//        sram_buf[index++] = 'C';
//        sram_buf[index++] = 'H';

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

    sram_buf[index++] = 'z';
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

//        NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_SM_REG, 0xFF, 0x01);
//        NTAG_WriteRegister(ntag, 0x00, 0x42, 0x02);
        int ret = NTAG_EnableSRAMMirrorMode(ntag);
        wait_ms(100);
//        int ret = NTAG_SetPassThroughRFtoI2C(ntag);

//        printf("ret%d\r\n", ret);

        while(1){

            NTAG_ReadBytes(ntag, NTAG_MEM_START_ADDR_SRAM, rxbuffer, 10);
            dbg_dump("RRAM", rxbuffer, 10);
            wait_ms(500);

        }
        // write back Data
//        NTAG_SetPassThroughI2CtoRF(ntag);

        // waiting till RF has read
//        while (NTAG_WaitForEvent(ntag, NTAG_EVENT_RF_READ_SRAM, NTAG_MEM_SIZE_SRAM));


//        // Enable Passthrough Mode RF->I2C
//        NTAG_SetPassThroughRFtoI2C(ntag);
//        NTAG_EnableSRAM(ntag);
//
//        // wait for RF Write in the SRAM terminator page
//        while (NTAG_WaitForEvent(ntag, NTAG_EVENT_RF_WROTE_SRAM,
//                                 SRAM_TIMEOUT)) {
//
//            // check if PT is off(happens when NTAG is not in the field anymore)
//            // and switch it back on
//            NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NC_REG, &reg);
//            if (!(reg & NTAG_NC_REG_MASK_TR_SRAM_ON_OFF)) {
//                NTAG_SetPassThroughRFtoI2C_withEn(ntag);
//            }
//        }
//
//        // get the SRAM Data
//        memset(sram_buf, 0, NTAG_MEM_SIZE_SRAM);
//        NTAG_ReadBytes(ntag, NTAG_MEM_START_ADDR_SRAM, sram_buf,
//                       NTAG_MEM_SIZE_SRAM);

//        dbg_dump("NFCM", sram_buf, 6);
        wait(2);

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