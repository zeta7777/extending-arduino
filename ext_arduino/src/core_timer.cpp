
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			core_timer.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "core_timer.h"

/*
	timer interrupt source
	
	timer0, timer2
		compa, compb, ovf
	timer1,3,4,5
		capt, compa, compb, compc, ovf
		
	2x3 + 4*5 = 26
	
	( function pointer size ) * 26 = 52 bytes consumed to 
	implement timer interrupt redirection
*/

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#if CONFIG_TIMER_ISR_REDIRECT==1
static TIMER_ISR	timerIsr[26];
#endif

/////////////////////////////////////////////////////////////////////
//
//	[todo]	include ACSR:ACIC for Timer1 ICP
//
/////////////////////////////////////////////////////////////////////
#define TIMER_NO( t ) 		( ( (t) >> __TIMER_NO_SHIFT ) & __TIMER_NO_MSK )

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void timerInit( uint16 timer, uint8 wgm, uint8 cs, uint8 icnc, uint8 ices )
{
	volatile uint8* reg = (uint8*)( timer & __TCCRA_MSK );
	uint8 			no 	= TIMER_NO( timer );
	
	if( no == 0 || no == 2 )
	{
		*reg = ( wgm & 3 );
		*(reg+1) = (( wgm & 4 ) << 1 ) | ( cs );
	}
	else		// 1, 3, 4, 5
	{
		*reg = ( wgm & 3 );
		*(reg+1) = icnc | ices | (( wgm & 0x0c ) << 1 ) | ( cs );
	}
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define TIMER_INT_NO( tint )		(( (tint)>>8 ) & 0x1f )
#define TIMER_INT_FLAG( tint )		( (tint) & 0xff )

void timerEnableInt( uint16 tint, TIMER_ISR isr )
{
	volatile uint8* reg = &TIMSK0 + TIMER_NO( tint );
	
#if CONFIG_TIMER_ISR_REDIRECT==1	
	timerIsr[ TIMER_INT_NO( tint ) ] = isr;
#endif	

	*reg |= (uint8)TIMER_INT_FLAG( tint );
}

void timerDisableInt( uint16 tint )
{
	volatile uint8* reg = &TIMSK0 + TIMER_NO( tint );

#if CONFIG_TIMER_ISR_REDIRECT==1	
	timerIsr[ TIMER_INT_NO( tint ) ] = 0;
#endif

	*reg &= ( ~((uint8)TIMER_INT_FLAG( tint ) ) );
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void timerSync( )
{
	GTCCR = 0x83;
	GTCCR = 0x00;
}

#if CONFIG_TIMER_ISR_REDIRECT==1

// FIXME : USE MACRO

//////
ISR( TIMER2_COMPA_vect ) {
	if( timerIsr[__T2INT_COMPA] )
		timerIsr[__T2INT_COMPA]( );
}

ISR( TIMER2_COMPB_vect ) {
	if( timerIsr[__T2INT_COMPB] )
		timerIsr[__T2INT_COMPB]( );
}

ISR( TIMER2_OVF_vect ) {
	if( timerIsr[__T2INT_OVF] )
		timerIsr[__T2INT_OVF]( );
}

//////
ISR( TIMER1_CAPT_vect ) {
	if( timerIsr[__T1INT_CAPT] )
		timerIsr[__T1INT_CAPT]( );
}

ISR( TIMER1_COMPA_vect ) {		
	if( timerIsr[__T1INT_COMPA] )
		timerIsr[__T1INT_COMPA]( );
}

ISR( TIMER1_COMPB_vect ) {
	if( timerIsr[__T1INT_COMPB] )
		timerIsr[__T1INT_COMPB]( );
}

ISR( TIMER1_COMPC_vect ) {
	if( timerIsr[__T1INT_COMPC] )
		timerIsr[__T1INT_COMPC]( );
}

ISR( TIMER1_OVF_vect ) {
	if( timerIsr[__T1INT_OVF] )
		timerIsr[__T1INT_OVF]( );
}

/////
ISR( TIMER0_COMPA_vect ) {
	if( timerIsr[__T0INT_COMPA] )
	timerIsr[__T0INT_COMPA]( );
}

ISR( TIMER0_COMPB_vect ) {
	if( timerIsr[__T0INT_COMPB] )
	timerIsr[__T0INT_COMPB]( );
}

/* wiring.c use this vector
ISR( TIMER0_OVF_vect ) {
	if( timerIsr[__T0INT_OVF] )
	timerIsr[__T0INT_OVF]( );
}
*/

/////
ISR( TIMER3_CAPT_vect ) {
	if( timerIsr[__T3INT_CAPT] )
	timerIsr[__T3INT_CAPT]( );
}

ISR( TIMER3_COMPA_vect ) {
	if( timerIsr[__T3INT_COMPA] )
	timerIsr[__T3INT_COMPA]( );
}

ISR( TIMER3_COMPB_vect ) {
	if( timerIsr[__T3INT_COMPB] )
		timerIsr[__T3INT_COMPB]( );
}

ISR( TIMER3_COMPC_vect ) {
	if( timerIsr[__T3INT_COMPC] )
		timerIsr[__T3INT_COMPC]( );
}

ISR( TIMER3_OVF_vect ) {
	if( timerIsr[__T3INT_OVF] )
		timerIsr[__T3INT_OVF]( );
}

//////
ISR( TIMER4_CAPT_vect ) {
	if( timerIsr[__T4INT_CAPT] )
	timerIsr[__T4INT_CAPT]( );
}

ISR( TIMER4_COMPA_vect ) {
	if( timerIsr[__T4INT_COMPA] )
		timerIsr[__T4INT_COMPA]( );
}

ISR( TIMER4_COMPB_vect ) {
	if( timerIsr[__T4INT_COMPB] )
		timerIsr[__T4INT_COMPB]( );
}

ISR( TIMER4_COMPC_vect ) {
	if( timerIsr[__T4INT_COMPC] )
		timerIsr[__T4INT_COMPC]( );
}

ISR( TIMER4_OVF_vect ) {
	if( timerIsr[__T4INT_OVF] )
		timerIsr[__T4INT_OVF]( );
}

//
ISR( TIMER5_CAPT_vect ) {
	if( timerIsr[__T5INT_CAPT] )
		timerIsr[__T5INT_CAPT]( );
}

ISR( TIMER5_COMPA_vect ) {
	if( timerIsr[__T5INT_COMPA] )
		timerIsr[__T5INT_COMPA]( );
}

ISR( TIMER5_COMPB_vect ) {
	if( timerIsr[__T5INT_COMPB] )
		timerIsr[__T5INT_COMPB]( );
}

ISR( TIMER5_COMPC_vect ) {
	if( timerIsr[__T5INT_COMPC] )
		timerIsr[__T5INT_COMPC]( );
}

ISR( TIMER5_OVF_vect ) {
	if( timerIsr[__T5INT_OVF] )
		timerIsr[__T5INT_OVF]( );
}

#endif