//
//  Saucer.cpp
//  Invaders
//
//  Created by Joan Lluch on 3/5/24.
//
//  This is a refactoring of the Space Invaders game clone
//  developed by Carsten Herting in Java/Processing as part of his
//  "Minimal 64x4" Home Computer architecture https://www.youtube.com/@slu467
//  The C++ port was made with the explicit permision of the author


#include "Saucer.hpp"
#include "SpaceInvaders.hpp"

#include "Text.hpp"

#include "Aliens.hpp"
extern Aliens a;

#include "Player.hpp"
extern Player p;

#include "Display.hpp"
extern Display ds;


#define PROGMEM
uint8_t SaucerSprite [] PROGMEM = {
  0b00000000,0b00000000,0b00000000,
  0b00000000,0b01111110,0b00000000,
  0b00000001,0b11111111,0b10000000,
  0b00000011,0b11111111,0b11000000,
  0b00000110,0b11011011,0b01100000,
  0b00001111,0b11111111,0b11110000,
  0b00000011,0b10011001,0b11000000,
  0b00000001,0b00000000,0b10000000,
};

const uint8_t SaucerExplosion [] PROGMEM = {
  0b00010010,0b10000001,0b01001000,
  0b00001000,0b00000110,0b00010000,
  0b01010001,0b11100011,0b00000000,
  0b00000011,0b11111001,0b11001000,
  0b00000111,0b01010100,0b11100100,
  0b00010001,0b11110001,0b10000000,
  0b01000000,0b10100011,0b00010000,
  0b00010001,0b00010000,0b10000000,

};

void Saucer::reset()
{
  state = 0;
  timer = T600;
  tries = 0;
}


void Saucer::update()
{
  if (timer == -1)
  {
    if ((ds.frameCount() & 1) == 0) return;         // update only once every two frames
    x+=step;
    if (x < 0 || x > W-24) reset();
    ds.drawBitmap(x, Y4, SaucerSprite, 24, 8, state?SWCOLOR_RGB_RED:SWCOLOR_BLACK, SWCOLOR_BLACK); // show/delete saucer
    return;
  }

  if ( p.isFrozen() ) return;     // do not even update the timer if the game is frozen
  
  if (timer-- == 0)
  {
    if (state == 0 && a.getTotalAliens() > 12 )    // no saucer
    {
      step = (random(2)<<1) - 1;        // -1 or 1
      x = (step == -1 ? W-24 : 0);
      state = 1; tries = 0;
      return;
    }
    
    if (state == 3)   // delete explosion, show value
    {
      ds.drawBitmap(x, Y4, SaucerExplosion, 24, 8, SWCOLOR_BLACK, SWCOLOR_BLACK);      // delete explosion
      if (tries < 9)
      {
        drawNum(x, Y4, value[tries], 3, SWCOLOR_WHITE);
        score += value[tries];
        state = 4; timer = T12*2;
      }
      else reset();
      return;
    }
    
    if (state == 4)  // delete points
    {
      ds.fillRect(x, Y4, 24, 8, SWCOLOR_BLACK);
      reset();
    }
  }
}
  

bool Saucer::didHitSaucer(int sy)
{
  if ( sy > 32+8 )  // no need to check x position
    return false;
  
  ds.drawBitmap(x, 32, SaucerExplosion, 24, 8, SWCOLOR_RGB_RED, SWCOLOR_BLACK);  // overwrite saucer with explosion
  timer = T12; state = 3;                                                      // show explosion for some time
  return true;
}

