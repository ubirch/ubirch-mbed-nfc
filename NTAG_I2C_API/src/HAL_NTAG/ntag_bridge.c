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
/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#include "HAL_I2C_driver.h"
#include "ntag_bridge.h"

/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/

/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/

/***********************************************************************/
/* LOCAL VARIABLES                                                     */
/***********************************************************************/
static int PT_fail=0;

/***********************************************************************/
/* GLOBAL FUNCTIONS                                                    */
/***********************************************************************/
//---------------------------------------------------------------------
BOOL NTAG_EnablePT(NTAG_HANDLE_T ntag)
{
	int16_t error_code = NTAG_ERR_OK;

	// set start address for SRAM mirror to page 4
	error_code = NTAG_SetSRAMMirrorLowerPageAddr(ntag,
			NTAG_MEM_BLOCK_START_ADDR_USER_MEMORY );
	if ( error_code != NTAG_ERR_OK )
		return ( error_code );

	error_code = NTAG_EnableSRAM(ntag);
	if ( error_code != NTAG_ERR_OK )
		return ( error_code );

	error_code = NTAG_EnableSRAMMirrorMode(ntag);
	if ( error_code != NTAG_ERR_OK )
		return ( error_code );

	return ( error_code );
}

//---------------------------------------------------------------------
BOOL NTAG_DisablePT(NTAG_HANDLE_T ntag)
{
	int16_t error_code = NTAG_ERR_OK;
	error_code = NTAG_DisableSRAM(ntag);
	error_code |= NTAG_DisableSRAMMirrorMode(ntag);
	return ( error_code );
}


BOOL NTAG_SetPassThroughRFtoI2C_withEn(NTAG_HANDLE_T ntag)
{
	PT_fail++;
	NTAG_DisableSRAM(ntag);
	NTAG_SetPassThroughRFtoI2C(ntag);
	NTAG_EnableSRAM(ntag);
	return NTAG_ERR_OK;
}

//---------------------------------------------------------------------
BOOL NTAG_EnableSRAM(NTAG_HANDLE_T ntag)
{
	return NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG, NTAG_NC_REG_MASK_TR_SRAM_ON_OFF,
	                          NTAG_NC_REG_MASK_TR_SRAM_ON_OFF);
}

//---------------------------------------------------------------------
BOOL NTAG_DisableSRAM(NTAG_HANDLE_T ntag)
{
	return NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG, NTAG_NC_REG_MASK_TR_SRAM_ON_OFF,
	                         ~NTAG_NC_REG_MASK_TR_SRAM_ON_OFF);
}

//---------------------------------------------------------------------
BOOL NTAG_EnableSRAMMirrorMode(NTAG_HANDLE_T ntag)
{
	int16_t err = NTAG_ERR_OK;
	uint8_t current_ses_reg = 0;
	err = NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NC_REG, &current_ses_reg);
	if (current_ses_reg & NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF)
	{
		return (err); // already enabled
	}
	else
	{
		current_ses_reg |= NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF;
		err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG,
				NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF, current_ses_reg);
	}

	return (err);
}

//---------------------------------------------------------------------
BOOL NTAG_DisableSRAMMirrorMode(NTAG_HANDLE_T ntag)
{
	int16_t err = NTAG_ERR_OK;
	uint8_t current_ses_reg = 0;
	NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NC_REG, &current_ses_reg);

	if (current_ses_reg & NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF)
	{
		current_ses_reg &= ~NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF;
		err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG,
				NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF, current_ses_reg);
	}
	return (err);
}

//---------------------------------------------------------------------
BOOL NTAG_SetPassThroughI2CtoRF(NTAG_HANDLE_T ntag)
{
	int16_t err = NTAG_ERR_OK;
	uint8_t current_ses_reg = 0;
	NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NC_REG, &current_ses_reg);

	if (current_ses_reg & NTAG_NC_REG_MASK_RF_WRITE_ON_OFF)
	{
		if (current_ses_reg & NTAG_NC_REG_MASK_TR_SRAM_ON_OFF)
		{
			NTAG_DisableSRAM(ntag);

			current_ses_reg &= ~NTAG_NC_REG_MASK_RF_WRITE_ON_OFF;
			err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG,
					NTAG_NC_REG_MASK_RF_WRITE_ON_OFF, current_ses_reg);

			NTAG_EnableSRAM(ntag);
		}
		else
		{
			current_ses_reg &= ~NTAG_NC_REG_MASK_RF_WRITE_ON_OFF;
			err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG,
					NTAG_NC_REG_MASK_RF_WRITE_ON_OFF, current_ses_reg);
		}

	}
	return (err);
}

