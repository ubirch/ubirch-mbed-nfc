//
// Created by nirao on 24.04.17.
//
#include "ex_defs.h"

#ifdef ndef_test

#include "mbed.h"
#include "ntag_defines.h"
#include "ntag_driver.h"
#include "HAL_I2C_driver.h"
#include "ntag_bridge.h"
#include <ntag_driver_intern.h>
#include <ntag_parser.h>

#include "../crypto/crypto.h"
#include "../config.h"
#include "../wolfSSL/wolfssl/wolfcrypt/ed25519.h"

void ndbg_dump(const char *prefix, const uint8_t *b, size_t size) {
    for (int i = 0; i < size; i += 16) {
        if (prefix && strlen(prefix) > 0) printf("%s %06x: ", prefix, i);
        for (int j = 0; j < 16; j++) {
            if ((i + j) < size) printf("%02x", b[i + j]); else printf("  ");
            if ((j+1) % 2 == 0) putchar(' ');
        }
        putchar(' ');
        for (int j = 0; j < 16 && (i + j) < size; j++) {
            putchar(b[i + j] >= 0x20 && b[i + j] <= 0x7E ? b[i + j] : '.');
        }
        printf("\r\n");
        wait_ms(100);
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

DigitalOut extPower(PTC8);


int main() {

    extPower.write(1);

    int index = 0;
    /* Crypto stuff*/
    static uc_ed25519_key uc_key;

    uc_init();
    uc_import_ecc_key(&uc_key, device_ecc_key, device_ecc_key_len);

    uint8_t pubkeyLen = 32;
    uint8_t messageLen = uint8_t(pubkeyLen + 7);
    /* End Crypto stuff*/

    uint8_t sram_buf[NTAG_MEM_SRAM_SIZE];
    memset(sram_buf, 0, NTAG_MEM_SRAM_SIZE);

    uint8_t rxbuffer[64];// * NTAG_I2C_BLOCK_SIZE];
    memset(rxbuffer, 0, 64);// * NTAG_I2C_BLOCK_SIZE);

    sram_buf[index++] = 0x03; //field type NDEF
    sram_buf[index++] = 0x27; //0x0D; //0x27; // Message length
    sram_buf[index++] = 0xD1; //11010001
    sram_buf[index++] = 0x01; // Text Record type "T"
    sram_buf[index++] = 0x23; //09; //20; // Record length
    sram_buf[index++] = 0x54; // text "T"
    sram_buf[index++] = 0x02; //
    sram_buf[index++] = 0x65; //"E"
    sram_buf[index++] = 0x6e; //"N"

    memcpy(&sram_buf[10], uc_key.p, 32);

    sram_buf[43] = 0xFE;
    sram_buf[44] = 0x00;

    ndbg_dump("PAYL", sram_buf, 44);

    NTAG_HANDLE_T *ntag_handle = new NTAG_HANDLE_T;
    ntag_handle->address = 0xAA;

    NTAG_EnableSRAM(ntag_handle);

    NTAG_ReadBytes(ntag_handle, NTAG_MEM_ADDR_START_SRAM, rxbuffer,
                   NTAG_MEM_SRAM_SIZE);

    NTAG_WriteBytes(ntag_handle, NTAG_MEM_ADDR_START_SRAM, sram_buf, 44);
    ndbg_dump("WRAM", sram_buf, 44);
    wait_ms(500);

    NTAG_ReadBytes(ntag_handle, NTAG_MEM_ADDR_START_SRAM, rxbuffer, 44);
    ndbg_dump("RRAM", rxbuffer, 44);
    wait_ms(500);

    NTAG_SetSRAMMirrorLowerPageAddr(ntag_handle, 0x01);

    NTAG_SetSRAMMirrorOnOff(ntag_handle, true);
    wait_ms(100);

    int ret = -1;
    uint8_t rxBufSize;
    while (1) {

        uint8_t msgrxbuffer[64];
        NTAG_ReadBytes(ntag_handle, NTAG_MEM_ADDR_START_SRAM, rxbuffer, 64);
        ndbg_dump("RRAM", rxbuffer, 64);
        printf("rx %s\r\n", rxbuffer);
        wait_ms(50);
        parse_ndef(rxbuffer, msgrxbuffer, &rxBufSize);
        ndbg_dump("recv", msgrxbuffer, 64);
        wait_ms(500);
        memset(rxbuffer, 0, 64);
        memset(msgrxbuffer, 0, 64);

    }
}

#endif
