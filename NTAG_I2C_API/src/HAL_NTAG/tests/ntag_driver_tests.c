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
#include "unity_fixture.h"
#include "ntag_driver.h"
#include "../ntag_driver_intern.h"

/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
#define NTAG_TEST_BUFFER_SIZE   NTAG_BLOCK_SIZE * 16
#define NTAG_TEST_BLOCK_START   1

#  define NFC_TEST_DEVICE         NTAG0
#  define NFC_ID_MAX_DEVICES      NTAG_ID_MAX_DEVICES
#  define NFC_INVALID_HANDLE      NTAG_INVALID_HANDLE
#  define NFC_HANDLE_T            NTAG_HANDLE_T
#  define NFC_InitDevice          NTAG_InitDevice
#  define NFC_CloseDevice         NTAG_CloseDevice
#  define NFC_ReadBytes           NTAG_ReadBytes
#  define NFC_WriteBytes          NTAG_WriteBytes
#  define NFC_GetLastError        NTAG_GetLastError

/***********************************************************************/
/* LOCAL VARIABLES                                                     */
/***********************************************************************/
static HAL_I2C_HANDLE_T i2c_handle;
static NFC_HANDLE_T ntag_handle;
static BOOL retval;
static uint8_t ntag_test_buffer[NTAG_TEST_BUFFER_SIZE];

/***********************************************************************/
/* LOCAL FUNCTION PROTOTYPES                                           */
/***********************************************************************/
static void clearData( uint8_t *buffer, size_t len, uint8_t clear_byte );
static void checkData( uint8_t *buffer, size_t len, uint8_t check_byte );

/***********************************************************************/
/* TEST GROUP DEFINITION                                               */
/***********************************************************************/
TEST_GROUP(ntag_driver);

TEST_SETUP(ntag_driver)
{
	ntag_handle = NFC_INVALID_HANDLE;
	retval = FALSE;

	/* use first device for tests */
	ntag_handle = NFC_InitDevice(NFC_TEST_DEVICE, i2c_handle);

	clearData(ntag_test_buffer, NTAG_TEST_BUFFER_SIZE, 0xAA);
}

TEST_TEAR_DOWN(ntag_driver)
{
	NFC_CloseDevice(ntag_handle);
}

TEST_GROUP_RUNNER(ntag_driver)
{
	/* setup test group */
	i2c_handle = HAL_I2C_InitDevice(HAL_I2C_INIT_DEFAULT);

	/* test device list structure */
	RUN_TEST_CASE(ntag_driver, device_list_not_empty);
	RUN_TEST_CASE(ntag_driver, device_list_i2c_addresses_valid);

	/* test init/close handling */
	RUN_TEST_CASE(ntag_driver, handle_closed_after_closing_device);
	RUN_TEST_CASE(ntag_driver, init_not_possible_if_handle_not_closed)
	RUN_TEST_CASE(ntag_driver, init_returns_valid_ntag_handle);
	RUN_TEST_CASE(ntag_driver, init_more_than_max_devices_not_possible)
	RUN_TEST_CASE(ntag_driver, i2c_handle_valid_after_init);
	RUN_TEST_CASE(ntag_driver, i2c_handle_invalid_after_close);
	RUN_TEST_CASE(ntag_driver, no_error_set_after_init);
	RUN_TEST_CASE(ntag_driver, close_protected_against_null_ptr_dereference);

	/* test write/read block handling */
//	RUN_TEST_CASE(ntag_driver, read_half_block_check_id);
	RUN_TEST_CASE(ntag_driver, read_full_block_check_CC);
	RUN_TEST_CASE(ntag_driver, write_single_block_of_AA);
	RUN_TEST_CASE(ntag_driver, write_multiple_blocks);
	RUN_TEST_CASE(ntag_driver, zero_len_block_read_fills_internal)
	RUN_TEST_CASE(ntag_driver, write_half_block_of_55);
	/* optimize private functions
	 * RUN_TEST_CASE(ntag_driver, block_read_not_possible_on_closed_handle);
	 * RUN_TEST_CASE(ntag_driver, block_write_not_possible_on_closed_handle); */

	/* test write/read byte(s) API */
	RUN_TEST_CASE(ntag_driver, single_unaligned_byte_read);
	RUN_TEST_CASE(ntag_driver, single_byte_read_whole_block);
	RUN_TEST_CASE(ntag_driver, single_unaligned_byte_write);
	RUN_TEST_CASE(ntag_driver, single_byte_write_whole_block);
	RUN_TEST_CASE(ntag_driver, write_bytes_crossing_block_boundary);
	RUN_TEST_CASE(ntag_driver, write_bytes_multiple_blocks_unaligned);
	RUN_TEST_CASE(ntag_driver, read_bytes_not_possible_on_closed_handle);
	RUN_TEST_CASE(ntag_driver, write_bytes_not_possible_on_closed_handle);

	/* tear down test group */
	HAL_I2C_CloseDevice(i2c_handle);
}

