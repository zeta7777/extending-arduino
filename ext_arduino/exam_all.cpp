/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			exam_all.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include "exam_config.h"

#if EXAM_ALL

#include "src/config.h"
#include "src/system.h"
#include "src/uart.h"
#include "src/twi.h"

#include "src/app_led.h"
#include "src/app_cli.h"
#include "src/app_switch.h"
#include "src/app_pms5003.h"
#include "src/app_lcd_i2c.h"

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define APP_ALL_RUN_INTERVAL		10

#define APP_LED_RUN_COUNT			( APP_LED_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )
#define APP_CLI_RUN_COUNT			( APP_CLI_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )
#define APP_PMS5003_RUN_COUNT		( APP_PMS5003_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )
#define APP_SWITCH_RUN_COUNT		( APP_SWITCH_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )
#define APP_LCD_I2C_RUN_COUNT		( APP_LCD_I2C_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define UART_CONSOLE				0
#define UART_PMS5003				2
/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
static uint16	tick;

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void setup( )
{
	// core device init
	DEFINE_UART( UART_CONSOLE, CONFIG_UART0_BAUD, CONFIG_UART0_RX_BUF_SIZE, CONFIG_UART0_TX_BUF_SIZE, UART_LINE_ENDING_CR_NL );
	DEFINE_UART( UART_PMS5003, PMS5003_BAUD, 128, 128, UART_LINE_ENDING_NONE );
	
	twiInit( CONFIG_I2C_FREQ );
	
	// app init
	ledInit( LED_MODE_IO );
	switchInit( );
	cliInit( UART_CONSOLE, true );
	pms5003Init( UART_PMS5003, UART_CONSOLE );
	lcdInit( LCD_I2C_ADDR, LCD_I2C_ROW_COUNT, LCD_I2C_COL_COUNT );
}


/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void loop( )
{
	if( tick % APP_LED_RUN_COUNT == 0 )
		ledProcess( );
		
	if( tick % APP_CLI_RUN_COUNT == 0 )
		cliProcess( );
		
	if( tick % APP_SWITCH_RUN_COUNT == 0 )
		switchProcess( );
		
	if( tick % APP_PMS5003_RUN_COUNT == 0 )
		pms5003Process( );
	
	if( tick % APP_LCD_I2C_RUN_COUNT == 0 )
		lcdProcess( );
	
	delay( APP_ALL_RUN_INTERVAL );
	tick++;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#endif		// EXAM_ALL
