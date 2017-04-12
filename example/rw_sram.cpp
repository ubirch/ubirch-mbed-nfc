//
// Created by nirao on 04.04.17.
//
/* Read/ WRITE SRAM*/
#if 1
#include "mbed.h"
#include "ntag_defines.h"
#include "ntag_driver.h"
#include "HAL_I2C_driver.h"
#include "ntag_bridge.h"

#include <ntag_driver_intern.h>
#include "../NTAG_I2C_API/src/inc/global_types.h"

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


BOOL NTAG_SetSRAMMirrorLowerPageAddr(NTAG_HANDLE_T *ntag, uint8_t addr)
{
    int err = NTAG_ERR_OK;
    if (addr > 0x74)
    {
        return (NTAG_ERR_INVALID_PARAM);
    }
    err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_SRAM_MIRROR_BLOCK, 0xFF, addr);
    return (err);
}

//---------------------------------------------------------------------
BOOL NTAG_EnableSRAM(NTAG_HANDLE_T *ntag)
{
    return NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG, NTAG_NC_REG_MASK_PTHRU_ON_OFF,
                              NTAG_NC_REG_MASK_PTHRU_ON_OFF);
}

//---------------------------------------------------------------------
BOOL NTAG_DisableSRAM(NTAG_HANDLE_T *ntag)
{
    return NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG, NTAG_NC_REG_MASK_PTHRU_ON_OFF,
                              ~NTAG_NC_REG_MASK_PTHRU_ON_OFF);
}


osThreadDef(led_thread, osPriorityNormal, DEFAULT_STACK_SIZE);

int main() {

    osThreadCreate(osThread(led_thread), NULL);

    printf("Lets write to the NFC\r\n");

    extPower.write(1);

    NTAG_HANDLE_T *ntag_handle = new NTAG_HANDLE_T;
    ntag_handle->address = 0xAA;

    uint8_t sram_buf[NTAG_MEM_SRAM_SIZE];
    memset(sram_buf, 0, NTAG_MEM_SRAM_SIZE);

    uint8_t rxbuffer[4 * NTAG_I2C_BLOCK_SIZE];
    memset(rxbuffer, 0, 4 * NTAG_I2C_BLOCK_SIZE);
    uint16_t index = 0;
    uint8_t reg = 0;

    NTAG_DisableSRAM(ntag_handle);

    while (1) {

        NTAG_EnableSRAM(ntag_handle);

        NTAG_ReadBytes(ntag_handle, NTAG_MEM_ADDR_START_SRAM, rxbuffer,
                       NTAG_MEM_SRAM_SIZE);
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

        NTAG_WriteBytes(ntag_handle, NTAG_MEM_ADDR_START_SRAM, sram_buf, index);
        dbg_dump("WRAM", sram_buf, index);
        wait_ms(500);

        NTAG_ReadBytes(ntag_handle, NTAG_MEM_ADDR_START_SRAM, rxbuffer, 6);
        dbg_dump("RRAM", rxbuffer, 6);
        wait_ms(500);

        NTAG_SetSRAMMirrorLowerPageAddr(ntag_handle, 0x01);

        NTAG_SetSRAMMirrorOnOff(ntag_handle, true);
//            NTAG_EnableSRAMMirrorMode(ntag);
        wait_ms(100);


        int ret = -1;
//        uint8_t rxbuffer[16];

        for (int i = 0; i < 3 && ret != 0; i++) {
            ret = NTAG_ReadBytes(ntag_handle, 0x00, rxbuffer, 16);
            dbg_dump("TAKE", rxbuffer, 16);
            wait_ms(300);
            printf("%d\r\n", ret);
            wait_ms(300);

        }

        while (1) {

            NTAG_ReadBytes(ntag_handle, NTAG_MEM_ADDR_START_SRAM, rxbuffer, 10);
            dbg_dump("RRAM", rxbuffer, 10);
            wait_ms(500);

        }
    }
}

#endif