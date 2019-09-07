/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			core_eint.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "core_eint.h"
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static EINT_ISR	eintIsr[8];

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void eintEnable( uint8 eintNo, uint8 sense, EINT_ISR isr )
{
	if( eintNo > 7 )
		return;
		
	uint8 v, s;
	
	if( eintNo < 4 )
	{
		v = EICRA;
		s = eintNo * 2;
		EICRA = ( v & ~(3<<s) ) | (sense<<s);
	}
	else
	{
		v = EICRB;
		s = (eintNo-4) * 2;
		EICRB = ( v & ~(3<<s) ) | (sense<<s);
	}
	
	EIFR |= (1<<eintNo);
	EIMSK |= (1<<eintNo);
	
	eintIsr[ eintNo ] = isr;
}


void eintDisable( uint8 eintNo )
{
	if( eintNo > 7 )
		return;
		
	EIMSK &= ~(1<<eintNo);
	EIFR |= (1<<eintNo);
	
	eintIsr[ eintNo ] = 0;
}


//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
ISR( INT0_vect )
{
	if( eintIsr[0] )
		eintIsr[0]( 0 );
}

ISR( INT1_vect )
{
	if( eintIsr[1] )
		eintIsr[1]( 1 );
}

ISR( INT2_vect )
{
	if( eintIsr[2] )
	eintIsr[2]( 2 );
}

ISR( INT3_vect )
{
	if( eintIsr[3] )
		eintIsr[3]( 3 );
}

ISR( INT4_vect )
{
	if( eintIsr[4] )
		eintIsr[4]( 4 );
}

ISR( INT5_vect )
{
	if( eintIsr[5] )
		eintIsr[5]( 5 );
}

ISR( INT6_vect )
{
	if( eintIsr[6] )
		eintIsr[6]( 6 );
}

ISR( INT7_vect )
{
	if( eintIsr[7] )
		eintIsr[7]( 7 );
}