/***********************************************************************/
/* TESTS                                                               */
/***********************************************************************/
/* not strictly necessary - empty list will trigger compile error */
TEST(ntag_driver, device_list_not_empty)
{
	TEST_ASSERT_NOT_EQUAL(0, NFC_ID_MAX_DEVICES);
}

TEST(ntag_driver, device_list_i2c_addresses_valid)
{
	HAL_I2C_STATUS_T retval;
	uint8_t sendBuffer[TX_START+1];

	size_t i = 0;
	for( i = 0; i < NFC_ID_MAX_DEVICES; i++ )
	{
		uint8_t address = ntag_device_list[i].address;
		TEST_ASSERT_TRUE(address >   0);
		TEST_ASSERT_TRUE(address < 128);

		/* check for ACK from addressed device on i2cbus */
		retval = HAL_I2C_SendBytes(i2c_handle, address, sendBuffer,0);
		TEST_ASSERT_EQUAL(HAL_I2C_OK, retval);
	}
}

TEST(ntag_driver, handle_closed_after_closing_device)
{
	NFC_CloseDevice(ntag_handle);
	TEST_ASSERT_EQUAL(NTAG_CLOSED, NFC_GetLastError(ntag_handle));
}

TEST(ntag_driver, init_not_possible_if_handle_not_closed)
{
	NFC_CloseDevice(ntag_handle);

	/* setup */
	ntag_device_list[0].status = NTAG_OK;


	/* test */
	ntag_handle = NFC_InitDevice(NFC_TEST_DEVICE, i2c_handle);
	TEST_ASSERT_EQUAL(NFC_INVALID_HANDLE, ntag_handle);

	/* tear down */

	ntag_device_list[0].status = NTAG_CLOSED;


}

TEST(ntag_driver, init_returns_valid_ntag_handle)
{
	NFC_CloseDevice(ntag_handle);
	ntag_handle = NFC_InitDevice(NFC_TEST_DEVICE, i2c_handle);
	TEST_ASSERT_NOT_EQUAL(NFC_INVALID_HANDLE, ntag_handle);
}

TEST(ntag_driver, init_more_than_max_devices_not_possible)
{
	NFC_CloseDevice(ntag_handle);
	ntag_handle = NFC_InitDevice(NFC_ID_MAX_DEVICES, i2c_handle);
	TEST_ASSERT_EQUAL(NFC_INVALID_HANDLE, ntag_handle);
}

TEST(ntag_driver, i2c_handle_valid_after_init)
{
	NFC_CloseDevice(ntag_handle);
	ntag_handle = NFC_InitDevice(NFC_TEST_DEVICE, i2c_handle);

	TEST_ASSERT_NOT_EQUAL(NFC_INVALID_HANDLE, ntag_device_list[NFC_TEST_DEVICE].i2cbus);
}

