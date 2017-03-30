//#if 1

//#include <rfid_api_full.h>
//#include <ntag_driver_intern.h>
//#include <curses.h>
#include "mbed.h"
//#include "../NTAG_I2C_API/src/HAL_I2C/inc/HAL_I2C_driver.h"
//#include "../NTAG_I2C_API/src/HAL_NTAG/inc/ntag_driver.h"
//#include "../NTAG_I2C_API/src/HAL_NTAG/inc/ntag_defines.h

//################################################
#ifndef NFC_DEVICE_H
#define NFC_DEVICE_H

#if 0
//#define HAVE_NTAG_INTERRUPT 0
struct nNTAG_DEVICE
{
    NTAG_STATUS_T status;
    HAL_I2C_HANDLE_T i2cbus;
    uint8_t address;
//#ifdef HAVE_NTAG_INTERRUPT
//    ISR_SOURCE_T isr;
//#endif
    uint8_t tx_buffer[TX_START+NTAG_BLOCK_SIZE+NTAG_ADDRESS_SIZE];
    uint8_t rx_buffer[RX_START+NTAG_BLOCK_SIZE];
} ntpa;
//#include "demo_settings.h"

#  define NFC_TEST_DEVICE         NTAG0
#  define NFC_ID_MAX_DEVICES      NTAG_ID_MAX_DEVICES
#  define NFC_INVALID_HANDLE      NTAG_INVALID_HANDLE
#  define NFC_HANDLE_T            NTAG_HANDLE_T
#  define NFC_InitDevice          NTAG_InitDevice
#  define NFC_CloseDevice         NTAG_CloseDevice
#  define NFC_ReadBytes           NTAG_ReadBytes
#  define NFC_WriteBytes          NTAG_WriteBytes
#  define NFC_GetLastError        NTAG_GetLastError
#  define NFC_WaitForEvent        NTAG_WaitForEvent
#  define NFC_ReadRegister        NTAG_ReadRegister
#  define NFC_WriteRegister       NTAG_WriteRegister

#  define NFC_ReadRegister 	      NTAG_ReadRegister
#  define NFC_DisableSRAM         NTAG_DisableSRAM
#  define NFC_EnableSRAM          NTAG_EnableSRAM
#  define NFC_SetPassThroughRFtoI2C NTAG_SetPassThroughRFtoI2C
#  define NFC_SetPassThroughRFtoI2C_withEn NTAG_SetPassThroughRFtoI2C_withEn
#  define NFC_SetPassThroughI2CtoRF NTAG_SetPassThroughI2CtoRF

#  define NFC_BLOCK_SIZE          NTAG_BLOCK_SIZE
#  define NFC_MEM_SIZE_SRAM	      NTAG_MEM_SIZE_SRAM
#  define NFC_MEM_START_ADDR_SRAM         NTAG_MEM_START_ADDR_SRAM
#  define NFC_MEM_START_ADDR_USER_MEMORY  NTAG_MEM_START_ADDR_USER_MEMORY
#  define NFC_MEM_OFFSET_NC_REG   NTAG_MEM_OFFSET_NC_REG
#  define NFC_MEM_OFFSET_NS_REG   NTAG_MEM_OFFSET_NS_REG
#  define NFC_NC_REG_MASK_TR_SRAM_ON_OFF  NTAG_NC_REG_MASK_TR_SRAM_ON_OFF
#  define NFC_NS_REG_MASK_I2C_IF_ON_OFF   NTAG_NS_REG_MASK_I2C_IF_ON_OFF


//switch between Interrupt and Polling mode
//#define INTERRUPT
#ifdef INTERRUPT
#define NTAG_EVENT_RF_WROTE_SRAM       NTAG_EVENT_RF_WROTE_SRAM_INTERRUPT
#define NTAG_EVENT_RF_READ_SRAM        NTAG_EVENT_RF_READ_SRAM_INTERRUPT
#define NTAG_EVENT_FIELD_DETECTED      NTAG_EVENT_FIELD_DETECTED_INTERRUPT
#else
#define NTAG_EVENT_RF_WROTE_SRAM       NTAG_EVENT_RF_WROTE_SRAM_POLLED
#define NTAG_EVENT_RF_READ_SRAM        NTAG_EVENT_RF_READ_SRAM_POLLED
#define NTAG_EVENT_FIELD_DETECTED      NTAG_EVENT_FIELD_DETECTED_POLLED
#endif

