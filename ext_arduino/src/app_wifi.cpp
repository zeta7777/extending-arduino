/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			app_wifi.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "system.h"
#include "core_io.h"
#include "core_uart.h"
#include "dev_esp.h"
#include "blockdev.h"
#include "utils.h"
#include "cmd.h"
#include "app_wifi.h"

///////////////////////////////////////////////////////////////////////
//	default value
///////////////////////////////////////////////////////////////////////
#define DEF_AP_SSID					"Design-It"
#define DEF_AP_PASSWORD				"by-design-it"
#define DEF_AP_CHANNEL				4
#define DEF_AP_ENC					ESP_ENC_OPEN

#define DEF_SERVER_IP				"192.168.4.1"
#define DEF_SERVER_PORT				1284


////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////
#define WIFI_MODE_AP			SETTING_WIFI_MODE_AP
#define WIFI_MODE_STA			SETTING_WIFI_MODE_STA

#define SERVER_TIMEOUT			0	// (60*30)			// sec
#define WIFI_TIMEOUT			1500
#define WIFI_TIMEOUT_JOIN_AP	(20*1000)
#define WIFI_RETRY_COUNT		5
#define WIFI_RETRY_DELAY		1000

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
static bool	initCommon( );
static bool	initAP( AP_INFO* apInfo );
static bool	initSTA( STA_INFO* staInfo );
static bool	initPost( );

static void	cbProcess( uint8 id, uint16 len, char* dat );
static int  getWifiSettings( AP_INFO* apInfo, STA_INFO* staInfo );
static void processStream( );

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
static CEsp		wifiEsp;

static uint8		wifiUartNo;
static uint8		wifiLogUartNo;
static uint8		wifiMode		= WIFI_MODE_AP;
static uint8		wifiIP[4];
static uint8		wifiId;

typedef struct __wifi_stream
{
	volatile uint16*	ptr;
	bool	on;
} WIFI_STREAM;

#define WIFI_STREAM_COUNT		5

WIFI_STREAM			wifiStream[ WIFI_STREAM_COUNT ];
uint32				wifiStreamTimer;

bool wifiRegisterStream( uint8 no, uint16* pval )
{
	if( no < WIFI_STREAM_COUNT )
	{
		wifiStream[ no ].on = true;
		wifiStream[ no ].ptr = pval;
		return true;
	}
	
	return false;
}

void wifiUnregisterStream( uint8 no )
{
	if( no < WIFI_STREAM_COUNT )
		wifiStream[no].on = false;
}


///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
static bool isCmdOk( ESPRes res )
{
	if( res == ESP_RES_OK || res == ESP_RES_NO_CHANGE  )
		return true;
	
	return false;
}