TEST(ntag_driver, i2c_handle_invalid_after_close)
{
	NFC_CloseDevice(ntag_handle);
	TEST_ASSERT_EQUAL(NFC_INVALID_HANDLE, ntag_device_list[NFC_TEST_DEVICE].i2cbus);
}

/* not sure if failure would trigger on all platforms */
TEST(ntag_driver, close_protected_against_null_ptr_dereference)
{
	NFC_CloseDevice(0);
}

TEST(ntag_driver, no_error_set_after_init)
{
	NFC_CloseDevice(ntag_handle);
	ntag_handle = NFC_InitDevice(NFC_TEST_DEVICE, i2c_handle);
	TEST_ASSERT_EQUAL(NTAG_OK, NFC_GetLastError(ntag_handle));
}

/* BEWARE THIS TEST DEPENDS ON YOUR NFC TAG */
/* DIFFERENT NFC TAG -> DIFFERENT ID        */
TEST(ntag_driver, read_half_block_check_id)
{
	retval = NTAG_ReadBlock(ntag_handle, 0, ntag_test_buffer, 7);
	TEST_ASSERT_FALSE(retval);

	TEST_ASSERT_EQUAL_HEX8(0x04, ntag_test_buffer[0]);
	TEST_ASSERT_EQUAL_HEX8(0x1A, ntag_test_buffer[1]);
	TEST_ASSERT_EQUAL_HEX8(0xA3, ntag_test_buffer[2]);
	TEST_ASSERT_EQUAL_HEX8(0x20, ntag_test_buffer[3]);
	TEST_ASSERT_EQUAL_HEX8(0x00, ntag_test_buffer[4]);
	TEST_ASSERT_EQUAL_HEX8(0x00, ntag_test_buffer[5]);
	TEST_ASSERT_EQUAL_HEX8(0x00, ntag_test_buffer[6]);
}

