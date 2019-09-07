/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			exam_oled.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#include "exam_config.h"

#if EXAM_OLED

#include "src/config.h"
#include "src/system.h"
#include "src/twi.h"
#include "src/oled_i2c.h"
#include "src/lcd_i2c.h"
#include <stdio.h>


void setup( )
{
	twiInit( CFG_I2C_FREQ );
	oledInit( );
	
	
	lcdInit( 0x27, 16, 2 );
	lcdSetCursor( 0, 0 );
	lcdClear( );
	lcdPrint( "DESIGN-AIR" );
	lcdEnableDisplay( true );
}

void loop( )
{
	static int k = 0;
	char buf[32];
	
	oledStart( );
next:
	
	sprintf( buf, "%d OLED TEST", k );
	oledPrint( 0, 0, buf );
	
	if( oledNextPage( ) )
			goto next;
			
	lcdClear( );
	lcdSetCursor( 0, 0 );
	sprintf( buf, "%d", k++ );
	lcdPrint( buf );
	lcdSetCursor( 0, 1 );
	lcdPrint( "LCD TEST" );
	
	delay( 1000 );
}

#endif		// EXAM_OLED