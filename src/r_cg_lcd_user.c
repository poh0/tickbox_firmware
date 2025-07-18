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
* File Name    : r_cg_lcd_user.c
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

static void empty_hour_tens_digit(void);
static void set_digit(uint8_t digit, uint8_t value);

/* bitfield representation of segments of digit. */
static const uint8_t g_digit_segments[10] = {
    0b00111111, // 0: a b c d e f
    0b00000110, // 1: b c
    0b01011011, // 2: a b d e g
    0b01001111, // 3: a b c d g
    0b01100110, // 4: b c f g
    0b01101101, // 5: a c d f g
    0b01111101, // 6: a c d e f g
    0b00000111, // 7: a b c
    0b01111111, // 8: a b c d e f g
    0b01101111  // 9: a b c d f g
};

static volatile uint8_t* const g_digit_seg_map[4][7] = {
  { &SEG2, &SEG3, &SEG24, &SEG23, &SEG22, &SEG1, &SEG0 }, // Digit 0
  { &SEG9, &SEG10, &SEG28, &SEG26, &SEG25, &SEG8, &SEG6 }, // Digit 1
  { &SEG14, &SEG15, &SEG33, &SEG32, &SEG31, &SEG13, &SEG12 }, // Digit 2
  { &SEG18, &SEG37, &SEG36, &SEG35, &SEG34, &SEG17, &SEG16 } // Digit 3
}; /* should this be volatile ?*/


/***********************************************************************************************************************
* Function Name: R_LCD_Init_Segments
* Description  : This function initialises SEG pin registers to 0.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Init_Segments(void)
{
    uint8_t i, j;
	for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 7; j++)
        {
            *g_digit_seg_map[i][j] = 0x00;
        }
    }

    SEG_COL = 0x00;
}

/***********************************************************************************************************************
* Function Name: R_LCD_Display_Hours
* Description  : This function writes to segment registers that correspond to hour digits on the LCD.
* Arguments    : hour - number 00-23 to be shown on the display
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Display_Hours(uint8_t hour)
{
    if (hour > 23)
    {
        return;
    }

    if (hour < 10)
    {
    	empty_hour_tens_digit();
    	set_digit(1, hour);
    }
    else
    {
    	set_digit(0, hour / 10);
    	set_digit(1, hour % 10);
    }

}

/***********************************************************************************************************************
* Function Name: R_LCD_Display_Minutes
* Description  : This function writes to segment registers that correspond to Minute digits on the LCD.
* Arguments    : minutes - number 0 to 59 to be shown on the display
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Display_Minutes(uint8_t minutes)
{
    if (minutes > 59)
    {
        return;
    }

    if (minutes < 10)
    {
    	set_digit(2, 0);
    	set_digit(3, minutes);
    }
    else
    {
    	set_digit(2, minutes / 10);
    	set_digit(3, minutes % 10);
    }
}

/***********************************************************************************************************************
* Function Name: R_LCD_Display_Colon
* Description  : This function displays the colon segemnt on the LCD.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Display_Colon(void)
{
	SEG_COL |= 1u;
}

/* End user code. Do not edit comment generated here */

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
