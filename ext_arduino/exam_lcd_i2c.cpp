
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			exam_lcd_i2c.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include "exam_config.h"

#if EXAM_LCD_I2C

#include <stdio.h>
#include <string.h>
#include "src/config.h"
#include "src/system.h"

#include "src/core_uart.h"
#include "src/core_twi.h"
#include "src/dev_lcd_i2c.h"
#include "src/app_led_pwm.h"
#include "src/app_lcd_i2c.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define APP_ALL_RUN_INTERVAL		10

#define APP_LED_RUN_COUNT			( APP_LED_PWM_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )
#define APP_LCD_I2C_RUN_COUNT		( APP_LCD_I2C_RUN_INTERVAL / APP_ALL_RUN_INTERVAL )

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static void demoLCD( );

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
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void loop( )
{
	if( tick % APP_LED_RUN_COUNT == 0 )
		ledPwmProcess( );
	
	//
	// in this demo we don't use lcdProcess( ) in app_lcd_i2c.cpp
	//
	if( tick % APP_LCD_I2C_RUN_COUNT == 0 )	
	{
		// lcdProcess( );
		demoLCD( );
	}

	delay( APP_ALL_RUN_INTERVAL );
	
	tick++;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
#define DEMO_TIME	6

enum
{
	 DEMO_DISP_START = 0,
	 DEMO_DISP,
	 DEMO_DISP_END,
	 
	 DEMO_CURSOR_START,
	 DEMO_CURSOR,
	 DEMO_CURSOR_END,
	 
	 DEMO_BLINKING_START,
	 DEMO_BLINKING,
	 DEMO_BLINKING_END,
	 
	 DEMO_CURSOR_MOVE_START,
	 DEMO_CURSOR_MOVE,
	 DEMO_CURSOR_MOVE_END,
	 
	 DEMO_BLINKING_MOVE_START,
	 DEMO_BLINKING_MOVE,
	 DEMO_BLINKING_MOVE_END,
	 
	 DEMO_DISP_MOVE_START,
	 DEMO_DISP_MOVE,
	 DEMO_DISP_MOVE_END,
};

static int demo = DEMO_DISP_MOVE_START; 

static void demoLCD( )
{
	static int	elapsed		= 0;
	
	static bool dispOn		= false;
	static bool cursorOn	= true;
	static bool blinkingOn	= true;
	static int  cursorPos   = 0;
	
	switch( demo )
	{
	////////////////////////////////////////////////////////////////
	case DEMO_DISP_START:
		lcdSetCursor( 0, 0 );
		lcdPrint( "Display On/Off" );
		demo = DEMO_DISP;
		break;
		
	case DEMO_DISP:
		lcdEnableDisplay( dispOn );
		dispOn = !dispOn;
		if( elapsed > DEMO_TIME )
			demo = DEMO_DISP_END;
		break;
		
	case DEMO_DISP_END:
		dispOn = false;
		elapsed = 0;
		lcdClear( );
		lcdEnableDisplay( true );
		demo = DEMO_CURSOR_START;
		break;
		
	////////////////////////////////////////////////////////////////	
	case DEMO_CURSOR_START:
		lcdSetCursor( 0, 0 );
		lcdPrint( "Cursor On/Off" );
		lcdSetCursor( 0, 1 );
		demo = DEMO_CURSOR;
		break;
		
	case DEMO_CURSOR:
		lcdShowCursor( cursorOn );
		cursorOn = !cursorOn;
		if( elapsed > DEMO_TIME )
			demo = DEMO_CURSOR_END;
		break;
		
	case DEMO_CURSOR_END:
		cursorOn = true;
		elapsed = 0;
		lcdClear( );
		lcdShowCursor( false );
		demo = DEMO_BLINKING_START;
		break;
		
	////////////////////////////////////////////////////////////////
	case DEMO_BLINKING_START:
		lcdSetCursor( 0, 0 );
		lcdPrint( "Blinking" );
		lcdSetCursor( 0, 1 );
		demo = DEMO_BLINKING;
		break;
		
	case DEMO_BLINKING:
		lcdEnableBlinking( blinkingOn );
		if( elapsed > DEMO_TIME )
			demo = DEMO_BLINKING_END;
		break;
		
	case DEMO_BLINKING_END:
		elapsed = 0;
		blinkingOn = true;
		lcdClear( );
		lcdEnableBlinking( false );
		demo = DEMO_CURSOR_MOVE_START;
		break;
	
	//////////////////////////////////////////////////////////////////
	case DEMO_CURSOR_MOVE_START:
		lcdSetCursor( 0, 0 );
		lcdPrint( "Cursor Move" );
		lcdSetCursor( 0, 1 );
		lcdShowCursor( true );
		demo = DEMO_CURSOR_MOVE;
		break;
	case DEMO_CURSOR_MOVE:
		lcdSetCursor( cursorPos++, 1 );
		if( elapsed > DEMO_TIME )
			demo = DEMO_CURSOR_MOVE_END;
		break;
	case DEMO_CURSOR_MOVE_END:
		elapsed = 0;
		lcdClear( );
		lcdShowCursor( false );
		cursorPos = 0;
		demo = DEMO_BLINKING_MOVE_START;
		break;
		
	//////////////////////////////////////////////////////////////////
	case DEMO_BLINKING_MOVE_START:
		lcdSetCursor( 0, 0 );
		lcdPrint( "Blinking Move" );
		lcdSetCursor( 0, 1 );
		lcdEnableBlinking( true );
		demo = DEMO_BLINKING_MOVE;
		break;
	
	case DEMO_BLINKING_MOVE:
		lcdSetCursor( cursorPos++, 1 );
		if( elapsed > DEMO_TIME )
			demo = DEMO_BLINKING_MOVE_END;
		break;
	
	case DEMO_BLINKING_MOVE_END:
		elapsed = 0;
		lcdClear( );
		lcdEnableBlinking( false );
		cursorPos = 0;
		demo = DEMO_DISP_MOVE_START;
		break;
		
	//////////////////////////////////////////////////////////////////
	case DEMO_DISP_MOVE_START:
		lcdSetCursor( 0, 0 );
		lcdPrint( "Display Move" );
		lcdSetCursor( 0, 1 );
		lcdPrint( "Display Move" );
		demo = DEMO_DISP_MOVE;
		break;
		
	case DEMO_DISP_MOVE:
		if( elapsed < 5 )
			lcdShift( true, true );
		else if( elapsed < 10 )
			lcdShift( true, false );
			
		if( elapsed >= 10 )
			demo = DEMO_DISP_MOVE_END;
		break;
		
	case DEMO_DISP_MOVE_END:
		elapsed = 0;
		lcdClear( );
		demo = DEMO_DISP_START;
		break;
	}

	elapsed++;		
}







#endif		// EXAM_LCD_I2C