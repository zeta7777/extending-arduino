/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			exam_uart.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include "exam_config.h"

#if EXAM_UART

#include "src/config.h"
#include "src/system.h"
#include "src/core_uart.h"


#define USE_MACRO

#ifndef USE_MACRO

static UART_CTRL	uart0Ctrl;
static char 		uart0RxBuf[ CONFIG_UART0_RX_BUF_SIZE ];
static char			uart0TxBuf[ CONFIG_UART0_TX_BUF_SIZE ];

#endif

void setup( )
{
#ifdef USE_MACRO
	DEFINE_UART( 0, 115200, 128, 128, UART_LINE_ENDING_CR_NL );
#else
	uartInit( 0, 115200, &uart0Ctrl, uart0RxBuf, CONFIG_UART0_RX_BUF_SIZE, uart0TxBuf, CONFIG_UART0_TX_BUF_SIZE );
#endif
}

void loop( )
{
	static uint16 line = 0;
	uartPrintf( 0, "%d Hello World~\r\n", line++ );
	delay( 1000 );
}

#endif
