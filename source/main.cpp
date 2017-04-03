//#if 1

//#include <rfid_api_full.h>
//#include <ntag_driver_intern.h>
//#include <curses.h>
#include "mbed.h"
#include "ntag_defines.h"
#include "ntag_driver.h"
#include "HAL_timer_driver.h"
#include "HAL_I2C_driver.h"
#include "ntag_bridge.h"

#define NFC_SDA I2C_SDA
#define NFC_SCL I2C_SCL

#define NFC_I2C_ADDR     (0xAA)
#define NFC_GET_VERSION  (0x60)

DigitalInOut led1(LED1);
I2C nfc_i2c(NFC_SDA, NFC_SCL);

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

void HAL_Timer_delay_ms(uint32_t ms){
    wait_ms(ms);
}

#ifdef __cplusplus
extern "C" {
#endif

uint16_t uNFC_send(HAL_I2C_HANDLE_T i2cbus, uint8_t address, const uint8_t *bytes, uint8_t len) {
    int ret = nfc_i2c.write(address, (char *) bytes, len);
    return (uint16_t) ret;
}

uint16_t uNFC_recv(HAL_I2C_HANDLE_T i2cbus, uint8_t address, uint8_t *bytes, uint8_t len) {
    int ret = nfc_i2c.read(address, (char *) bytes, len);
    return (uint16_t) ret;
}
#ifdef __cplusplus
}
#endif

void led_thread(void const *args) {
    while (true) {
        led1 = !led1;
        Thread::wait(1000);
    }
}
osThreadDef(led_thread, osPriorityNormal, DEFAULT_STACK_SIZE);

DigitalOut extPower(PTC8);

int main() {

    NTAG_HANDLE_T *ntag = new NTAG_HANDLE_T;

//    ntag = &NTAG_DEVICE_DEFAULTS;

    ntag->address = 0xAA;
    osThreadCreate(osThread(led_thread), NULL);

    printf("Lets write to the NFC\r\n");

    extPower.write(1);


//    NTAG_EnableSRAM(ntag);
//    while(1) {

        uint8_t sram_buf[NTAG_MEM_SIZE_SRAM];
        memset(sram_buf, 0, NTAG_MEM_SIZE_SRAM);
        uint16_t index = 0;
        uint8_t reg = 0;

        NTAG_DisableSRAM(ntag);
//        NTAG_EnableSRAM(ntag);

#if 1
        // Header of the NDEF Message
        sram_buf[index++] = 0x03;
        sram_buf[index++] = 0x1C; // Message length
        sram_buf[index++] = 0xd1;

        sram_buf[index++] = 0x01; // Text Record
        sram_buf[index++] = 0x18; // Record length
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
#endif

#if 0

    // Header of the NDEF Message
    sram_buf[index++] = 0x03;
    sram_buf[index++] = 0x1C; // Message length
    sram_buf[index++] = 0xd1;

    sram_buf[index++] = 0x01; // Text Record
    sram_buf[index++] = 0x18; // Record length
    sram_buf[index++] = 0x54;
    sram_buf[index++] = 0x02;

    sram_buf[index++] = 0x65;
    sram_buf[index++] = 0x6e;

    char temp[5];
    uint8_t k;

    memset(temp, 0, 5);
//    itoa(time1, temp, 10);

    sram_buf[index++] = '(';

    for (k = 0; k < 5 - strlen(temp); k++)
        sram_buf[index++] = '0';

    for (k = 0; k < strlen(temp); k++)
        sram_buf[index++] = temp[k];

    sram_buf[index++] = ')';

    memset(temp, 0, 5);
//    itoa(time2, temp, 10);

    sram_buf[index++] = '(';

    for (k = 0; k < 5 - strlen(temp); k++)
        sram_buf[index++] = '0';

    for (k = 0; k < strlen(temp); k++)
        sram_buf[index++] = temp[k];

    sram_buf[index++] = ')';

    memset(temp, 0, 5);

    sram_buf[index++] = '(';

    for (k = 0; k < 5 - strlen(temp); k++)
        sram_buf[index++] = '0';

    for (k = 0; k < strlen(temp); k++)
        sram_buf[index++] = temp[k];

    sram_buf[index++] = ')';

// end of NDEF Message
    sram_buf[index++] = 0xFE;
    sram_buf[index++] = 0x00;
#endif

        printf("Lets write to the NDEF\r\n");

        NTAG_WriteBytes(ntag, NTAG_MEM_START_ADDR_USER_MEMORY,
                       sram_buf, index);
        NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NS_REG, &reg);
        while (reg & NTAG_NS_REG_MASK_I2C_IF_ON_OFF) {
            NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NS_REG, &reg);
//        }
//        memset(sram_buf, 0, NTAG_MEM_SIZE_SRAM);
//        memcpy(&sram_buf[index], "Board Ver.: ", 12);
//        index = 12;
//        sram_buf[index++] = 'z';
//        sram_buf[index++] = '.';
//        sram_buf[index++] = 'x';
//        sram_buf[index++] = '\n';
//
//        memcpy(&sram_buf[index], "FW    Ver.: ", 12);
//        index += 12;
//        sram_buf[index++] = 'z';
//        sram_buf[index++] = '.';
//        sram_buf[index++] = 'z';
//        sram_buf[index++] = '\n';
//
//        // write back Version information
//        NTAG_SetPassThroughI2CtoRF(ntag);
//        NTAG_WriteBytes(ntag, NTAG_MEM_START_ADDR_SRAM, sram_buf,
//                       NTAG_MEM_SIZE_SRAM);
//
//        while (NTAG_WaitForEvent(ntag, NTAG_EVENT_RF_READ_SRAM_POLLED, 10));

