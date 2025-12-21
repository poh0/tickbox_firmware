/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012, 2024 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_cgc.c
* Version      : CodeGenerator for RL78/L12 V2.04.06.02 [03 Jun 2024]
* Device(s)    : R5F10RLA
* Tool-Chain   : GCCRL78
* Description  : This file implements device driver for CGC module.
* Creation Date: 10/07/2025
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_CGC_Create
* Description  : This function initializes the clock generator.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CGC_Create(void)
{
	volatile uint32_t w_count;
	/* Variables temp_stab_set and temp_stab_wait removed as they are for X1 only */

	/* Set fMX */
	/* * FIXED SETTINGS:
	 * _00_CGC_HISYS_PORT      : X1/X2 are PORTS (Disables missing high-speed crystal)
	 * _10_CGC_SUB_OSC         : XT1/XT2 are CRYSTAL (Enables 32kHz crystal)
	 * _00_CGC_SYSOSC_UNDER10M : Default setting
	 * _02_CGC_SUBMODE_NORMAL  : Normal drive for 32kHz (More reliable startup than ULOW)
	 */
	CMC = _00_CGC_HISYS_PORT | _10_CGC_SUB_OSC | _00_CGC_SYSOSC_UNDER10M | _02_CGC_SUBMODE_NORMAL;

	/* OSTS is for X1 stabilization. Since X1 is OFF, this is not strictly needed,
	   but we leave it to default or previous setting. */
	OSTS = _07_CGC_OSCSTAB_SEL18;

	MSTOP = 1U;   /* FORCE X1 High-Speed Crystal OFF (It is not on the board) */
	HIOSTOP = 0U; /* ENSURE Internal High-Speed OCO is ON (This runs the CPU) */

	/* * DELETED: The OSTC wait loop.
	 * The OSTC register only counts pulses from the X1 crystal.
	 * Since X1 is not present, OSTC never increments, causing the infinite hang.
	 */

	/* Set fMAIN */
	MCM0 = 0U; /* Select Internal High-Speed OCO as Main Clock */

	/* Set fSUB */
	XTSTOP = 0U; /* Start the 32kHz Subsystem Crystal */

	/* Change the waiting time according to the system */
	/* INCREASED WAIT: 32kHz crystals take a long time to stabilize (200-500ms).
	   The previous loop was too fast. We use a larger count here to be safe. */
	for (w_count = 0U; w_count <= 50000; w_count++)
	{
		NOP();
	}

	/* Configure Subsystem Clock for Peripherals */
	OSMC = _00_CGC_SUBINHALT_ON | _00_CGC_RTC_CLK_FSUB;

	/* Set fCLK */
	CSS = 0U; /* CPU Clock Source = Main System Clock (HOCO) */

}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
