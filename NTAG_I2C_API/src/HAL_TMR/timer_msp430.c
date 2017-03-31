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
#include <msp430.h>
#include "HAL_timer_driver.h"

/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/
static volatile uint32_t timer = 0;

/***********************************************************************/
/* LOCAL FUNCTION PROTOTYPES                                           */
/***********************************************************************/
/***********************************************************************/
/* GLOBAL FUNCTIONS                                                    */
/***********************************************************************/
void HAL_Timer_Init()
{
	WDTCTL = WDT_MDLY_0_5;            // WDT 0.5ms, SMCLK, interval timer
	SFRIE1 |= WDTIE;                  // Enable WDT interrupt
}

void HAL_Timer_delay_ms(uint32_t ms)
{
	volatile uint32_t until = timer + ms*2;
	while (until > timer)
		;
}

uint32_t HAL_Timer_getTime_ms()
{
	return (timer >> 1);
}
/***********************************************************************/
/* LOCAL FUNCTIONS                                                     */
/***********************************************************************/
/***********************************************************************/
/* INTERUPT SERVICE ROUTINES                                           */
/***********************************************************************/
#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void) /* every 0.5 ms */
{
	timer++;
}



