
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			app_lcd_i2c.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include <stdio.h>
#include <string.h>
#include "dev_lcd_i2c.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static char		lcdBuf[LCD_I2C_ROW_COUNT][LCD_I2C_COL_COUNT+1];
static bool		lcdUpdate = false;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void lcdInit( )
{
	lcdInit( LCD_I2C_ADDR, LCD_I2C_ROW_COUNT, LCD_I2C_COL_COUNT );
	memset( lcdBuf, 0, sizeof( lcdBuf ) );
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void lcdProcess( )
{
	if( !lcdUpdate )
		return;
				
	lcdClear( );
	
	for( int i = 0; i < LCD_I2C_ROW_COUNT; i++ )
	{
		lcdSetCursor( 0, i );
		lcdPrint( lcdBuf[i] );
	}
	
	lcdUpdate = false;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void lcdSetText( uint8 lineNo, const char* text, int len )
{
	if( lineNo >= LCD_I2C_ROW_COUNT )
		return;
	
	int k = ( len > LCD_I2C_COL_COUNT ) ? LCD_I2C_COL_COUNT : len;
	
	memcpy( lcdBuf[lineNo], text, k );
	
	lcdBuf[lineNo][k] = 0;
	
	lcdUpdate = true;
}