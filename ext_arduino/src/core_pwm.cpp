
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			core_pwm.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#include <avr/io.h>
#include "core_pwm.h"

#define OCR_REG( port )			( (port) & __OCR_MASK )
#define OCR_NO( port )			( ((port)>> __OCR_NO_SHIFT) & __OCR_NO_MASK )
#define TIMER_NO( port )		( ((port)>> __TIMER_NO_SHIFT) & __TIMER_NO_MASK ) 

/////////////////////////////////////////////////////////////////////
//
//	Note
//	1. val range
//		8-bit for timer0, 2
//		16-bit for timer1, 3, 4, 5
//
/////////////////////////////////////////////////////////////////////
void pwmWrite( uint16 port, uint16 val )
{	
	uint16 	reg 	= OCR_REG( port );
	uint8	timer 	= TIMER_NO( port );
	
	if( timer == 0 || timer == 2 )
		*((volatile uint8*)reg) = (uint8)val;
	else
		*((volatile uint16*)reg) = val;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void pwmStart( uint16 port, uint8 mode )
{
	uint8 timer = TIMER_NO( port );
	uint8 abc 	= OCR_NO( port );
	
	uint8 com	= mode << ( abc*2 );
	uint8 mask	= ~(3 << ( abc*2 ));
	
	if( timer == 0 )
		TCCR0A = ( TCCR0A & mask ) | com;
	else if( timer == 1 )
		TCCR1A = ( TCCR1A & mask ) | com;
	else if( timer == 2 )
		TCCR2A = ( TCCR2A & mask ) | com;
	else if( timer == 3 )
		TCCR3A = ( TCCR3A & mask ) | com;
	else if( timer == 4 )
		TCCR4A = ( TCCR4A & mask ) | com;
	else if( timer == 5 )
		TCCR5A = ( TCCR5A & mask ) | com;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void pwmStop( uint16 port )
{
	uint8 timer = TIMER_NO( port );
	uint8 abc 	= OCR_NO( port );
	
	uint8 mask	= ~(3 << (abc*2));
	
	if( timer == 0 )
		TCCR0A &= ( mask );
	else if( timer == 1 )
		TCCR1A &= ( mask );
	else if( timer == 2 )
		TCCR2A &= ( mask );
	else if( timer == 3 )
		TCCR3A &= ( mask );
	else if( timer == 4 )
		TCCR4A &= ( mask );
	else if( timer == 5 )
		TCCR5A &= ( mask );
}