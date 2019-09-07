
#ifndef __OLED_H
#define __OLED_H


void oledInit( );
void oledSetPos( int x, int y );
void oledPrint( int x, int y, const char* str );
void oledStart( );
uint8 oledNextPage( );
void oledSetContrast( uint8 con );
void oledSetEntireDispOn( bool on );
void oledSetInverseDisp( bool en );
void oledSetDispOn( bool on );

void oledTest( );
void oledDevTest( );

#endif	// __OLED_H