
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			core_io.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "core_io.h"

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

// PINx register address related with "port"
#define PIN_REG( port )			( (volatile uint8*)( (port) & __PIN_MASK ) )

// port no. ( 0 ~ 7 )
#define PORT_NO( port )			( ( (port)>>__PORT_NO_SHIFT ) & __PORT_NO_MASK )

// DIRx, PORTx offset from PINx
#define OFFSET_DIR			1
#define OFFSET_PORT			2

/////////////////////////////////////////////////////////////////////
//	
/////////////////////////////////////////////////////////////////////
void ioSetMode( uint16 port, uint8 mode )
{
	uint8 bit = ( 1 << PORT_NO( port ) );
	
	// reg points to PORTx
	volatile uint8* dirReg	= PIN_REG( port ) + OFFSET_DIR;
	volatile uint8* portReg = dirReg + 1;
		
	uint8 oldSREG = SREG;
	cli();

	// pull-up ( in case of INPUT )
	// initial output value ( in case of OUTPUT )	
	if( mode & 1 )
		*portReg |= bit;		
	else
		*portReg &= ~bit;		
	
	// set-up port direction
	if( mode & 2 )
		*dirReg |= bit;		// output
	else
		*dirReg &= ~bit;		// input
	
	SREG = oldSREG;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void ioWrite( uint16 port, uint8 val )
{
	uint8 bit = ( 1 << PORT_NO( port ) );
	
	// PORTx
	volatile uint8* portReg = PIN_REG( port ) + OFFSET_PORT;
	
	if( val )
		*portReg |= bit;		// HIGH
	else
		*portReg &= ~bit;		// LOW
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
uint8 ioRead( uint16 port )
{
	// PINx
	volatile uint8* pinReg = PIN_REG( port );
	
	return ( (*pinReg) >> PORT_NO( port ) ) & 1;
}

/////////////////////////////////////////////////////////////////////
//  toggling : writing 1 to PINxn, independent of DDRxn value
/////////////////////////////////////////////////////////////////////
void ioToggle( uint16 port )
{
	// PINx
	volatile uint8* pinReg = PIN_REG( port );
	
	// just write logic '1' to the coressponding bit 
	// ( no need of "read and write" because writing 0 has no meaning )
	*pinReg = ( 1 << PORT_NO( port ) );
}
