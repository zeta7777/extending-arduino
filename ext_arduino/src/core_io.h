
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			core_io.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#ifndef __CORE_IO_H
#define __CORE_IO_H

#include "types.h"

/////////////////////////////////////////////////////////////////////
// 
// definition format (uint32)
//
// [09:00]		PINx register address	( x : A ~ L )
// [15:13]		port no.	
//
// NOTE
// 1. no PG6, PG7
//
/////////////////////////////////////////////////////////////////////
#define __PORT_NO_SHIFT			13

#define __PIN_MASK				0x03ff		// 10-bit
#define __PORT_NO_MASK			0x0007		//  3-bit


//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define IO_PA0					( (0<<__PORT_NO_SHIFT) | (0x20) )
#define IO_PA1					( (1<<__PORT_NO_SHIFT) | (0x20) )
#define IO_PA2					( (2<<__PORT_NO_SHIFT) | (0x20) )
#define IO_PA3					( (3<<__PORT_NO_SHIFT) | (0x20) )
#define IO_PA4					( (4<<__PORT_NO_SHIFT) | (0x20) )
#define IO_PA5					( (5<<__PORT_NO_SHIFT) | (0x20) )
#define IO_PA6					( (6<<__PORT_NO_SHIFT) | (0x20) )
#define IO_PA7					( (7<<__PORT_NO_SHIFT) | (0x20) )

#define IO_PB0					( (0<<__PORT_NO_SHIFT) | (0x23) )
#define IO_PB1					( (1<<__PORT_NO_SHIFT) | (0x23) )
#define IO_PB2					( (2<<__PORT_NO_SHIFT) | (0x23) )
#define IO_PB3					( (3<<__PORT_NO_SHIFT) | (0x23) )
#define IO_PB4					( (4<<__PORT_NO_SHIFT) | (0x23) )
#define IO_PB5					( (5<<__PORT_NO_SHIFT) | (0x23) )
#define IO_PB6					( (6<<__PORT_NO_SHIFT) | (0x23) )
#define IO_PB7					( (7<<__PORT_NO_SHIFT) | (0x23) )

#define IO_PC0					( (0<<__PORT_NO_SHIFT) | (0x26) )
#define IO_PC1					( (1<<__PORT_NO_SHIFT) | (0x26) )
#define IO_PC2					( (2<<__PORT_NO_SHIFT) | (0x26) )
#define IO_PC3					( (3<<__PORT_NO_SHIFT) | (0x26) )
#define IO_PC4					( (4<<__PORT_NO_SHIFT) | (0x26) )
#define IO_PC5					( (5<<__PORT_NO_SHIFT) | (0x26) )
#define IO_PC6					( (6<<__PORT_NO_SHIFT) | (0x26) )
#define IO_PC7					( (7<<__PORT_NO_SHIFT) | (0x26) )

#define IO_PD0					( (0<<__PORT_NO_SHIFT) | (0x29) )
#define IO_PD1					( (1<<__PORT_NO_SHIFT) | (0x29) )
#define IO_PD2					( (2<<__PORT_NO_SHIFT) | (0x29) )
#define IO_PD3					( (3<<__PORT_NO_SHIFT) | (0x29) )
#define IO_PD4					( (4<<__PORT_NO_SHIFT) | (0x29) )
#define IO_PD5					( (5<<__PORT_NO_SHIFT) | (0x29) )
#define IO_PD6					( (6<<__PORT_NO_SHIFT) | (0x29) )
#define IO_PD7					( (7<<__PORT_NO_SHIFT) | (0x29) )

#define IO_PE0					( (0<<__PORT_NO_SHIFT) | (0x2c) )
#define IO_PE1					( (1<<__PORT_NO_SHIFT) | (0x2c) )
#define IO_PE2					( (2<<__PORT_NO_SHIFT) | (0x2c) )
#define IO_PE3					( (3<<__PORT_NO_SHIFT) | (0x2c) )
#define IO_PE4					( (4<<__PORT_NO_SHIFT) | (0x2c) )
#define IO_PE5					( (5<<__PORT_NO_SHIFT) | (0x2c) )
#define IO_PE6					( (6<<__PORT_NO_SHIFT) | (0x2c) )       
#define IO_PE7					( (7<<__PORT_NO_SHIFT) | (0x2c) )

#define IO_PF0					( (0<<__PORT_NO_SHIFT) | (0x2f) )
#define IO_PF1					( (1<<__PORT_NO_SHIFT) | (0x2f) )
#define IO_PF2					( (2<<__PORT_NO_SHIFT) | (0x2f) )
#define IO_PF3					( (3<<__PORT_NO_SHIFT) | (0x2f) )
#define IO_PF4					( (4<<__PORT_NO_SHIFT) | (0x2f) )
#define IO_PF5					( (5<<__PORT_NO_SHIFT) | (0x2f) )
#define IO_PF6					( (6<<__PORT_NO_SHIFT) | (0x2f) )
#define IO_PF7					( (7<<__PORT_NO_SHIFT) | (0x2f) )

