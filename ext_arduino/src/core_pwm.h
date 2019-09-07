
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			core_pwm.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#ifndef __PWM_H
#define __PWM_H

#include "types.h"

//////////////////////////////////////////////////////////////////////
// [09:00]		OCRnx register address 	( n : 0~5, x : A/B/C )
// [11:10]		OCx						( x : A/B/C )
// [14:12]		timer no.
//////////////////////////////////////////////////////////////////////
#define __OCR_NO_SHIFT			10
#define __TIMER_NO_SHIFT		13

#define __OCR_MASK				0x03ff		// 10-bit
#define __OCR_NO_MASK			0x0003		//  2-bit
#define __TIMER_NO_MASK			0x0007		//  3-bit

#define __OCA					3
#define __OCB					2
#define __OCC					1

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define PWM_OC0A				( (0<<__TIMER_NO_SHIFT) | (__OCA<<__OCR_NO_SHIFT) | (0x047) )
#define PWM_OC0B				( (0<<__TIMER_NO_SHIFT) | (__OCB<<__OCR_NO_SHIFT) | (0x048) )

#define PWM_OC2A				( (2<<__TIMER_NO_SHIFT) | (__OCA<<__OCR_NO_SHIFT) | (0x0b3) )
#define PWM_OC2B				( (2<<__TIMER_NO_SHIFT) | (__OCB<<__OCR_NO_SHIFT) | (0x0b4) )

#define PWM_OC1A				( (1<<__TIMER_NO_SHIFT) | (__OCA<<__OCR_NO_SHIFT) | (0x088) )
#define PWM_OC1B				( (1<<__TIMER_NO_SHIFT) | (__OCB<<__OCR_NO_SHIFT) | (0x08a) )
#define PWM_OC1C				( (1<<__TIMER_NO_SHIFT) | (__OCC<<__OCR_NO_SHIFT) | (0x08c) )

#define PWM_OC3A				( (3<<__TIMER_NO_SHIFT) | (__OCA<<__OCR_NO_SHIFT) | (0x098) )
#define PWM_OC3B				( (3<<__TIMER_NO_SHIFT) | (__OCB<<__OCR_NO_SHIFT) | (0x09a) )
#define PWM_OC3C				( (3<<__TIMER_NO_SHIFT) | (__OCC<<__OCR_NO_SHIFT) | (0x09c) )

#define PWM_OC4A				( (4<<__TIMER_NO_SHIFT) | (__OCA<<__OCR_NO_SHIFT) | (0x0a8) )
#define PWM_OC4B				( (4<<__TIMER_NO_SHIFT) | (__OCB<<__OCR_NO_SHIFT) | (0x0aa) )
#define PWM_OC4C				( (4<<__TIMER_NO_SHIFT) | (__OCC<<__OCR_NO_SHIFT) | (0x0ac) )

#define PWM_OC5A				( (5<<__TIMER_NO_SHIFT) | (__OCA<<__OCR_NO_SHIFT) | (0x128) )
#define PWM_OC5B				( (5<<__TIMER_NO_SHIFT) | (__OCB<<__OCR_NO_SHIFT) | (0x12a) )
#define PWM_OC5C				( (5<<__TIMER_NO_SHIFT) | (__OCC<<__OCR_NO_SHIFT) | (0x12c) )

/////////////////////////////////////////////////////////////////////
//	PWM mode ( Compare Output Mode )
/////////////////////////////////////////////////////////////////////
#define PWM_MODE_NONE					0		// disconnect
#define PWM_MODE_TOGGLE					1
#define PWM_MODE_NON_INVERTING			2		// clear-on-match
#define PWM_MODE_INVERTING				3		// set-on-match

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void pwmStart( uint16 pwmPort, uint8 mode );
void pwmStop( uint16 pwmPort );

void pwmWrite( uint16 pwmPort, uint16 val );

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#endif



