/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			core_uart.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "config.h"
#include "system.h"
#include "core_uart.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static	UART_CTRL* uartCtrl[ CONFIG_UART_COUNT ];

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
static void uartISR_txEmpty( uint8 no );

void quePut_core( QUEUE* que, uint8 c );
uint8 queGet_core( QUEUE* que );


//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void uartInit( uint8 no, uint32 baud, 
				UART_CTRL* ctrl, 
				uint8* rxBuf, uint16 rxBufSize, 
				uint8* txBuf, uint16 txBufSize,
				uint8 lineEnding )
{
	// init UART control block
	memset( ctrl, 0, sizeof( UART_CTRL ) );
	queInit( &ctrl->rxQue, rxBuf, rxBufSize, lineEnding );
	queInit( &ctrl->txQue, txBuf, txBufSize, QUE_LINE_ENDING_NONE );

	uartCtrl[no] = ctrl;
	
	// init UART registers
	
	// 2x mode baudrate to reduce error rate
	uint16 	rate  = (uint16)( ( CONFIG_CPU_FREQ / 4 / baud - 1 ) / 2 );
	uint8	ucsrb = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
	uint8	ucsrc = (1<<UCSZ01) | (1<<UCSZ00);
	
	if( no == 0 )
	{
		ctrl->udr = &UDR0;
		ctrl->ucsra = &UCSR0A;
		ctrl->ucsrb = &UCSR0B;

		UBRR0H = ( rate >> 8 );
		UBRR0L = ( rate );
		
		UCSR0C = ucsrc;
		UCSR0B = ucsrb;
		
		UCSR0A = 2;
	}
	else if( no == 1 )
	{
		ctrl->udr = &UDR1;
		ctrl->ucsra = &UCSR1A;
		ctrl->ucsrb = &UCSR1B;

		UBRR1H = ( rate >> 8 );
		UBRR1L = ( rate );
		
		UCSR1C = ucsrc;
		UCSR1B = ucsrb;
		
		UCSR1A = 2;
	}
	else if( no == 2 )
	{
		ctrl->udr = &UDR2;
		ctrl->ucsra = &UCSR2A;
		ctrl->ucsrb = &UCSR2B;

		UBRR2H = ( rate >> 8 );
		UBRR2L = ( rate );
		
		UCSR2C = ucsrc;
		UCSR2B = ucsrb;
		
		UCSR2A = 2;
	}
	else if( no == 3 )
	{
		ctrl->udr = &UDR3;
		ctrl->ucsra = &UCSR3A;
		ctrl->ucsrb = &UCSR3B;

		UBRR3H = ( rate >> 8 );
		UBRR3L = ( rate );
		
		UCSR3C = ucsrc;
		UCSR3B = ucsrb;
		
		UCSR3A = 2;
	}
}

//////////////////////////////////////////////////////////////////////
//	always return true
//////////////////////////////////////////////////////////////////////
bool uartPut( uint8 no, char c )
{
	UART_CTRL*	ctrl 	= uartCtrl[no];
	QUEUE*		que 	= &ctrl->txQue;
	
	if( ctrl == NULL )
		return false;
	
#if 0
	// polling mode test : await "data register empty" 
	while( !( *ctrl->ucsra & ( 1 << UDRE0 ) ) );
	*ctrl->udr = c;
	return true;
#endif
		
	if( queIsEmpty( que ) && ( ( *ctrl->ucsra ) & ( 1 << UDRE0 ) ) )
	{
		*ctrl->udr = c;
		return true;
	}
	else
	{
		while( queIsFull( que ) )
		{
			if( SREG & 0x80 )		// global int enabled
			{
				
			}
			else
			{
				if( ( *ctrl->ucsra ) & ( 1<<UDRE0 ) )
					uartISR_txEmpty( no );
			}
		}
			
		// protect queue internal status
		// because the funciton can be called from 
		// ISR and non-ISR context simultaneously
		uint8 oldSREG = SREG;
		cli( );
		
		quePut_core( que, c );

		*ctrl->ucsrb |= ( 1 << UDRIE0 );
		
		SREG = oldSREG;
		return true;
	}
}

