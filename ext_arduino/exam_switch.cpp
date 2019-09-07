/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			exam_switch.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#include "exam_config.h"

#if EXAM_SWITCH

#include <stdio.h>
#include <string.h>
#include "src/config.h"
#include "src/system.h"
#include "src/core_uart.h"
#include "src/core_twi.h"
#include "src/app_lcd_i2c.h"
#include "src/app_led_pwm.h"
#include "src/app_switch.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define APP_ALL_RUN_INTERVAL		10

#define APP_LED_RUN_COUNT			( APP_LED_PWM_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )
#define APP_LCD_I2C_RUN_COUNT		( APP_LCD_I2C_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )
#define APP_SWITCH_RUN_COUNT		( APP_SWITCH_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static uint16	tick;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void setup( )
{
	DEFINE_UART( 0, 115200, 128, 128, UART_LINE_ENDING_NONE );
	twiInit( CONFIG_I2C_FREQ );
	lcdInit( );
	ledPwmInit( );
	switchInit( );
	
	lcdSetText( 0, "Press any key", 16 );
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void loop( )
{
	if( tick % APP_LED_RUN_COUNT == 0 )
		ledPwmProcess( );
		
	if( tick % APP_LCD_I2C_RUN_COUNT == 0 )
		lcdProcess( );
	
	if( tick % APP_SWITCH_RUN_COUNT == 0 )
	{
		if( switchProcess( ) )
		{
			// valid only when switchProcess( ) returns true
			uint8 state[APP_SWITCH_COUNT];
			switchGetState( state );		
			
			char line[2][17];
			memset( line, ' ', sizeof( line ) );
			char buf[16];

			for( int i = 0; i < APP_SWITCH_COUNT; i++ )
			{
				char* ptr = &line[i>>1][(i&1)*8];
				int len;
				
				if( state[i] == SW_STATE_LONG_PRESS )
				{
					len = sprintf( buf, "%d:long", i );
					memcpy( ptr, buf, len );
					uartPrintln( 0, buf );
				}
				else if( state[i] == SW_STATE_SHORT_PRESS )
				{
					len = sprintf( buf, "%d:short", i );
					memcpy( ptr, buf, len );
					uartPrintln( 0, buf );
				}
				else
				{
					len = sprintf( buf, "%d:none", i );
					memcpy( ptr, buf, len );
				}
			}
			
			lcdSetText( 0, line[0], 16 );
			lcdSetText( 1, line[1], 16 );
		}
	}
	
	delay( APP_ALL_RUN_INTERVAL );
	
	tick++;
}

#endif
