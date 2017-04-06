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
#include <stddef.h>
#include <fsl_common.h>
#include "inc/HAL_timer_driver.h"
#include "inc/ntag_defines.h"
#include "inc/ntag_driver.h"
#include "inc/HAL_I2C_driver.h"
/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
//#undef NTAG_DEVICE_LIST_BEGIN
//#undef NTAG_DEVICE_ENTRY
//#undef NTAG_DEVICE_LIST_END
//
//#define NTAG_DEVICE_LIST_BEGIN                    struct NTAG_DEVICE ntag_device_list[NTAG_ID_MAX_DEVICES] = \
//                                                  {
//#ifdef HAVE_NTAG_INTERRUPT
//#define NTAG_DEVICE_ENTRY(label, i2c_address, isr)     { NTAG_CLOSED, HAL_I2C_INVALID_HANDLE, i2c_address, isr, {0}, {0} }
//#else
//#define NTAG_DEVICE_ENTRY(label, i2c_address, isr)     { NTAG_CLOSED, HAL_I2C_INVALID_HANDLE, i2c_address, {0}, {0} }
//#endif
//
//#define NTAG_DEVICE_LIST_END                      };

#define TRUE 1
#define FALSE 0
/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/
/* second include of device list for generation of ntag_device_list array */
//NTAG_DEVICE_LIST_BEGIN
//#include "ntag_device_list.h"
//NTAG_DEVICE_LIST_END

/***********************************************************************/
/* GLOBAL PUBLIC FUNCTIONS                                             */
/***********************************************************************/
//NTAG_HANDLE_T NTAG_InitDevice(NTAG_ID_T ntag_id, HAL_I2C_HANDLE_T i2cbus)
//{
//	if( ntag_id < NTAG_ID_MAX_DEVICES )
//	{
//		if( ntag_device_list[ntag_id].status == NTAG_CLOSED )
//		{
//			ntag_device_list[ntag_id].i2cbus = i2cbus;
//			ntag_device_list[ntag_id].status = NTAG_OK;
//			return &ntag_device_list[ntag_id];
//		}
//	}
//	return NTAG_INVALID_HANDLE;
//}

//void NTAG_CloseDevice(NTAG_HANDLE_T ntag)
//{
//	if( ntag )
//	{
//		ntag->i2cbus = HAL_I2C_INVALID_HANDLE;
//		ntag->status = NTAG_CLOSED;
//	}
//}

int NTAG_ReadBytes(NTAG_HANDLE_T *ntag, uint16_t address, uint8_t *bytes, uint16_t len)
{
	uint16_t bytes_read = 0;

	if( ntag->status == NTAG_CLOSED )
		return TRUE;

	ntag->status = NTAG_OK;

	while( bytes_read < len )
	{
		uint8_t current_block = (address+bytes_read) / NTAG_BLOCK_SIZE;
		uint8_t begin         = (address+bytes_read) % NTAG_BLOCK_SIZE;
		uint8_t current_len   = MIN(len-bytes_read, NTAG_BLOCK_SIZE - begin);

		if( current_len < NTAG_BLOCK_SIZE )
		{
			size_t i = 0;

			/* read block into ntag->rx_buffer only */
			if( NTAG_ReadBlock(ntag, current_block, NULL, 0) )
				break;

			/* modify rx_buffer */
			for( i = 0; i < current_len; i++ )
				bytes[bytes_read+i] = ntag->rx_buffer[RX_START+begin+i];
		}
		else
		{
			/* full block read */
			if( NTAG_ReadBlock(ntag, current_block, bytes + bytes_read, NTAG_BLOCK_SIZE) )
				break;
		}

		bytes_read += current_len;
	}
	return ntag->status;
}

