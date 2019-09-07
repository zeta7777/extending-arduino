/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			core_uart.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#ifndef __UART_H
#define __UART_H

#include "types.h"
#include "queue.h"


#define UART_LINE_ENDING_NONE			QUE_LINE_ENDING_NONE	
#define UART_LINE_ENDING_CR_NL			QUE_LINE_ENDING_CR_NL	

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#define DEFINE_UART( uartNo, baud, rxBufSize, txBufSize, lineEnding )			\
	static UART_CTRL	__define_uartCtrl##uartNo;					\
	static unsigned char __define_uartRxBuf##uartNo[ rxBufSize ];	\
	static unsigned char __define_uartTxBuf##uartNo[ txBufSize ];	\
	uartInit( uartNo, baud, &__define_uartCtrl##uartNo, __define_uartRxBuf##uartNo, rxBufSize, __define_uartTxBuf##uartNo, txBufSize, lineEnding );

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
typedef struct __uart_ctrl
{
	QUEUE	rxQue;
	QUEUE	txQue;
	
	volatile uint8*	udr;			// UDR
	volatile uint8* ucsra;			// UCSRnA
	volatile uint8* ucsrb;			// UCSRnB
} UART_CTRL;

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void	uartInit( uint8 no, uint32 baud, 
					UART_CTRL* ctrl, 
					uint8* rxBuf, uint16 rxBufSize, 
					uint8* txBuf, uint16 txBufSize,
					uint8 lineEnding );

bool	uartPut( uint8 no, char c );
int		uartPrint( uint8 no, const char* str );
int		uartPrintln( uint8 no, const char* str );
int		uartPrintf( uint8 no, const char* fmt, ... );
int		uartWrite( uint8 no, uint8* data, uint16 len );

bool	uartIsAvailable( uint8 no );
bool	uartIsLineAvailable( uint8 no );
uint8	uartGet( uint8 no );
int		uartGetLine( uint8 no, uint8* buf, int bufLen );
int		uartRead( uint8 no );
int		uartPeek( uint8 no );
int		uartGetCount( uint8 no );
int		uartGetLineCount( uint8 no );

#endif	// __UART_H
