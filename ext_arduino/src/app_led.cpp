
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			app_led.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include "core_io.h"
#include "app_led.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define IO_LED1				IO_PB5
#define IO_LED2				IO_PB4
#define IO_LED3				IO_PH6
#define IO_LED4				IO_PH5
#define IO_LED5				IO_PH4
#define IO_LED6				IO_PH3
#define IO_LED7				IO_PE3
#define IO_LED8				IO_PE5

#define LED_COUNT			8

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static uint16 ioPort[LED_COUNT] =
{
	IO_LED1, IO_LED2, IO_LED3, IO_LED4,
	IO_LED5, IO_LED6, IO_LED7, IO_LED7
};

static uint8 ledVal = 0;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void ledInit( )
{
	for( int i = 0; i < LED_COUNT; i++ )
		ioSetMode( ioPort[i], IO_MODE_OUTPUT_LOW );
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void ledProcess( )
{
	ledPut( ledVal++ );
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void ledPut( uint8 val )
{
	for( int i = 0; i < LED_COUNT; i++ )
	{
		ioWrite( ioPort[i], val & 1 );
		val >>= 1;
	}
}