//---------------------------------------------------------------------
BOOL NTAG_SetPassThroughRFtoI2C(NTAG_HANDLE_T ntag)
{
	int16_t err = NTAG_ERR_OK;
	uint8_t current_ses_reg = 0;
	NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NC_REG, &current_ses_reg);
	if (current_ses_reg & NTAG_NC_REG_MASK_RF_WRITE_ON_OFF)
	{
		return (err); // already enabled
	}
	else
	{
		if (current_ses_reg & NTAG_NC_REG_MASK_TR_SRAM_ON_OFF)
		{
			NTAG_DisableSRAM(ntag);

			current_ses_reg |= NTAG_NC_REG_MASK_RF_WRITE_ON_OFF;
			err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG,
					NTAG_NC_REG_MASK_RF_WRITE_ON_OFF, current_ses_reg);

			NTAG_EnableSRAM(ntag);
		}
		else
		{
			current_ses_reg |= NTAG_NC_REG_MASK_RF_WRITE_ON_OFF;
			err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG,
					NTAG_NC_REG_MASK_RF_WRITE_ON_OFF, current_ses_reg);
		}

	}
	return (err);
}

//---------------------------------------------------------------------
BOOL NTAG_GetLastPageNDEFMessage(NTAG_HANDLE_T ntag, uint8_t *addr)
{
	int16_t err = NTAG_ERR_OK;
	uint8_t current_ses_reg = 0;
	err = NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_LD_REG, &current_ses_reg);
	*addr = current_ses_reg;
	return (err);
}

//---------------------------------------------------------------------
BOOL NTAG_SetSRAMMirrorLowerPageAddr(NTAG_HANDLE_T ntag, uint8_t addr)
{
	int16_t err = NTAG_ERR_OK;
	if (addr > 0x74)
	{
		return (NTAG_ERR_INVALID_PARAM);
	}
	err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_SM_REG, 0xFF, addr);
	return (err);
}

//---------------------------------------------------------------------
BOOL NTAG_SetWatchdogTime(NTAG_HANDLE_T ntag, uint16_t time)
{
	int16_t err = NTAG_ERR_OK;
	err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_WDT_LS, 0xFF,
			(uint8_t) (time & 0xFF));
	if (err != NTAG_ERR_OK)
		return (err);

	err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_WDT_MS, 0xFF,
			(uint8_t) ((time >> 8) & 0xFF));
	return (err);
}

//---------------------------------------------------------------------
BOOL NTAG_GetWatchdogTime(NTAG_HANDLE_T ntag, uint16_t *time)
{
	int16_t err = NTAG_ERR_OK;
	uint8_t reg = 0;
	err = NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_WDT_LS, &reg);
	*time |= (uint16_t) ((reg << 8) & 0xFF00);

	err = NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_WDT_MS, &reg);
	*time |= (uint16_t) (reg & 0x00FF);

	return (err);
}

//---------------------------------------------------------------------
BOOL NTAG_DefaultRegWriteAccess(NTAG_HANDLE_T ntag, BOOL i2c, BOOL rf)
{
	uint8_t current_ses_reg = 0;
	int16_t err = NTAG_ERR_OK;
	err = NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_REG_LOCK, &current_ses_reg);
	if (err != NTAG_ERR_OK)
		return (err);

	if (i2c)
	{
		if (current_ses_reg & NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_I2C)
		{
			err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_REG_LOCK,
					NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_I2C,
					(current_ses_reg & ~NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_I2C));
		}
	}
	else
	{
		if (!(current_ses_reg & NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_I2C))
		{
			err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_REG_LOCK,
					NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_I2C,
					(current_ses_reg & NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_I2C));
		}
	}
	if (rf)
	{
		if (current_ses_reg & NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_RF)
		{
			err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_REG_LOCK,
					NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_RF,
					(current_ses_reg & ~NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_RF));
		}
	}
	else
	{
		if (!(current_ses_reg & NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_RF))
		{
			err = NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_REG_LOCK,
					NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_I2C,
					(current_ses_reg & NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_RF));
		}
	}
	return (err);
}
