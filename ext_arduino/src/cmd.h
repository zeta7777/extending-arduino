/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			cmd.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/


#ifndef __CMD_H
#define __CMD_H

#include "types.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
enum {
	CMD_SRC_ID0,
	CMD_SRC_ID1,
	CMD_SRC_ID_COUNT
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
typedef void (*CMD_SEND_RESP)( char* str );

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void cmdInit( CMD_SEND_RESP sendResp, uint8 srcId );
void cmdDo( char* cmd, uint8 srcId );
bool cmdGetStrmOn( );
void cmdSetStrmOn( bool on );

#endif	// __APP_CLI_H
