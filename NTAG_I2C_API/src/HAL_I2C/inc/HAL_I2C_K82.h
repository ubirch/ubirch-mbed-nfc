////
//// Created by nirao on 28.03.17.
////
//
//#ifndef NFC_DEVICE_H
//#define NFC_DEVICE_H
//
////#include "demo_settings.h"
//
//#  define NFC_TEST_DEVICE         NTAG0
//#  define NFC_ID_MAX_DEVICES      NTAG_ID_MAX_DEVICES
//#  define NFC_INVALID_HANDLE      NTAG_INVALID_HANDLE
//#  define NFC_HANDLE_T            NTAG_HANDLE_T
//#  define NFC_InitDevice          NTAG_InitDevice
//#  define NFC_CloseDevice         NTAG_CloseDevice
//#  define NFC_ReadBytes           NTAG_ReadBytes
//#  define NFC_WriteBytes          NTAG_WriteBytes
//#  define NFC_GetLastError        NTAG_GetLastError
//#  define NFC_WaitForEvent        NTAG_WaitForEvent
//#  define NFC_ReadRegister        NTAG_ReadRegister
//#  define NFC_WriteRegister       NTAG_WriteRegister
//
//#  define NFC_ReadRegister 	      NTAG_ReadRegister
//#  define NFC_DisableSRAM         NTAG_DisableSRAM
//#  define NFC_EnableSRAM          NTAG_EnableSRAM
//#  define NFC_SetPassThroughRFtoI2C NTAG_SetPassThroughRFtoI2C
//#  define NFC_SetPassThroughRFtoI2C_withEn NTAG_SetPassThroughRFtoI2C_withEn
//#  define NFC_SetPassThroughI2CtoRF NTAG_SetPassThroughI2CtoRF
//
//#  define NFC_BLOCK_SIZE          NTAG_BLOCK_SIZE
//#  define NFC_MEM_SIZE_SRAM	      NTAG_MEM_SIZE_SRAM
//#  define NFC_MEM_START_ADDR_SRAM         NTAG_MEM_START_ADDR_SRAM
//#  define NFC_MEM_START_ADDR_USER_MEMORY  NTAG_MEM_START_ADDR_USER_MEMORY
//#  define NFC_MEM_OFFSET_NC_REG   NTAG_MEM_OFFSET_NC_REG
//#  define NFC_MEM_OFFSET_NS_REG   NTAG_MEM_OFFSET_NS_REG
//#  define NFC_NC_REG_MASK_TR_SRAM_ON_OFF  NTAG_NC_REG_MASK_TR_SRAM_ON_OFF
//#  define NFC_NS_REG_MASK_I2C_IF_ON_OFF   NTAG_NS_REG_MASK_I2C_IF_ON_OFF
//
//
////switch between Interrupt and Polling mode
////#define INTERRUPT
//#ifdef INTERRUPT
//#define NTAG_EVENT_RF_WROTE_SRAM       NTAG_EVENT_RF_WROTE_SRAM_INTERRUPT
//#define NTAG_EVENT_RF_READ_SRAM        NTAG_EVENT_RF_READ_SRAM_INTERRUPT
//#define NTAG_EVENT_FIELD_DETECTED      NTAG_EVENT_FIELD_DETECTED_INTERRUPT
//#else
//#define NTAG_EVENT_RF_WROTE_SRAM       NTAG_EVENT_RF_WROTE_SRAM_POLLED
//#define NTAG_EVENT_RF_READ_SRAM        NTAG_EVENT_RF_READ_SRAM_POLLED
//#define NTAG_EVENT_FIELD_DETECTED      NTAG_EVENT_FIELD_DETECTED_POLLED
//#endif
//
//#define SRAM_TIMEOUT 500
//
//#endif //NTAG_HAL_I2C_K82_H
