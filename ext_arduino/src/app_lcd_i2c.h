
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			app_lcd_i2c.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#ifndef __APP_LCD_I2C_H
#define __APP_LCD_I2C_H

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define APP_LCD_I2C_RUN_INTERVAL		1000

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void lcdInit( );
void lcdProcess( );
void lcdSetText( uint8 lineNo, const char* text, int len );


#endif	// __APP_LCD_I2C_H