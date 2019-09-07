
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			core_timer.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#ifndef __TIMER_H
#define __TIMER_H

#include "types.h"

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
typedef void (*TIMER_ISR)( void );

/////////////////////////////////////////////////////////////////////
//
//	timer 0, 2			: 8-bit timer
// 	timer 1, 3, 4, 5	: 16-bit timer
//
//	definition format
//	[15:13]		timer no
//	[09:00]		TCCRA (memory) address
//
/////////////////////////////////////////////////////////////////////
#define __TIMER_NO_SHIFT				13

#define __TCCRA_MSK						0x03ff
#define __TIMER_NO_MSK					0x7

#define TIMER0							((0<<__TIMER_NO_SHIFT)|0x044)
#define TIMER1							((1<<__TIMER_NO_SHIFT)|0x080)
#define TIMER2							((2<<__TIMER_NO_SHIFT)|0x0b0)
#define TIMER3							((3<<__TIMER_NO_SHIFT)|0x090)
#define TIMER4							((4<<__TIMER_NO_SHIFT)|0x0a0)
#define TIMER5							((5<<__TIMER_NO_SHIFT)|0x120)

/////////////////////////////////////////////////////////////////////
//		wgm
/////////////////////////////////////////////////////////////////////

// timer 0, 2
// 4, 5 reserved
#define TIMER02_WGM_NORMAL				0
#define TIMER02_WGM_PHASE_PWM			1
#define TIMER02_WGM_CTC					2
#define TIMER02_WGM_FAST_PWM			3
#define TIMER02_WGM_PHASE_PWM_OCRA		5
#define TIMER02_WGM_FAST_PWM_OCRA		7

// timer 1, 3, 4, 5
// 13 reserved
#define TIMER_WGM_NORMAL				0
#define TIMER_WGM_PHASE_PWM_8BIT		1
#define TIMER_WGM_PHASE_PWM_9BIT		2
#define TIMER_WGM_PHASE_PWM_10BIT		3
#define TIMER_WGM_CTC_OCRA				4
#define TIMER_WGM_FAST_PWM_8BIT			5
#define TIMER_WGM_FAST_PWM_9BIT			6
#define TIMER_WGM_FAST_PWM_10BIT		7
#define TIMER_WGM_PHASE_FREQ_PWM_ICR	8
#define TIMER_WGM_PHASE_FREQ_PWM_OCRA	9
#define TIMER_WGM_PHASE_PWM_ICR			10
#define TIMER_WGM_PHASE_PWM_OCRA		11
#define TIMER_WGM_CTC_ICR				12
#define TIMER_WGM_FAST_PWM_ICR			14
#define TIMER_WGM_FAST_PWM_OCRA			15


/////////////////////////////////////////////////////////////////////
//		clock select
/////////////////////////////////////////////////////////////////////

// timer 0, 1, 3, 4, 5
#define TIMER_CS_NONE					0
#define TIMER_CS_1						1
#define TIMER_CS_8						2
#define TIMER_CS_64						3
#define TIMER_CS_256					4
#define TIMER_CS_1024					5
#define TIMER_CS_T_FALLING				6
#define TIMER_CS_T_RISING				7

// timer 2
#define TIMER2_CS_NONE					0
#define TIMER2_CS_1						1
#define TIMER2_CS_8						2
#define TIMER2_CS_32					3
#define TIMER2_CS_64					4
#define TIMER2_CS_128					5
#define TIMER2_CS_256					6
#define TIMER2_CS_1024					7

/////////////////////////////////////////////////////////////////////
//		interrupt mask/flag
/////////////////////////////////////////////////////////////////////
#define TIMER_ICP_EDGE_FALLING			0x00
#define TIMER_ICP_EDGE_RISING			0x40
#define TIMER_ICP_ENABLE_NC				0x80	// noise canceller

/////////////////////////////////////////////////////////////////////
//
//	[07:00]		interrupt flag
//	[12:08]		interrupt no.
//	[15:13]		timer no.
//
/////////////////////////////////////////////////////////////////////
#define __TINT_TOV					0x01	// all
#define __TINT_OCA					0x02	// all
#define __TINT_OCB					0x04	// all
#define __TINT_OCC					0x08	// 1345
#define __TINT_ICP					0x20	// 1345

#define __T2INT_COMPA				0
#define __T2INT_COMPB				1
#define __T2INT_OVF					2

#define __T1INT_CAPT				3
#define __T1INT_COMPA				4
#define __T1INT_COMPB				5
#define __T1INT_COMPC				6
#define __T1INT_OVF					7

#define __T0INT_COMPA				8
#define __T0INT_COMPB				9
#define __T0INT_OVF					10

#define __T3INT_CAPT				11
#define __T3INT_COMPA				12
#define __T3INT_COMPB				13
#define __T3INT_COMPC				14
#define __T3INT_OVF					15

