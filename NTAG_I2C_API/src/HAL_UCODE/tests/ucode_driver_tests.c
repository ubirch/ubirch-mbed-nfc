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
#include "ucode_driver.h"
#include "../ucode_driver_intern.h"

/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
#define UCODE_TEST_BUFFER_SIZE  UCODE_MAX_READ_SIZE * 16
#define UCODE_TEST_ADDRESS      UCODE_USER_MEMORY
#define UCODE_TEST_DEVICE		UCODE0

/***********************************************************************/
/* LOCAL VARIABLES                                                     */
/***********************************************************************/
static HAL_I2C_HANDLE_T i2c_handle;
static UCODE_HANDLE_T ucode_handle;
static BOOL retval;
static uint8_t ucode_test_buffer[UCODE_TEST_BUFFER_SIZE];


/***********************************************************************/
/* LOCAL FUNCTION PROTOTYPES                                           */
/***********************************************************************/
static void clearData( uint8_t *buffer, size_t len, uint8_t clear_byte );
static void checkData( uint8_t *buffer, size_t len, uint8_t check_byte );


/***********************************************************************/
/* TEST GROUP DEFINITION                                               */
/***********************************************************************/
TEST_GROUP(ucode_driver);

TEST_SETUP(ucode_driver)
{
	ucode_handle = UCODE_INVALID_HANDLE;
	retval = FALSE;

	/* use first device for tests */
	ucode_handle = UCODE_InitDevice(UCODE_TEST_DEVICE, i2c_handle);

	clearData(ucode_test_buffer, UCODE_TEST_BUFFER_SIZE, 0xAA);
}

TEST_TEAR_DOWN(ucode_driver)
{
	UCODE_CloseDevice(ucode_handle);
}

TEST_GROUP_RUNNER(ucode_driver)
{
	/* setup test group */
	i2c_handle = HAL_I2C_InitDevice(HAL_I2C_INIT_DEFAULT);

	/* test device list structure */
	RUN_TEST_CASE(ucode_driver, device_list_not_empty);
	RUN_TEST_CASE(ucode_driver, device_list_i2c_addresses_valid);

	/* test init/close handling */
	RUN_TEST_CASE(ucode_driver, handle_closed_after_closing_device);
	RUN_TEST_CASE(ucode_driver, init_not_possible_if_handle_not_closed)
	RUN_TEST_CASE(ucode_driver, init_returns_valid_ucode_handle);
	RUN_TEST_CASE(ucode_driver, init_more_than_max_devices_not_possible)
	RUN_TEST_CASE(ucode_driver, i2c_handle_valid_after_init);
	RUN_TEST_CASE(ucode_driver, i2c_handle_invalid_after_close);
	RUN_TEST_CASE(ucode_driver, no_error_set_after_init);
	RUN_TEST_CASE(ucode_driver, close_protected_against_null_ptr_dereference);

	/* test write/read page handling */
	RUN_TEST_CASE(ucode_driver, read_tid_check_model_id);
	RUN_TEST_CASE(ucode_driver, write_single_page_of_AA);
	RUN_TEST_CASE(ucode_driver, write_multiple_pages);
	RUN_TEST_CASE(ucode_driver, write_single_word_of_AA);
	RUN_TEST_CASE(ucode_driver, write_multiple_words);

	RUN_TEST_CASE(ucode_driver, null_buffer_block_read_fills_internal);
	RUN_TEST_CASE(ucode_driver, write_half_a_page_of_55);
	RUN_TEST_CASE(ucode_driver, write_half_a_word_of_55);

	RUN_TEST_CASE(ucode_driver, bigger_buffer_only_writes_first_page);
	RUN_TEST_CASE(ucode_driver, bigger_buffer_only_writes_first_word);

	/* optimize private functions
	 * RUN_TEST_CASE(ucode_driver, block_read_not_possible_on_closed_handle);
	 * RUN_TEST_CASE(ucode_driver, block_write_not_possible_on_closed_handle); */

	/* test write/read byte(s) API */
	RUN_TEST_CASE(ucode_driver, single_unaligned_byte_read);
	RUN_TEST_CASE(ucode_driver, single_byte_read_whole_page);

	RUN_TEST_CASE(ucode_driver, single_unaligned_byte_write);
	RUN_TEST_CASE(ucode_driver, single_byte_write_whole_block);
	RUN_TEST_CASE(ucode_driver, write_bytes_crossing_page_boundary);
	RUN_TEST_CASE(ucode_driver, write_bytes_multiple_pages_unaligned);

	/* tear down test group */
	HAL_I2C_CloseDevice(i2c_handle);
}

