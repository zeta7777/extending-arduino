/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			queue.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "queue.h"

/////////////////////////////////////////////////////////////////////
//	size should be ( 2^n )
/////////////////////////////////////////////////////////////////////
void queInit( QUEUE* que, uint8* buf, uint16 size, uint8 lineEnding )
{
	que->buf = buf;
	que->size = size;
	
	que->head = que->tail = 0;
	
	que->ptrMask = size - 1;
	que->szMask = size | que->ptrMask;
	
	que->lineEnding = lineEnding;
	que->lineCount = 0;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
void queFlush( QUEUE* que )
{
	uint8 oldSREG = SREG;
	cli(); 

	que->head = que->tail = 0;
	que->lineCount = 0;
	
	SREG = oldSREG;
}

/////////////////////////////////////////////////////////////////////
//	Data Manipulation Functions : Put, Get, Peek
// 	Before calling one of these functions, user should check 
//	queue status ( full, empty, available )
/////////////////////////////////////////////////////////////////////

//
//	[cr][nl]
//	only check last [NL]
//
static bool inline is_line_ending( QUEUE* que, uint8 c )
{
	if( c == '\n' && que->lineEnding == QUE_LINE_ENDING_CR_NL )
		return true;
	return false;
}

void quePut_core( QUEUE* que, uint8 c )
{
	que->buf[ que->head & que->ptrMask ] = c;
	que->head++;
	
	if( is_line_ending( que, c ) )
		que->lineCount++;
}

uint8 queGet_core( QUEUE* que )
{
	uint8 c = que->buf[ que->tail & que->ptrMask ];
	que->tail++;
	
	if( is_line_ending( que, c ) )
		que->lineCount--;
	
	return c;
}

// 
void quePut( QUEUE* que, uint8 c )
{
	uint8 oldSREG = SREG;
	cli( );
	
	quePut_core( que, c );
		
	SREG = oldSREG;
}

//
uint8 queGet( QUEUE* que )
{
	uint8 oldSREG = SREG;
	cli( );
	
	uint8 c = queGet_core( que );
	
	SREG = oldSREG;
	return c;
}

//
//	return : # of bytes in a line 
//			not include line-ending char's
//
int queGetLine( QUEUE* que, uint8* buf, int bufLen )
{
	int count = 0;
	
	while( bufLen-- )
	{
		uint8 c = que->buf[ que->tail & que->ptrMask ];
		que->tail++;
		
		if( is_line_ending( que, c ) )
		{
			uint8 oldSREG = SREG;
			cli( );
			que->lineCount--;
			SREG = oldSREG;
			
			// remove [CR]
			// [NL] not yet added to buf
			if( count > 0 && *(buf-1) == '\r' )
			{
				count--;
				buf--;
			}
			break;
		}
			
		*buf++ = c;
		count++;
	}
	
	*buf = 0;
	return count;
}

//
// note that there is no tail-increase 
// which is in queGet( ). Data is still in queue
//
uint8 quePeek( QUEUE* que )
{
	return que->buf[ que->tail & que->ptrMask ];
}

/////////////////////////////////////////////////////////////////////
//	status check
/////////////////////////////////////////////////////////////////////
bool queIsEmpty( QUEUE* que )
{
	return ( que->head == que->tail );
}

bool queIsFull( QUEUE* que )
{
	return ( ( ( que->head - que->tail ) & que->szMask ) == que->size );
}

bool queIsAvailable( QUEUE* que )
{
	return ( que->head != que->tail );
}

int queGetCount( QUEUE* que )
{
	return ( que->head - que->tail ) & que->szMask;
}

bool queIsLineAvailable( QUEUE* que )
{
	return ( que->lineCount > 0 );
}

int queGetLineCount( QUEUE* que )
{
	return que->lineCount;
}


