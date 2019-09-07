/********************************************************************
		  (c) Copyright 2019 DesignIt Co.

		  All rights reserved.

	FILE NAME:			ext_arduino.ino
	DATE:				JULY.05.2019
	AUTHOR:				SuW
	Q/A:				zeta71@naver.com

	CHANGE LOG:

	IMPLEMENTATION NOTES:

********************************************************************/

#include "exam_config.h"

#if ( EXAM_LED + EXAM_LED_PWM + EXAM_UART + EXAM_LCD_I2C + EXAM_OLED + EXAM_CLI + EXAM_1SEC_TIMER + EXAM_SOUND + EXAM_PMS5003 + EXAM_SWITCH + EXAM_ALL + EXAM_WIFI != 1 )

#error( "choose only one example in 'exam_config.h'" );

#endif

