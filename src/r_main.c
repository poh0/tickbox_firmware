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
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/L12 V2.04.06.02 [03 Jun 2024]
* Device(s)    : R5F10RLC
* Tool-Chain   : GCCRL78
* Description  : This file implements main function.
* Creation Date: 17/07/2025
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_intc.h"
#include "r_cg_rtc.h"
#include "r_cg_it.h"
#include "r_cg_pclbuz.h"
#include "r_cg_lcd.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
enum WatchState {
	STATE_SET_ALARM,
	STATE_SET_TIME,
	STATE_NORMAL,
	STATE_ALARM_ACTIVE
} g_watch_state;
static uint8_t g_minute = 0, g_hour = 0;
static uint8_t g_alarm_minute = 0, g_alarm_hour = 0;
static uint8_t beep_cnt = 0;
rtc_counter_value_t g_time_data;

volatile uint8_t g_rtc_tick_flag = 0;
volatile uint8_t g_intp0_flag = 0;

static void beep(uint8_t times);
void r_main_handle_rtc(void);
void r_main_handle_interrupt(void);

/* End user code. Do not edit comment generated here */
void R_MAIN_UserInit(void);

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
    R_MAIN_UserInit();
    /* Start user code. Do not edit comment generated here */
    NOP();
    /* R_LCD_Display_Colon(); */
    r_main_handle_rtc(); /* show 00:00 */
    while (1U)
    {
    	/* The assembly is EI; STOP;
    	 * according to the software manual:
    	 * No interrupts are acknowledged between EI and the next instruction.
    	 * Meaning that pending interrupts will be ack'd after STOP instruction
    	 * Which means that there will be no lost wakeups.
    	 *  */
        EI(); /* Enable interrupt acknowledgement */
        STOP(); /* enter stop mode */
        r_main_handle_interrupt(); /* We woke up because of an interrupt */
    }
    /* End user code. Do not edit comment generated here */
}


/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */

    g_watch_state = STATE_NORMAL; /* set state to normal operation */

    g_minute = 0x00;
    g_hour = 0x00;

    /* Enable interrupts INTP0, INTP2 and INTP5 */
    R_INTC0_Start();
    R_RTC_Set_ConstPeriodInterruptOn(ONEMIN);   /* Enable RTC interrupt */
    R_RTC_Start();                              /* Start RTC operation */

    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */

void r_main_handle_interrupt(void)
{
	DI(); /* No more ISRs until we enter STOP mode */
    if (g_rtc_tick_flag)
    {
    	g_rtc_tick_flag = 0;
        r_main_handle_rtc();
    }

    if (g_intp0_flag)
    {
    	g_intp0_flag = 0;
    	P13 ^= 1U;
    }
}

/* Handler for RTC interrupts */
void r_main_handle_rtc(void)
{
	if (g_watch_state == STATE_NORMAL)
	{
		R_RTC_Get_CounterValue(&g_time_data);
		g_hour = g_time_data.hour;
		g_minute = g_time_data.min;

		if (g_hour == g_alarm_hour && g_minute == g_alarm_minute)
		{
			/* TODO: check the switch, now defaulted to on */
			if (1)
			{
				//g_watch_state = STATE_ALARM_ACTIVE;
			}

		}

		/*
		R_LCD_Display_Hours(g_hour);
		R_LCD_Display_Minutes(g_minute);
		*/
	}
	else if (g_watch_state == STATE_SET_TIME)
	{
		g_hour = g_time_data.hour;
		g_minute = g_time_data.min;
	}
	else if (g_watch_state == STATE_SET_ALARM)
	{
		g_hour = g_time_data.hour;
		g_minute = g_time_data.min;
	}
	else if (g_watch_state == STATE_ALARM_ACTIVE)
	{
		g_hour = g_time_data.hour;
		g_minute = g_time_data.min;

		/* Probably not gonna happen */
		/*
		R_LCD_Display_Hours(g_hour);
		R_LCD_Display_Minutes(g_minute);
		*/
	}
}

/* Handler for INTP0 */
void r_main_handle_intp0(void)
{
	if (g_watch_state == NORMAL_OP)
	{
		/* Enter set alarm */
		beep(2);
	}
	else if (g_watch_state == SET_ALARM || g_watch_state == SET_TIME)
	{
		/* SWTICH BETWEEN MIN/HR */
	}
}

static void beep(uint8_t times)
{
	uint8_t i;
	for (i = 0; i < times; i++)
	{
		R_PCLBUZ0_Start();
		R_IT_Delay(IT_DELAY_ONESEC); /* one second */
		R_PCLBUZ0_Stop();
		if (i < times - 1) R_IT_Delay(IT_DELAY_HALFSEC); /* one second */
	}
}


/* End user code. Do not edit comment generated here */
