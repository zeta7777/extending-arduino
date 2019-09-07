/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			esp8266.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#ifndef __ESP8266_H
#define __ESP8266_H

#include "types.h"

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
enum ESPRes
{
	ESP_RES_TIMEOUT				= -1,		// no response from ESP
	
	ESP_RES_OK					= 0,
	ESP_RES_NO_CHANGE,
	ESP_RES_ALREADY_CONNECTED,
	ESP_RES_BUSY,							// busy doing something else
	ESP_RES_ERROR,
	ESP_RES_FAIL,
	ESP_RES_NO_LINK,
	ESP_RES_TOO_LONG,
	
	// parse error
	ESP_RES_PARSE_ERROR,
	ESP_RES_INVALID_ARG,
	
	// postpone actual response( ok, error.. ) to the next line
	// internal use only
	ESP_RES_NEXT,							
};

/////////////////////////////////////////////////////////////////////
//	ESP8266 constant parameters	from spec
/////////////////////////////////////////////////////////////////////
#define ESP_SERVER_DEFAULT_IP		"192.168.4.1"
#define ESP_SERVER_DEFAULT_PORT		333
#define ESP_MAX_CONNS				5
#define ESP_SINGLE_CONN_ID			5

enum ESPMode 
{
    ESP_MODE_STATION		= 1,
    ESP_MODE_AP,
    ESP_MODE_BOTH
};

enum ESPEnc 
{
    ESP_ENC_OPEN			= 0,
    ESP_ENC_WEP,
    ESP_ENC_WPA_PSK,
    ESP_ENC_WPA2_PSK,
    ESP_ENC_WPA_WPA2_PSK
};

enum ESPConnStatus 
{
    ESP_CONN_STATUS_GOT_IP			= 2,
    ESP_CONN_STATUS_CONNECTED,
    ESP_CONN_STATUS_DISCONNECTED
};

enum ESPProto
{
    ESP_PROTO_TCP	= 0,
    ESP_PROTO_UDP
};

enum ESPType
{
    ESP_TYPE_CLIENT = 0,
    ESP_TYPE_SERVER
};

/////////////////////////////////////////////////////////////////////
//	get connection status 
/////////////////////////////////////////////////////////////////////
typedef struct __esp_conn
{
	uint8		id;
	ESPProto	proto;
	uint8		ip[4];
	uint16		port;
	ESPType		type;
} ESPConn;

// list of conn status linked to this AP/STA
typedef struct __conn_status
{
	int				inCount;		// input : length of ESPConn
	int				retCount;		// actual count filled by ESP
	ESPConnStatus	status;
	ESPConn*		conns;
} CONN_STATUS;

/////////////////////////////////////////////////////////////////////
//	get connected station info
/////////////////////////////////////////////////////////////////////
typedef struct __esp_station
{
    uint8	ip[4];
    uint8	mac[6];
} ESPStation;

// list stations connected to this AP
typedef struct __conn_stations
{
	int				inCount;
	int				retCount;
	ESPStation*		stations;
} CONN_STATIONS;

/////////////////////////////////////////////////////////////////////
//	get/set AP configuration
/////////////////////////////////////////////////////////////////////
typedef struct __ap_config
{
	char	ssid[CONFIG_WIFI_SSID_LEN];
	char	password[CONFIG_WIFI_PASSWORD_LEN];
	uint8	channel;
	ESPEnc	enc;
} AP_CONFIG;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
typedef void (*ESP_CALLBACK)( uint8 id, uint16 len, char* dat );

class CEsp
{
public:
    CEsp( )								{		}		
	
	void	init( uint8 uartNo );
	void	setTimeout( uint16 ms )			{ mTimeout = ms;		}
	void	setRetry( bool retry )			{ mRetry = retry;		}
	void	setRetryCount( uint8 count )	{ mRetryCount = count;	}		
	void	setRetryDelay( uint16 ms )		{ mRetryDelay = ms;		}
	void	setCallback( ESP_CALLBACK cb )	{ mCallback = cb;		}
		
	/////////////////////////////////////////////////////////////////////
	//		AT command
	/////////////////////////////////////////////////////////////////////
	
    ESPRes atTest( );
    ESPRes atReset( );
	ESPRes atGetVersion( char* buf, int len );
	ESPRes atDeepSleep( uint32 time );
	ESPRes atSetEcho( bool enable );
	void   atSetBaudrate( uint32 baudrate );
	
	ESPRes atSetMode( ESPMode mode );
	ESPRes atGetMode( ESPMode* mode );
    ESPRes atGetMAC( ESPMode mode, uint8 mac[6] );
	ESPRes atSetIP( ESPMode mode, uint8 ip[4] );
    ESPRes atGetIP( ESPMode mode, uint8 ip[4] );
	ESPRes atConfigAP( AP_CONFIG* apConfig );
	ESPRes atGetAPConfig( AP_CONFIG* apConfig );
	ESPRes atSetDHCP( ESPMode mode, bool enable );
	ESPRes atSetMAC( ESPMode mode, uint8 mac[6] );
	ESPRes atQuitAP( );
	ESPRes atGetSSID( char* ssid );
	
	ESPRes atSetServerTimeout( uint16 timeout );
	ESPRes atGetServerTimeout( uint16* timeout );
	ESPRes atSetMultipleConnections( int enable );
	ESPRes atGetMultipleConnections( int* enable);
	ESPRes atSetTransferMode( int mode );
	ESPRes atGetTransferMode( int* mode );
	ESPRes atJoinAP( uint8* ssid, uint8* password );
	ESPRes atConfigServer( int mode, uint16 port );
	ESPRes atGetConnStatus( CONN_STATUS* status );
	ESPRes atGetConnStations( CONN_STATIONS* connStations );
	ESPRes atClose( uint16 id );
	ESPRes atSend( uint16 id, uint8* data, uint16 size, bool lineEnding = true );
	ESPRes atConnect( uint16 id, ESPProto proto, uint8 ip[4], uint16 port );
	ESPRes atConnect( uint16 id, ESPProto proto, const char* host, uint16 port );
	ESPRes atListAP( );
	
	void process( );

private:
	
	uint8		mUartNo;
	
	uint16		mTimeout;
	
	bool		mRetry;
	uint8		mRetryCount;
	uint16		mRetryDelay;
	
	// for no-keyword response parsing
	uint8		mNKCommand;
	uint16		mNKArg;
	uint16		mNKState;
	
	ESP_CALLBACK mCallback;
	
	ESPRes	waitForResp( const char* str, void* retbuf = NULL );
	bool		parseResp( char* buf, ESPRes* res, void* retbuf );
	
    void		clear( );
	void		flush( );
	
	// utils
	bool		parseAPConfig( AP_CONFIG* apConfig );
	bool		parseInt( int* val );
	bool		parseIP( uint8* ip );
	bool		parseMAC( uint8* mac );
	bool		parseConn( ESPConn* conn );
};

#endif	// __ESP8266_H