/***********************************************************************/
/* TESTS                                                               */
/***********************************************************************/
/* not strictly necessary - empty list will trigger compile error */
TEST(ucode_driver, device_list_not_empty)
{
	TEST_ASSERT_NOT_EQUAL(0, UCODE_ID_MAX_DEVICES);
}

TEST(ucode_driver, device_list_i2c_addresses_valid)
{
	HAL_I2C_STATUS_T retval;
	uint8_t sendBuffer[TX_START+1];

	size_t i = 0;
	for( i = 0; i < UCODE_ID_MAX_DEVICES; i++ )
	{
		uint8_t address = ucode_device_list[i].address;
		TEST_ASSERT_TRUE(address >   0);
		TEST_ASSERT_TRUE(address < 128);

		/* check for ACK from addressed device on i2cbus */
		retval = HAL_I2C_SendBytes(i2c_handle, address, sendBuffer,0);
		TEST_ASSERT_EQUAL(HAL_I2C_OK, retval);
	}
}

TEST(ucode_driver, handle_closed_after_closing_device)
{
	UCODE_CloseDevice(ucode_handle);
	TEST_ASSERT_EQUAL(UCODE_CLOSED, UCODE_GetLastError(ucode_handle));
}

TEST(ucode_driver, init_not_possible_if_handle_not_closed)
{
	UCODE_CloseDevice(ucode_handle);
	ucode_device_list[0].status = UCODE_OK;     /* setup */
	ucode_handle = UCODE_InitDevice(UCODE_TEST_DEVICE, i2c_handle);
	TEST_ASSERT_EQUAL(UCODE_INVALID_HANDLE, ucode_handle);
	ucode_device_list[0].status = UCODE_CLOSED; /* tear down */
}

TEST(ucode_driver, init_returns_valid_ucode_handle)
{
	UCODE_CloseDevice(ucode_handle);
	ucode_handle = UCODE_InitDevice(UCODE_TEST_DEVICE, i2c_handle);
	TEST_ASSERT_NOT_EQUAL(UCODE_INVALID_HANDLE, ucode_handle);
}

TEST(ucode_driver, init_more_than_max_devices_not_possible)
{
	UCODE_CloseDevice(ucode_handle);
	ucode_handle = UCODE_InitDevice(UCODE_ID_MAX_DEVICES, i2c_handle);
	TEST_ASSERT_EQUAL(UCODE_INVALID_HANDLE, ucode_handle);
}

TEST(ucode_driver, i2c_handle_valid_after_init)
{
	UCODE_CloseDevice(ucode_handle);
	ucode_handle = UCODE_InitDevice(UCODE_TEST_DEVICE, i2c_handle);
	TEST_ASSERT_NOT_EQUAL(UCODE_INVALID_HANDLE, ucode_device_list[0].i2cbus);
}

TEST(ucode_driver, i2c_handle_invalid_after_close)
{
	UCODE_CloseDevice(ucode_handle);
	TEST_ASSERT_EQUAL(UCODE_INVALID_HANDLE, ucode_device_list[0].i2cbus);
}

/* not sure if failure would trigger on all platforms */
TEST(ucode_driver, close_protected_against_null_ptr_dereference)
{
	UCODE_CloseDevice(0);
}

TEST(ucode_driver, no_error_set_after_init)
{
	UCODE_CloseDevice(ucode_handle);
	ucode_handle = UCODE_InitDevice(UCODE_TEST_DEVICE, i2c_handle);
	TEST_ASSERT_EQUAL(UCODE_OK, UCODE_GetLastError(ucode_handle));
}