/* this tests checks if it is a factory new NTAG I2C 2k */
TEST(ntag_driver, read_full_block_check_CC)
{
	retval = NTAG_ReadBlock(ntag_handle, 0, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	TEST_ASSERT_EQUAL_HEX8(0xe1, ntag_test_buffer[12]);
	TEST_ASSERT_EQUAL_HEX8(0x10, ntag_test_buffer[13]);
	TEST_ASSERT_EQUAL_HEX8(0xea, ntag_test_buffer[14]); // 6Dh for 1K NTAG
	TEST_ASSERT_EQUAL_HEX8(0x00, ntag_test_buffer[15]);
}

TEST(ntag_driver, write_single_block_of_AA)
{
	/* write */
	clearData(ntag_test_buffer, NTAG_BLOCK_SIZE, 0xAA);
	retval = NTAG_WriteBlock(ntag_handle, NTAG_TEST_BLOCK_START, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* read */
	clearData(ntag_test_buffer, NTAG_BLOCK_SIZE, 0x00);
	retval = NTAG_ReadBlock(ntag_handle, NTAG_TEST_BLOCK_START, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* check */
	checkData(ntag_test_buffer, NTAG_BLOCK_SIZE, 0xAA);
}

TEST(ntag_driver, write_multiple_blocks)
{
	size_t block = 0;
	size_t nr_of_blocks = 15;
	size_t skip_blocks = 0;

	/* write */
	for( block = 0; block < nr_of_blocks; block++ )
		clearData(ntag_test_buffer+block*NTAG_BLOCK_SIZE, NTAG_BLOCK_SIZE, (block+1)*0x11);

	for( block = 0; block < nr_of_blocks; block++)
	{
		retval = NTAG_WriteBlock(ntag_handle, NTAG_TEST_BLOCK_START+block+skip_blocks,
		                         ntag_test_buffer+block*NTAG_BLOCK_SIZE, NTAG_BLOCK_SIZE);
		TEST_ASSERT_FALSE(retval);
	}

	/* read */
	clearData(ntag_test_buffer, NTAG_BLOCK_SIZE*nr_of_blocks, 0x00);

	skip_blocks = 0;
	for( block = 0; block < nr_of_blocks; block++ )
	{

		retval = NTAG_ReadBlock (ntag_handle, NTAG_TEST_BLOCK_START+block+skip_blocks,
		                         ntag_test_buffer+block*NTAG_BLOCK_SIZE, NTAG_BLOCK_SIZE);
		TEST_ASSERT_FALSE(retval);
	}

	/* check */
	for( block = 0; block < nr_of_blocks; block++ )
		checkData(ntag_test_buffer+block*NTAG_BLOCK_SIZE, NTAG_BLOCK_SIZE, (block+1)*0x11);
}

TEST(ntag_driver, zero_len_block_read_fills_internal)
{
	/* write 1 block 0x55 */
	clearData(ntag_test_buffer, NTAG_BLOCK_SIZE, 0x55);
	retval = NTAG_WriteBlock(ntag_handle, NTAG_TEST_BLOCK_START, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* clear internal receive buffer */
	clearData(ntag_handle->rx_buffer+RX_START, NTAG_BLOCK_SIZE, 0x00);

	/* read 1 block */
	retval = NTAG_ReadBlock(ntag_handle, NTAG_TEST_BLOCK_START, NULL, 0);
	TEST_ASSERT_FALSE(retval);

	/* check internal receive buffer */
	checkData(ntag_handle->rx_buffer+RX_START, NTAG_BLOCK_SIZE, 0x55);
}

TEST(ntag_driver, write_half_block_of_55)
{
	/* write block of 0xAA */
	retval = NTAG_WriteBlock(ntag_handle, NTAG_TEST_BLOCK_START, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* read back block */
	retval = NTAG_ReadBlock(ntag_handle, NTAG_TEST_BLOCK_START, NULL, 0);
	TEST_ASSERT_FALSE(retval);

	/* directly modify rx_buffer and write */
	clearData(ntag_handle->rx_buffer + RX_START + NTAG_BLOCK_SIZE / 2, NTAG_BLOCK_SIZE / 2, 0x55);
	retval = NTAG_WriteBlock(ntag_handle, NTAG_TEST_BLOCK_START, ntag_handle->rx_buffer+RX_START, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* read */
	clearData(ntag_test_buffer, NTAG_BLOCK_SIZE*2, 0x00);
	retval = NTAG_ReadBlock(ntag_handle, NTAG_TEST_BLOCK_START, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* check */
	checkData(ntag_test_buffer                      , NTAG_BLOCK_SIZE / 2, 0xAA);
	checkData(ntag_test_buffer + NTAG_BLOCK_SIZE / 2, NTAG_BLOCK_SIZE / 2, 0x55);
}

TEST(ntag_driver, single_unaligned_byte_read)
{
	size_t i = 0;
	uint16_t address = NTAG_TEST_BLOCK_START * NTAG_BLOCK_SIZE;
	uint16_t offset = NTAG_BLOCK_SIZE/2;
	uint8_t value = 0;

	/* fill first block with byte numbers [0-15] */
	for( i = 0; i < NTAG_BLOCK_SIZE; i++)
		ntag_test_buffer[i] = i;

	retval = NTAG_WriteBlock(ntag_handle, NTAG_TEST_BLOCK_START, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	retval = NFC_ReadBytes(ntag_handle, address+offset, &value, 1);
	TEST_ASSERT_FALSE(retval);

	/* check */
	TEST_ASSERT_EQUAL_HEX8(ntag_test_buffer[offset], value);

}

TEST(ntag_driver, single_byte_read_whole_block)
{
	size_t i = 0;
	uint16_t address = NTAG_TEST_BLOCK_START * NTAG_BLOCK_SIZE;
	uint8_t value = 0;

	/* fill first block with byte numbers [0-UCODE_BLOCK_SIZE] */
	for( i = 0; i < NTAG_BLOCK_SIZE; i++)
		ntag_test_buffer[i] = i;

	/* write block to device */
	retval = NTAG_WriteBlock(ntag_handle, NTAG_TEST_BLOCK_START, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* single read bytes and check */
	for( i = 0; i < NTAG_BLOCK_SIZE; i++ )
	{
		/* read */
		retval = NFC_ReadBytes(ntag_handle, address+i, &value, 1);
		TEST_ASSERT_FALSE(retval);

		/* check */
		TEST_ASSERT_EQUAL_HEX8(ntag_test_buffer[i], value);
	}
}

TEST(ntag_driver, single_unaligned_byte_write)
{
	uint16_t address = NTAG_TEST_BLOCK_START * NTAG_BLOCK_SIZE + NTAG_BLOCK_SIZE/2;
	uint8_t value = 0x55;
	size_t i = 0;

	/* clear first block */
	retval = NTAG_WriteBlock(ntag_handle,NTAG_TEST_BLOCK_START, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* write single byte */
	retval = NFC_WriteBytes(ntag_handle, address, &value, 1);
	TEST_ASSERT_FALSE(retval);

	retval = NTAG_ReadBlock(ntag_handle, NTAG_TEST_BLOCK_START, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	for( i = 0; i < NTAG_BLOCK_SIZE; i++ )
	{
		if( NTAG_BLOCK_SIZE / 2 == i )
			TEST_ASSERT_EQUAL_HEX8(0x55, ntag_test_buffer[i]);
		else
			TEST_ASSERT_EQUAL_HEX8(0xAA, ntag_test_buffer[i]);
	}
}

TEST(ntag_driver, single_byte_write_whole_block)
{
	uint16_t address = NTAG_TEST_BLOCK_START * NTAG_BLOCK_SIZE;
	uint8_t value = 0x55;
	size_t i = 0;

	/* clear first block */
	retval = NTAG_WriteBlock(ntag_handle,NTAG_TEST_BLOCK_START, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* write byte number to each byte in block [0-15] */
	for( i = 0; i < NTAG_BLOCK_SIZE; i++)
	{
		/* write single byte */
		value = i;
		retval = NFC_WriteBytes(ntag_handle, address+i, &value, 1);
		TEST_ASSERT_FALSE(retval);
	}

	retval = NTAG_ReadBlock(ntag_handle, NTAG_TEST_BLOCK_START, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	for( i = 0; i < NTAG_BLOCK_SIZE; i++ )
	{
		TEST_ASSERT_EQUAL_HEX8(i, ntag_test_buffer[i]);
	}
}

TEST(ntag_driver, write_bytes_crossing_block_boundary)
{
	uint16_t address = NTAG_TEST_BLOCK_START * NTAG_BLOCK_SIZE;

	/* setup - write 2 blocks of 0xAA */
	retval = NTAG_WriteBlock(ntag_handle,NTAG_TEST_BLOCK_START+0, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);
	retval = NTAG_WriteBlock(ntag_handle,NTAG_TEST_BLOCK_START+1, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* write NTAG_BLOCK_SIZE bytes of 0x55 crossing block boundary */
	address = address + NTAG_BLOCK_SIZE / 2;
	clearData(ntag_test_buffer, NTAG_BLOCK_SIZE, 0x55);

	retval = NFC_WriteBytes(ntag_handle, address, ntag_test_buffer, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* check 2 blocks */
	clearData(ntag_test_buffer, NTAG_BLOCK_SIZE*2, 0x00);
	retval = NTAG_ReadBlock(ntag_handle, NTAG_TEST_BLOCK_START+0, ntag_test_buffer                , NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);
	retval = NTAG_ReadBlock(ntag_handle, NTAG_TEST_BLOCK_START+1, ntag_test_buffer+NTAG_BLOCK_SIZE, NTAG_BLOCK_SIZE);
	TEST_ASSERT_FALSE(retval);

	checkData(ntag_test_buffer                          , NTAG_BLOCK_SIZE / 2, 0xAA);
	checkData(ntag_test_buffer + NTAG_BLOCK_SIZE / 2    , NTAG_BLOCK_SIZE    , 0x55);
	checkData(ntag_test_buffer + NTAG_BLOCK_SIZE * 3 / 2, NTAG_BLOCK_SIZE / 2, 0xAA);
}

TEST(ntag_driver, write_bytes_multiple_blocks_unaligned)
{
	uint16_t address = 0;
	uint8_t nr_of_blocks = 10;
	size_t i = 0;
	size_t oversize = 1;


	/* setup - write nr_of_blocks+1 of 0xAA */
	for( i = 0; i < (size_t) nr_of_blocks+oversize; i++ )
	{
		retval = NTAG_WriteBlock(ntag_handle,NTAG_TEST_BLOCK_START+i, ntag_test_buffer, NTAG_BLOCK_SIZE);
		TEST_ASSERT_FALSE(retval);
	}

	/* write 10 blocks of 0x55 unaligned */
	address = NTAG_TEST_BLOCK_START * NTAG_BLOCK_SIZE + NTAG_BLOCK_SIZE / 2;
	clearData(ntag_test_buffer, NTAG_BLOCK_SIZE*nr_of_blocks, 0x55);

	retval = NFC_WriteBytes(ntag_handle, address, ntag_test_buffer, NTAG_BLOCK_SIZE*nr_of_blocks);
	TEST_ASSERT_FALSE(retval);

	address = NTAG_TEST_BLOCK_START * NTAG_BLOCK_SIZE;

	/* read back all blocks */
	clearData(ntag_test_buffer, NTAG_BLOCK_SIZE*(nr_of_blocks+1), 0x00);
	retval = NFC_ReadBytes(ntag_handle, address, ntag_test_buffer, NTAG_BLOCK_SIZE*(nr_of_blocks+1));
	TEST_ASSERT_FALSE(retval);

	/* check */
	checkData(ntag_test_buffer                          , NTAG_BLOCK_SIZE / 2, 0xAA);
	checkData(ntag_test_buffer + NTAG_BLOCK_SIZE / 2    , NTAG_BLOCK_SIZE * nr_of_blocks, 0x55);
	checkData(ntag_test_buffer + NTAG_BLOCK_SIZE * (2 * nr_of_blocks + 1) / 2, NTAG_BLOCK_SIZE / 2, 0xAA);
}

TEST(ntag_driver, read_bytes_not_possible_on_closed_handle)
{
    uint16_t address = NTAG_TEST_BLOCK_START * NTAG_BLOCK_SIZE;
    uint8_t value = 0;

    NFC_CloseDevice(ntag_handle);

    retval = NFC_ReadBytes(ntag_handle, address, &value, 1);
    TEST_ASSERT_TRUE(retval);

}

TEST(ntag_driver, write_bytes_not_possible_on_closed_handle)
{
    uint16_t address = NTAG_TEST_BLOCK_START * NTAG_BLOCK_SIZE;
    uint8_t value = 0;

    NFC_CloseDevice(ntag_handle);

    retval = NFC_WriteBytes(ntag_handle, address, &value, 1);
    TEST_ASSERT_TRUE(retval);

}

/***********************************************************************/
/* LOCAL FUNCTIONS                                                     */
/***********************************************************************/
static void clearData( uint8_t *buffer, size_t len, uint8_t clear_byte )
{
	size_t i = 0;
	for( i = 0; i < len; i++ )
	{
		buffer[i] = clear_byte;
	}
}

static void checkData( uint8_t *buffer, size_t len, uint8_t check_byte )
{
	size_t i = 0;
	for( i = 0; i < len; i++ )
	{
		TEST_ASSERT_EQUAL_HEX8(check_byte, buffer[i]);
	}
}