//        NTAG_EnableSRAMMirrorMode(ntag);
//        wait(3);
    }



//    while(1) {
//        int err = NTAG_ERR_OK;
//        uint8_t current_ses_reg = 0;
//        err = NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NC_REG, &current_ses_reg);
//        if (current_ses_reg & NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF) {
//            printf("sram on %x\r\n", current_ses_reg);
//            current_ses_reg |= NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF;
////            current_ses_reg &= ~NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF;
//            err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG,
//                                     NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF, current_ses_reg);
//        } else {
//            printf("sram off %x\r\n", current_ses_reg);
//            current_ses_reg &= ~NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF;
////            current_ses_reg |= NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF;
//            err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG,
//                                     NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF, current_ses_reg);
//        }
//        printf("ss\r\n");
//        wait(4);
//    }
#if 0
//    while (1) {
        char reg[18] = {0};
        reg[0] = 0x05;

//        int TX_START = 1;
        uint8_t tx_buffer[32];

//        tx_buffer[TX_START+0] = NTAG_MEM_BLOCK_SESSION_REGS;
//        tx_buffer[TX_START+1] = NTAG_MEM_OFFSET_NC_REG;
//        tx_buffer[TX_START+2] = NTAG_NC_REG_MASK_TR_SRAM_ON_OFF;
//        tx_buffer[TX_START+3] = ~NTAG_NC_REG_MASK_TR_SRAM_ON_OFF;

        NTAG_HANDLE_T ntag;
//        NFC_ReadRegister(ntag, NTAG_MEM_OFFSET_NC_REG, NTAG_NC_REG_MASK_TR_SRAM_ON_OFF,
//                           NTAG_NC_REG_MASK_TR_SRAM_ON_OFF);
//        extern "C" {

//        NFC_DisableSRAM(ntag);

//            NTAG_EnableSRAM(ntag);
//        }

        tx_buffer[TX_START+0] = NTAG_MEM_BLOCK_SESSION_REGS;
        tx_buffer[TX_START+1] = 0x00;

        if( 0 != nfc_i2c.write(0xAB,(char *)tx_buffer, 2) )
        {
//            printf("error\r\n");
        }

        uint8_t rx_buffer;// = {0};
        /* receive bytes */
        if( 0 != nfc_i2c.read(0xAB, (char *)rx_buffer, 1))
        {
//            printf("error\r\n");
        }

//        printf("The reg is %x\r\n", rx_buffer);
        if (rx_buffer & NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF)
        {
            printf("sram enabled\r\n");
            rx_buffer &= ~NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF;
            tx_buffer[TX_START+0] = NTAG_MEM_BLOCK_SESSION_REGS;
            tx_buffer[TX_START+1] = NTAG_MEM_OFFSET_NC_REG;
            tx_buffer[TX_START+2] = NTAG_NC_REG_MASK_TR_SRAM_ON_OFF;
            tx_buffer[TX_START+3] = rx_buffer;

            int ret = nfc_i2c.write(0xAA, (char *)tx_buffer, 4);
        }
        else
        {
            printf("the sram off \r\n");
            rx_buffer |= NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF;

        tx_buffer[TX_START+0] = NTAG_MEM_BLOCK_SESSION_REGS;
        tx_buffer[TX_START+1] = NTAG_MEM_OFFSET_NC_REG;
        tx_buffer[TX_START+2] = NTAG_NC_REG_MASK_TR_SRAM_ON_OFF;
        tx_buffer[TX_START+3] = rx_buffer;

            int ret = nfc_i2c.write(0xAA, (char *)tx_buffer, 4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            size_t i = 0;

            uint8_t block = NTAG_MEM_START_ADDR_USER_MEMORY % NTAG_BLOCK_SIZE;
            tx_buffer[TX_START] = block;

            /* send block number */
            if( 0 != nfc_i2c.write(0xAA, (char *)tx_buffer, 1) )
            {
                printf("write lock\r\n");
            }


//            /* receive bytes */
//            if( HAL_I2C_OK != HAL_I2C_RecvBytes(ntag->i2cbus, ntag->address, ntag->rx_buffer, NTAG_BLOCK_SIZE) )
//            {
//                ntag->status = NTAG_ERROR_RX_FAILED;
//                return TRUE;
//            }

        }


//        int ret = -1;
//        ret = nfc_i2c.write(0xAB, (char *)tx_buffer, 1);
//
////        printf("write ret %d \r\n", ret);
//        ret =  nfc_i2c.read(0xAB, &reg[1], 5);
//        printf("the reg values %x, %x, %d\r\n", reg[2], reg, ret);
//        printf("read ret %d \r\n", ret);
//        NTAG_ReadRegister(ntag_handle, NFC_GET_VERSION, reg);

//        dbg_dump("NFC", reg, 7);
//        printf("\r\n");
//        wait(1);
//    }
    // prepare defined state for SRAM
//    NFC_DisableSRAM(ntag_handle);

    //Read NDEF from EEPROM
//    NFC_ReadBytes(ntag_handle, NFC_MEM_START_ADDR_USER_MEMORY, rxbuffer,
//                  4 * NFC_BLOCK_SIZE);

#endif

}