#define SRAM_TIMEOUT 500

#endif

//################################################

#  define NFC_HANDLE_T            NTAG_HANDLE_T
//NFC_HANDLE_T ntag_handle;

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

#if 0

extern "C" {

HAL_I2C_STATUS_T uNFC_send(HAL_I2C_HANDLE_T i2cbus, uint8_t address, const uint8_t *bytes, uint8_t len);
HAL_I2C_STATUS_T uNFC_recv(HAL_I2C_HANDLE_T i2cbus, uint8_t address, uint8_t *bytes, uint8_t len);

HAL_I2C_STATUS_T uNFC_send(HAL_I2C_HANDLE_T i2cbus, uint8_t address, const uint8_t *bytes, uint8_t len) {
    int ret = nfc_i2c.write(address, (char *) bytes, len);
    return (uint16_t) ret;
}

HAL_I2C_STATUS_T uNFC_recv(HAL_I2C_HANDLE_T i2cbus, uint8_t address, uint8_t *bytes, uint8_t len) {
    int ret = nfc_i2c.read(address, (char *) bytes, len);
    return (uint16_t) ret;
}

} // extern "C"


HAL_I2C_HANDLE_T i2cbus;

//BOOL theNTAG_ReadRegister (uint8_t reg, uint8_t *val)

BOOL theNTAG_ReadRegister (nNTAG_DEVICE *ntag, uint8_t reg, uint8_t *val)
{
        ntag->tx_buffer[TX_START+0] = NTAG_MEM_BLOCK_SESSION_REGS;
        ntag->tx_buffer[TX_START+1] = reg;

        /* send block number */
        if( HAL_I2C_OK != HAL_I2C_SendBytes(ntag->i2cbus, ntag->address, ntag->tx_buffer, 2) )
        {
            ntag->status = NTAG_ERROR_TX_FAILED;
            return TRUE;
        }

        /* receive bytes */
        if( HAL_I2C_OK != HAL_I2C_RecvBytes(ntag->i2cbus, ntag->address, ntag->rx_buffer, 1) )
        {
            ntag->status = NTAG_ERROR_RX_FAILED;
            return TRUE;
        }

        *val = ntag->rx_buffer[RX_START+0];
        return FALSE;
//    uint8_t tx_buffer[18];
//    tx_buffer[0+0] = NTAG_MEM_BLOCK_SESSION_REGS;
//    tx_buffer[0+1] = reg;
//
//    /* send block number */
//    if( HAL_I2C_OK != uNFC_send(i2cbus, address, tx_buffer, 2) )
//    {
////        ntag->status = NTAG_ERROR_TX_FAILED;
//        return TRUE;
//    }
//
//    /* receive bytes */
//    if( HAL_I2C_OK != uNFC_recv(ntag->i2cbus, ntag->address, ntag->rx_buffer, 1) )
//    {
//        ntag->status = NTAG_ERROR_RX_FAILED;
//        return TRUE;
//    }
//
//    *val = ntag->rx_buffer[RX_START+0];
//    return FALSE;
}

#endif

//---------------------------------------------------------------------
//void HW_getTemp(uint8_t Buffer[])
//{
////#ifdef Board_Demo_v1_4
//    uint8_t RX_Buffer[HAL_I2C_RX_RESERVED_BYTES + 2];
//  uint8_t TX_Buffer[HAL_I2C_TX_RESERVED_BYTES + 1];
//
//  /* get temperature value from sensor */
//  TX_Buffer[HAL_I2C_TX_RESERVED_BYTES + 0] = 0;
//  HAL_I2C_SendBytes(i2cHandleMaster, TEMP_I2C_ADDRESS >> 1, TX_Buffer, 1);
//  HAL_I2C_RecvBytes(i2cHandleMaster, TEMP_I2C_ADDRESS >> 1, RX_Buffer, 2);
//
//  Buffer[0] = RX_Buffer[HAL_I2C_RX_RESERVED_BYTES + 0];
//  Buffer[1] = RX_Buffer[HAL_I2C_TX_RESERVED_BYTES + 1];
////#else
////    Buffer[0] = 0;
////    Buffer[1] = 0;
////    return;
////#endif
//}


