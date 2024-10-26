//
//  Text.hpp
//  Invaders
//
//  Created by Joan Lluch on 5/5/24.
//

#ifndef Text_hpp
#define Text_hpp

#include <stdint.h>

extern void print( int16_t cx, int16_t cy, const char *str, uint16_t color );
extern void print( int16_t cx, int16_t cy, uint16_t num, uint16_t size, uint16_t color );
extern void drawNum( int16_t x, int16_t y, uint16_t num, uint16_t size, uint16_t color );

#endif /* Text_hpp */