int NTAG_WriteBytes(NTAG_HANDLE_T *ntag, uint16_t address, const uint8_t *bytes, uint16_t len)
{
	uint16_t bytes_written = 0;

	if( ntag->status == NTAG_CLOSED )
		return TRUE;

	ntag->status = NTAG_OK;

	while( bytes_written < len )
	{
		uint8_t current_block = (address+bytes_written) / NTAG_BLOCK_SIZE;
		uint8_t begin         = (address+bytes_written) % NTAG_BLOCK_SIZE;
		uint8_t current_len   = MIN(len-bytes_written, NTAG_BLOCK_SIZE - begin);

		if( current_len < NTAG_BLOCK_SIZE )
		{
			size_t i = 0;

			/* read block into ntag->rx_buffer only */
			if( NTAG_ReadBlock(ntag, current_block, NULL, 0) )
				break;

			/* be careful with writing of first byte in management block */
			/* the byte contains part of the serial number on read but   */
			/* on write the I2C address of the device can be modified    */
			if( NTAG_MEM_BLOCK_MANAGEMENT == current_block && NTAG_I2C_ADDRESS_SELECT < begin )
				ntag->rx_buffer[RX_START+0] = ntag->address;

			/* modify rx_buffer */
			for( i = 0; i < current_len; i++ )
				ntag->rx_buffer[RX_START+begin+i] = bytes[bytes_written+i];

			/* writeback modified buffer */
			if( NTAG_WriteBlock(ntag, current_block, ntag->rx_buffer+RX_START, NTAG_BLOCK_SIZE) )
				break;
		}
		else
		{
			/* full block write */
			if( NTAG_WriteBlock(ntag, current_block, bytes + bytes_written, NTAG_BLOCK_SIZE) )
				break;
		}

		bytes_written += current_len;
	}

	return ntag->status;
}

int NTAG_ReadRegister(NTAG_HANDLE_T *ntag, uint8_t reg, uint8_t *val)
{
	ntag->tx_buffer[TX_START+0] = NTAG_MEM_BLOCK_SESSION_REGS;
	ntag->tx_buffer[TX_START+1] = reg;

    ntag->address = HAL_DEFAULT_READ_ADDRESS;

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

int NTAG_WriteRegister(NTAG_HANDLE_T *ntag, uint8_t reg, uint8_t mask, uint8_t val)
{
	ntag->tx_buffer[TX_START+0] = NTAG_MEM_BLOCK_SESSION_REGS;
	ntag->tx_buffer[TX_START+1] = reg;
	ntag->tx_buffer[TX_START+2] = mask;
	ntag->tx_buffer[TX_START+3] = val;

    ntag->address = HAL_DEFAULT_WRITE_ADDRESS;

    if( HAL_I2C_OK != HAL_I2C_SendBytes(ntag->i2cbus, ntag->address, ntag->tx_buffer, 4) )
	{
		ntag->status = NTAG_ERROR_TX_FAILED;
		return TRUE;
	}

	return FALSE;
}

int NTAG_WaitForEvent(NTAG_HANDLE_T *ntag, NTAG_EVENT_T event, uint32_t timeout_ms)
{
	uint8_t reg = 0;
	uint8_t reg_mask = 0;
	size_t i = 0;

	switch(event)
	{
#ifdef HAVE_NTAG_INTERRUPT
		case NTAG_EVENT_FIELD_HI_INTERRUPT:
			HAL_ISR_RegisterCallback(ntag->isr, ISR_LEVEL_HI, NULL, NULL);
			return HAL_ISR_SleepWithTimeout(ntag->isr, timeout_ms);

		case NTAG_EVENT_FIELD_DETECTED_INTERRUPT:
		NTAG_WriteRegister(ntag, NTAG_MEM_OFFSET_NC_REG,
		                   NTAG_NC_REG_MASK_FD_OFF | NTAG_NC_REG_MASK_FD_ON,
		                   0);  /* switch to FD ON/OFF mode 0 */
		/* fall through */
		case NTAG_EVENT_RF_WROTE_SRAM_INTERRUPT:
		case NTAG_EVENT_FIELD_LO_INTERRUPT:
			HAL_ISR_RegisterCallback(ntag->isr, ISR_LEVEL_LO, NULL, NULL);
			return HAL_ISR_SleepWithTimeout(ntag->isr, timeout_ms);

		case NTAG_EVENT_RF_READ_SRAM_INTERRUPT:
			HAL_ISR_RegisterCallback(ntag->isr, ISR_EDGE_FALL, NULL, NULL);
			return HAL_ISR_SleepWithTimeout(ntag->isr, timeout_ms);
#endif
		case NTAG_EVENT_FIELD_DETECTED_POLLED:
			reg_mask = NTAG_NS_REG_MASK_RF_FIELD_PRESENT;
			break;
		case NTAG_EVENT_RF_READ_SRAM_POLLED:
			reg_mask = NTAG_NS_REG_MASK_SRAM_RF_READY;
			break;
		case NTAG_EVENT_RF_WROTE_SRAM_POLLED:
			reg_mask = NTAG_NS_REG_MASK_SRAM_I2C_READY;
			break;
		default:
			return TRUE;
	}

	NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NS_REG, &reg);
	for( /* */; i < timeout_ms && !(reg & reg_mask); i++)
	{
		HAL_Timer_delay_ms(1);
		NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NS_REG, &reg);
	}

	if( NTAG_EVENT_RF_READ_SRAM_POLLED == event )
	{
		/* wait for SRAM ready for RF flag to clear */
		for( /* */; i < timeout_ms && (reg & reg_mask); i++)
		{
			HAL_Timer_delay_ms(1);
			NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NS_REG, &reg);
		}
		return (reg & reg_mask);
	}

	return !(reg & reg_mask);
}

