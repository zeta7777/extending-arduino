
/********************************************************************
		  (c) Copyright 2018 DesignIt Co. Ltd.

		  All rights reserved.

	FILE NAME:			cmd.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "blockdev.h"
#include "utils.h"
#include "cmd.h"

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
#define DELIM     			" \t"

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
typedef int (*CMD_FNT)( uint8 srcId );

typedef struct __cmd_desc
{
	const char* name;
	CMD_FNT		fnt;
	const char* help;
} CMD_DESC;

///////////////////////////////////////////////////////////////////////
//	cmd function
///////////////////////////////////////////////////////////////////////
static int cmd_help( uint8 srcId );
static int cmd_rdi( uint8 srcId );
static int cmd_rdd( uint8 srcId );
static int cmd_wrd( uint8 srcId );
static int cmd_wrap( uint8 srcId );
static int cmd_wrsta( uint8 srcId );
static int cmd_wifi( uint8 srcId );
static int cmd_rdset( uint8 srcId );
static int cmd_strm( uint8 srcId );

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
static CMD_DESC* getCmdDesc( char* name );

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
static const CMD_DESC CmdDesc[] =
{
	{ "help", 	cmd_help,		"help"									},
	{ "rdi",	cmd_rdi,		"rdi [0xADDR]"							},
	{ "rdd", 	cmd_rdd,		"rdd [0xADDR]"							},
	{ "wrd",	cmd_wrd,		"wrd [0xADDR] [0xDATA]"					},
	{ "wrap",	cmd_wrap,		"wrap [ssid] [passwd] [ip] [port]"		},
	{ "wrsta",	cmd_wrsta,		"wrsta [ssid] [passwd] [port]"			},
	{ "wifi",	cmd_wifi,		"wifi [sta/ap]"							},
	{ "rdset",	cmd_rdset,		"rdset"									},
	{ "strm",	cmd_strm,		"strm [0/1]"							},
	{ NULL,		},
};

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
static CMD_SEND_RESP	cmdSendResp[CMD_SRC_ID_COUNT];
static volatile bool	cmdStrmOn;

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
#define MEM_READ8( addr )        ( *(volatile unsigned char*)( addr ) )

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
void cmdInit( CMD_SEND_RESP respSend, uint8 srcId )
{
	if( srcId >= CMD_SRC_ID_COUNT )
		return;
	
	cmdSendResp[srcId] = respSend;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
void cmdDo( char* cmd, uint8 srcId )
{
	if( srcId >= CMD_SRC_ID_COUNT )
		return;
		
    char* tok = strtok( cmd, DELIM );
    if( tok == 0 || *tok == 0 )
    	return;
    	
    CMD_DESC* cmdDesc = getCmdDesc( tok );
    
    if( cmdDesc == NULL )
    {
    	cmdSendResp[srcId]( "! Invalid command" );
    	return;
    }
    
	int ret = cmdDesc->fnt( srcId );
	
	if( ret < 0 )
    	cmdSendResp[srcId]( "! Error" );
	else if( ret == 0 )
		cmdSendResp[srcId]( "! Ok" );
}

bool cmdGetStrmOn( )
{
	return cmdStrmOn;
}

void cmdSetStrmOn( bool on )
{
	cmdStrmOn = on;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
static int cmd_help( uint8 srcId )
{
	const CMD_DESC* cmdDesc = CmdDesc;
	for( ; cmdDesc->name; cmdDesc++ )
	{
		cmdSendResp[srcId]( (char*)cmdDesc->help );
	}
	
	return 0;
}

// read from instruction memory
static int cmd_rdi( uint8 srcId )
{
	return 0;
}

// read from data memory
static int cmd_rdd( uint8 srcId )
{
	uint16 addr = 0;

	char* addrStr = getNextToken( DELIM );
	if( addrStr )
	{
		char* endptr;
		addr = strtol( addrStr, &endptr, 16 );
		if( endptr && *endptr )
			return -1;
	}
		
	// fixme : check data memory range
	char str[128];
	sprintf( str, "%0.4x : %0.2x", addr, MEM_READ8( addr ) );
	cmdSendResp[srcId]( str );
	
	return 0;
}

// write to data memory
static int cmd_wrd( uint8 srcId )
{
	cmdSendResp[srcId]( "not impl");
	return 0;
}

// write ap info to block dev
static int cmd_wrap( uint8 srcId )
{
	char* ssid		= getNextToken( DELIM );
	char* passwd	= getNextToken( DELIM );
	char* ip		= getNextToken( DELIM );
	char* port		= getNextToken( DELIM );
	
	if( !ssid || !passwd || !ip || !port )
	{
		cmdSendResp[srcId]( "invalid argument" );
		return -1;
	}

	AP_INFO apInfo;
	memset( &apInfo, 0, sizeof( AP_INFO ) );
		
	strncpy( (char*)apInfo.ssid, ssid, CONFIG_WIFI_SSID_LEN-1 );
	strncpy( (char*)apInfo.password, passwd, CONFIG_WIFI_PASSWORD_LEN-1 );
	parseIP( ip, apInfo.ip );
	apInfo.port = atoi( port );
	
	int ret = blockWrite( BLOCK_NO_AP_INFO, (uint8*)&apInfo, sizeof( AP_INFO ) );
	if( ret != BLOCK_ERR_NONE )
	{
		char resp[64];
		sprintf( resp, "error : blockWrite : %d", ret );
		cmdSendResp[srcId]( resp );
		return -1;
	}
	
	return 0;
}

// write sta info to block dev
static int cmd_wrsta( uint8 srcId )
{
	char* ssid = getNextToken( DELIM );
	char* passwd = getNextToken( DELIM );
	char* port = getNextToken( DELIM );
	
	if( !ssid || !passwd || !port )
	{
		cmdSendResp[srcId]( "invalid argument" );
		return -1;
	}

	STA_INFO staInfo;
	memset( &staInfo, 0, sizeof( STA_INFO ) );
		
	strncpy( (char*)staInfo.ssid, ssid, CONFIG_WIFI_SSID_LEN - 1 );
	strncpy( (char*)staInfo.password, passwd, CONFIG_WIFI_PASSWORD_LEN - 1 );
	staInfo.port = atoi( port );
	staInfo.dhcp = 1;
		
	int ret = blockWrite( BLOCK_NO_STA_INFO, (uint8*)&staInfo, sizeof( STA_INFO ) );
	if( ret != BLOCK_ERR_NONE )
	{
		char resp[64];
		sprintf( resp, "error : blockWrite : %d", ret );
		cmdSendResp[srcId]( resp );
		return -1;
	}

	return 0;
}

// wifi setting : sta or ap mode
static int cmd_wifi( uint8 srcId )
{
	SETTINGS set;
	memset( &set, 0, sizeof( SETTINGS ) );
	
	char* arg = getNextToken( DELIM );
	
	if( strcmp( arg, "sta" ) == 0 )
		set.wifiMode = SETTING_WIFI_MODE_STA;
	else if( strcmp( arg, "ap" ) == 0 )
		set.wifiMode = SETTING_WIFI_MODE_AP;
	else
		return -1;	
		
	int ret = blockWrite( BLOCK_NO_SETTINGS, (uint8*)&set, sizeof( SETTINGS ) );
	if( ret != BLOCK_ERR_NONE )
	{
		char resp[64];
		sprintf( resp, "error : blockWrite : %d", ret );
		cmdSendResp[srcId]( resp );
		return -1;	
	}

	return 0;	
}

static int cmd_rdset( uint8 srcId )
{
	SETTINGS set;
	int ret = blockRead( BLOCK_NO_SETTINGS, (uint8*)&set, sizeof( SETTINGS ) );
	if( ret != BLOCK_ERR_NONE )
		goto err;
		
	char* mode;
	if( set.wifiMode == SETTING_WIFI_MODE_AP )
		mode = "AP";
	else if( set.wifiMode == SETTING_WIFI_MODE_STA )
		mode = "STA";
	else
		mode = "NONE";
		
	char resp[64];
		
	sprintf( resp, "-------- General ----------" );
	cmdSendResp[srcId]( resp );
	sprintf( resp, "wifi mode : %s", mode );
	cmdSendResp[srcId]( resp );
		
	//
	//	ap info
	//
	AP_INFO apInfo;
	ret = blockRead( BLOCK_NO_AP_INFO, (uint8*)&apInfo, sizeof( AP_INFO ) );
	if( ret != BLOCK_ERR_NONE )
		goto err;
			
	sprintf( resp, "-------- AP info ----------" );
	cmdSendResp[srcId]( resp );
	sprintf( resp, "ssid : %s", apInfo.ssid );
	cmdSendResp[srcId]( resp );
	sprintf( resp, "passwd : %s", apInfo.password );
	cmdSendResp[srcId]( resp );
	sprintf( resp, "ip : %d.%d.%d.%d", apInfo.ip[0], apInfo.ip[1], apInfo.ip[2], apInfo.ip[3] );
	cmdSendResp[srcId]( resp );
	sprintf( resp, "port : %d", apInfo.port );
	cmdSendResp[srcId]( resp );
	
	//
	//	sta info
	//
	STA_INFO staInfo;
	ret = blockRead( BLOCK_NO_STA_INFO, (uint8*)&staInfo, sizeof( STA_INFO ) );
	if( ret != BLOCK_ERR_NONE )
		goto err;
	
	sprintf( resp, "-------- STA info ----------" );
	cmdSendResp[srcId]( resp );
	sprintf( resp, "ssid : %s", staInfo.ssid );
	cmdSendResp[srcId]( resp );
	sprintf( resp, "passwd : %s", staInfo.password );
	cmdSendResp[srcId]( resp );
	sprintf( resp, "port : %d", staInfo.port );
	cmdSendResp[srcId]( resp );

	return 0;
	
err:
	sprintf( resp, "erorr : blockRead : %d", ret );
	cmdSendResp[srcId]( resp );
	return -1;
}

static int cmd_strm( uint8 srcId )
{
	char* arg = getNextToken( DELIM );
	if( arg )
	{
		if( *arg == '0' )
		{
			cmdStrmOn = false;
			cmdSendResp[srcId]( "stream off" );
		}
		else if( *arg == '1' )
		{
			cmdStrmOn = true;
			cmdSendResp[srcId]( "stream on" );
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
static CMD_DESC* getCmdDesc( char* cmd )
{
	CMD_DESC* cmdDesc = (CMD_DESC*)CmdDesc;
	
	for( ; cmdDesc->fnt; cmdDesc++ )
	{
		if( strcmp( cmd, cmdDesc->name ) == 0 )
		return cmdDesc;
	}
	
	return NULL;
}