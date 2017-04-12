//
// Created by nirao on 06.04.17.
//

#if 0
#ifndef NFC_HAL_I2C_K82_H
#define NFC_HAL_I2C_K82_H

#include "global_types.h"
#ifdef __cplusplus
extern "C" {
#endif

/** 7-bit slave address */
//#define I2C_SLAVE_ADDR_7BIT     (0xAA)

/** 1Mbps I2C bit-rate */
//#define I2C_BITRATE             (100)
//#define I2C_BITRATE_370         (370)


uint16_t uNFC_recv(uint8_t i2cbus, uint8_t address, uint8_t *bytes, uint8_t len);

uint16_t uNFC_send(uint8_t i2cbus, uint8_t address, const uint8_t *bytes, uint8_t len);

#ifdef __cplusplus
}
#endif
#endif //MBED_NFC_HAL_I2C_K82_H
#endif // #if 0