NTAG_STATUS_T NTAG_GetLastError(NTAG_HANDLE_T *ntag)
{
	return ntag->status;
}

/***********************************************************************/
/* GLOBAL PRIVATE FUNCTIONS                                            */
/***********************************************************************/
int NTAG_ReadBlock(NTAG_HANDLE_T *ntag, uint8_t block, uint8_t *bytes, uint8_t len)
{
	size_t i = 0;

	ntag->tx_buffer[TX_START] = block;
    ntag->address = HAL_DEFAULT_READ_ADDRESS;

	/* send block number */
	if( HAL_I2C_OK != HAL_I2C_SendBytes(ntag->i2cbus, ntag->address, ntag->tx_buffer, 1) )
	{
		ntag->status = NTAG_ERROR_TX_FAILED;
		return TRUE;
	}

	/* receive bytes */
	if( HAL_I2C_OK != HAL_I2C_RecvBytes(ntag->i2cbus, ntag->address, ntag->rx_buffer, NTAG_BLOCK_SIZE) )
	{
		ntag->status = NTAG_ERROR_RX_FAILED;
		return TRUE;
	}

	len = MIN(len, NTAG_BLOCK_SIZE);

	/* write to bytes buffer */
	for( i = 0; i < len; i++ )
		bytes[i] = ntag->rx_buffer[RX_START+i];

	return FALSE;
}

int NTAG_WriteBlock(NTAG_HANDLE_T *ntag, uint8_t block, const uint8_t *bytes, uint8_t len)
{
	uint8_t ns_reg = 0;
	uint32_t timeout = NTAG_MAX_WRITE_DELAY_MS / 5 + 1;
	size_t i = 0;

	ntag->tx_buffer[TX_START] = block;
    ntag->address = HAL_DEFAULT_WRITE_ADDRESS;

	len = MIN(len, NTAG_BLOCK_SIZE);

	/* copy len bytes */
	for( i = 0; i < len; i++ )
		ntag->tx_buffer[TX_START+i+1] = bytes[i];

	/* zero rest of the buffer */
	for( i = len; i < NTAG_BLOCK_SIZE; i++ )
		ntag->tx_buffer[TX_START+i+1] = 0;

	/* send block number */
	if( HAL_I2C_OK != HAL_I2C_SendBytes(ntag->i2cbus, ntag->address, ntag->tx_buffer, NTAG_BLOCK_SIZE+1) )
	{
		ntag->status = NTAG_ERROR_TX_FAILED;
		return TRUE;
	}

	/* do not wait for completion when writing SRAM */
	if( block >= NTAG_MEM_BLOCK_START_ADDR_SRAM && block < NTAG_MEM_BLOCK_START_ADDR_SRAM + NTAG_MEM_BLOCK_SIZE_SRAM )
		return ntag->status;

	/* wait for completion */
	do
	{
		HAL_Timer_delay_ms(5);
		if( NTAG_ReadRegister(ntag, NTAG_MEM_OFFSET_NS_REG, &ns_reg) )
			break;
		timeout--;
	}while(timeout && ns_reg & NTAG_NS_REG_MASK_EEPROM_WR_BUSY);

	if(0 == timeout)
		ntag->status = NTAG_ERROR_WRITE_TIMEOUT;

	return ntag->status;
}

int NTAG_GetManufData(NTAG_HANDLE_T *ntag, ManufData *manufdata){

    int ret = -1;
    uint8_t rxbuffer[16];

    for (int i = 0; i < 3 && ret != 0;i++) {
        ret = NTAG_ReadBytes(ntag, 0x00, rxbuffer, 16);
    }
    memcpy(manufdata, rxbuffer, 16);

    return ret;
}

