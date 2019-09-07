/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			system.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif

void delay( unsigned long );
void delayMicroseconds( unsigned int );
uint32 millis( );
void init(void);
void initVariant(void);
void setup( );
void loop( );

#ifdef __cplusplus
} // extern "C"
#endif

void serialEventRun(void) __attribute__((weak));

#define F_CPU	16000000

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


#endif
