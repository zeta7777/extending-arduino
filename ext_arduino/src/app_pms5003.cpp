/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			app_pms5003.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#include "core_uart.h"
#include "app_pms5003.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define STATE_SYNC0				0
#define STATE_SYNC1				1
#define STATE_DATA_LEN_HIGH		2
#define STATE_DATA_LEN_LOW		3
#define STATE_DATA				4

#define SYNC0_BYTE				0x42
#define SYNC1_BYTE				0x4d

// data + checksum
#define DATA_LEN				28

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static uint16 checksum( uint8* dat, int len );
static void sendCmd( uint8 cmd, uint8 arg );

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static uint8	pmsUartNo;
static int8		pmsLogUartNo = -1;

typedef struct __pms_data
{
	union
	{
		uint8	raw[DATA_LEN];
		uint16	data[DATA_LEN/2];
	} data;
} PMS_DATA;

static uint8		pmsState;
static uint16		pmsDataLen;
static PMS_DATA		pmsData;
static uint8		pmsIndex;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void pms5003Init( uint8 uartNo, uint8 logUartNo )
{
	pmsUartNo = uartNo;
	pmsLogUartNo = logUartNo;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
bool pms5003Process( )
{
	bool ret = false;
	
	while( uartIsAvailable( pmsUartNo ) )
	{
		uint8 c = uartGet( pmsUartNo );
		
		switch( pmsState )
		{
		/////////////////////////////////////////////////////////////
		case STATE_SYNC0:
			if( c == SYNC0_BYTE )
				pmsState = STATE_SYNC1;
			else 
			{
				if( pmsLogUartNo >= 0 )
					uartPrintf( pmsLogUartNo, "lost sync0\r\n" );
			}
			break;
			
		/////////////////////////////////////////////////////////////	
		case STATE_SYNC1:
			if( c == SYNC1_BYTE )
				pmsState = STATE_DATA_LEN_HIGH;
			else
			{
				pmsState = STATE_SYNC0;
				if( pmsLogUartNo >= 0 )
					uartPrintf( pmsLogUartNo, "lost sync1\r\n" );
			}
			break;
			
		/////////////////////////////////////////////////////////////	
		case STATE_DATA_LEN_HIGH:
			pmsDataLen = c;
			pmsState = STATE_DATA_LEN_LOW;
			break;
			
		case STATE_DATA_LEN_LOW:
			pmsDataLen = ( pmsDataLen << 8 ) + c;
			pmsState = STATE_DATA;
			pmsIndex = 0;
			
			if( pmsDataLen > DATA_LEN )
				pmsDataLen = DATA_LEN;
			
			if( pmsLogUartNo >= 0 )
				uartPrintf( pmsLogUartNo, "len : %d\r\n", pmsDataLen );
			break;
			
		/////////////////////////////////////////////////////////////	
		case STATE_DATA:
			// swap low-high
			pmsData.data.raw[pmsIndex ^ 1] = c;
			
			// end of data packet
			if( ++pmsIndex >= pmsDataLen )
			{
				pmsState = STATE_SYNC0;
				ret = true;
				
				uint16 sum = SYNC0_BYTE + SYNC1_BYTE + pmsDataLen;
				sum += checksum( pmsData.data.raw, 26 );
				
				if( pmsLogUartNo >= 0 )
				{
					// can change ret to false
					if( pmsData.data.data[13] != sum )
						uartPrintf( pmsLogUartNo, "check sum failed : %d\r\n", pmsData.data.data[13] - sum );
						
					uartPrintf( pmsLogUartNo, "1.0: %d\r\n", pmsData.data.data[4] );
					uartPrintf( pmsLogUartNo, "2.5: %d\r\n", pmsData.data.data[5] );
					uartPrintf( pmsLogUartNo, "10.: %d\r\n", pmsData.data.data[6] );
				}
			}
			
			break;			
		}
		/////////////////////////////////////////////////////////////
	}
	
	return ret;
}

void pms5003GetData( uint16 data[PMS5003_DATA_COUNT] )
{
	for( int i = 0; i < PMS5003_DATA_COUNT; i++ )
		data[i] = pmsData.data.data[i];
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void pms5003ChangeMode( uint8 mode )
{
	sendCmd( 0xe1, mode );
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void pms5003SetOperatingMode( uint8 op )
{
	sendCmd( 0xe4, op );
}

//////////////////////////////////////////////////////////////////////
//	should be called in passive mode 
//	i.e. after calling pms5003ChangeMode( PMS5003_MODE_PASSIVE )
//////////////////////////////////////////////////////////////////////
void pms5003Read( )
{
	sendCmd( 0xe2, 0 );
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static uint16 checksum( uint8* dat, int len )
{
	uint16 sum = 0;
	for( int i = 0; i < len; i++ )
		sum += *dat++;
	return sum;
}

static void sendCmd( uint8 cmd, uint8 arg )
{
	uint8 pkt[7] = { SYNC0_BYTE, SYNC1_BYTE, cmd, 0, arg, 0, 0 };
	
	uint16 sum = checksum( pkt, 5 );
	
	// big-endian
	pkt[5] = sum >> 8;
	pkt[6] = sum;
	
	uartWrite( pmsUartNo, pkt, 7 );
}