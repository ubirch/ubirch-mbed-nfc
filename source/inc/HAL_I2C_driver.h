/*
****************************************************************************
* Copyright(c) 2014 NXP Semiconductors                                     *
* All rights are reserved.                                                 *
*                                                                          *
* Software that is described herein is for illustrative purposes only.     *
* This software is supplied "AS IS" without any warranties of any kind,    *
* and NXP Semiconductors disclaims any and all warranties, express or      *
* implied, including all implied warranties of merchantability,            *
* fitness for a particular purpose and non-infringement of intellectual    *
* property rights.  NXP Semiconductors assumes no responsibility           *
* or liability for the use of the software, conveys no license or          *
* rights under any patent, copyright, mask work right, or any other        *
* intellectual property rights in or to any products. NXP Semiconductors   *
* reserves the right to make changes in the software without notification. *
* NXP Semiconductors also makes no representation or warranty that such    *
* application will be suitable for the specified use without further       *
* testing or modification.                                                 *
*                                                                          *
* Permission to use, copy, modify, and distribute this software and its    *
* documentation is hereby granted, under NXP Semiconductors' relevant      *
* copyrights in the software, without fee, provided that it is used in     *
* conjunction with NXP Semiconductor products(UCODE I2C, NTAG I2C).        *
* This  copyright, permission, and disclaimer notice must appear in all    *
* copies of this code.                                                     *
****************************************************************************
*/
#ifndef _HAL_I2C_DRIVER_H_
#define _HAL_I2C_DRIVER_H_
/** @file HAL_I2C_driver.h
 * \brief Public interface to access the I2C hardware.
 *
 * This is a documentation of the API the other modules expect to see,
 * the actual implementation is platform dependent. Please also consult
 * the porting guide for further information regarding the functions in
 * this module.
 */
//#include "mbed.h"

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/***********************************************************************/
/* API DESCRIPTION                                                     */
/***********************************************************************/
#if 0 // API_DESCRIPTION
/*
 * PLEASE MODIFY HAL_I2C_HANDLE_T, STATUS_T AND STATUS_OK TO SUIT YOUR NEED
 * IMPLEMENT HAL_I2C_InitDevice() / HAL_I2C_CloseDevice()
 *           HAL_I2C_RecvBytes() / HAL_I2C_SendBytes() AS SPECIFIED
 *
 * IF NECESSARY SPECIFY THE NUMBER OF BYTES NEEDED INTERNALLY BY
 * YOUR I2C IMPLEMENTATION IN THE HAL_I2C_XX_RESERVED_BYTES FIELDS
 * ALL BUFFERS WILL BE INCREASED BY THAT AMOUNT.
 *
 * e.g. HAL_I2C_TX_RESERVED_BYTES == 2, tx len = 5
 *      bytes[0-1] are reserved for i2c driver
 *      bytes[2-7] contain to be transmitted bytes
 */
#define HAL_I2C_HANDLE_T          < e.g. void*>
#define HAL_I2C_INIT_PARAM_T      < e.g. uint32_t bitrate >
#define HAL_I2C_INIT_DEFAULT      < e.g. 400000 >
#define HAL_I2C_INVALID_HANDLE    < e.g. NULL >
#define HAL_I2C_STATUS_T          < e.g. bool >
#define HAL_I2C_OK                < e.g. false>
#define HAL_I2C_RX_RESERVED_BYTES < e.g. 0 >
#define HAL_I2C_TX_RESERVED_BYTES < e.g. 1 >

/**
 * \brief initialize the I2C hardware and return a handle
 *
 *	This functions initializes the I2C hardware as specified by the
 *	HAL_I2C_INIT_PARAM_T parameter and returns a handle to be used
 *	for further operations.
 *
 * \param 	params 	structure holding all information necessary for
 * 					initializing the device
 *
 * \return 		 	HAL_I2C_INVALID_HANDLE on failure
 */
HAL_I2C_HANDLE_T HAL_I2C_InitDevice(HAL_I2C_INIT_PARAM_T params);

/**
 * \brief close handle and shutdown the I2C hardware
 *
 *	This functions puts the I2C hardware specified by handle into a
 *	save state and closes the handle
 *
 * \param 	handle 	handle to identify the i2c-bus instance
 *
 * \return 		 	HAL_I2C_INVALID_HANDLE on failure
 */
void HAL_I2C_CloseDevice(HAL_I2C_HANDLE_T handle);

/**
 * \brief receive len number of bytes on the specified I2C bus
 *
 *	This functions sends a start condition and then the i2c-address of the
 *	destination device. Afterwards it reads len number of bytes from the
 *	device followed by a stop condition.
 *
 *	| +START | +ADDR |-A| -D0 |+A| ... | -DN |+A| +STOP |
 *	+ send / - receive / A Ack-bit
 *
 * \param 	i2cbus 	handle to identify the i2c-bus instance
 * \param 	bytes  	array of bytes to be received.
 * \param   address 7-bit device address
 * \param 	len    	length of the bytes array
 *
 * \return 		 	HAL_I2C_OK on success
 */
HAL_I2C_STATUS_T HAL_I2C_RecvBytes(HAL_I2C_HANDLE_T i2cbus, uint8_t address, uint8_t *bytes, uint8_t len);

/**
 * \brief receive len number of bytes on the specified I2C bus
 *
 *	This functions sends a start condition followed by len number of bytes and
 *	a stop condition.
 *
 *	| +START | +ADDR |-A| +D0 |-A| ... | +DN |-A| +STOP |
 *	+ send / - receive / A Ack-bit
 *
 * \param 	i2cbus 	handle to identify the i2c-bus instance
 * \param 	bytes  	array of bytes to be send.
 * \param   address 7-bit device address
 * \param 	len    	length of the bytes array
 *
 * \return 			HAL_I2C_OK on success
 */
HAL_I2C_STATUS_T HAL_I2C_SendBytes(HAL_I2C_HANDLE_T i2cbus, uint8_t address, const uint8_t *bytes, uint8_t len);

#endif /* used for doxygen */

/***********************************************************************/
/* INTERFACING FOR UBIRCH K82 - MACRO BASED EXAMPLE                      */
/***********************************************************************/

#define HAL_I2C_HANDLE_T            uint8_t

uint16_t uNFC_recv(HAL_I2C_HANDLE_T i2cbus, uint8_t address, uint8_t *bytes, uint8_t len);
uint16_t uNFC_send(HAL_I2C_HANDLE_T i2cbus, uint8_t address, const uint8_t *bytes, uint8_t len);

#define HAL_I2C_INIT_PARAM_T        uint32_t
#define HAL_I2C_INVALID_HANDLE      NULL
#define HAL_I2C_STATUS_T            uint16_t
#define HAL_I2C_OK                  0
#define HAL_I2C_SendBytes(handle, address, bytes, len) uNFC_send(handle,address,bytes,len)
#define HAL_I2C_RecvBytes(handle, address, bytes, len) uNFC_recv(handle,address,bytes,len)
#define HAL_I2C_InitDevice          I2CMasterInit
#define HAL_I2C_CloseDevice(handle) /* not necessary */
#define HAL_I2C_RX_RESERVED_BYTES   0
#define HAL_I2C_TX_RESERVED_BYTES   0

#ifdef __cplusplus
}
#endif

#endif /* _HAL_I2C_DRIVER_H_ */
