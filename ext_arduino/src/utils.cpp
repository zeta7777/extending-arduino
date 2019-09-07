/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			utils.cpp
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include <string.h>
#include <stdlib.h>
#include "utils.h"

char* getToken( char* str, const char* delimiter )
{
	char* t = strtok( str, delimiter );
	if( t && *t )
		return t;
	
	return NULL;
}

char* getNextToken( const char* delimiter )
{
	char* t = strtok( NULL, delimiter );
	if( t && *t )
		return t;
	
	return NULL;
}

bool parseIP( char* str, uint8 ip[4] )
{
	char* p = getToken( str, "." );
	for( int i = 0; i < 4; i++ )
	{
		ip[i] = atoi( p );
		p = getNextToken( "." );
	}
	
	return true;
}