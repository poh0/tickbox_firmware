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
* Device(s)    : R5F10RLA
* Tool-Chain   : GCCRL78
* Description  : This file implements main function.
* Creation Date: 10/07/2025
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

/* 125ms * 300 */
#define ALARM_LENGHT 300

enum watch_state_t {
	STATE_SET_ALARM,
	STATE_SET_TIME,
	STATE_NORMAL,
	STATE_ALARM_ACTIVE
} g_watch_state;

static uint8_t g_min_bcd = 0, g_hour_bcd = 0;
static uint8_t g_alarm_min_bcd = 0, g_alarm_hour_bcd = 0;

static uint16_t g_timer_cnt = 0;
rtc_counter_value_t g_time_data;

static uint8_t g_adjust_state = 0;

/* INTERRUPT FLAGS */
volatile uint8_t g_rtc_tick_flag = 0;
volatile uint8_t g_intp0_flag = 0;
volatile uint8_t g_intp2_flag = 0;
volatile uint8_t g_intp5_flag = 0;
volatile uint8_t g_it_flag = 0;
volatile uint8_t g_rtc_alarm_flag = 0;
/* -------------------- */

void r_main_handle_rtc(void);
void r_main_handle_interrupt(void);
static void handle_timer(void);
static void alarm_stop(void);
static void alarm_start(void);

static uint8_t is_alarm_on(void);
static uint8_t inc_bcd(uint8_t bcd_data);

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
    R_LCD_Display_Colon();
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

    g_min_bcd = 0x00;
    g_hour_bcd = 0x00;
    g_alarm_min_bcd = 0x00;
    g_alarm_hour_bcd = 0x00;

    /* Start LCD */
    R_LCD_Set_VoltageOn();
    R_LCD_Start();

    /* Enable interrupts INTP0, INTP2 and INTP5 */
    R_INTC0_Start();
    /*R_INTC2_Start();*/
    R_INTC5_Start();

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

    if (g_rtc_alarm_flag)
    {
    	g_rtc_alarm_flag = 0;

    	if (is_alarm_on() && g_watch_state == STATE_NORMAL)
    	{
    		alarm_start();
    	}

    	r_main_handle_rtc();
    }

    if (g_intp0_flag)
    {
    	g_intp0_flag = 0;

    	if (g_watch_state == STATE_NORMAL)
    	{
    		g_watch_state = STATE_SET_ALARM;
			g_adjust_state = HOUR_ADJUST;
			R_LCD_Display_Hours(g_alarm_hour_bcd);
			R_LCD_Display_Minutes(g_alarm_min_bcd);
    	}

    	else if (g_watch_state == STATE_SET_TIME)
    	{
    		if (g_adjust_state == MINUTE_ADJUST)
    		{
        		rtc_counter_value_t time_data;
        		time_data.sec = 0x00;
        		time_data.hour = g_hour_bcd;
        		time_data.min = g_min_bcd;
        		R_RTC_Set_CounterValue(time_data);
        		g_watch_state = STATE_NORMAL;
    		}
    		else if (g_adjust_state == HOUR_ADJUST)
    		{
    			g_adjust_state = MINUTE_ADJUST;
    		}

    	}

    	else if (g_watch_state == STATE_SET_ALARM)
    	{
    		if (g_adjust_state == MINUTE_ADJUST)
    		{
    			rtc_alarm_value_t alarm_val;
    			alarm_val.alarmwh = g_alarm_hour_bcd;
    			alarm_val.alarmwm = g_alarm_min_bcd;
    			alarm_val.alarmww = 0b01111111; /* Every day of week */
    			R_RTC_Set_AlarmValue(alarm_val);
        		g_watch_state = STATE_NORMAL;
    			R_LCD_Display_Hours(g_hour_bcd);
    			R_LCD_Display_Minutes(g_min_bcd);
    		}
    		else if (g_adjust_state == HOUR_ADJUST)
    		{
    			g_adjust_state = MINUTE_ADJUST;
    		}

    	}
    }

    /* INTP2 - Snooze / backlight */
    if (g_intp2_flag)
    {
    	g_intp2_flag = 0;

    	if (g_watch_state == STATE_ALARM_ACTIVE)
    	{
    		alarm_stop();
    	}
    }

    /* INTP5 - SET TIME / +1 */
    if (g_intp5_flag)
    {
    	g_intp5_flag = 0;

    	if (g_watch_state == STATE_NORMAL)
    	{
    		g_adjust_state = HOUR_ADJUST;
    		g_watch_state = STATE_SET_TIME;
    	}
    	else if (g_watch_state == STATE_SET_TIME)
    	{
    		if (g_adjust_state == HOUR_ADJUST)
    		{
    			g_hour_bcd = inc_bcd(g_hour_bcd);
    			if (g_hour_bcd >= 0x24)
    			{
    				g_hour_bcd = 0x00;
    			}
    			R_LCD_Display_Hours(g_hour_bcd);
    		}
    		else if (g_adjust_state == MINUTE_ADJUST)
    		{
    			g_min_bcd = inc_bcd(g_min_bcd);
    			if (g_min_bcd >= 0x60)
    			{
    				g_min_bcd = 0x00;
    			}
    			R_LCD_Display_Minutes(g_min_bcd);
    		}
    	}
    	else if (g_watch_state == STATE_SET_ALARM)
    	{
    		if (g_adjust_state == HOUR_ADJUST)
    		{
    			g_alarm_hour_bcd = inc_bcd(g_alarm_hour_bcd);
    			if (g_alarm_hour_bcd >= 0x24)
    			{
    				g_alarm_hour_bcd = 0x00;
    			}
    			R_LCD_Display_Hours(g_alarm_hour_bcd);
    		}
    		else if (g_adjust_state == MINUTE_ADJUST)
    		{
    			g_alarm_min_bcd = inc_bcd(g_alarm_min_bcd);
    			if (g_alarm_min_bcd >= 0x60)
    			{
    				g_alarm_min_bcd = 0x00;
    			}
    			R_LCD_Display_Minutes(g_alarm_min_bcd);
    		}
    	}
    }

    /* Handle interval timer interrupt */
    if(g_it_flag)
    {
    	g_it_flag = 0;
    	handle_timer();
    }
}

