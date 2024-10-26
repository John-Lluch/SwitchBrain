//
//  Shot.cpp
//  Invaders
//
//  Created by Joan Lluch on 3/5/24.
//
//  This is a refactoring of the Space Invaders game clone
//  developed by Carsten Herting in Java/Processing as part of his
//  "Minimal 64x4" Home Computer architecture https://www.youtube.com/@slu467
//  The C++ port was made with the explicit permision of the author


#include "Shot.hpp"
#include "SpaceInvaders.hpp"

#include "Text.hpp"

#include "Aliens.hpp"
extern Aliens a;

#include "Saucer.hpp"
extern Saucer u;

#include "Player.hpp"
extern Player p;

#include "Display.hpp"
extern Display ds;

#define PROGMEM
const uint8_t PlayerShotExploding [] PROGMEM = {
  0b10001001,
  0b00100010,
  0b01111110,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00100100,
  0b10010001,
};


void Shot::reset()
{
  state = 0;
}


void Shot::place()
{
  if (state != 0)
    return;
  
  state = 1;
  x = p.getGunPos(); y = Y24 ;             // set TOP of the shot...
  timer = -1;
  u.notifyShot();                          // notify to Saucer object
}

void Shot::update()
{
  if ( timer == -1 )
  {
    if (state == 0) return;                                 // no shot
    if (y < Y24) ds.fillRect(x, y, 1, 4, SWCOLOR_BLACK);    // delete shot?
    
    if ( state > 3 )                                        // exploding collision?
    {
      x = x-4;
      ds.drawBitmap(x, y, PlayerShotExploding, 8, 8, SWCOLOR_LIGHT_ORANGE);   // show little explosion for a while, store position
      timer = T12;
      return;
    }
    
    if (state == 1)                                      // shot fired
    {
      y -= 4;                                            // move the shot up
      ds.fillRect(x, y, 1, 4, SWCOLOR_WHITE);            // draw propagated shot in free space
      if ( ds.didCollide() )                             // shot collided ?
      {
        state = 3;
        if ( u.didHitSaucer( y ) ) timer = 0;            // Saucer hit
        else if ( a.didHitAlien( x, y ) ) timer = T12/2; // Alien hit (NOTE: set timer to T12 to match original game)
        else state = 4;                                  // wall or alien shot hit
      }

      if ( y <= Y3 ) state = 5;                          // shot reached top of the screen
    }
    return;
  }
  
  if (timer-- == 0)
  {
    if ( state > 3 )
      ds.drawBitmap(x, y, PlayerShotExploding, 8, 8, SWCOLOR_BLACK);    // delete small explosion
    
    state = 0;
  }
}

