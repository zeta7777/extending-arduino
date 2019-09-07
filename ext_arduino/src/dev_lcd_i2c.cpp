/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			dev_lcd_i2c.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#include <stdio.h>

#include "system.h"
#include "core_twi.h"
#include "core_uart.h"
#include "dev_lcd_i2c.h"

/////////////////////////////////////////////////////////////////////
//	I2C clock frequency 100KHz corresponds to 10 usec
//
//  one twiWrite( ) takes at least ( 8 + 1 + 8 + 1 ) = 18 SCL clock
//  10usec x 18 = 180 usec which is added before exp_write( ) complete
//  so, delay which requires less than 180usec is not needed
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define LOG_LCD_I2C				1

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

// clear display
#define LCD_CLEAR_DISPLAY		0x01

// cursor return home
#define LCD_RETURN_HOME			0x02

// entry mode
#define LCD_ENTRY_MODE_SET		0x04

	#define ENTRY_MODE_INC			0x02		// 1 : inc, 0 : dec
	#define ENTRY_MODE_DEC			0x00

	#define ENTRY_MODE_SHIFT		0x01		// bit 0
	#define ENTRY_MODE_NO_SHIFT		0x00

// display on/off
#define LCD_DISPLAY				0x08

	#define DISPLAY_ON				0x04		// 1 : on, 0 : off
	#define CURSOR_ON				0x02		// 1 : on, 0 : off
	#define BLINKING_ON				0x01		// 1 : on, 0 : off
	
#define LCD_SHIFT				0x10
	
	#define DISPLAY_SHIFT			0x08		// 1 : display shift, 0 : cursor shift
	#define SHIFT_RIGHT				0x04		// 1 : right, 0 : left

// function set
#define LCD_FUNCTION_SET		0x20

	// interface
	#define INF_8BIT				0x10
	#define INF_4BIT				0x00

	// # of lines, font size
	#define LINES_2					0x08
	#define LINES_1					0x00

	#define FONT_5x10				0x04		// cannot be used with 2-line
	#define FONT_5x8				0x00

// set CGRAM address ( + 6-bit address )
// ( NOT IMPL )
#define LCD_SET_CGRAM_ADDR		0x40

// set DDRAM address ( + 7-bit address )
#define LCD_SET_DDRAM_ADDR		0x80


//
// control signal
// PCF8574 to HD44780D connection
//
// P[7:4] -> D[7:4]
//
#define RS						0x01	// P0 : 1 (data), 0 (control register)
#define RW						0x02	// P1 : 1 (Read), 0 (Write)
#define EN						0x04	// P2 : latch data during ( 1 -> 0 )
#define BACKLIGHT				0x08	// P3 : on/off back-light

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
static uint8	lcdAddress;
static uint8	lcdCols;
static uint8	lcdRows;
static uint8	lcdBacklight;
static uint8	lcdDisplay;		// display status : display, cursor, blinking
 
/////////////////////////////////////////////////////////////////////
//	write to expander ( PCF8574 )
/////////////////////////////////////////////////////////////////////
static uint8 exp_write( uint8 data )
{
	 // 
	 // expander's P[3] directly controls back-light on/off
	 // whenever writing to expander, we have to set/reset P3 value
	 //
	 data |= lcdBacklight;
	 
	 if( twiWrite( lcdAddress, &data, 1 ) != 1 )
	 {
		 #if LOG_LCD_I2C==1
	     uartPrintf( 0, "! error : exp_write : %d\r\n", twiGetError( ) );
		 #endif
		 
		 return 1;
	 }
	 
	 return 0;
 }

/////////////////////////////////////////////////////////////////////
//  EN in val should be 0
/////////////////////////////////////////////////////////////////////  
static uint8 lcd_write4( uint8 val )
{
	exp_write( val | EN );		// EN high, setup > 450 nsec
	exp_write( val );			// EN low, hold > 37 usec

	return 0;
}

