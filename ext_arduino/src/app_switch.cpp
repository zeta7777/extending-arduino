/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			app_switch.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#include "core_io.h"
#include "core_uart.h"
#include "app_switch.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define LONG_PRESS_TIME      	1500		// ms
#define SHORT_PRESS_TIME     	100

#define LONG_PRESS_COUNT       	( LONG_PRESS_TIME / APP_SWITCH_RUN_INTERVAL )
#define SHORT_PRESS_COUNT		( SHORT_PRESS_TIME / APP_SWITCH_RUN_INTERVAL )

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static const uint16	swPort[APP_SWITCH_COUNT] = { PORT_SW0, PORT_SW1, PORT_SW2, PORT_SW3 };

static uint8 swState[APP_SWITCH_COUNT];
static uint8 swCount[APP_SWITCH_COUNT];	

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void switchInit( )
{
	for( int i = 0; i < APP_SWITCH_COUNT; i++ )
		ioSetMode( swPort[i], IO_MODE_INPUT_PULLUP );
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
bool switchProcess( )
{
	// true when some button pressed, released
	bool updated = false;
	
	for( uint8 i = 0; i < APP_SWITCH_COUNT; i++ )
	{
		switch( swState[i] )
		{
		case SW_STATE_NONE:
			if( ioRead( swPort[i] ) == IO_LOW )
			{
				swState[i] = SW_STATE_PRESSED;
				swCount[i] = 0;
			}
			break;
		
		case SW_STATE_PRESSED:
			if( ioRead( swPort[i] ) == IO_LOW )
			{
				if( ++swCount[i] > LONG_PRESS_COUNT )
				{
					swState[i] = SW_STATE_LONG_PRESS;
					updated = true;
				}
			}
			else
				swState[i] = SW_STATE_RELEASED;
			break;
			
		case SW_STATE_RELEASED:
			if( swCount[i] > SHORT_PRESS_COUNT )
			{
				swState[i] = SW_STATE_SHORT_PRESS;
				updated = true;
			}
			else 
				swState[i] = SW_STATE_NONE;
			break;
			
		case SW_STATE_SHORT_PRESS:
			swState[i] = SW_STATE_NONE;
			break;
			
		case SW_STATE_LONG_PRESS:
			if( ioRead( swPort[i] ) == IO_HIGH )
				swState[i] = SW_STATE_NONE;
			break;
			
		default:
			break;
		}
	}
	
	return updated;
}

//////////////////////////////////////////////////////////////////////
//
//	return states are valid only when switchPress( ) returns true
//
//////////////////////////////////////////////////////////////////////
void switchGetState( uint8 state[ APP_SWITCH_COUNT ] )
{
	for( int i = 0; i < APP_SWITCH_COUNT; i++ )
	{
		state[i] = swState[i];
	}
}

