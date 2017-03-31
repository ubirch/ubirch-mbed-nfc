////
//// Created by nirao on 31.03.17.
////
//
//#ifndef NTAG_HAL_I2C_K82_H
//#define NTAG_HAL_I2C_K82_H
//
//#include "HAL_I2C_driver.h"
////#include "board.h"
//
///** 7-bit slave address */
//#define I2C_SLAVE_ADDR_7BIT     (0xAA)
//
///** 1Mbps I2C bit-rate */
//#define I2C_BITRATE             (100)
//#define I2C_BITRATE_370         (370)
//
///** Memory size for I2C ROM */
//#define I2C_ROM_MEM_SIZE        (0x100UL)
//
///* Initializes pin muxing for I2C interface - note that SystemInit() may
// already setup your pin muxing at system startup */
//void Init_I2C_PinMux(void);
//
///* If error occurs, turn LED 0 (red LED) on and wait forever */
//void spin_on_error(void);
//
///* I2C Receive Callback function */
//void App_I2C_Callback_Receive(uint32_t err_code, uint32_t n);
//
///* I2C Transmit Callback function */
//void App_I2C_Callback_Transmit(uint32_t err_code, uint32_t n);
//
///* I2C Transmit and Receive Callback function */
//void App_I2C_Callback_MasterTxRx(uint32_t err_code, uint32_t n);
//
///* I2C Interrupt Service Routine */
//void I2C_IRQHandler(void);
//
///* Set up hardware for lpc8xx */
//int16_t I2CMasterInit(uint32_t bitrate);
//
//int16_t TransmitReceive(uint8_t *tx, uint8_t tx_len, uint8_t *rx,
//                        uint8_t rx_len);
//int16_t TransmitInvalid();
//
//int16_t i2c_recv(uint8_t *rx, uint8_t rx_len);
//int16_t i2c_send(uint8_t *tx, uint8_t tx_len);
//
////extern "C" {
////uint16_t uNFC_recv(uint8_t address, uint8_t *bytes, uint8_t len);
////uint16_t uNFC_send(uint8_t address, const uint8_t *bytes, uint8_t len);
////}
//#endif //NTAG_HAL_I2C_K82_H
