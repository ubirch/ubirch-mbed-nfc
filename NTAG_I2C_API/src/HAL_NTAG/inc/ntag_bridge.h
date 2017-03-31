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
#ifndef _NTAG_BRIDGE_H_
#define _NTAG_BRIDGE_H_
/** @file ntag_bridge.h
 * \brief Public interface for using an NTAG I2C tag as I2C<->NFC bridge.
 */

/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#include "../../inc/global_types.h"
#include "ntag_driver.h"

#ifdef __cplusplus
extern "C" {
#endif
/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/

/***********************************************************************/
/* TYPES                                                               */
/***********************************************************************/

/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/

/***********************************************************************/
/* GLOBAL FUNCTION PROTOTYPES                                          */
/***********************************************************************/

/**
 * \brief enable the I2C<->NFC bridge
 *
 * Activates the pass-through mode, that uses the NTAG's 64 Byte SRAM
 * for communication between a NFC device and the I2C bus
 * \return	error code as defined in ntag_defines.h
 */
BOOL NTAG_EnablePT(NTAG_HANDLE_T ntag);

/**
 * \brief disable the I2C<->NFC bridge
 *
 * Disable the pass-through mode. Further read and write operations
 * will access the user memory
 *
 * \return		error code as defined in ntag_defines.h
 */
BOOL NTAG_DisablePT(NTAG_HANDLE_T ntag);

/**
 * \brief set direction for pass-through mode to RF -> I2C and enables PT
 *
 * \param	ntag	handle to identify the NTAG device instance
 *
 * \return			TRUE on failure*
 */
BOOL NTAG_SetPassThroughRFtoI2C_withEn(NTAG_HANDLE_T ntag);

/**
 * \brief set direction for pass-through mode to I2C -> RF
 *
 * \param	ntag	handle to identify the NTAG device instance
 *
 * \return			TRUE on failure*
 */
BOOL NTAG_SetPassThroughI2CtoRF(NTAG_HANDLE_T ntag);

/**
 * \brief set direction for pass-through mode to RF -> I2C
 *
 * \param	ntag	handle to identify the NTAG device instance
 *
 * \return			TRUE on failure*
 */
BOOL NTAG_SetPassThroughRFtoI2C(NTAG_HANDLE_T ntag);

/**
 * \brief enables SRAM for data transfer
 *
 * \param	ntag	handle to identify the NTAG device instance
 *
 * \return			TRUE on failure*
 */
BOOL NTAG_EnableSRAM(NTAG_HANDLE_T ntag);

/**
 * \brief disables SRAM for data transfer
 *
 * \param	ntag	handle to identify the NTAG device instance
 *
 * \return			TRUE on failure*
 */
BOOL NTAG_DisableSRAM(NTAG_HANDLE_T ntag);

/**
 * \brief enable SRAM projection to user memory addresses
 *
 * \param	ntag	handle to identify the NTAG device instance
 *
 * \return			TRUE on failure*
 */
BOOL NTAG_EnableSRAMMirrorMode(NTAG_HANDLE_T ntag);

/**
 * \brief disable SRAM projection to user memory addresses
 *
 * \param	ntag	handle to identify the NTAG device instance
 *
 * \return			TRUE on failure*
 */
BOOL NTAG_DisableSRAMMirrorMode(NTAG_HANDLE_T ntag);

/**
 * \brief get the address of last NDEF message page
 *
 * \param	ntag	handle to identify the NTAG device instance
 * \param	addr	pointer to address
 *
 * \return			TRUE on failure*
 */
BOOL NTAG_GetLastPageNDEFMessage(NTAG_HANDLE_T ntag, uint8_t *addr);

/**
 * \brief change the first page of user memory when mirroring the SRAM
 *
 *  Change the first page of user memory when mirroring the SRAM.
 *  Granularity is 4, so addr * 4 is the real page address.
 *
 * \param	ntag	handle to identify the NTAG device instance
 * \param	addr	page address, 0x74 is the highest allowed value
 * \return			TRUE on failure*
 */
BOOL NTAG_SetSRAMMirrorLowerPageAddr(NTAG_HANDLE_T ntag, uint8_t addr);

/**
 * \brief change the value of the watchdog timer
 *
 * \param	ntag	handle to identify the NTAG device instance
 * \param	time	new time value of the watchdog timer
 * \return			TRUE on failure*
 */
BOOL NTAG_SetWatchdogTime(NTAG_HANDLE_T ntag, uint16_t time);

/**
 * \brief get the value of the watchdog timer
 *
 * \param	ntag	handle to identify the NTAG device instance
 * \param	time	pointer where to store the value
 * \return			TRUE on failure*
 */
BOOL NTAG_GetWatchdogTime(NTAG_HANDLE_T ntag, uint16_t *time);

/**
 * \brief set the access rights of the default register values
 *
 * \param	ntag	handle to identify the NTAG device instance
 * \param	i2c 	i2c allowed to write
 * \param	rf  	rf allowed to write
 * \return			TRUE on failure*
 */
BOOL NTAG_DefaultRegWriteAccess(NTAG_HANDLE_T ntag, BOOL i2c, BOOL rf);

/**
 * \brief write session registers to the default registers
 *
 * This function writes the current content of the session registers to
 * the default registers. The default registers are persistent even after
 * a reset of the device.
 *
 * \param	ntag	handle to identify the NTAG device instance
 * \return			TRUE on failure*
 */
BOOL NTAG_UpdateDefaultReg(NTAG_HANDLE_T ntag);

#ifdef __cplusplus
}
#endif
#endif /* _NTAG_BRIDGE_H_ */
