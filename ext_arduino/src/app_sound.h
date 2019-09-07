/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			app_sound.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#ifndef __APP_SOUND_H
#define __APP_SOUND_H


#include "types.h"
#include "sound.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void soundInit( );
void soundProcess( );

void soundStart( const SOUND_INFO* sinfo );

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

#endif	// __APP_SOUND_H