void led_thread(void const *args) {
    while (true) {
        led1 = !led1;
        Thread::wait(1000);
    }
}
osThreadDef(led_thread, osPriorityNormal, DEFAULT_STACK_SIZE);

DigitalOut extPower(PTC8);

#define NTAG_MEM_BLOCK_SESSION_REGS                    0xFE

#define NTAG_MEM_OFFSET_NC_REG                         0x00
#define NTAG_NC_REG_MASK_TR_SRAM_ON_OFF                0x40
//return NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG, NTAG_NC_REG_MASK_TR_SRAM_ON_OFF,
//NTAG_NC_REG_MASK_TR_SRAM_ON_OFF);


BOOL NTAG_ReadRegister (NTAG_HANDLE_T ntag, uint8_t reg, uint8_t *val)
{

    ntag->tx_buffer[TX_START+0] = NTAG_MEM_BLOCK_SESSION_REGS;
    ntag->tx_buffer[TX_START+1] = reg;

    /* send block number */
    if( HAL_I2C_OK != HAL_I2C_SendBytes(ntag->i2cbus, ntag->address, ntag->tx_buffer, 2) )
    {
        ntag->status = NTAG_ERROR_TX_FAILED;
        return TRUE;
    }

    /* receive bytes */
    if( HAL_I2C_OK != HAL_I2C_RecvBytes(ntag->i2cbus, ntag->address, ntag->rx_buffer, 1) )
    {
        ntag->status = NTAG_ERROR_RX_FAILED;
        return TRUE;
    }

    *val = ntag->rx_buffer[RX_START+0];
    return FALSE;
}

BOOL NTAG_WriteRegister(NTAG_HANDLE_T ntag, uint8_t reg, uint8_t mask, uint8_t val)
{
    ntag->tx_buffer[TX_START+0] = NTAG_MEM_BLOCK_SESSION_REGS;
    ntag->tx_buffer[TX_START+1] = reg;
    ntag->tx_buffer[TX_START+2] = mask;
    ntag->tx_buffer[TX_START+3] = val;

    if( HAL_I2C_OK != HAL_I2C_SendBytes(ntag->i2cbus, ntag->address, ntag->tx_buffer, 4) )
    {
        ntag->status = NTAG_ERROR_TX_FAILED;
        return TRUE;
    }

    return FALSE;
}


int main() {
    osThreadCreate(osThread(led_thread), NULL);
    printf("Lets write to the NFC\r\n");

    extPower.write(1);
    // Initialize main buffer used to read and write user memory
//    uint8_t rxbuffer[4 * NTAG_BLOCK_SIZE];
//    uint8_t txbuffer[2 * NTAG_BLOCK_SIZE];
//    memset(rxbuffer, 0, 4 * NTAG_BLOCK_SIZE);
//    memset(txbuffer, 0, 2 * NTAG_BLOCK_SIZE);

    while (1) {
        char reg[18] = {0};
        reg[0] = 0x05;

        tx_buffer[TX_START+0] = NTAG_MEM_BLOCK_SESSION_REGS;
        tx_buffer[TX_START+1] = NTAG_MEM_OFFSET_NC_REG;
        tx_buffer[TX_START+2] = NTAG_NC_REG_MASK_TR_SRAM_ON_OFF;
        tx_buffer[TX_START+3] = ~NTAG_NC_REG_MASK_TR_SRAM_ON_OFF;


        int ret = -1;
        ret = nfc_i2c.write(0xAB, reg, 1);

//        printf("write ret %d \r\n", ret);
        ret =  nfc_i2c.read(0xAB, &reg[1], 5);
        printf("the reg values %x, %x, %d\r\n", reg[2], reg, ret);
//        printf("read ret %d \r\n", ret);
//        NTAG_ReadRegister(ntag_handle, NFC_GET_VERSION, reg);

//        dbg_dump("NFC", reg, 7);
        printf("\r\n");
        wait(1);
    }
    // prepare defined state for SRAM
//    NFC_DisableSRAM(ntag_handle);

    //Read NDEF from EEPROM
//    NFC_ReadBytes(ntag_handle, NFC_MEM_START_ADDR_USER_MEMORY, rxbuffer,
//                  4 * NFC_BLOCK_SIZE);

}


#endif