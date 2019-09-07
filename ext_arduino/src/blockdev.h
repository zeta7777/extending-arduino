
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			blockdev.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#ifndef __BLOCK_DEV_H
#define __BLOCK_DEV_H

#include "config.h"
#include "types.h"

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
#define BLOCK_SIZE						128
#define BLOCK_SIZE_SHIFT				7			// 2^7 = 128 bytes

#define BLOCK_MAGIC						0x0705
#define BLOCK_ERASE_CHAR				0xff

///////////////////////////////////////////////////////////////////////
//	block no.
///////////////////////////////////////////////////////////////////////
#define BLOCK_NO_SETTINGS				0
#define BLOCK_NO_AP_INFO				1
#define BLOCK_NO_STA_INFO				2

#define BLOCK_NO_MAX					( (4096 / BLOCK_SIZE) - 1 )

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
#define BLOCK_ERR_NONE			0
#define BLOCK_ERR_PARAM			-1
#define BLOCK_ERR_HDR			-2
#define BLOCK_ERR_CHECKSUM		-3
#define BLOCK_ERR_READ			-4
#define BLOCK_ERR_WRITE			-5

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
typedef struct __block_hdr
{
	uint16		magic;
	uint16		len;
	uint8		checksum;
	uint8		reserved;
} BLOCK_HDR;

///////////////////////////////////////////////////////////////////////
//	block data
///////////////////////////////////////////////////////////////////////
#define SETTING_WIFI_MODE_AP			1
#define SETTING_WIFI_MODE_STA			2

typedef struct __settings
{
	uint8		wifiMode;	
} SETTINGS;

typedef struct __ap_info
{
	// access control
	uint8		ssid[ CONFIG_WIFI_SSID_LEN ];
	uint8		password[ CONFIG_WIFI_PASSWORD_LEN ];
	uint8		encrypt;			// NOT USED
	
	// server ip/port
	uint8		ip[4];
	uint16		port;
} AP_INFO;

typedef struct __sta_info
{
	// AP to connect to
	uint8		ssid[ CONFIG_WIFI_SSID_LEN ];
	uint8		password[ CONFIG_WIFI_PASSWORD_LEN ];
	
	// dhcp (1) or fixed ip (0)
	uint8		dhcp;				// always 1
	// in case of fixed ip
	uint8		ip[4];				// 
	// server port
	uint16		port;
} STA_INFO;

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
int blockRead( uint16 blkno, uint8* buf, int len );
int blockWrite( uint8 blkno, uint8* dat, int len );
int blockErase( uint8 blkno );


#endif	// __BLOCK_DEV_H