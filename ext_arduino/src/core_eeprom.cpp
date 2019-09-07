
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			eeprom.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "core_eeprom.h"

//////////////////////////////////////////////////////////////////////
//	fixme 
//	- return value
//////////////////////////////////////////////////////////////////////
void eepromWrite( uint16 addr, uint8 data )
{
	// wait for previous write complete
	while( EECR & ( 1<<EEPE ) );
	
	uint8 oldSREG = SREG;
	cli( );

	EEAR = addr;
	EEDR = data;
	EECR |= ( 1<<EEMPE );
	EECR |= ( 1<<EEPE );
	
	SREG = oldSREG;
}

bool eepromWrite( uint16 addr, uint8* dat, int len )
{
	while( len-- )
	{
		// no failure ?
		eepromWrite( addr++, *dat++ );
	}
	
	return true;
}

bool eepromErase( uint16 addr, int len, uint8 fill )
{
	while( len-- )
	{
		eepromWrite( addr++, fill );
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
uint8 eepromRead( uint16 addr )
{
	// wait for previous read complete
	while( EECR & ( 1<<EEPE ) );
	
	uint8 oldSREG = SREG;
	cli( );
	
	EEAR = addr;
	EECR |= ( 1<<EERE );

	uint8 c = EEDR;
	SREG = oldSREG;

	return c;
}

bool eepromRead( uint16 addr, uint8* buf, int len )
{
	while( len-- )
	{
		*buf++ = eepromRead( addr++ );
	}	
	return true;
}


