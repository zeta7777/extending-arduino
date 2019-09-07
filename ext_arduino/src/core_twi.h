/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			core_twi.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#ifndef __TWI_H
#define __TWI_H

#include "types.h"

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define TWI_ERR_NONE			0
#define TWI_ERR_BUF				1
#define TWI_ERR_SLA_NACK		2
#define TWI_ERR_DATA_NACK		3
#define TWI_ERR_ARB_LOST		4
#define TWI_ERR_NO_INFO			5
#define TWI_ERR_BUS				6

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void	twiInit( uint32 freq );
void	twiClean( );
uint8	twiWrite( uint8 address, uint8* data, uint8 length );
uint8	twiRead( uint8 address, uint8* buf, uint8 length );
uint8	twiGetError( );

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#endif	// __TWI_H