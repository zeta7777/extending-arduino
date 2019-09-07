
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			blockdev.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#include <string.h>

#include "config.h"
#include "core_eeprom.h"
#include "core_uart.h"

#include "blockdev.h"

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
static uint8 checksum( uint8* dat, int len );

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
int blockRead( uint16 blkno, uint8* buf, int len )
{
	if( blkno > BLOCK_NO_MAX || len > ( BLOCK_SIZE - sizeof( BLOCK_HDR ) ) )
	{
		uartPrintf( 0, "%d %d %d %d\r\n", blkno, len, BLOCK_NO_MAX, BLOCK_SIZE - sizeof( BLOCK_HDR )  );
		return BLOCK_ERR_PARAM;
	}
	uint16 addr = blkno << BLOCK_SIZE_SHIFT;
	BLOCK_HDR hdr;
	
	// read header	
	if( !eepromRead( addr, (uint8*)&hdr, sizeof( BLOCK_HDR ) ) )
		return BLOCK_ERR_READ;
	
	if( hdr.magic != BLOCK_MAGIC ||	hdr.len != len )
	{
		uartPrintf( 0, "magic : %x len : %d(%d)\r\n", hdr.magic, hdr.len, len );
		return BLOCK_ERR_HDR;
	}

	addr += sizeof( BLOCK_HDR );
	if( !eepromRead( addr, (uint8*)buf, len ) )
		return BLOCK_ERR_READ;
	
	if( hdr.checksum != checksum( buf, len ) )
		return BLOCK_ERR_CHECKSUM;
	
	return BLOCK_ERR_NONE;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
int blockWrite( uint8 blkno, uint8* dat, int len )
{
	if( blkno > BLOCK_NO_MAX || len > ( BLOCK_SIZE - sizeof( BLOCK_HDR ) ) )
		return BLOCK_ERR_PARAM;
		
	uint16 addr = blkno << BLOCK_SIZE_SHIFT;

	BLOCK_HDR hdr;
	hdr.magic = BLOCK_MAGIC;
	hdr.len = len;
	hdr.checksum = checksum( dat, len );
	
	// info header
	if( !eepromWrite( addr, (uint8*)&hdr, sizeof( BLOCK_HDR ) ) )
		return BLOCK_ERR_WRITE;
	
	addr += sizeof( BLOCK_HDR );
	if( !eepromWrite( addr, dat, len ) )
		return BLOCK_ERR_WRITE;

	return BLOCK_ERR_NONE;
}

int blockErase( uint8 blkno )
{
	if( blkno > BLOCK_NO_MAX )
		return BLOCK_ERR_PARAM;
		
	if( !eepromErase( blkno << BLOCK_SIZE_SHIFT, BLOCK_SIZE, BLOCK_ERASE_CHAR ) )
		return BLOCK_ERR_WRITE;
	return BLOCK_ERR_NONE;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
uint8 checksum( uint8* dat, int len )
{
	uint8 sum = 0;
	
	while( len-- )
		sum += *dat++;
		
	return sum;
}