//////////////////////////////////////////////////////////////////////
//		write to UART
//////////////////////////////////////////////////////////////////////
int uartPrint( uint8 no, const char* str )
{
	if( no >= CONFIG_UART_COUNT )
		return 0;
		
	int written = 0;
	
	while( *str )
	{
		if( uartPut( no, *str++ ) )
			written++;
		else
			break;
	}
			
	return written;
}

int uartPrintln( uint8 no, const char* str )
{
	int written = uartPrint( no, str );
	
	if( written == 0 )
		return 0;
	
	// FIXME : line ending 
	written += uartPrint( no, "\r\n" );

	return written;
}

int	uartPrintf( uint8 no, const char* fmt, ... )
{
	va_list v;
	va_start( v, fmt );
	
	char buf[128];
	vsnprintf( buf, 127, fmt, v );
	
	va_end( v );
	return uartPrint( no, buf );
}

int uartWrite( uint8 no, uint8* data, uint16 len )
{
	int written = 0;
	while( len-- )
	{
		if( uartPut( no, *data++ ) )
			written++;
		else
			break;
	}
	
	return written;
}

//////////////////////////////////////////////////////////////////////
//		read from UART
//////////////////////////////////////////////////////////////////////
bool uartIsAvailable( uint8 no )
{
	return queIsAvailable( &uartCtrl[no]->rxQue );
}

//
//	- should be called after checking with uartAvailable( )
//	- can be called from ISR or non-ISR
//
uint8 uartGet( uint8 no )
{
	return queGet( &uartCtrl[no]->rxQue );
}

//
//	return -1 on no-data
//
int uartRead( uint8 no )
{
	QUEUE* que = &uartCtrl[no]->rxQue;
	
	if( queIsEmpty( que ) )
		return -1;
	
	uint8 c = queGet( que );
	
	return c;
}

int uartPeek( uint8 no )
{
	return quePeek( &uartCtrl[no]->rxQue );
}

bool uartIsLineAvailable( uint8 no )
{
	#if 0
	QUEUE* que = &uartCtrl[no]->rxQue;
	if( no != 0 )
	{
		uartPrintf( 0, "%d %d %d\r\n", que->lineCount, que->head, que->tail );
	}
	#endif
	return queIsLineAvailable( &uartCtrl[no]->rxQue );
}

int uartGetLine( uint8 no, uint8* buf, int bufLen )
{
	return queGetLine( &uartCtrl[no]->rxQue, buf, bufLen );
}

int uartGetLineCount( uint8 no )
{
	return queGetLineCount( &uartCtrl[no]->rxQue );
}

int uartGetCount( uint8 no )
{
	return queGetCount( &uartCtrl[no]->rxQue );
	
}


//////////////////////////////////////////////////////////////////////
//		ISR
//	always called in ISR context
//////////////////////////////////////////////////////////////////////
static void uartISR_txEmpty( uint8 no )
{
	UART_CTRL* ctrl = uartCtrl[no];
	QUEUE* que = &ctrl->txQue;
	
	*ctrl->udr = queGet_core( que );
	
	// if last byte just written, turn off tx int's
	if( queIsEmpty( que ) )
		*ctrl->ucsrb &= ~(1<<UDRIE0);
}

static void uartISR_rxData( uint8 no )
{
	UART_CTRL* ctrl = uartCtrl[no];

	quePut_core( &ctrl->rxQue, *( ctrl->udr ) );
}

#if 1
//////////////////////////////////////////////////////////////////////
//	tx empty interrupt
//////////////////////////////////////////////////////////////////////
ISR( USART0_UDRE_vect )
{
	uartISR_txEmpty( 0 );
}

ISR( USART1_UDRE_vect )
{
	uartISR_txEmpty( 1 );
}

ISR( USART2_UDRE_vect )
{
	uartISR_txEmpty( 2 );
}

ISR( USART3_UDRE_vect )
{
	uartISR_txEmpty( 3 );
}

//////////////////////////////////////////////////////////////////////
//	rx data interrupt
//////////////////////////////////////////////////////////////////////
ISR( USART0_RX_vect )
{
	uartISR_rxData( 0 );
}

ISR( USART1_RX_vect )
{
	uartISR_rxData( 1 );
}

ISR( USART2_RX_vect )
{
	uartISR_rxData( 2 );
}

ISR( USART3_RX_vect )
{
	uartISR_rxData( 3 );
}

#endif
