/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			app_switch.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#ifndef __APP_SWITCH_H
#define __APP_SWITCH_H

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define APP_SWITCH_RUN_INTERVAL		50

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define PORT_SW0					IO_PA0
#define PORT_SW1					IO_PA1
#define PORT_SW2					IO_PA2
#define PORT_SW3					IO_PA3

#define APP_SWITCH_COUNT			4

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define SW_STATE_NONE				0
#define SW_STATE_PRESSED			1
#define SW_STATE_RELEASED			2
#define SW_STATE_SHORT_PRESS		3
#define SW_STATE_LONG_PRESS			4

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void switchInit( );
bool switchProcess( );
void switchGetState( uint8 state[ APP_SWITCH_COUNT ] );

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

#endif		//	__APP_SWITCH_H