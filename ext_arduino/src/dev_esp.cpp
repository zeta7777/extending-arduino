
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			esp8266.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>

#include "config.h"
#include "system.h"
#include "core_uart.h"
#include "dev_esp.h"
#include "utils.h"


#define __LOG_ESP	1

#if __LOG_ESP==1
#define LOG( str )		uartPrintln( 0, (str) )
#define LOG_( str )		uartPrint( 0, (str) )
#define LOGC( c )		uartPut( 0, c )
#else
#define LOG( str )		{ }
#define LOG_( str )		{ }
#define LOGC( c )		{ }
#endif

#define AT_GET_VERSION			1
#define AT_LIST_CONN_STATIONS	2


//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void CEsp::init( uint8 uartNo )
{
	mUartNo		= uartNo;
	mTimeout	= 1000;
	mRetry		= false;
	mRetryCount	= 5;
	mRetryDelay = 1000;
	mNKCommand	= 0;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define DELIMITER0		",:\r\n \t"

void CEsp::process( )
{
	char line[128];

	if( !uartIsLineAvailable( mUartNo ) )
		return;
	
	int count = uartGetLine( mUartNo, (uint8*)line, 127 );

	if( count <= 0 )
		return;
		
	if( strncmp( line, "+IPD", 4 ) == 0 )
	{
		char* tok	= getToken( line, DELIMITER0 );		// should be "+IPD"
		char* ids	= getNextToken( DELIMITER0 );
		char* lens	= getNextToken( DELIMITER0 );
		char* str	= getNextToken( "\r\n" );
		
		if( ids && lens && str )
		{
			int id = atoi( ids );
			int len = atoi( lens );
			if( len > 0 )
				mCallback( id, len-2, str );
			
			return;
		}
	}
	else
		uartPrintln( 0, line );
}

//////////////////////////////////////////////////////////////////////
//	AT command
//////////////////////////////////////////////////////////////////////
ESPRes CEsp::atTest( )
{
	clear( );
	LOG( "* at" );
	return waitForResp( "AT\r\n" );
}

ESPRes CEsp::atSetEcho( bool enable )
{
	clear( );
	LOG( "* set echo" );
	char cmd[16];
	sprintf( cmd, "ATE%d\r\n", (int)enable );
	return waitForResp( cmd );
}

// 
//	0 : normal mode 
//	1 : unvarnished transmission mode
//
ESPRes CEsp::atGetTransferMode( int* mode )
{
	clear( );
	LOG( "* get transfer mode" );
	return waitForResp( "AT+CIPMODE?\r\n", mode );
}

ESPRes CEsp::atSetTransferMode( int mode )
{
	clear( );
	LOG( "* set transfer mode" );
	char cmd[32];
	sprintf( cmd, "AT+CIPMODE=%d\r\n", mode );
	return waitForResp( cmd );
}

ESPRes CEsp::atGetMode( ESPMode* mode )
{
	clear( );
	LOG( "* get mode" );
	return waitForResp( "AT+CWMODE?\r\n", mode );
}

ESPRes CEsp::atSetMode( ESPMode mode )
{
	clear( );
	LOG( "* set mode" );
	char cmd[32];
	sprintf( cmd, "AT+CWMODE=%d\r\n", mode );
	return waitForResp( cmd );
}

ESPRes CEsp::atConfigAP( AP_CONFIG* apConfig )
{
	clear( );
	LOG( "* config AP" );
	char cmd[128];
	sprintf( cmd, "AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n", apConfig->ssid, apConfig->password, apConfig->channel, apConfig->enc );
	return waitForResp( cmd );
}

ESPRes CEsp::atGetAPConfig( AP_CONFIG* apConfig )
{
	clear( );
	
	LOG( "* get ap config" );
	
	// +CWSAP:"ssid","password",ch0,enc0,ch1,end1	
	// maybe ch0, enc0 is current setting
	// ch1, enc1 is user-supplied value
	return waitForResp( "AT+CWSAP?\r\n", apConfig );
}

ESPRes CEsp::atGetMAC( ESPMode mode, uint8 mac[6] )
{
	clear( );
	
	LOG( "* get mac" );
	char cmd[32];
	sprintf( cmd, "AT+CIP%sMAC?\r\n", mode == ESP_MODE_STATION ? "STA" : "AP" );
	return waitForResp( cmd, mac );
}

ESPRes CEsp::atSetMAC( ESPMode mode, uint8 mac[6] )
{
	clear( );
	LOG( "* set mac" );
	char cmd[128];
	sprintf( cmd, "AT+CIP%sMAC=\"%0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x\"\r\n",  
				mode == ESP_MODE_STATION ? "STA" : "AP", 
				mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
	
	return waitForResp( cmd );
}

// +CIPSTA:ip:"192.168.11.8"[C][L]
// +CIPSTA:gateway:"192.168.11.1"[C][L]
// +CIPSTA:netmask:"255.255.255.0"
// [C][L]
// [C][L]
// OK[C][L]
//
// when busy, "busy p..." response
ESPRes CEsp::atGetIP( ESPMode mode, uint8 ip[4] )
{
	clear( );
	LOG( "* get ip" );
	if( mode == ESP_MODE_STATION )
		return waitForResp( "AT+CIPSTA?\r\n", ip  );
	
	return waitForResp( "AT+CIPAP?\r\n", ip );
}

ESPRes CEsp::atSetIP( ESPMode mode, uint8 ip[4] )
{
	clear( );
	LOG( "* set ip" );

	char cmd[64];	
	sprintf( cmd, "AT+CIP%s=%d.%d.%d.%d\r\n", mode == ESP_MODE_STATION ? "STA" : "AP", ip[0], ip[1], ip[2], ip[3] );
	
	return waitForResp( cmd );
}

ESPRes CEsp::atReset( )
{
	clear( );
	LOG( "* reset" );
	return waitForResp( "AT+RST\r\n" );
}

ESPRes CEsp::atJoinAP( uint8* ssid, uint8* password )
{
	clear();
	
	LOG( "* join ap" );
	char cmd[128];
	sprintf( cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password );
	
	char retstr[64];
	ESPRes res = waitForResp( cmd, retstr );
	uartPrintf( 0, "* ret : %s\r\n", retstr );

	return res;
}

//
//	mode : 0 ( delete server ), 1 ( create server )
//	port : 0 ( use default port )
//
ESPRes CEsp::atConfigServer( int mode, uint16 port )
{
	clear( );
	LOG( "* config server" );

	char cmd[128];
	
	if( mode == 0 )
		sprintf( cmd, "AT+CIPSERVER=0\r\n" );
	else if( port == 0 )
		sprintf( cmd, "AT+CIPSERVER=1\r\n" );
	else
		sprintf( cmd, "AT+CIPSERVER=1,%d\r\n", port );

	return waitForResp( cmd );
}

ESPRes CEsp::atSetMultipleConnections( int enable )
{
	clear( );
	LOG( "* set mul conns" );

	char cmd[128];
	sprintf( cmd, "AT+CIPMUX=%d\r\n", enable );

	return waitForResp( cmd );
}

ESPRes CEsp::atGetMultipleConnections( int* enable )
{
	clear( );
	LOG( "* get mul conns" );
	
	return waitForResp( "AT+CIPMUX?\r\n", enable );
}

ESPRes CEsp::atSetDHCP( ESPMode mode, bool enable )
{
	clear( );
	
	LOG( "* set dhcp" );
	int m;

	if( mode == ESP_MODE_STATION )
		m = 1;
	else if( mode == ESP_MODE_AP )
		m = 0;
	else if( mode == ESP_MODE_BOTH )
		m = 2;
	else
		m = 0;
	
	char cmd[64];
	sprintf( cmd, "AT+CWDHCP=%d,%d\r\n", m, (int)enable );
	
	return waitForResp( cmd );
}

ESPRes CEsp::atDeepSleep( uint32 time )
{
	clear( );
	LOG( "* deep sleep" );
	char cmd[64];
	sprintf( cmd, "AT+GSLP=%ld\r\n", time );
	return waitForResp( cmd );
}

void CEsp::atSetBaudrate( uint32 baudrate )
{
	clear( );
	LOG( "* set baudrate" );
	uartPrintf( mUartNo, "AT+IPR=%ld\r\n", baudrate );
}

ESPRes CEsp::atQuitAP( )
{
	clear( );
	LOG( "* quit ap" );
	return waitForResp( "AT+CWQAP\r\n" );
}


ESPRes CEsp::atGetSSID( char* ssid )
{
	clear( );
	LOG( "* get ap" );
	return waitForResp( "AT+CWJAP?\r\n", ssid );
}

ESPRes CEsp::atSetServerTimeout( uint16 timeout )
{
	clear( );
	LOG( "* set server timeout" );
	char cmd[64];
	sprintf( cmd, "AT+CIPSTO=%d\r\n", timeout);
	return waitForResp( cmd );
}

ESPRes CEsp::atGetServerTimeout( uint16* timeout )
{
	clear();
	LOG( "* get server timeout" );
	return waitForResp( "AT+CIPSTO?\r\n", timeout );
}


ESPRes CEsp::atGetConnStatus( CONN_STATUS* connStatus )
{
	clear( );
	LOG( "* get conn status" );
	connStatus->retCount = 0;
	return waitForResp( "AT+CIPSTATUS\r\n", connStatus );
}


ESPRes CEsp::atClose( unsigned int id )
{
	clear( );
	LOG( "* close" );
	char cmd[64];
	sprintf( cmd, "AT+CIPCLOSE=%d\r\n", id );
	return waitForResp( cmd );
}



//
//	check this out.
//  because waitForResp cannot parse this.
//  response does not contain any keyword. just ip and mac address only
//
ESPRes CEsp::atGetConnStations( CONN_STATIONS* connStations )
{
	clear( );
	LOG( "* get conn stations" );
	
	mNKCommand = AT_LIST_CONN_STATIONS;
	mNKArg = 0;
	mNKState = 0;
	
	connStations->retCount = 0;
	return waitForResp( "AT+CWLIF\r\n", connStations );
}

// response just contain string without any key
ESPRes CEsp::atGetVersion( char* buf, int len )
{
	clear( );
	LOG( "* get version" );
	
	mNKCommand	= AT_GET_VERSION;
	mNKArg		= len;
	mNKState	= 0;
	
	return waitForResp( "AT+GMR\r\n", buf );
}

ESPRes CEsp::atListAP( )
{
	clear( );
	LOG( "* list ap" );
	waitForResp( "AT+CWLAP\r\n" );
	return waitForResp( "AT+CWDHCP=1,1\r\n" );
}


// max data size : 2048
ESPRes CEsp::atSend( uint16 id, uint8* data, uint16 size, bool lineEnding )
{
	clear( );
	
	LOG( "* send");
	char cmd[64];
	
	if( lineEnding )
		size += 2;
		
	sprintf( cmd, "AT+CIPSEND=%d,%d\r\n", id, size );
	ESPRes res = waitForResp( cmd );

	// fixme :
	//			OK, '>'
	//	
	if( res == ESP_RES_OK )
	{
		uartWrite( mUartNo, data, size );
		if( lineEnding )
			uartWrite( mUartNo, (uint8*)"\r\n", 2 );
			
		return waitForResp( NULL );
	}

	return res;
}

//////////////////////////////////////////////////////////////////////
//		connect
//////////////////////////////////////////////////////////////////////

// change timeout to 10sec
ESPRes CEsp::atConnect( uint16 id, ESPProto proto, uint8 ip[4], uint16 port )
{
	const char* protostr = ( proto == ESP_PROTO_TCP ) ? "TCP" : "UDP";

	clear( );
	char cmd[64];
	
	sprintf( cmd, "AT+CIPSTART=%d,\"%s\",\"%d.%d.%d.%d\",%d\r\n", id, protostr, ip[0], ip[1], ip[2], ip[3], port );
	
	return waitForResp( cmd );
}

// change timeout to 10sec
ESPRes CEsp::atConnect( uint16 id, ESPProto proto, const char* host, uint16 port)
{
	const char* protostr = ( proto == ESP_PROTO_TCP ) ? "TCP" : "UDP";

	clear( );
	char cmd[64];
	sprintf( cmd,"AT+CIPSTART=%d,\"%s\",\"%s\",%d\r\n", id, protostr, host, port );
	
	return waitForResp( cmd );
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
ESPRes CEsp::waitForResp( const char* cmdStr, void* retbuf )
{
	uint8		maxRetry = mRetry ? mRetryCount : 1;
	ESPRes	res;
	char		buf[256];
	int			count;
	
	for( int retry = 0; retry < maxRetry; retry++ )
	{
		if( cmdStr )
		{
			uartPrint( mUartNo, cmdStr );
			LOG( cmdStr );
		}


		uint32 start = millis( );
	
		while( 1 )
		{
			if( uartIsLineAvailable( mUartNo ) )
			{
				count = uartGetLine( mUartNo, (uint8*)buf, 256 );
				
				LOG( buf );
				if( parseResp( buf, &res, retbuf ) )
				{
					if( res != ESP_RES_NEXT )
						goto end;
				}
			}
			
			if( millis( ) - start >= mTimeout )
			{
				res = ESP_RES_TIMEOUT;
				goto end;
			}
		}	// end of while( 1 )
end:	
		if( ( res == ESP_RES_BUSY || res == ESP_RES_TIMEOUT ) && 
			 mRetry && 
			 retry < maxRetry - 1 )
		{
			if( res == ESP_RES_BUSY )
				delay( mRetryDelay );
				
			uartPrintf( 0, "\r\n* retrying on code[%d]...%d\r\n", res, retry + 1 );
		}
		else
			break;
	}	
	
	uartPrintf( 0, "-->res : %d\r\n", res );
	return res;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define PARSE_NONE				0
#define PARSE_IP				1
#define PARSE_INT				2
#define PARSE_STR				3
#define PARSE_MAC				4
#define PARSE_AP_CONFIG			5
#define PARSE_CONN_STATUS0		6
#define PARSE_CONN_STATUS1		7

typedef struct __at_resp
{
	const char*		key;
	ESPRes		res;
	uint8			next;
} AT_RESP;

const AT_RESP	ATResp[] =
{
	{	"OK",				ESP_RES_OK,					PARSE_NONE,	},
	{	"no change",		ESP_RES_NO_CHANGE,			PARSE_NONE,	},
	{	"ready",			ESP_RES_OK,					PARSE_NONE,	},
	{	"ERROR",			ESP_RES_ERROR,				PARSE_NONE,	},
	{	"link is not",		ESP_RES_NO_LINK,			PARSE_NONE,	},
	{	"too long",			ESP_RES_TOO_LONG,			PARSE_NONE,	},
	{	"FAIL",				ESP_RES_FAIL,				PARSE_NONE,	},
	{	"ALREADY CONNECT",	ESP_RES_ALREADY_CONNECTED,	PARSE_NONE,	},
	{	"busy",				ESP_RES_BUSY,				PARSE_NONE,	},
	{	"Recv",				ESP_RES_NEXT,				PARSE_NONE	},			// send
	{	"SEND OK",			ESP_RES_OK,					PARSE_NONE	},			// send ( ok )
	{	"SEND FAIL",		ESP_RES_ERROR,				PARSE_NONE	},			// send ( error )
	{	"CIPSTA",			ESP_RES_NEXT,				PARSE_IP,	},			// get ip
	{	"CIPAP",			ESP_RES_NEXT,				PARSE_IP,	},			// get ip
	{	"CIPMODE",			ESP_RES_NEXT,				PARSE_INT,	},			// get transfer mode
	{	"CWMODE",			ESP_RES_NEXT,				PARSE_INT,	},			// get mode
	{	"CIPSTAMAC",		ESP_RES_NEXT,				PARSE_MAC,	},			// get mac
	{	"CIPAPMAC",			ESP_RES_NEXT,				PARSE_MAC,	},			// get mac
	{	"CIPMUX",			ESP_RES_NEXT,				PARSE_INT,	},			// get mul conns
	{	"CWJAP",			ESP_RES_NEXT,				PARSE_STR,	},			// join ap
	{	"CIPSTO",			ESP_RES_NEXT,				PARSE_INT,	},			// get server timeout
	{	"CWSAP",			ESP_RES_NEXT,				PARSE_AP_CONFIG,	},	// get ap config
	{	"STATUS",			ESP_RES_NEXT,				PARSE_CONN_STATUS0,	},	// get conn status	( STATUS:%d )
	{	"CIPSTATUS",		ESP_RES_NEXT,				PARSE_CONN_STATUS1, },	// get conn status	( CIPSTATUS )
	{	NULL,															}
};

#define DELIMITER		"\r\n+:\t.\",>"

//
// return true when final esp result determined
//
bool CEsp::parseResp( char* resp, ESPRes* res, void* retbuf )
{
	char* tok = getToken( resp, DELIMITER );

	if( tok == NULL )
		return false;
		
	AT_RESP* atResp = (AT_RESP*)ATResp;	
	
	for( ; atResp->key; atResp++ )
	{
		if( strcmp( atResp->key, tok ) )
			continue;
		
		// found the result keyword
		if( atResp->next == PARSE_NONE )
		{
			mNKCommand = 0;
			*res = atResp->res;
			return true;
		}
		
		// try to read more ...
		if( retbuf == NULL )
		{
			*res = ESP_RES_INVALID_ARG;
			return true;
		}		
		
		if( atResp->next == PARSE_INT )
		{
			if( !parseInt( (int*)retbuf ) )
			{
				*res = ESP_RES_PARSE_ERROR;
				return true;
			}
		}
		else if( atResp->next == PARSE_IP )
		{
			tok = getNextToken( DELIMITER );
			if( tok && strcmp( tok, "ip" ) == 0 )
			{
				if( !parseIP( (uint8*)retbuf ) )
				{
					*res = ESP_RES_PARSE_ERROR;
					return true;
				}
			}
		}
		else if( atResp->next == PARSE_MAC )
		{
			if( !parseMAC( (uint8*)retbuf ) )
			{
				*res = ESP_RES_PARSE_ERROR;
				return true;
			}
		}
		else if( atResp->next == PARSE_STR )
		{
			tok = getNextToken( "\"" );
			strcpy( (char*)retbuf, tok );
		}
		else if( atResp->next == PARSE_AP_CONFIG )
		{
			if( !parseAPConfig( (AP_CONFIG*)retbuf ) )
			{
				*res = ESP_RES_PARSE_ERROR;
				return true;
			}
		}
		else if( atResp->next == PARSE_CONN_STATUS0 )
		{
			CONN_STATUS* cs = (CONN_STATUS*)retbuf;

			int status;
			if( !parseInt( &status ) )
			{
				*res = ESP_RES_PARSE_ERROR;
				return true;
			}
			cs->status = (ESPConnStatus)status;
		}
		else if( atResp->next == PARSE_CONN_STATUS1 )
		{
			CONN_STATUS* cs = (CONN_STATUS*)retbuf;
			int idx = cs->retCount;
			if( idx < cs->inCount )
			{
				ESPConn* conn = &cs->conns[ idx ];
					
				if( !parseConn( conn ) )
				{
					*res = ESP_RES_PARSE_ERROR;
					return true;
				}
					
				cs->retCount++;
			}
		}
		
		return false;
	}	// end of for( )
	
	if( mNKCommand == AT_GET_VERSION )
	{
		char* buf = (char*)retbuf;
		int len = strlen( resp );
		
		if( mNKState < mNKArg )
			strncpy( buf + mNKState, resp, mNKArg - mNKState );

		mNKState += len;
	}
	else if( mNKCommand == AT_LIST_CONN_STATIONS )
	{
		CONN_STATIONS* stations = (CONN_STATIONS*)retbuf;
		if( stations->retCount < stations->inCount )
		{
			ESPStation* sta = &stations->stations[stations->retCount];
			if( !parseIP( sta->ip ) )
			{
				*res = ESP_RES_PARSE_ERROR;
				return true;
			}
			
			stations->retCount++;
		}
	}
	
	// no matching keyword
	return false;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void CEsp::flush( )
{
	// uartFlush( mUartNo );
}

void CEsp::clear( )
{
	while( uartIsAvailable( mUartNo ) )
		uartGet( mUartNo );
}

//////////////////////////////////////////////////////////////////////
//		utils
//////////////////////////////////////////////////////////////////////
bool CEsp::parseAPConfig( AP_CONFIG* apConfig )
{
	char* tok;
	
	if( ( tok = getNextToken( DELIMITER ) ) )
	{
		strcpy( apConfig->ssid, tok );
		if( ( tok = getNextToken( DELIMITER ) ) )
		{
			strcpy( apConfig->password, tok );
			if( ( tok = getNextToken( DELIMITER ) ) )
			{
				apConfig->channel = atoi( tok );
				if( ( tok = getNextToken( DELIMITER ) ) )
				{
					apConfig->enc = (ESPEnc)atoi( tok );
					return true;
				}
			}
		}
	}
	
	return false;
}

bool CEsp::parseInt( int* val )
{
	char* tok;
	if( ( tok = getNextToken( DELIMITER ) ) )
	{
		*val = atoi( tok );
		return true;
	}
	
	return false;
}

bool CEsp::parseIP( uint8* ip )
{
	char* tok;

	for( int i = 0; i < 4; i++ )
	{
		if( ( tok = getNextToken( DELIMITER ) ) )
			*ip++ = atoi( tok );
		else
			return false;
	}
	
	return true;
}

bool CEsp::parseMAC( uint8* mac )
{
	char* tok;
	char* endptr;
	for( int i = 0; i < 6; i++ )
	{
		if( ( tok = getNextToken( DELIMITER ) ) )
			*mac++ = (uint8)strtol( tok, &endptr, 16 );
		else
			return false;
	}
	
	return true;
}

bool CEsp::parseConn( ESPConn* conn )
{
	char* tok;
	
	if( ( tok = getNextToken( DELIMITER ) ) )
	{
		conn->id = atoi( tok );
		if( ( tok = getNextToken( DELIMITER ) ) )
		{
			if( strcmp( "TCP", tok ) == 0 )
				conn->proto = ESP_PROTO_TCP;
			else if( strcmp( "UDT", tok ) == 0 )
				conn->proto = ESP_PROTO_UDP;
			else
				return false;
				
			if( (  tok = getNextToken( DELIMITER ) ) )
			{
				if( !parseIP( conn->ip ) )
					return false;
				if( (  tok = getNextToken( DELIMITER ) ) )
				{
					conn->port = atoi( tok );
					if( ( tok = getNextToken( DELIMITER ) ) )
					{
						conn->type = (ESPType)atoi( tok );
						return true;
					}
				}
			}
		}
	}
	
	return false;
}


