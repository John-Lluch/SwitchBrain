//
//  Aliens.cpp
//  Invaders
//
//  Created by Joan Lluch on 3/5/24.
//
//  This is a refactoring of the Space Invaders game clone
//  developed by Carsten Herting in Java/Processing as part of his
//  "Minimal 64x4" Home Computer architecture https://www.youtube.com/@slu467
//  The C++ port was made with the explicit permision of the author

#include "Aliens.hpp"
#include "SpaceInvaders.hpp"

#include "Display.hpp"
extern Display ds;

#include "Player.hpp"
extern Player p;


#define PROGMEM
uint8_t AlienSprite [] PROGMEM = {
  0b00000011,0b11000000,  // alien bottom costume 0
  0b00011111,0b11111000,
  0b00111111,0b11111100,
  0b00111001,0b10011100,
  0b00111111,0b11111100,
  0b00001110,0b01110000,
  0b00011001,0b10011000,
  0b00001100,0b00110000,
 
  0b00000011,0b11000000,  // alien bottom costume 1
  0b00011111,0b11111000,
  0b00111111,0b11111100,
  0b00111001,0b10011100,
  0b00111111,0b11111100,
  0b00001110,0b01110000,
  0b00010001,0b10001000,
  0b00100000,0b00000100,

  0b00000100,0b00010000,  // alien middle costume 0
  0b00000010,0b00100000,
  0b00000111,0b11110000,
  0b00001101,0b11011000,
  0b00011111,0b11111100,
  0b00010111,0b11110100,
  0b00010100,0b00010100,
  0b00000011,0b01100000,

  0b00000100,0b00010000,  // alien middle costume 1
  0b00000010,0b00100000,
  0b00010111,0b11110100,
  0b00010101,0b11010100,
  0b00011111,0b11111100,
  0b00000111,0b11110000,
  0b00000100,0b00010000,
  0b00001000,0b00001000,

  0b00000001,0b10000000,  // alien top costume 0
  0b00000011,0b11000000,
  0b00000111,0b11100000,
  0b00001101,0b10110000,
  0b00001111,0b11110000,
  0b00000010,0b01000000,
  0b00000101,0b10100000,
  0b00001010,0b01010000,

  0b00000001,0b10000000,  // alien top costume 1
  0b00000011,0b11000000,
  0b00000111,0b11100000,
  0b00001101,0b10110000,
  0b00001111,0b11110000,
  0b00000101,0b10100000,
  0b00001000,0b00010000,
  0b00000100,0b00100000,
};

const uint8_t AlienExploding [] PROGMEM = {
  0b00000100,0b01000000,
  0b00100010,0b10001000,
  0b00010000,0b00010000,
  0b00001000,0b00100000,
  0b01100000,0b00001100,
  0b00001000,0b00100000,
  0b00010000,0b00010000,
  0b00100010,0b10001000,
};


#define TOTAL 55

void Aliens::reset()
{
  x = 16+10;
  y = (level-1 + R15)<<3;          // bottom line of alien rect
  num = -1;
  total = TOTAL;
  step = 2;
  costume = 0;
  timer = -1;
  for(int i=0; i<TOTAL; i++) alive[i] = true;
}


void Aliens::update()
{
  if (timer == -1)
  {
    if (p.isFrozen()) return;
  
    while( true )
    {
      if (++num == TOTAL)
      {
        num = 0; costume ^= 1; x += step;                       // alternate costume
      }
      if (alive[num]) break;                                    // look for living alien
    }
    
    int c = (num % 11)<<4;
    int r = num / 11;
    int pic = (r & 6) | costume;
    r = r<<4;
    
    ds.fillRect(x + c, y - r - 16, 16, 8, SWCOLOR_BLACK); // erase area on top of the alien
    ds.drawBitmap(x + c, y - r - 8, AlienSprite + (pic<<4), 16, 8, SWCOLOR_WHITE, SWCOLOR_BLACK); // draw alien

    if (y - r >= Y25)                                          // aliens have reached the earth?
      p.killAll();                                             // stop updating aliens
    
    if (x + c < 10 || x + c >= W-16-10)                        // alien has reached the left/right border?
    {
      step = -step; x += step; y += 8; num = -1;
    }
    return;
  }
  
  if (timer-- == 0)
    ds.fillRect(xkill, ykill, 16, 8, SWCOLOR_BLACK);   // clear out alien pop at stored (x,y)
}


bool Aliens::didHitAlien(int sx, int sy)
{
  int dx = sx - x;
  int dy = y-1 - sy;
  int ix = dx>>4;                       // horizontal tile index of current alien
  int iy = dy>>4;                       // vertical tile index
  int alien = ix+iy*11;
  if ( !(dx >= 0 && dx < 11*16 && dy >= 0 && dy < 10*8 && alive[alien]) )
    return false;                       // did not hit alien slot
  
  alive[alien] = false;
  total--;  score += 10 * ((iy>>1) + 1);
  xkill = x + (dx & ~15); ykill = y - (dy & ~7) - 8;        // store (x,y)
  ds.drawBitmap(xkill, ykill, AlienExploding, 16, 8, SWCOLOR_WHITE, SWCOLOR_BLACK);  // overwrite alien and shot with explosion
  timer = T12;                                              // explosion duration
  return true;
}
