/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			eeprom.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#ifndef __EEPROM_H
#define __EEPROM_H

#include "types.h"

void	eepromWrite( uint16 addr, uint8 data );
uint8	eepromRead( uint16 addr );

bool	eepromWrite( uint16 addr, uint8* dat, int len );
bool	eepromRead( uint16 addr, uint8* buf, int len );

bool	eepromErase( uint16 addr, int len, uint8 fill );

#endif	// __EEPROM_H