/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			app_cli.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/


#ifndef __APP_CLI_H
#define __APP_CLI_H

#include "types.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define APP_CLI_RUN_INTERVAL			100
#define CLI_UART						0

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void cliInit( uint8 uartNo, bool echoOn );
void cliProcess( );


#endif	// __APP_CLI_H
