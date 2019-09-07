/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			exam_pms5003.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include "exam_config.h"

#if EXAM_PMS5003

#include <stdio.h>
#include <string.h>
#include "src/config.h"
#include "src/system.h"
#include "src/core_uart.h"
#include "src/core_twi.h"
#include "src/app_lcd_i2c.h"
#include "src/app_led_pwm.h"
#include "src/app_switch.h"
#include "src/app_pms5003.h"

#include "src/test.cpp"
#include "src/app_sound.h"

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define APP_ALL_RUN_INTERVAL		10

#define APP_LED_RUN_COUNT			( APP_LED_PWM_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )
#define APP_LCD_I2C_RUN_COUNT		( APP_LCD_I2C_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )
#define APP_SWITCH_RUN_COUNT		( APP_SWITCH_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )
#define APP_PMS5003_RUN_COUNT		( APP_PMS5003_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
static void switchEvent( );
static void pms5003Event( );
static void updateLCD( );

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
static uint16 tick;

enum
{
	LCD_PAGE_INTRO	= 0,
	LCD_PAGE_PMS5003,
	LCD_PAGE_PMS5003_1,
	LCD_PAGE_SWITCH,
	LCD_PAGE_COUNT
};

static uint8 lcdPage = LCD_PAGE_INTRO;

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void setup( )
{
	DEFINE_UART( PMS5003_UART, PMS5003_BAUD, 128, 128, UART_LINE_ENDING_NONE );
	DEFINE_UART( 0, CONFIG_UART0_BAUD, CONFIG_UART0_RX_BUF_SIZE, CONFIG_UART0_TX_BUF_SIZE, UART_LINE_ENDING_NONE );
	
	twiInit( CONFIG_I2C_FREQ );
	lcdInit( );
	ledPwmInit( );
	switchInit( );
	soundInit( );
	pms5003Init( PMS5003_UART, 0 );
	
	updateLCD( );
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void loop( )
{
	if( tick % APP_LED_RUN_COUNT == 0 )
		ledPwmProcess( );
	
	if( tick % APP_LCD_I2C_RUN_COUNT == 0 )
		lcdProcess( );
	
	if( tick % APP_SWITCH_RUN_COUNT == 0 )
	{
		if( switchProcess( ) )
			switchEvent( );
	}
	
	if( tick % APP_PMS5003_RUN_COUNT == 0 )
	{
		if( pms5003Process( ) )
			pms5003Event( );
	}
	
	delay( APP_ALL_RUN_INTERVAL );
	
	tick++;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
static void switchEvent( )
{
	bool update = ( lcdPage == LCD_PAGE_SWITCH );
	
	// valid only when switchProcess( ) returns true
	uint8 state[APP_SWITCH_COUNT];
	switchGetState( state );

	for( int i = 0; i < APP_SWITCH_COUNT; i++ )
	{
		if( state[i] == SW_STATE_LONG_PRESS )
		{
		}
		else if( state[i] == SW_STATE_SHORT_PRESS )
		{
			if( i == 0 )
			{
				if( ++lcdPage >= LCD_PAGE_COUNT )
					lcdPage = LCD_PAGE_INTRO;
					
				update = true;
			}
			else if( i == 1 )
				soundStart( &test_info );
		}
	}
	
	if( update )
		updateLCD( );
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
static void pms5003Event( )
{
	if( lcdPage == LCD_PAGE_PMS5003 || lcdPage == LCD_PAGE_PMS5003_1 )
		updateLCD( );
}

/////////////////////////////////////////////////////////////////////
//
//	SW0	-> change LCD page
//  SW1 -> sound
//
/////////////////////////////////////////////////////////////////////
static void updateLCD( )
{
	int len;
	char buf[16];
	char line[2][17];
	memset( line, ' ', sizeof( line ) );
	
	if( lcdPage == LCD_PAGE_INTRO )
	{
		len = sprintf( buf, "Hello World!" );
		memcpy( line[0], buf, len );
	}
	else if( lcdPage == LCD_PAGE_PMS5003 )
	{
		uint16 data[ PMS5003_DATA_COUNT ];
		pms5003GetData( data );
		
		len = sprintf( buf, "P1.0:%5d", data[4] );
		memcpy( line[0], buf, len );
		len = sprintf( buf, "P2.5:%5d", data[5] );
		memcpy( line[1], buf, len );
	}
	else if( lcdPage == LCD_PAGE_PMS5003_1 )
	{
		uint16 data[ PMS5003_DATA_COUNT ];
		pms5003GetData( data );
		
		len = sprintf( buf, "P10:%5d", data[6] );
		memcpy( line[0], buf, len );
	}
	else if( lcdPage == LCD_PAGE_SWITCH )
	{
		uint8 state[APP_SWITCH_COUNT];
		switchGetState( state );
		
		for( int i = 0; i < APP_SWITCH_COUNT; i++ )
		{
			char* ptr = &line[i>>1][(i&1)*8];
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
	}
	
	lcdSetText( 0, line[0], 16 );
	lcdSetText( 1, line[1], 16 );
}

#endif		// EXAM_PMS5003
