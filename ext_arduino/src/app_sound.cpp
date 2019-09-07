
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			app_sound.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>

#include "config.h"
#include "core_io.h"
#include "core_timer.h"
#include "core_uart.h"
#include "core_pwm.h"

#include "app_sound.h"

//////////////////////////////////////////////////////////////////////
//	use OCB or OCC in the same timer to be used
//////////////////////////////////////////////////////////////////////

#if 0
#define SOUND_TIMER				TIMER1

#define SOUND_PWM_LEFT			PWM_OC1C
#define SOUND_PWM_RIGHT			PWM_OC1B

#define SOUND_OCRA				PWM_OC1A

#define SOUND_IO_LEFT			IO_PB6
#define SOUND_IO_RIGHT			IO_PB7
#define SOUND_INT				TIMER1_INT_COMPA

#else

#define SOUND_TIMER				TIMER5

#define SOUND_PWM_LEFT			PWM_OC5C
#define SOUND_PWM_RIGHT			PWM_OC5B

#define SOUND_OCRA				PWM_OC5A

#define SOUND_IO_LEFT			IO_PL4
#define SOUND_IO_RIGHT			IO_PL5
#define SOUND_INT				TIMER5_INT_COMPA


#endif

#define SOUND_STATE_IDLE		0
#define SOUND_STATE_START		1
#define SOUND_STATE_PLAYING		2
#define SOUND_STATE_END			3

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static void soundIsr( void );

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static SOUND_INFO	soundInfo;
static uint16		soundSlot;

static volatile int soundState;

static uint16		soundLeft, soundRight;
static uint16		soundLeft0, soundRight0;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void soundInit( )
{
	timerInit( SOUND_TIMER, TIMER_WGM_FAST_PWM_OCRA, TIMER_CS_1 );
	
	ioSetMode( SOUND_IO_LEFT, IO_MODE_OUTPUT_LOW );
	ioSetMode( SOUND_IO_RIGHT, IO_MODE_OUTPUT_LOW );
	
	pwmStart( SOUND_PWM_LEFT, PWM_MODE_NON_INVERTING );
	pwmStart( SOUND_PWM_RIGHT, PWM_MODE_NON_INVERTING );
	
	soundState = SOUND_STATE_IDLE;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void soundProcess( )
{
	// progress or etc...
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void soundStart( const SOUND_INFO* sinfo )
{
	uartPrintf( 0, "sound start : %d\r\n", soundState );
	if( soundState != SOUND_STATE_IDLE )
		return;
	
	soundState = SOUND_STATE_START;
	
	memcpy( &soundInfo, sinfo, sizeof( SOUND_INFO ) );
	soundSlot = 0;
	
	soundLeft = soundRight = 0;

	// linear interpolation 	
	if( soundInfo.bytesPerSample == 2 )
	{
		soundLeft0 = pgm_read_word_near( sinfo->data );
		
		if( sinfo->channels == 2 )
			soundRight0 = pgm_read_word_near( sinfo->data + 2 );
		else
			soundRight0 = soundLeft0;
	}
	else
	{
		soundLeft0 = pgm_read_byte_near( sinfo->data );
		if( sinfo->channels == 2 )
			soundRight0 = pgm_read_byte_near( sinfo->data + 1 );
		else
			soundRight0 = soundLeft0;
			
		soundLeft0 <<= sinfo->shift;
		soundRight0 <<= sinfo->shift;
	}
	
	//
	// OCR value : CONFIG_CPU_FREQ / sinfo->sampleRate - 1
	//
	pwmWrite( SOUND_OCRA, sinfo->OCR );
	timerEnableInt( SOUND_INT, soundIsr );
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static void soundIsr( )
{
	switch( soundState )
	{
	case SOUND_STATE_START:
		if( soundLeft < soundLeft0 )
			soundLeft++;
		
		if( soundRight < soundRight0 )
			soundRight++;
		
		if( soundLeft == soundLeft0 && soundRight == soundRight0 )
			soundState = SOUND_STATE_PLAYING;
		break;
		
	case SOUND_STATE_PLAYING:
		if( soundSlot >= soundInfo.slots )
		{
			soundState = SOUND_STATE_END;
		}
		else if( soundInfo.bytesPerSample == 2 )
		{
			soundLeft = pgm_read_word_near( soundInfo.data );
			soundInfo.data += 2;	
			
			if( soundInfo.channels == 2 )
			{
				soundRight = pgm_read_word_near( soundInfo.data );
				soundInfo.data += 2;
			}
			else
				soundRight = soundLeft;
		}
		else	// 8-bit sound
		{
			// fixme : add random noise to lower bits
				
			soundLeft = pgm_read_byte_near( soundInfo.data );
			soundLeft <<= soundInfo.shift;
			soundInfo.data++;
				
			if( soundInfo.channels == 2 )
			{
				soundRight = pgm_read_byte_near( soundInfo.data );
				soundRight <<= soundInfo.shift;
				soundInfo.data++;
			}
			else
				soundRight = soundLeft;
		}

		soundSlot++;
		break;
		
	case SOUND_STATE_END:
		if( soundLeft > 0 )
			soundLeft--;
			
		if( soundRight > 0 )
			soundRight--;
		
		if( soundLeft == 0 && soundRight == 0 )
		{
			soundState = SOUND_STATE_IDLE;
			timerDisableInt( SOUND_INT );
		}
		break;
	}

	pwmWrite( SOUND_PWM_LEFT, soundLeft );
	pwmWrite( SOUND_PWM_RIGHT, soundRight );
	
	#if 0
	if( soundState == SOUND_STATE_START || 
	    soundState == SOUND_STATE_END )
		uartPrintf( 0, "%d %d\r\n", soundLeft, soundRight );
	#endif
}