#define IO_PG0					( (0<<__PORT_NO_SHIFT) | (0x32) )
#define IO_PG1					( (1<<__PORT_NO_SHIFT) | (0x32) )
#define IO_PG2					( (2<<__PORT_NO_SHIFT) | (0x32) )
#define IO_PG3					( (3<<__PORT_NO_SHIFT) | (0x32) )
#define IO_PG4					( (4<<__PORT_NO_SHIFT) | (0x32) )
#define IO_PG5					( (5<<__PORT_NO_SHIFT) | (0x32) )
                                                 
#define IO_PH0					( (0<<__PORT_NO_SHIFT) | (0x100) )
#define IO_PH1					( (1<<__PORT_NO_SHIFT) | (0x100) )
#define IO_PH2					( (2<<__PORT_NO_SHIFT) | (0x100) )
#define IO_PH3					( (3<<__PORT_NO_SHIFT) | (0x100) )
#define IO_PH4					( (4<<__PORT_NO_SHIFT) | (0x100) )
#define IO_PH5					( (5<<__PORT_NO_SHIFT) | (0x100) )
#define IO_PH6					( (6<<__PORT_NO_SHIFT) | (0x100) )
#define IO_PH7					( (7<<__PORT_NO_SHIFT) | (0x100) )
                                      
#define IO_PJ0					( (0<<__PORT_NO_SHIFT) | (0x103) )
#define IO_PJ1					( (1<<__PORT_NO_SHIFT) | (0x103) )
#define IO_PJ2					( (2<<__PORT_NO_SHIFT) | (0x103) )
#define IO_PJ3					( (3<<__PORT_NO_SHIFT) | (0x103) )
#define IO_PJ4					( (4<<__PORT_NO_SHIFT) | (0x103) )
#define IO_PJ5					( (5<<__PORT_NO_SHIFT) | (0x103) )
#define IO_PJ6					( (6<<__PORT_NO_SHIFT) | (0x103) )
#define IO_PJ7					( (7<<__PORT_NO_SHIFT) | (0x103) )

#define IO_PK0					( (0<<__PORT_NO_SHIFT) | (0x106) )
#define IO_PK1					( (1<<__PORT_NO_SHIFT) | (0x106) )
#define IO_PK2					( (2<<__PORT_NO_SHIFT) | (0x106) )
#define IO_PK3					( (3<<__PORT_NO_SHIFT) | (0x106) )
#define IO_PK4					( (4<<__PORT_NO_SHIFT) | (0x106) )
#define IO_PK5					( (5<<__PORT_NO_SHIFT) | (0x106) )
#define IO_PK6					( (6<<__PORT_NO_SHIFT) | (0x106) )
#define IO_PK7					( (7<<__PORT_NO_SHIFT) | (0x106) )

#define IO_PL0					( (0<<__PORT_NO_SHIFT) | (0x109) )
#define IO_PL1					( (1<<__PORT_NO_SHIFT) | (0x109) )
#define IO_PL2					( (2<<__PORT_NO_SHIFT) | (0x109) )
#define IO_PL3					( (3<<__PORT_NO_SHIFT) | (0x109) )
#define IO_PL4					( (4<<__PORT_NO_SHIFT) | (0x109) )
#define IO_PL5					( (5<<__PORT_NO_SHIFT) | (0x109) )
#define IO_PL6					( (6<<__PORT_NO_SHIFT) | (0x109) )
#define IO_PL7					( (7<<__PORT_NO_SHIFT) | (0x109) )

/////////////////////////////////////////////////////////////////////
// input/output mode
//
// format : 0bDP
//
// 		D [1] : DDRxn
// 		P [0] : PORTxn
//
/////////////////////////////////////////////////////////////////////
#define IO_MODE_INPUT			0		// 0b00
#define IO_MODE_INPUT_PULLUP	1		// 0b01
#define IO_MODE_OUTPUT_LOW		2		// 0b10
#define IO_MODE_OUTPUT_HIGH		3		// 0b11

#define IO_MODE_TRI_STATE		IO_INPUT

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define IO_LOW					0
#define IO_HIGH					1

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void	ioSetMode( uint16 port, uint8 mode );
void	ioWrite( uint16 port, uint8 val );
uint8	ioRead( uint16 port );
void	ioToggle( uint16 port );

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#endif	// __CORE_IO_H

