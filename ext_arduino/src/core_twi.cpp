
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			core_twi.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#include "config.h"
#include "core_twi.h"
#include "core_io.h"

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define TWI_READY				0
#define TWI_MASTER_TX			1
#define TWI_MASTER_RX			2

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define TWI_BUF_SIZE			32

static volatile uint8	twiState;
static uint8			twiAddress;
static uint8			twiError;

static uint8			twiBuf[ CONFIG_TWI_BUF_SIZE ];
static uint8			twiBufLength;
static volatile uint8	twiBufIndex;

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void twiInit( uint32 freq )
{
	twiState = TWI_READY;
  
	// internal pull-up : does it have effect on TWI?
	ioSetMode( IO_PD0, IO_MODE_INPUT_PULLUP );
	ioSetMode( IO_PD1, IO_MODE_INPUT_PULLUP );

	// TWPS = 1 for PS0 = PS1 = 0
	TWSR = 0;
	
	// (SCL freq) = ( CPU_FREQ ) / ( 16 + 2*TWBR*4^TWPS ) where TWPS = 1
	TWBR = ( ( CONFIG_CPU_FREQ / freq ) - 16) / 8;
 
	// enable
	TWCR = 1 << TWEN;
}

void twiClean( )
{
	// terminate all previous operation
	// wait here if any?
	TWCR = 0;
	
	ioSetMode( IO_PD0, IO_MODE_INPUT );
	ioSetMode( IO_PD1, IO_MODE_INPUT );
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
static void twi_start( )
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWIE);
}

static void twi_stop( )
{
	TWCR = (1<<TWEN) | (1<<TWSTO) | (1<<TWINT);
	twiState = TWI_READY;
}

static void twi_next( uint8 ack )
{
	if( ack )		// send ack
	{
		TWCR = (1<<TWEN) | (1<<TWIE) | (1<<TWINT) | (1<<TWEA);
	}
	else
	{
		TWCR = (1<<TWEN) | (1<<TWIE) | (1<<TWINT);
	}
}

static void twi_abort( )
{
	TWCR = (1<<TWEN) | (1<<TWINT);
	twiState = TWI_READY;
}

/////////////////////////////////////////////////////////////////////
//	return : actual # of bytes written
/////////////////////////////////////////////////////////////////////
uint8 twiWrite( uint8 address, uint8* data, uint8 length )
{
	if( length > TWI_BUF_SIZE || length == 0 )
	{	
		twiError = TWI_ERR_BUF;
		return 0;
	}
	
	// wait until twi is ready
	while( twiState != TWI_READY );

	twiState	= TWI_MASTER_TX;
	twiError	= TWI_ERR_NONE;
	
	twiAddress	= ( address << 1 ) | 0;
	twiBufLength = length; 
	twiBufIndex  = 0;
	
	for( uint8 i = 0; i < length; ++i )
	{
		twiBuf[i] = data[i];
	}
  
	twi_start( );

	// wait for completion
	// CAN BE COMMENTED OUT
	while( twiState == TWI_MASTER_TX );
		
	return twiBufIndex;
}

/////////////////////////////////////////////////////////////////////
//	returns # of bytes read
/////////////////////////////////////////////////////////////////////
uint8 twiRead( uint8 address, uint8* buf, uint8 length )
{
	if( length > TWI_BUF_SIZE || length == 0 )
	{
		twiError = TWI_ERR_BUF;
		return 0;
	}

	// wait until twi is ready
	while( twiState != TWI_READY );
	
	twiState = TWI_MASTER_RX;
	twiError = TWI_ERR_NONE;

	twiAddress = ( address << 1 ) | 1;
	twiBufLength = length;
	twiBufIndex = 0;
	
	twi_start( );

	// wait for read operation to complete
	while( twiState == TWI_MASTER_RX );
	
	length = twiBufIndex;

	for( uint8 i = 0; i < length; ++i )
		buf[i] = twiBuf[i];
	
	return length;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
uint8 twiGetError( )
{
	return twiError;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define TWSR_START				0x08
#define TWSR_REP_START			0x10

#define TWSR_MTX_SLA_ACK		0x18
#define TWSR_MTX_SLA_NACK		0x20
#define TWSR_MTX_DATA_ACK		0x28
#define TWSR_MTX_DATA_NACK		0x30

#define TWSR_MRX_SLA_ACK		0x40
#define TWSR_MRX_SLA_NACK		0x48
#define TWSR_MRX_DATA_ACK		0x50
#define TWSR_MRX_DATA_NACK		0x58

#define TWSR_MTX_ARB_LOST		0x38
#define TWSR_NO_INFO			0xF8		// ?
#define TWSR_BUS_ERROR			0x00		// ?

#if 1
ISR( TWI_vect )
{
	switch( TWSR & ~7 )
	{
	
	case TWSR_START:				// start-ed
	case TWSR_REP_START:
		TWDR = twiAddress;
		twi_next( 0 );
		break;

	////////////////////////////////////////////////////////////////////
	//	master transmitter
	////////////////////////////////////////////////////////////////////
	case TWSR_MTX_SLA_ACK:					// slave ack-ed address
		TWDR = twiBuf[ twiBufIndex++ ];
		twi_next( 0 );
		break;
		
	case TWSR_MTX_DATA_ACK:					// slave ack-ed data
		if( twiBufIndex < twiBufLength )
		{
			TWDR = twiBuf[ twiBufIndex++ ];
			twi_next( 0 );
		}
		else
		{
			twi_stop( );
		}
		break;
		
	case TWSR_MTX_SLA_NACK:					// nack for address from slave
		twiError = TWI_ERR_SLA_NACK;
		twi_stop( );
		break;
		
	case TWSR_MTX_DATA_NACK:				// nack for data from slave
		twiError = TWI_ERR_DATA_NACK;
		twi_stop( );
		break;

	////////////////////////////////////////////////////////////////////
	//	master receiver
	////////////////////////////////////////////////////////////////////
	case TWSR_MRX_SLA_ACK:					// ack for address
		if( twiBufLength == 1 )
			twi_next( 0 );
		else
			twi_next( 1 );
		break;
	
	case TWSR_MRX_DATA_ACK:					// data received, ack sent ( total data length >= 2 )
		twiBuf[twiBufIndex++] = TWDR;
		if( twiBufIndex < twiBufLength-1 )
			twi_next( 1 );
		else
			twi_next( 0 );
		break;
		
	case TWSR_MRX_DATA_NACK:				// last byte
		twiBuf[twiBufIndex++] = TWDR;
		twi_stop();
		break;

	case TWSR_MRX_SLA_NACK:					// nack for address from slave
		twiError = TWI_ERR_SLA_NACK;
		twi_stop();
		break;

	// error
	case TWSR_MTX_ARB_LOST:					// lost bus arbitration
		twiError = TWI_ERR_ARB_LOST;
		twi_abort( );
		break;
	
	case TWSR_NO_INFO:						// no state information
		break;

	case TWSR_BUS_ERROR:					// bus error, illegal stop/start
		twiError = TWI_ERR_BUS;
		twi_stop();
		break;
	}
}

#endif