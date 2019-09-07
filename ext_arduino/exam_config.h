
/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			exam_config.h
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#ifndef __EXAM_CONFIG_H
#define __EXAM_CONFIG_H

/////////////////////////////////////////////////////////////////////
//	choose only one among the followings by defining 1
/////////////////////////////////////////////////////////////////////
#define EXAM_LED						0
#define EXAM_LED_PWM					0
#define EXAM_UART						0
#define EXAM_LCD_I2C					0
#define EXAM_OLED						0
#define EXAM_CLI						0
#define EXAM_SWITCH						0
#define EXAM_1SEC_TIMER 				0
#define EXAM_PMS5003					0
#define EXAM_WIFI						1
#define EXAM_SOUND						0
#define EXAM_ALL						0

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
#endif		// __EXAM_CONFIG_H