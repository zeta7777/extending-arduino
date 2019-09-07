
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			queue.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#ifndef __QUEUE_H
#define __QUEUE_H

#include "types.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#define QUE_LINE_ENDING_NONE			0
#define QUE_LINE_ENDING_CR_NL			1

//////////////////////////////////////////////////////////////////////
//	Data Queue ( FIFO ) 
//
//	[caution]
//		size 	: should be 2^n bytes, where n = 1 ~ 15
//				  ( 15 = (# of bits used for "data type" of head/tail) - 1
//		buf		: user should allocate buf memory
//////////////////////////////////////////////////////////////////////
typedef struct __queue
{
	volatile uint8*	buf;
	volatile uint16	head, tail;		// write to head, read from tail
		
	uint16 	size;
	uint16 	szMask;
	uint16 	ptrMask;
	
	uint8	lineEnding;
	volatile int lineCount;
} QUEUE;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void 	queInit( QUEUE* que, uint8* buf, uint16 size, uint8 lineEnding = QUE_LINE_ENDING_NONE );
void	queFlush( QUEUE* que );

void 	quePut( QUEUE* que, uint8 c );
uint8 	queGet( QUEUE* que );
int		queGetLine( QUEUE* que, uint8* buf, int bufLen );
uint8 	quePeek( QUEUE* que );

bool 	queIsEmpty( QUEUE* que );
bool 	queIsFull( QUEUE* que );
bool 	queIsAvailable( QUEUE* que );
int		queGetCount( QUEUE* que );
bool	queIsLineAvailable( QUEUE* que );
int		queGetLineCount( QUEUE* que );

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#endif	// __QUEUE_H