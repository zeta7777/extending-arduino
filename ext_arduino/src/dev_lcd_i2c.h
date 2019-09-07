
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			dev_lcd_i2c.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#ifndef __LCD_I2C_H
#define __LCD_I2C_H

#include "types.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define LCD_I2C_ADDR					0x27
#define LCD_I2C_ROW_COUNT				2
#define LCD_I2C_COL_COUNT				16

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void lcdInit( uint8 addr, uint8 rows, uint8 cols );

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void lcdEnableBacklight( bool on );
void lcdEnableDisplay( bool on );
void lcdShowCursor( bool on );
void lcdShift( bool display, bool right );
void lcdEnableBlinking( bool en );
void lcdSetCursor( uint8 x, uint8 y );
void lcdClear( );
void lcdHome( );
void lcdPrint( const char* str );

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

#endif		// __LCD_I2C_H