/* Handler for RTC interrupts */
void r_main_handle_rtc(void)
{
	if (g_watch_state == STATE_NORMAL || g_watch_state == STATE_ALARM_ACTIVE)
	{
		rtc_counter_value_t time_data;
		if (R_RTC_Get_CounterValue(&time_data) == MD_OK)
		{
			g_hour_bcd = time_data.hour;
			g_min_bcd = time_data.min;
		}
		else
		{
			/* */
		}

		R_LCD_Display_Hours(g_hour_bcd);
		R_LCD_Display_Minutes(g_min_bcd);
	}
	else if (g_watch_state == STATE_SET_TIME)
	{
		/* do nothing */
	}
	else if (g_watch_state == STATE_SET_ALARM)
	{
		rtc_counter_value_t time_data;
		if (R_RTC_Get_CounterValue(&time_data) == MD_OK)
		{
			g_hour_bcd = time_data.hour;
			g_min_bcd = time_data.min;
		}
	}
}

static void alarm_start(void)
{
	g_watch_state = STATE_ALARM_ACTIVE;
	g_timer_cnt = 0;

	/* Play 4khz buzzer sound */
	R_PCLBUZ0_Start();
	/* Start 125 ms timer */
	R_IT_Start();
}

static void alarm_stop(void)
{
	g_watch_state = STATE_NORMAL;
	/* Stop buzzer */
	R_PCLBUZ0_Stop();
	/* Stop interval timer */
	R_IT_Stop();
}

static void handle_timer(void)
{
	if (g_timer_cnt == ALARM_LENGHT)
	{
		alarm_stop();
	}

	/* BEEP pause BEEP pause pause*/
	g_timer_cnt++;
    switch (g_timer_cnt % 5) {
        case 0:
        case 2:
            R_PCLBUZ0_Start();
            break;
        case 3:
            R_PCLBUZ0_Stop();
            break;
    }
}

static uint8_t is_alarm_on(void)
{
	return 1; /* for dev purposes */
	ALARM_SWOUT = 0;

	if (ALARM_SWIN)
	{
		/* switch is open, no alarm*/
		ALARM_SWOUT = 1U;
		return 0;
	}
	else
	{
		/* Switch is closed, play alarm */
		ALARM_SWOUT = 1U;
		return 1;
	}
}

static uint8_t inc_bcd(uint8_t bcd_data)
{
    // Increment the lower nibble
    bcd_data++;

    // If the lower nibble is goes to 0xA, increment upper nibble.
    if ((bcd_data & 0x0F) > 0x09) {
        bcd_data += 0x06;
    }

    return bcd_data;
}

/* End user code. Do not edit comment generated here */
