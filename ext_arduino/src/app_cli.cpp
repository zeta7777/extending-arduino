
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			app_cli.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "core_uart.h"
#include "utils.h"
#include "blockdev.h"
#include "cmd.h"
#include "app_cli.h"

// FIXME : character-wise echo on

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
#define CLI_LINE_BUF_LEN			128

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
static int getLine( uint8* buf, int bufLen );
static void uartSendResp( char* resp );

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
static uint8 	cliUartNo;
static bool	 	cliEchoOn;

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
void cliInit( uint8 uartNo, bool echoOn )
{
	cmdInit( uartSendResp, CMD_SRC_ID0 );
	
	cliUartNo = uartNo;
	cliEchoOn = echoOn;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
void cliProcess( )
{
    uint8 lineBuf[CLI_LINE_BUF_LEN];
       
    if( getLine( lineBuf, CLI_LINE_BUF_LEN-1 ) == 0 )
    	return;
		
	cmdDo( (char*)lineBuf, CMD_SRC_ID0 );
}

//////////////////////////////////////////////////////////////////////
//	return : # of bytes in command line
//////////////////////////////////////////////////////////////////////
static int getLine( uint8* buf, int bufLen )
{
	if( !uartIsLineAvailable( cliUartNo ) )
		return 0;
	
	// not include line-ending chars
	int count = uartGetLine( cliUartNo, buf, bufLen );
	
	if( cliEchoOn )
		uartPrintf( cliUartNo, "%s (%d bytes)\r\n", buf, count );
	
	return count;
}

static void uartSendResp( char* resp )
{
	uartPrintln( cliUartNo, resp );
}
