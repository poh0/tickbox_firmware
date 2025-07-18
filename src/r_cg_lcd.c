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
* File Name    : r_cg_lcd.c
* Version      : CodeGenerator for RL78/L12 V2.04.06.02 [03 Jun 2024]
* Device(s)    : R5F10RLA
* Tool-Chain   : GCCRL78
* Description  : This file implements device driver for LCD module.
* Creation Date: 10/07/2025
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_lcd.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_LCD_Create
* Description  : This function initializes the LCD module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Create(void)
{
    RTCEN = 1U;    /* supply LCD clock */
    LCDON = 0U;    /* disable LCD clock operation */
    LCDM0 = _00_LCD_DISPLAY_WAVEFORM_A | _00_LCD_DISPLAY_STATIC;
    LCDM0 |= _00_LCD_VOLTAGE_MODE_EXTERNAL;
    /* Set segment pins */
    PFSEG0 |= 0x40U;
    PFSEG1 |= 0xFFU;
    PFSEG2 |= 0xFDU;
    PFSEG3 |= 0x97U;
    PFSEG4 |= 0x7FU;
    POM1 &= 0x7EU;
    PIM1 &= 0xFEU;
    PU1 &= 0x66U;
    PMC1 &= 0xE7U;
    P1 &= 0x66U;
    PM1 &= 0x66U;
    PU3 &= 0xFCU;
    P3 &= 0xFCU;
    PM3 &= 0xFCU;
    PU4 &= 0xF1U;
    PMC4 &= 0xFDU;
    P4 &= 0xF1U;
    PM4 &= 0xF1U;
    PU5 &= 0xE1U;
    P5 &= 0xE1U;
    PM5 &= 0xE1U;
    P6 &= 0xFCU;
    PM6 &= 0xFCU;
    PU7 &= 0xE0U;
    P7 &= 0xE0U;
    PM7 &= 0xE0U;
    PU12 &= 0xFEU;
    PMC12 &= 0xFEU;
    P12 &= 0xFEU;
    PM12 &= 0xFEU;
    PU14 &= 0x01U;
    PMC14 &= 0xC3U;
    P14 &= 0x01U;
    PM14 &= 0x01U;
    LCDM1 |= _00_LCD_DISPLAY_PATTERN_A;
    LCDC0 = _08_LCD_CLOCK_FSUB_FIL_9;
}

/***********************************************************************************************************************
* Function Name: R_LCD_Start
* Description  : This function enables the LCD display.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Start(void)
{
    LCDON = 1U;
}

/***********************************************************************************************************************
* Function Name: R_LCD_Stop
* Description  : This function disables the LCD display.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Stop(void)
{
    LCDON = 0U;
}

/***********************************************************************************************************************
* Function Name: R_LCD_Set_VoltageOn
* Description  : This function enables voltage boost circuit or capacitor split circuit.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Set_VoltageOn(void)
{
    SCOC = 1U;
}

/***********************************************************************************************************************
* Function Name: R_LCD_Set_VoltageOff
* Description  : This function disables voltage boost circuit or capacitor split circuit.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Set_VoltageOff(void)
{
    SCOC = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