static void wifiSendResp( char* resp )
{
	wifiEsp.atSend( wifiId, (uint8*)resp, strlen( resp ) );
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
bool wifiInit( uint8 uartNo, uint8 logUartNo )
{
	STA_INFO	staInfo;
	AP_INFO		apInfo;
	
	wifiUartNo = uartNo;
	wifiLogUartNo = logUartNo;
	
	cmdInit( wifiSendResp, CMD_SRC_ID1 );
	
	wifiMode = getWifiSettings( &apInfo, &staInfo );

	wifiEsp.init( uartNo );
	
	if( !initCommon( ) )
		return false;
#if 1
	if( wifiMode == WIFI_MODE_AP )
	{
		uartPrintln( wifiLogUartNo, "- init as AP mode" );
		if( !initAP(  &apInfo ) )
			return false;
	}	
	else 
	{
		uartPrintln( wifiLogUartNo, "- init as STA mode" );
		if( !initSTA( &staInfo ) )
			return false;
	}
#endif	
	return initPost( );
}

static int getWifiSettings( AP_INFO* apInfo, STA_INFO* staInfo )
{
	SETTINGS set;
	
	int ret = blockRead( BLOCK_NO_SETTINGS, (uint8*)&set, sizeof( SETTINGS ) );
	if( ret == BLOCK_ERR_NONE )
	{
		if( set.wifiMode == SETTING_WIFI_MODE_AP )
		{
			ret = blockRead( BLOCK_NO_AP_INFO, (uint8*)apInfo, sizeof( AP_INFO ) );
			if( ret == BLOCK_ERR_NONE )
				return WIFI_MODE_AP;
		}
		else if( set.wifiMode == SETTING_WIFI_MODE_STA )
		{
			ret = blockRead( BLOCK_NO_STA_INFO, (uint8*)staInfo, sizeof( STA_INFO ) );
			if( ret == BLOCK_ERR_NONE )
				return WIFI_MODE_STA;
		}
	}

	// default
	strcpy( (char*)apInfo->ssid,		DEF_AP_SSID );
	strcpy( (char*)apInfo->password,	DEF_AP_PASSWORD );
	parseIP( (char*)DEF_SERVER_IP, apInfo->ip );
	apInfo->encrypt	= DEF_AP_ENC;
	apInfo->port	= DEF_SERVER_PORT;

	return WIFI_MODE_AP;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
static bool initCommon( )
{
	wifiEsp.setTimeout( WIFI_TIMEOUT );
	wifiEsp.setRetry( false );
	wifiEsp.setRetryDelay( WIFI_RETRY_DELAY );
	wifiEsp.setRetryCount( WIFI_RETRY_COUNT );
	wifiEsp.setCallback( cbProcess );
	
	wifiEsp.setRetry( true );
	if( !isCmdOk( wifiEsp.atTest( ) ) )
		return false;
	wifiEsp.setRetry( false );
		
	if( !isCmdOk( wifiEsp.atSetEcho( false ) ) )
		return false;
		
	//
	// 1. set transfer mode "0"
	// ( 2. multiple conns enabled )
	// ( 3. create server )
	//
	int tmode;
	if( !isCmdOk( wifiEsp.atGetTransferMode( &tmode ) ) )
		return false;
	
	if( tmode != 0 )
	{
		//
		// a. delete server
		// b. set multiple connection disable
		// c. set transfer mode 0
		//
		
		// delete server if exists
		// can return ERROR in the case of already "no server"
		wifiEsp.atConfigServer( 0, 0 );

		if( !isCmdOk( wifiEsp.atSetMultipleConnections( 0 ) ) )
			return false;

		// in order to enable multiple connections
		// transfer mode should be set to 0
		wifiEsp.setRetry( true );
		if( !isCmdOk( wifiEsp.atSetTransferMode( 0 ) ) )
			return false;
		wifiEsp.setRetry( false );
	}
	
	return true;		
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static bool initAP( AP_INFO* apInfo )
{	
	//
	// mode : BOTH ( AP + STA )
	//
	if( !isCmdOk( wifiEsp.atSetMode( ESP_MODE_BOTH ) ) )
		return false;

	//
	// config AP
	//		
	// FIXME : whenever using enc, always failed
	// length( password ) > 8, use mixed alpha, number
	AP_CONFIG apConfig;
	
	strcpy( apConfig.ssid,		(const char*)apInfo->ssid );
	strcpy( apConfig.password,	(const char*)apInfo->password );
	apConfig.channel	= DEF_AP_CHANNEL;
	apConfig.enc		= DEF_AP_ENC;
	
	if( !isCmdOk( wifiEsp.atConfigAP( &apConfig ) ) )
		return false;

	return true;
}

static bool initSTA( STA_INFO* staInfo )
{
	//
	//  mode : STA
	// 
	if( !isCmdOk( wifiEsp.atSetMode( ESP_MODE_STATION ) ) )
		return false;
		
	// not tested
	if( staInfo->dhcp == 0 )
	{
		if( isCmdOk( wifiEsp.atSetIP( ESP_MODE_STATION, staInfo->ip ) ) )
			return false;
	}
	
	wifiEsp.setTimeout( WIFI_TIMEOUT_JOIN_AP );
	wifiEsp.atListAP(  );
	
	// mac address
	uint8 buf[6];
	if( !isCmdOk( wifiEsp.atGetMAC( ESP_MODE_STATION, buf ) ) )
		return false;
	uartPrintf( 0, "- MAC : %0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x\r\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5] );
	
	//
	//	join to AP
	//
	wifiEsp.setTimeout( WIFI_TIMEOUT_JOIN_AP );
	if( !isCmdOk( wifiEsp.atJoinAP( staInfo->ssid, staInfo->password ) ) )
		return false;
	wifiEsp.setTimeout( WIFI_TIMEOUT );

	return true;
}

bool initPost( )
{
	ESPMode mode = ( wifiMode == WIFI_MODE_AP ) ? ESP_MODE_AP : ESP_MODE_STATION;
	
	// mac address
	uint8 buf[6];
	if( !isCmdOk( wifiEsp.atGetMAC( mode, buf ) ) )
		return false;
	uartPrintf( 0, "- MAC : %0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x\r\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5] );
	
	// IP address
	wifiEsp.setRetry( true );
	if( !isCmdOk( wifiEsp.atGetIP( mode, wifiIP ) ) )
		return false;
	wifiEsp.setRetry( false );
	uartPrintf( 0, "- IP : %d.%d.%d.%d\r\n", wifiIP[0], wifiIP[1], wifiIP[2], wifiIP[3] );

	
	// (a) enable multi conns
	// (b) launch server
	
	//
	// 2019.09.05
	// fixme : streaming in case of multiple connections
	// 
	#if 1
	int mul = 0;
	wifiEsp.atGetMultipleConnections( &mul );
	if( mul != 1 )
	{
		if( !isCmdOk( wifiEsp.atSetMultipleConnections( 1 ) ) )
			return false;
	}
	#else
	if( !isCmdOk( wifiEsp.atSetMultipleConnections( 0 ) ) )
		return false;
	#endif

	wifiEsp.setRetry( true );
	if( !isCmdOk( wifiEsp.atConfigServer( 1, DEF_SERVER_PORT ) ) )
		return false;
	wifiEsp.setRetry( false );

	wifiEsp.setRetry( true );
	uint16 timeout = 0;
	wifiEsp.atGetServerTimeout( &timeout );
	if( timeout != SERVER_TIMEOUT )
	{
		if( !isCmdOk( wifiEsp.atSetServerTimeout( SERVER_TIMEOUT ) ) )
			return false;
	}
	wifiEsp.setRetry( false );
	wifiEsp.setTimeout( WIFI_TIMEOUT * 2 );

	return true;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
void wifiProcess( )
{
	wifiEsp.process( );
	
	if( cmdGetStrmOn( ) )
	{
		uint32 cur = millis( );
		if( ( cur - wifiStreamTimer > 1000 ) )
		{
			processStream( );
		
			wifiStreamTimer = cur;
		}
	}
}

void cbProcess( uint8 id, uint16 len, char* dat )
{
	uartPrintf( 0, "wifi> %d:%d:%s\r\n", id, len, dat );
	
	wifiId = id;
	cmdDo( dat, CMD_SRC_ID1 );
}

void processStream( )
{
	WIFI_STREAM* stream = wifiStream;
	
	for( int i = 0; i < WIFI_STREAM_COUNT; i++ )
	{
		if( stream->on )
		{
			char buf[64];
			int len = sprintf( buf, "strm %d 0x%x\r\n", i, *( stream->ptr ) );
			
			if( !isCmdOk( wifiEsp.atSend( wifiId, (uint8*)buf, len ) ) )
			{
				cmdSetStrmOn( false );
				break;				
			}
		}
		
		stream++;		
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void wifiGetIP( uint8 ip[4] )
{
	for( int i = 0; i < 4; i++ )
	{
		ip[i] = wifiIP[i];		
	}
}

uint8 wifiGetMode( )
{
	return wifiMode;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#if 0
void wifiTest( )
{
	uartPrintln( 0, "Test Untested Command" );

	AP_CONFIG apConfig;
	
	wifiEsp.atGetAPConfig( &apConfig );
	
	uartPrintf( 0, "- ap ssid : %s\r\n", apConfig.ssid );
	uartPrintf( 0, "- ap password : %s\r\n", apConfig.password );
	uartPrintf( 0, "- ap ch : %d\r\n", apConfig.channel );
	uartPrintf( 0, "- ap enc : %d\r\n", apConfig.enc );
	
	CONN_STATUS status;
	ESPConn		conns[1];
	status.inCount = 1;
	status.conns = conns;
	wifiEsp.atGetConnStatus( &status );
	
	char version[64];
	wifiEsp.atGetVersion( version, 64 );
	version[63]=0;
	uartPrintln( 0, version );
	
	CONN_STATIONS connStations;
	ESPStation	stations[2];
	connStations.inCount = 2;
	connStations.stations = stations;
	wifiEsp.atGetConnStations( &connStations );
}
#endif

