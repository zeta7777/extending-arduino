
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			utils.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/
#ifndef __UTILS_H
#define __UTILS_H

#include "types.h"

char* getToken( char* str, const char* delimiter );
char* getNextToken( const char* delimiter );

bool parseIP( char* str, uint8 ip[4] );

#endif	// __UTILS_H