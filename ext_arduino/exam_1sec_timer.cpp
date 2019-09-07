/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			exam_1sec_timer.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include "exam_config.h"

#if EXAM_1SEC_TIMER

#include "src/config.h"
#include "src/system.h"
#include "src/core_timer.h"
#include "src/core_io.h"
#include "src/core_pwm.h"
#include "src/core_uart.h"

//
//  top value for CTC, 1Hz, prescale 256
// 
#define TOP_VALUE	( CONFIG_CPU_FREQ / 256 - 1 )

static void isr1sec( );

void setup( )
{
	DEFINE_UART( 0, 115200, CONFIG_UART0_RX_BUF_SIZE, CONFIG_UART0_TX_BUF_SIZE, UART_LINE_ENDING_NONE );
	timerInit( TIMER1, TIMER_WGM_CTC_OCRA, TIMER_CS_256 );
	
	timerEnableInt( TIMER1_INT_COMPA, isr1sec );
	pwmWrite( PWM_OC1A, TOP_VALUE );
	
	ioSetMode( IO_PB5, IO_MODE_OUTPUT_LOW );
	pwmStart( PWM_OC1A, PWM_MODE_TOGGLE );
}

void loop( )
{
    delay( 10 );    // do nothing
}

static void isr1sec( )
{
    static uint16 sec = 0;
	uartPrintf( 0, "%d\r\n", sec++ );
}


#endif  // EXAM_1SEC_TIMER