TEST(ucode_driver, read_tid_check_model_id)
{
	retval = UCODE_ReadBlock(ucode_handle, UCODE_TID, ucode_test_buffer, 6);
	TEST_ASSERT_FALSE(retval);

	TEST_ASSERT_EQUAL_HEX8(0xE2, ucode_test_buffer[0]);
	TEST_ASSERT_EQUAL_HEX8(0x00, ucode_test_buffer[1]);
	TEST_ASSERT_EQUAL_HEX8(0x68, ucode_test_buffer[2]);
	TEST_ASSERT_EQUAL_HEX8(0x0D, ucode_test_buffer[3]); /* SL3S4011 */
//  TEST_ASSERT_EQUAL_HEX8(0x8D, ucode_test_buffer[3]); /* SL3S4021 */
	TEST_ASSERT_EQUAL_HEX8(0x00, ucode_test_buffer[4]);
	TEST_ASSERT_EQUAL_HEX8(0x00, ucode_test_buffer[5]);
}

TEST(ucode_driver, write_single_page_of_AA)
{
	/* write */
	clearData(ucode_test_buffer, UCODE_PAGE_SIZE, 0xAA);
	retval = UCODE_WritePage(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* read */
	clearData(ucode_test_buffer, UCODE_PAGE_SIZE, 0x00);
	retval = UCODE_ReadBlock(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* check */
	checkData(ucode_test_buffer, UCODE_PAGE_SIZE, 0xAA);
}

TEST(ucode_driver, write_multiple_pages)
{
	size_t page = 0;
	size_t offset = 0;
	size_t nr_of_pages = 15;

	/* write */
	for( page = 0; page < nr_of_pages; page++ )
	{
		offset = page * UCODE_PAGE_SIZE;
		clearData(ucode_test_buffer+offset, UCODE_PAGE_SIZE, (page+1)*0x11);
	}

	for( page = 0; page < nr_of_pages; page++)
	{
		offset = page*UCODE_PAGE_SIZE;
		retval = UCODE_WritePage(ucode_handle, UCODE_TEST_ADDRESS+offset, ucode_test_buffer+offset, UCODE_PAGE_SIZE);
		TEST_ASSERT_FALSE(retval);
	}

	/* read */
	clearData(ucode_test_buffer, UCODE_PAGE_SIZE*nr_of_pages, 0xAA);

	for( page = 0; page < nr_of_pages; page++ )
	{
		offset = page*UCODE_PAGE_SIZE;
		retval = UCODE_ReadBlock (ucode_handle, UCODE_TEST_ADDRESS+offset, ucode_test_buffer+offset, UCODE_PAGE_SIZE);
		TEST_ASSERT_FALSE(retval);
	}

	/* check */
	for( page = 0; page < nr_of_pages; page++ )
		checkData(ucode_test_buffer+page*UCODE_PAGE_SIZE, UCODE_PAGE_SIZE, (page+1)*0x11);
}

TEST(ucode_driver, bigger_buffer_only_writes_first_page)
{
	uint16_t address = 0;

	/* setup - write 2 pages of 0xAA */
	clearData(ucode_test_buffer, UCODE_PAGE_SIZE, 0xAA);

	address = UCODE_TEST_ADDRESS;
	retval = UCODE_WritePage(ucode_handle, address, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	address = UCODE_TEST_ADDRESS + UCODE_PAGE_SIZE;
	retval = UCODE_WritePage(ucode_handle, address, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* try writing page size * 2 of 0x55 */
	address = UCODE_TEST_ADDRESS;
	clearData(ucode_test_buffer, UCODE_PAGE_SIZE*2, 0x55);
	retval = UCODE_WritePage(ucode_handle, address, ucode_test_buffer, UCODE_PAGE_SIZE*2);
	TEST_ASSERT_FALSE(retval);

	/* read */
	clearData(ucode_test_buffer, UCODE_PAGE_SIZE*2, 0x00);
	retval = UCODE_ReadBlock(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE*2);
	TEST_ASSERT_FALSE(retval);

	/* check */
	checkData(ucode_test_buffer                , UCODE_PAGE_SIZE, 0x55);
	checkData(ucode_test_buffer+UCODE_PAGE_SIZE, UCODE_PAGE_SIZE, 0xAA);
}

TEST(ucode_driver, bigger_buffer_only_writes_first_word)
{
	uint16_t address = 0;

	/* setup - write 2 words of 0xAA */
	clearData(ucode_test_buffer, UCODE_WORD_SIZE, 0xAA);

	address = UCODE_TEST_ADDRESS;
	retval = UCODE_WriteWord(ucode_handle, address, ucode_test_buffer, UCODE_WORD_SIZE);
	TEST_ASSERT_FALSE(retval);

	address = UCODE_TEST_ADDRESS + UCODE_WORD_SIZE;
	retval = UCODE_WriteWord(ucode_handle, address, ucode_test_buffer, UCODE_WORD_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* try writing word size * 2 of 0x55 */
	address = UCODE_TEST_ADDRESS;
	clearData(ucode_test_buffer, UCODE_WORD_SIZE*2, 0x55);
	retval = UCODE_WriteWord(ucode_handle, address, ucode_test_buffer, UCODE_WORD_SIZE*2);
	TEST_ASSERT_FALSE(retval);

	/* read */
	clearData(ucode_test_buffer, UCODE_WORD_SIZE*2, 0x00);
	retval = UCODE_ReadBlock(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_WORD_SIZE*2);
	TEST_ASSERT_FALSE(retval);

	/* check */
	checkData(ucode_test_buffer                , UCODE_WORD_SIZE, 0x55);
	checkData(ucode_test_buffer+UCODE_WORD_SIZE, UCODE_WORD_SIZE, 0xAA);
}

TEST(ucode_driver, write_single_word_of_AA)
{
	/* write */
	clearData(ucode_test_buffer, UCODE_WORD_SIZE, 0xAA);
	retval = UCODE_WriteWord(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_WORD_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* read */
	clearData(ucode_test_buffer, UCODE_WORD_SIZE, 0x00);
	retval = UCODE_ReadBlock(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_WORD_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* check */
	checkData(ucode_test_buffer, UCODE_WORD_SIZE, 0xAA);
}

TEST(ucode_driver, write_multiple_words)
{
	size_t word = 0;
	size_t offset = 0;
	size_t nr_of_words = 15;

	/* write */
	for( word = 0; word < nr_of_words; word++ )
	{
		offset = word * UCODE_WORD_SIZE;
		clearData(ucode_test_buffer+offset, UCODE_WORD_SIZE, (word+1)*0x11);
	}

	for( word = 0; word < nr_of_words; word++ )
	{
		offset = word*UCODE_WORD_SIZE;
		retval = UCODE_WriteWord(ucode_handle, UCODE_TEST_ADDRESS+offset, ucode_test_buffer+offset, UCODE_WORD_SIZE);
		TEST_ASSERT_FALSE(retval);
	}

	/* read */
	clearData(ucode_test_buffer, UCODE_WORD_SIZE*nr_of_words, 0xAA);

	for( word = 0; word < nr_of_words; word++ )
	{
		offset = word*UCODE_WORD_SIZE;
		retval = UCODE_ReadBlock (ucode_handle, UCODE_TEST_ADDRESS+offset, ucode_test_buffer+offset, UCODE_WORD_SIZE);
		TEST_ASSERT_FALSE(retval);
	}

	/* check */
	for( word = 0; word < nr_of_words; word++ )
		checkData(ucode_test_buffer+word*UCODE_WORD_SIZE, UCODE_WORD_SIZE, (word+1)*0x11);
}

TEST(ucode_driver, null_buffer_block_read_fills_internal)
{
	/* write 1 block 0x55 */
	clearData(ucode_test_buffer, UCODE_PAGE_SIZE, 0x55);
	retval = UCODE_WritePage(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* clear internal receive buffer */
	clearData(ucode_handle->rx_buffer+RX_START, UCODE_PAGE_SIZE, 0x00);

	/* read 1 block */
	retval = UCODE_ReadBlock(ucode_handle, UCODE_TEST_ADDRESS, NULL, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* check internal receive buffer */
	checkData(ucode_handle->rx_buffer+RX_START, UCODE_PAGE_SIZE, 0x55);
}

TEST(ucode_driver, write_half_a_page_of_55)
{
	/* write page of 0xAA */
	retval = UCODE_WritePage(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* read back page */
	retval = UCODE_ReadBlock(ucode_handle, UCODE_TEST_ADDRESS, NULL, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* directly modify rx_buffer and write */
	clearData(ucode_handle->rx_buffer + RX_START + UCODE_PAGE_SIZE / 2, UCODE_PAGE_SIZE / 2, 0x55);
	retval = UCODE_WritePage(ucode_handle, UCODE_TEST_ADDRESS, ucode_handle->rx_buffer+RX_START, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* read */
	clearData(ucode_test_buffer, UCODE_PAGE_SIZE*2, 0x00);
	retval = UCODE_ReadBlock(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* check */
	checkData(ucode_test_buffer                      , UCODE_PAGE_SIZE / 2, 0xAA);
	checkData(ucode_test_buffer + UCODE_PAGE_SIZE / 2, UCODE_PAGE_SIZE / 2, 0x55);
}

TEST(ucode_driver, write_half_a_word_of_55)
{
	/* write word of 0xAA */
	retval = UCODE_WriteWord(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_WORD_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* read back word */
	retval = UCODE_ReadBlock(ucode_handle, UCODE_TEST_ADDRESS, NULL, UCODE_WORD_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* directly modify rx_buffer and write */
	clearData(ucode_handle->rx_buffer + RX_START + UCODE_WORD_SIZE / 2, UCODE_WORD_SIZE / 2, 0x55);
	retval = UCODE_WriteWord(ucode_handle, UCODE_TEST_ADDRESS, ucode_handle->rx_buffer+RX_START, UCODE_WORD_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* read */
	clearData(ucode_test_buffer, UCODE_WORD_SIZE*2, 0x00);
	retval = UCODE_ReadBlock(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_WORD_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* check */
	checkData(ucode_test_buffer                      , UCODE_WORD_SIZE / 2, 0xAA);
	checkData(ucode_test_buffer + UCODE_WORD_SIZE / 2, UCODE_WORD_SIZE / 2, 0x55);
}

TEST(ucode_driver, single_unaligned_byte_read)
{
	size_t i = 0;
	uint16_t address = UCODE_TEST_ADDRESS;
	uint16_t offset = UCODE_PAGE_SIZE/2;
	uint8_t value = 0;

	/* fill first block with byte numbers [0-UCODE_PAGE_SIZE] */
	for( i = 0; i < UCODE_PAGE_SIZE; i++)
		ucode_test_buffer[i] = i;

	retval = UCODE_WritePage(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	retval = UCODE_ReadBytes(ucode_handle, address+offset, &value, 1);
	TEST_ASSERT_FALSE(retval);

	/* check */
	TEST_ASSERT_EQUAL_HEX8(ucode_test_buffer[offset], value);
}

TEST(ucode_driver, single_byte_read_whole_page)
{
	size_t i = 0;
	uint16_t address = UCODE_TEST_ADDRESS;
	uint8_t value = 0;

	/* fill first block with byte numbers [0-UCODE_PAGE_SIZE] */
	for( i = 0; i < UCODE_PAGE_SIZE; i++)
		ucode_test_buffer[i] = i;

	/* write page to device */
	retval = UCODE_WritePage(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* single read each byte and check */
	for( i = 0; i < UCODE_PAGE_SIZE; i++)
	{
		/* read */
		retval = UCODE_ReadBytes(ucode_handle, address+i, &value, 1);
		TEST_ASSERT_FALSE(retval);

		/* check */
		TEST_ASSERT_EQUAL_HEX8(ucode_test_buffer[i], value);
	}
}

TEST(ucode_driver, single_unaligned_byte_write)
{
	uint16_t address = UCODE_TEST_ADDRESS + UCODE_PAGE_SIZE/2;
	uint8_t value = 0x55;
	size_t i = 0;

	/* clear first page */
	retval = UCODE_WritePage(ucode_handle,UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* write single byte */
	retval = UCODE_WriteBytes(ucode_handle, address, &value, 1);
	TEST_ASSERT_FALSE(retval);

	retval = UCODE_ReadBlock(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	for( i = 0; i < UCODE_PAGE_SIZE; i++ )
	{
		if( UCODE_PAGE_SIZE / 2 == i )
			TEST_ASSERT_EQUAL_HEX8(0x55, ucode_test_buffer[i]);
		else
			TEST_ASSERT_EQUAL_HEX8(0xAA, ucode_test_buffer[i]);
	}
}

TEST(ucode_driver, single_byte_write_whole_block)
{
	uint16_t address = UCODE_TEST_ADDRESS;
	uint8_t value = 0x55;
	size_t i = 0;

	/* clear first page */
	retval = UCODE_WritePage(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* write byte number to each byte in page [0-UCODE_PAGE_SIZE] */
	for( i = 0; i < UCODE_PAGE_SIZE; i++)
	{
		/* write single byte */
		value = i;
		retval = UCODE_WriteBytes(ucode_handle, address+i, &value, 1);
		TEST_ASSERT_FALSE(retval);
	}

	/* read page and check */
	retval = UCODE_ReadBlock(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	for( i = 0; i < UCODE_PAGE_SIZE; i++ )
	{
		TEST_ASSERT_EQUAL_HEX8(i, ucode_test_buffer[i]);
	}
}

TEST(ucode_driver, write_bytes_crossing_page_boundary)
{
	uint16_t address = UCODE_TEST_ADDRESS;

	/* setup - write 2 pages of 0xAA */
	retval = UCODE_WritePage(ucode_handle,UCODE_TEST_ADDRESS                , ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);
	retval = UCODE_WritePage(ucode_handle,UCODE_TEST_ADDRESS+UCODE_PAGE_SIZE, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* write UCODE_PAGE_SIZE bytes of 0x55 crossing page boundary */
	address = address + UCODE_PAGE_SIZE / 2;
	clearData(ucode_test_buffer, UCODE_PAGE_SIZE, 0x55);

	retval = UCODE_WriteBytes(ucode_handle, address, ucode_test_buffer, UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* check 2 blocks */
	clearData(ucode_test_buffer, UCODE_PAGE_SIZE*2, 0x00);
	retval = UCODE_ReadBlock(ucode_handle, UCODE_TEST_ADDRESS, ucode_test_buffer, UCODE_PAGE_SIZE*2);

	checkData(ucode_test_buffer                          , UCODE_PAGE_SIZE / 2, 0xAA);
	checkData(ucode_test_buffer + UCODE_PAGE_SIZE / 2    , UCODE_PAGE_SIZE    , 0x55);
	checkData(ucode_test_buffer + UCODE_PAGE_SIZE * 3 / 2, UCODE_PAGE_SIZE / 2, 0xAA);
}

TEST(ucode_driver, write_bytes_multiple_pages_unaligned)
{
	uint16_t address = 0;
	uint8_t nr_of_pages = 10;
	uint16_t nr_of_bytes = nr_of_pages * UCODE_PAGE_SIZE;
	size_t i = 0;

	/* setup - write nr_of_pages+1 of 0xAA */
	for( i = 0; i < (size_t) nr_of_pages+1; i++ )
	{
		retval = UCODE_WritePage(ucode_handle,UCODE_TEST_ADDRESS+i*UCODE_PAGE_SIZE, ucode_test_buffer, UCODE_PAGE_SIZE);
		TEST_ASSERT_FALSE(retval);
	}

	/* write 10 pages of 0x55 unaligned */
	address = UCODE_TEST_ADDRESS + UCODE_PAGE_SIZE / 2;

	clearData(ucode_test_buffer, UCODE_PAGE_SIZE*nr_of_pages, 0x55);

	retval = UCODE_WriteBytes(ucode_handle, address, ucode_test_buffer, nr_of_bytes);
	TEST_ASSERT_FALSE(retval);

	address = UCODE_TEST_ADDRESS;

	/* read back all pages */
	clearData(ucode_test_buffer, nr_of_bytes+UCODE_PAGE_SIZE, 0x00);
	retval = UCODE_ReadBytes(ucode_handle, address, ucode_test_buffer, nr_of_bytes + UCODE_PAGE_SIZE);
	TEST_ASSERT_FALSE(retval);

	/* check */
	checkData(ucode_test_buffer                                    , UCODE_PAGE_SIZE / 2, 0xAA);
	checkData(ucode_test_buffer               + UCODE_PAGE_SIZE / 2, nr_of_bytes       , 0x55);
	checkData(ucode_test_buffer + nr_of_bytes + UCODE_PAGE_SIZE / 2, UCODE_PAGE_SIZE / 2, 0xAA);
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

