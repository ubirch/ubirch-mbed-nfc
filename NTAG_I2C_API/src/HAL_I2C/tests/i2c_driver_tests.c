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
#include "unity_fixture.h"
#include "HAL_I2C_driver.h"

#define TX_START HAL_I2C_TX_RESERVED_BYTES
#define RX_START HAL_I2C_RX_RESERVED_BYTES

#define RECV_BUFFER_SIZE (RX_START+16)
#define SEND_BUFFER_SIZE (TX_START+10)

#define TEST_VALID_ADDRESS   0x55
#define TEST_INVALID_ADDRESS 0x04
#define MASK_BYTE            0xAA

uint8_t recvData[RECV_BUFFER_SIZE];
uint8_t sendData[SEND_BUFFER_SIZE];

static HAL_I2C_HANDLE_T i2c_handle = NULL;

HAL_I2C_STATUS_T retval;

TEST_GROUP(i2c_driver);

TEST_SETUP(i2c_driver)
{
	int i = 0;
	i2c_handle=NULL;
	retval = HAL_I2C_OK;

	i2c_handle = HAL_I2C_InitDevice(HAL_I2C_INIT_DEFAULT);

	for(i = 0; i < RECV_BUFFER_SIZE; i++)
		recvData[i] = MASK_BYTE;

	for(i = 0; i < SEND_BUFFER_SIZE; i++)
		sendData[i] = MASK_BYTE;
}

TEST_TEAR_DOWN(i2c_driver)
{
	HAL_I2C_CloseDevice(i2c_handle);
}

TEST(i2c_driver, check_i2c_handle_valid)
{
	/* setUp done by test-fixture */
	TEST_ASSERT_NOT_EQUAL(i2c_handle, HAL_I2C_INVALID_HANDLE);
}

TEST(i2c_driver, nack_on_invalid_device_address_tx)
{
	retval = HAL_I2C_SendBytes(i2c_handle, TEST_INVALID_ADDRESS, sendData, 0);
	TEST_ASSERT_NOT_EQUAL(HAL_I2C_OK, retval);
}

TEST(i2c_driver, nack_on_invalid_device_address_rx)
{
	retval = HAL_I2C_RecvBytes(i2c_handle, TEST_INVALID_ADDRESS, recvData, 0);
	TEST_ASSERT_NOT_EQUAL(HAL_I2C_OK, retval);
}

TEST(i2c_driver, ack_on_valid_after_invalid_address_tx)
{
	retval = HAL_I2C_SendBytes(i2c_handle, TEST_INVALID_ADDRESS, recvData, 0);
	TEST_ASSERT_NOT_EQUAL(HAL_I2C_OK, retval);
	retval = HAL_I2C_SendBytes(i2c_handle, TEST_VALID_ADDRESS, recvData, 0);
	TEST_ASSERT_EQUAL(HAL_I2C_OK, retval);
}

/* BEWARE THIS TEST DEPENDS ON YOUR NTAG */
/* DIFFERENT NTAG -> DIFFERENT ID        */
TEST(i2c_driver, check_i2c_by_reading_ntag_id)
{
	sendData[TX_START+0] = 0;

	retval = HAL_I2C_SendBytes(i2c_handle, TEST_VALID_ADDRESS, sendData,1);
	TEST_ASSERT_EQUAL(HAL_I2C_OK, retval);

	retval = HAL_I2C_RecvBytes(i2c_handle, TEST_VALID_ADDRESS, recvData,7);
	TEST_ASSERT_EQUAL(HAL_I2C_OK, retval);

	TEST_ASSERT_EQUAL_HEX8(0x04, recvData[RX_START+0]);
	TEST_ASSERT_EQUAL_HEX8(0x1A, recvData[RX_START+1]);
	TEST_ASSERT_EQUAL_HEX8(0xA3, recvData[RX_START+2]);
	TEST_ASSERT_EQUAL_HEX8(0x20, recvData[RX_START+3]);
	TEST_ASSERT_EQUAL_HEX8(0x00, recvData[RX_START+4]);
	TEST_ASSERT_EQUAL_HEX8(0x00, recvData[RX_START+5]);
	TEST_ASSERT_EQUAL_HEX8(0x00, recvData[RX_START+6]);
}

/* this tests checks if it is a factory new NTAG I2C 2k */
TEST(i2c_driver, check_i2c_by_reading_ntag_CC)
{
	sendData[TX_START+0] = 0;
	HAL_I2C_SendBytes(i2c_handle, TEST_VALID_ADDRESS, sendData,1);
	HAL_I2C_RecvBytes(i2c_handle, TEST_VALID_ADDRESS, recvData,16);

	TEST_ASSERT_EQUAL_HEX8(0xe1, recvData[RX_START+12]);
	TEST_ASSERT_EQUAL_HEX8(0x10, recvData[RX_START+13]);
	TEST_ASSERT_EQUAL_HEX8(0xea, recvData[RX_START+14]); // 6Dh for 1K NTAG
	TEST_ASSERT_EQUAL_HEX8(0x00, recvData[RX_START+15]);
}

TEST_GROUP_RUNNER(i2c_driver)
{
	RUN_TEST_CASE(i2c_driver, check_i2c_handle_valid);
	RUN_TEST_CASE(i2c_driver, nack_on_invalid_device_address_tx);
	RUN_TEST_CASE(i2c_driver, nack_on_invalid_device_address_rx);
	RUN_TEST_CASE(i2c_driver, ack_on_valid_after_invalid_address_tx);
//	RUN_TEST_CASE(i2c_driver, check_i2c_by_reading_ntag_id);
	RUN_TEST_CASE(i2c_driver, check_i2c_by_reading_ntag_CC);
}
