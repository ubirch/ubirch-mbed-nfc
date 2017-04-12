//
// Created by nirao on 04.04.17.
//

#include "mbed.h"
#include "HAL_I2C_driver.h"

#define NFC_SDA I2C_SDA
#define NFC_SCL I2C_SCL

I2C nfc_i2c(NFC_SDA, NFC_SCL);

/*
 * HAL Function prototype description
 */
#ifdef __cplusplus
extern "C" {
#endif

void HAL_Timer_delay_ms(uint32_t ms){
    wait_ms(ms);
}

uint16_t uNFC_send(uint8_t i2cbus, uint8_t address, const uint8_t *bytes, uint8_t len) {
    int ret = nfc_i2c.write(address, (char *) bytes, len);
    return (uint16_t) ret;
}

uint16_t uNFC_recv(uint8_t i2cbus, uint8_t address, uint8_t *bytes, uint8_t len) {
    int ret = nfc_i2c.read(address, (char *) bytes, len);
    return (uint16_t) ret;
}
#ifdef __cplusplus
}
#endif