#define __T4INT_CAPT				16
#define __T4INT_COMPA				17
#define __T4INT_COMPB				18
#define __T4INT_COMPC				19
#define __T4INT_OVF					20

#define __T5INT_CAPT				21
#define __T5INT_COMPA				22
#define __T5INT_COMPB				23
#define __T5INT_COMPC				24
#define __T5INT_OVF					25

#define TIMER2_INT_COMPA			((2<<__TIMER_NO_SHIFT)|(__T2INT_COMPA<<8)|__TINT_OCA)
#define TIMER2_INT_COMPB			((2<<__TIMER_NO_SHIFT)|(__T2INT_COMPB<<8)|__TINT_OCB)
#define TIMER2_INT_OVF				((2<<__TIMER_NO_SHIFT)|(__T2INT_OVF  <<8)|__TINT_TOV)

#define	TIMER1_INT_CAPT				((1<<__TIMER_NO_SHIFT)|(__T1INT_CAPT <<8)|__TINT_ICP)
#define TIMER1_INT_COMPA			((1<<__TIMER_NO_SHIFT)|(__T1INT_COMPA<<8)|__TINT_OCA)
#define TIMER1_INT_COMPB			((1<<__TIMER_NO_SHIFT)|(__T1INT_COMPB<<8)|__TINT_OCB)
#define TIMER1_INT_COMPC			((1<<__TIMER_NO_SHIFT)|(__T1INT_COMPC<<8)|__TINT_OCC)
#define TIMER1_INT_OVF				((1<<__TIMER_NO_SHIFT)|(__T1INT_OVF  <<8)|__TINT_TOV)

#define TIMER0_INT_COMPA			((0<<__TIMER_NO_SHIFT)|(__T0INT_COMPA<<8)|__TINT_OCA)
#define TIMER0_INT_COMPB			((0<<__TIMER_NO_SHIFT)|(__T0INT_COMPB<<8)|__TINT_OCB)
#define TIMER0_INT_OVF				((0<<__TIMER_NO_SHIFT)|(__T0INT_OVF  <<8)|__TINT_TOV)

#define	TIMER3_INT_CAPT				((3<<__TIMER_NO_SHIFT)|(__T3INT_CAPT <<8)|__TINT_ICP)
#define TIMER3_INT_COMPA			((3<<__TIMER_NO_SHIFT)|(__T3INT_COMPA<<8)|__TINT_OCA)
#define TIMER3_INT_COMPB			((3<<__TIMER_NO_SHIFT)|(__T3INT_COMPB<<8)|__TINT_OCB)
#define TIMER3_INT_COMPC			((3<<__TIMER_NO_SHIFT)|(__T3INT_COMPC<<8)|__TINT_OCC)
#define TIMER3_INT_OVF				((3<<__TIMER_NO_SHIFT)|(__T3INT_OVF  <<8)|__TINT_TOV)

#define	TIMER4_INT_CAPT				((4<<__TIMER_NO_SHIFT)|(__T4INT_CAPT <<8)|__TINT_ICP)
#define TIMER4_INT_COMPA			((4<<__TIMER_NO_SHIFT)|(__T4INT_COMPA<<8)|__TINT_OCA)
#define TIMER4_INT_COMPB			((4<<__TIMER_NO_SHIFT)|(__T4INT_COMPB<<8)|__TINT_OCB)
#define TIMER4_INT_COMPC			((4<<__TIMER_NO_SHIFT)|(__T4INT_COMPC<<8)|__TINT_OCC)
#define TIMER4_INT_OVF				((4<<__TIMER_NO_SHIFT)|(__T4INT_OVF  <<8)|__TINT_TOV)

#define	TIMER5_INT_CAPT				((5<<__TIMER_NO_SHIFT)|(__T5INT_CAPT <<8)|__TINT_ICP)
#define TIMER5_INT_COMPA			((5<<__TIMER_NO_SHIFT)|(__T5INT_COMPA<<8)|__TINT_OCA)
#define TIMER5_INT_COMPB			((5<<__TIMER_NO_SHIFT)|(__T5INT_COMPB<<8)|__TINT_OCB)
#define TIMER5_INT_COMPC			((5<<__TIMER_NO_SHIFT)|(__T5INT_COMPC<<8)|__TINT_OCC)
#define TIMER5_INT_OVF				((5<<__TIMER_NO_SHIFT)|(__T5INT_OVF  <<8)|__TINT_TOV)

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void timerInit( uint16 timer, uint8 wgm, uint8 cs, uint8 icnc = 0, uint8 ices = 0 );

void timerEnableInt( uint16 tint, TIMER_ISR isr );
void timerDisableInt( uint16 tint );

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#endif	// __TIMER_H