/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			sound.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#ifndef __SOUND_H
#define __SOUND_H

#include "types.h"

typedef struct __sound_info
{
	uint8	channels;
	uint16	slots;
	uint8	bytesPerSample;
	uint16	shift;
	uint16	sampleRate;
	uint16	OCR;
	uint32	msec;
	const uint8*	data;
} SOUND_INFO;


#endif