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
#ifndef _NTAG_DRIVER_H_
#define _NTAG_DRIVER_H_

/** @file ntag_driver.h
 * \brief Public interface to access a NTAG I2C tag over I2C.
 */

/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#include <stdint.h>
#include "ntag_defines.h"
#include "HAL_I2C_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TX_START HAL_I2C_TX_RESERVED_BYTES
#define RX_START HAL_I2C_RX_RESERVED_BYTES
#define NTAG_ADDRESS_SIZE 1



typedef enum {
	NTAG_OK,
	NTAG_ERROR_TX_FAILED,
	NTAG_ERROR_RX_FAILED,
	NTAG_ERROR_WRITE_TIMEOUT,
	NTAG_ERROR_INVALID_PARAM,
	NTAG_CLOSED,
	NTAG_STATUS_MAX_NUMBER
} NTAG_STATUS_T;

typedef enum {
#ifdef HAVE_NTAG_INTERRUPT
    NTAG_EVENT_FIELD_DETECTED_INTERRUPT,
    NTAG_EVENT_RF_WROTE_SRAM_INTERRUPT,
    NTAG_EVENT_RF_READ_SRAM_INTERRUPT,
    NTAG_EVENT_FIELD_HI_INTERRUPT,
    NTAG_EVENT_FIELD_LO_INTERRUPT,
#endif
	NTAG_EVENT_FIELD_DETECTED_POLLED,
	NTAG_EVENT_RF_WROTE_SRAM_POLLED,
	NTAG_EVENT_RF_READ_SRAM_POLLED
} NTAG_EVENT_T;


struct NTAG_DEVICE {
	NTAG_STATUS_T status;
	HAL_I2C_HANDLE_T i2cbus;
	uint8_t address;
#ifdef HAVE_NTAG_INTERRUPT
	ISR_SOURCE_T isr;
#endif
	uint8_t tx_buffer[TX_START + NTAG_BLOCK_SIZE + NTAG_ADDRESS_SIZE];
	uint8_t rx_buffer[RX_START + NTAG_BLOCK_SIZE];
};

typedef struct NTAG_DEVICE NTAG_HANDLE_T;

// structure that contains the manufactoring data
typedef struct {
	uint8_t  slaveAddr;
	uint8_t  serialNumber[6];
	uint8_t  internalData[3];
	uint16_t lockBytes;
	uint32_t capabilityContainer;
} ManufData;
/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/

/***********************************************************************/
/* GLOBAL FUNCTION PROTOTYPES                                          */
/***********************************************************************/

/**
 * \brief initialize the selected NTAG device for operation
 *
 *	This function registers the specified I2C bus and returns a
 *	handle to the selected device to be used for further operations.
 *
 * \param	ntag_id	identifier of device to be selected
 * \param	i2cbus	handle to I2C bus instance to be used
 *
 * \return 		 	NTAG_INVALID_HANDLE on failure
 */
//NTAG_HANDLE_T NTAG_InitDevice(NTAG_ID_T ntag_id, HAL_I2C_HANDLE_T i2cbus);

/**
 * \brief close handle and unregister I2C bus instance
 *
 *	This function unregisters the associated I2C bus handle and closes
 *	the handle. The handle can no longer be used for further operation.
 *
 * \param	ntag	handle to identify the NTAG device instance
 *
 * \return	none
 */
//void NTAG_CloseDevice(NTAG_HANDLE_T ntag);

/**
 * \brief read len number of bytes from the selected NTAG device
 *
 *	This functions reads the specified number of bytes from the selected
 *	NTAG device, starting at the given address. Reading from an invalid address
 *	will result in a failed read.
 *
 * \param	ntag	handle to identify the NTAG device instance
 * \param	bytes	array of bytes to store read data
 * \param	address	byte address in device memory space to start reading from
 * \param	len 	number of bytes to be read
 *
 * \return			TRUE on failure
 */
int NTAG_ReadBytes(NTAG_HANDLE_T *ntag, uint16_t address, uint8_t *bytes, uint16_t len);

/**
 * \brief write len number of bytes to the selected NTAG device
 *
 *	This functions writes the specified number of bytes to the given address of
 *	the selected NTAG device. Writing to an invalid address
 *	is undefined.
 *
 * \param	ntag	handle to identify the NTAG device instance
 * \param	bytes	array of bytes to be written
 * \param	address	byte address in device memory space to start writing to
 * \param	len 	number of bytes to be written
 *
 * \return			TRUE on failure
 */
int NTAG_WriteBytes(NTAG_HANDLE_T *ntag, uint16_t address, const uint8_t *bytes, uint16_t len);

/**
 * \brief read a register from the selected NTAG device
 *
 *	This functions reads the specified 8-bit register from the selected
 *	NTAG device.
 *
 * \param	ntag	handle to identify the NTAG device instance
 * \param	reg 	register offset from the start of the register block
 * \param	val 	byte to store read value
 *
 * \return			TRUE on failure
 */
int NTAG_ReadRegister(NTAG_HANDLE_T *ntag, uint8_t reg, uint8_t *val);

/**
 * \brief write a register of the selected NTAG device
 *
 *	This functions writes the specified 8-bit register of the selected
 *	NTAG device. Only the mask selected bits will be written.
 *
 * \param	ntag	handle to identify the NTAG device instance
 * \param	reg 	register offset from the start of the register block
 * \param	mask	only selected(1) bits will be written
 * \param	val 	8-bit value to be written
 *
 * \return			TRUE on failure
 */
int NTAG_WriteRegister(NTAG_HANDLE_T *ntag, uint8_t reg, uint8_t mask, uint8_t val);

/**
 * \brief wait for selected event
 *
 *	This functions waits until the selected event occurs or the timeout value is
 *	reached. See NTAG_EVENT_T for possible events to be waited on.
 *
 * \param	ntag      	handle to identify the NTAG device instance
 * \param	event     	event to be waited for
 * \param	timeout_ms	timeout value in ms
 *
 * \return			TRUE on timeout
 */
int NTAG_WaitForEvent(NTAG_HANDLE_T *ntag, NTAG_EVENT_T event, uint32_t timeout_ms);

/**
 * \brief get the error code of the last failure
 *
 *	If the previous call to any of the read/write function failed this function
 *	will return the reason via the corresponding error code. The error code is not
 *	latched, therefore any successful read/write after a failure will reset the
 *	error code.
 *	This function will return NTAG_CLOSED on a closed handle.
 *
 * \param	ntag	handle to identify the NTAG device instance
 *
 * \return			NTAG status code
 */
NTAG_STATUS_T NTAG_GetLastError(NTAG_HANDLE_T *ntag);


int NTAG_ReadBlock(NTAG_HANDLE_T *ntag, uint8_t block, uint8_t *bytes, uint8_t len);

int NTAG_WriteBlock(NTAG_HANDLE_T *ntag, uint8_t block, const uint8_t *bytes, uint8_t len);

int NTAG_GetManufData(NTAG_HANDLE_T *ntag, ManufData *manufdata);

#ifdef __cplusplus
}
#endif
#endif /* _NTAG_DRIVER_H_ */
