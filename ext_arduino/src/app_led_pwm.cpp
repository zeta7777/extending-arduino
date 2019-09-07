/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			app_led_pwm.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include "core_io.h"
#include "core_pwm.h"
#include "core_timer.h"
#include "app_led_pwm.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define PWM_LED1			PWM_OC1A
#define PWM_LED2			PWM_OC2A
#define PWM_LED3			PWM_OC2B
#define PWM_LED4			PWM_OC4C
#define PWM_LED5			PWM_OC4B
#define PWM_LED6			PWM_OC4A
#define PWM_LED7			PWM_OC3A
#define PWM_LED8			PWM_OC3C

#define IO_LED1				IO_PB5
#define IO_LED2				IO_PB4
#define IO_LED3				IO_PH6
#define IO_LED4				IO_PH5
#define IO_LED5				IO_PH4
#define IO_LED6				IO_PH3
#define IO_LED7				IO_PE3
#define IO_LED8				IO_PE5

#define LED_COUNT			8

//////////////////////////////////////////////////////////////////////
//	for PWM mode LED
//////////////////////////////////////////////////////////////////////
#define MAX_PWM				240
#define MIN_PWM				0

#define STEP				( ( MAX_PWM - MIN_PWM ) / LED_COUNT )

#define STATE_INC 			0
#define STATE_DEC			1
#define STATE_WAIT			2

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static void ledPWM( );

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static uint16 ioPort[LED_COUNT] =
{
	IO_LED1, IO_LED2, IO_LED3, IO_LED4,
	IO_LED5, IO_LED6, IO_LED7, IO_LED8
};


static uint32	pwmPort[LED_COUNT] = 
{ 
	PWM_LED1, PWM_LED2, PWM_LED3, PWM_LED4, 
	PWM_LED5, PWM_LED6, PWM_LED7, PWM_LED8 
};

static uint8 	led[LED_COUNT];

static int 		ledState	= STATE_INC;
static int 		waitCount;
static bool		forward 	= true;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void ledPwmInit( )
{
	for( int i = 0; i < LED_COUNT; i++ )
		ioSetMode( ioPort[i], IO_MODE_OUTPUT_LOW );

	// change settings for timers used ( slowest )
	timerInit( TIMER1, TIMER_WGM_FAST_PWM_8BIT, TIMER_CS_1024 );
	timerInit( TIMER2, TIMER02_WGM_FAST_PWM, TIMER2_CS_1024 );    
	timerInit( TIMER3, TIMER_WGM_FAST_PWM_8BIT, TIMER_CS_1024 );    
	timerInit( TIMER4, TIMER_WGM_FAST_PWM_8BIT, TIMER_CS_1024 );    
		
	for( int i = 0; i < LED_COUNT; i++ )
	{
		pwmStart( pwmPort[i], PWM_MODE_NON_INVERTING );
	}
	
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void ledPwmProcess( )
{
	int16 val = forward ? led[0] : led[LED_COUNT-1];
	
	switch( ledState )
	{
	case STATE_INC:
		val += STEP;
		if( val >= MAX_PWM )
		{
			val = MAX_PWM;
			ledState = STATE_DEC;
		}
		break;
	case STATE_DEC:
		val -= STEP;
		if( val <= MIN_PWM )
		{
			val = MIN_PWM;
			ledState = STATE_WAIT;
			waitCount = LED_COUNT + 1;
		}
		break;
	case STATE_WAIT:
		if( --waitCount == 0 )
		{
			ledState = STATE_INC;
			forward = !forward;
		}
		break;
	}
	
	if( forward )
	{
		for( int i = LED_COUNT - 1; i > 0; i-- )
			led[i] = led[i-1];
			
		led[0] = val;
	}
	else
	{
		for( int i = 1; i < LED_COUNT; i++ )
			led[i-1] = led[i];
			
		led[LED_COUNT-1] = val;
	}
	
	for( int i = 0; i < LED_COUNT; i++ )
		pwmWrite( pwmPort[i], led[i] );
}