/////////////////////////////////////////////////////////////////////
//  used only after 4-bit interface is established
/////////////////////////////////////////////////////////////////////
static void lcd_command( uint8 val ) 
{
	lcd_write4( val & 0xf0 );				// RS 0, RW 0, EN 0
	lcd_write4( val << 4 );
}

static void lcd_data( uint8 val )
{
	lcd_write4( ( val & 0xf0 ) | RS );		// RS 1, RW 0, EN 0
	lcd_write4( ( val << 4 ) | RS );
}

/////////////////////////////////////////////////////////////////////
//	twiInit( ) should be called before lcdInit( )
/////////////////////////////////////////////////////////////////////
void lcdInit( uint8 addr, uint8 rows, uint8 cols )
{
	lcdAddress	= addr;			// I2C address
	lcdRows		= rows;			// Y
	lcdCols		= cols;			// X	
		
	// power-on reset value of expander is high
	lcdBacklight = BACKLIGHT;
		
	// clear expander output
	exp_write( 0 );

	// wait for settle
	delay( 100 );
	
	//
	// see HD44780U Fig-24
	// before 4-bit interface has been established, should use lcd_write4( )
	//
	lcd_write4( LCD_FUNCTION_SET | INF_8BIT );
	delay( 5 );
	
	// 
	lcd_write4( LCD_FUNCTION_SET | INF_8BIT );
	
	//
	lcd_write4( LCD_FUNCTION_SET | INF_8BIT );
	
	// now, set to 4-bit interface
	lcd_write4( LCD_FUNCTION_SET | INF_4BIT );

	//
	// from now on, we can use lcd_command, lcd_data
	//
	
	// # of lines, font size
	// cannot be changed after setting
	uint8 set	= LCD_FUNCTION_SET | FONT_5x8 | LINES_1;
	if( rows > 1 )
		set |= LINES_2;
	
	lcd_command( set );

	lcdDisplay = LCD_DISPLAY;
		
	// display off
	lcdEnableDisplay( false );
	
	// display clear
	lcdClear( );
	
	// set the entry mode
	lcd_command( LCD_ENTRY_MODE_SET | ENTRY_MODE_INC | ENTRY_MODE_NO_SHIFT );
	
	lcdSetCursor( 0, 0 );
	
	lcdEnableDisplay( true );
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void lcdEnableBacklight( bool on )
{
	lcdBacklight = on ? BACKLIGHT : 0;
	exp_write( 0 );
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void lcdEnableDisplay( bool on )
{
	if( on )
		lcdDisplay |= DISPLAY_ON;
	else
		lcdDisplay &= ~DISPLAY_ON;
		
	lcd_command( lcdDisplay );
}

void lcdShowCursor( bool on )
{
	if( on )
		lcdDisplay |= CURSOR_ON;
	else 
		lcdDisplay &= ~CURSOR_ON;
		
	lcd_command( lcdDisplay );
}

void lcdEnableBlinking( bool en )
{
	if( en )
		lcdDisplay |= BLINKING_ON;
	else
		lcdDisplay &= ~BLINKING_ON;
		
	lcd_command( lcdDisplay );
}

void lcdShift( bool display, bool right )
{
	uint8 cmd = LCD_SHIFT;
	
	if( display )
		cmd |= DISPLAY_SHIFT;
	
	if( right )
		cmd |= SHIFT_RIGHT;
		
	lcd_command( cmd );
}
/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void lcdClear( )
{
	lcd_command( LCD_CLEAR_DISPLAY );
	delay( 2 );
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void lcdSetCursor( uint8 x, uint8 y )
{
	if( y >= lcdRows || x >= lcdCols )
		return;
		
	// display data RAM start address for each rows
	int row_offsets[] = { 0, 0x40, 0 };

	lcd_command( LCD_SET_DDRAM_ADDR | (x + row_offsets[y]));
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void lcdHome( )
{
	lcd_command( LCD_RETURN_HOME );
	delay( 2 );
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void lcdPrint( const char* str )
{
	while( *str )
		lcd_data( *str++ );
}

