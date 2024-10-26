//
//  AlienShots.cpp
//  Invaders
//
//  Created by Joan Lluch on 3/5/24.
//
//  This is a refactoring of the Space Invaders game clone
//  developed by Carsten Herting in Java/Processing as part of his
//  "Minimal 64x4" Home Computer architecture https://www.youtube.com/@slu467
//  The C++ port was made with the explicit permision of the author


#include "AlienShots.hpp"
#include "SpaceInvaders.hpp"

#include "Aliens.hpp"
extern Aliens a;

#include "Player.hpp"
extern Player p;

#include "Display.hpp"
extern Display ds;

#define PROGMEM

const uint8_t AlienShot [] PROGMEM = {

  0b01000000,// Squigly Shot
  0b10000000,
  0b01000000,
  0b00100000,
  0b01000000,
  0b10000000,
  0b01000000,
  0b00000000,
  
  0b10000000,
  0b01000000,
  0b00100000,
  0b01000000,
  0b10000000,
  0b01000000,
  0b00100000,
  0b00000000,
  
  0b01000000,
  0b00100000,
  0b01000000,
  0b10000000,
  0b01000000,
  0b00100000,
  0b01000000,
  0b00000000,
  
  0b00100000,
  0b01000000,
  0b10000000,
  0b01000000,
  0b00100000,
  0b01000000,
  0b10000000,
  0b00000000,
  
  0b01000000,// Plunger Shot
  0b01000000,
  0b01000000,
  0b01000000,
  0b01000000,
  0b11100000,
  0b00000000,
  0b00000000,
  
  0b01000000,
  0b01000000,
  0b01000000,
  0b11100000,
  0b01000000,
  0b01000000,
  0b00000000,
  0b00000000,

  0b01000000,
  0b01000000,
  0b11100000,
  0b01000000,
  0b01000000,
  0b01000000,
  0b00000000,
  0b00000000,
  
  0b11100000,
  0b01000000,
  0b01000000,
  0b01000000,
  0b01000000,
  0b01000000,
  0b00000000,
  0b00000000,
  
  0b01000000,  // Rolling Shot
  0b01000000,
  0b01000000,
  0b01000000,
  0b01000000,
  0b01000000,
  0b01000000,
  0b00000000,
  
  0b01000000,
  0b01000000,
  0b11000000,
  0b01100000,
  0b01000000,
  0b11000000,
  0b01100000,
  0b00000000,
  
  0b01000000,
  0b01000000,
  0b01000000,
  0b01000000,
  0b01000000,
  0b01000000,
  0b01000000,
  0b00000000,
  
  0b01100000,
  0b11000000,
  0b01000000,
  0b01100000,
  0b11000000,
  0b01000000,
  0b01000000,
  0b00000000,
  0b00000000,
};

const uint8_t AlienShotExploding [] PROGMEM = {
  0b00010000,
  0b01000100,
  0b00011010,
  0b00111100,
  0b01011100,
  0b00111110,
  0b01011100,
  0b00101010,
};

void AlienShots::reset()
{
  for(int i=0; i<N; i++)
  {
    active[i] = -1;     // deactivate all slots
  }
}

void AlienShots::update()
{
  if ( !p.isFrozen()  && random(255) < (level<<2))         // time to place a new shot?
  {
    int col = random(11);                                   // pick a random column
    if (random(255) < (level<<3))                           // time for a precise shot?
    {
      int dx = p.getGunPos() - a.xpos();
      if (dx >= 0 && dx < 11*16) col = (dx>>4);             // pick the column under which the player ship sits
    }
    
    int r, c;
    for(c=0; c<11; c++)
    {
      for(r=0; r<5; r++) if (a.isAlive(((col+c) % 11)+r*11)) break;     // is there a living alien in this columns?
      if (r < 5)
      {
        col = (col + c) % 11;                              // put exactly below alien
        int px = a.xpos() + (col<<4) + 7;
        int py = a.ypos() - (r<<4);
        int i;
        for (i=0; i<N; i++) if (active[i] == -1) break;    // search a free slot
        if (i != N)                                        // free slot found?
        {
          active[i] = random(3);                           // chose random type
          shot[i] = -1;                                    // shot not ploted
          x[i] = px;                                       // store the CENTER TOP position of the shot
          y[i] = py;
          timer[i] = -1;                                   // -1: normal shot, >=0: show has exploded
        }
        break;
      }
    }
  }
  
  for (int i=0; i<N; i++)                                  // update all shot slots
  {
    if (active[i] == -1) continue;                         // skip unactive slots
    
    if (timer[i] == -1)                                    // explosion timer off? flying shot?
    {
      if (shot[i] >= 0)                                    // delete shot?
        ds.drawBitmap( x[i], y[i], AlienShot + (shot[i]<<3), 3, 8, SWCOLOR_BLACK );
      
      if (active[i] == 3)
      {
        active[i] = -1;
        continue;
      }
      
      if ( active[i] > 3 )                                 // exploding collision?
      {
        x[i] = x[i] - 3;                                   // remember this position
        if ( active[i] == 4 ) y[i] = y[i] + 2;             // adjust y so that it causes some deeper damage
        ds.drawBitmap(x[i], y[i], AlienShotExploding, 8, 8, SWCOLOR_LIGHT_ORANGE);   // plot explosion
        timer[i] = T12;
        continue;
      }
      
      y[i]++;                                              // shot falls one step down
      shot[i] = (active[i]<<2) + (((ds.frameCount()+i) & 15)>>2) ;                    // store the shot sprite index
      ds.drawBitmap( x[i], y[i], AlienShot + (shot[i]<<3), 3, 8, SWCOLOR_WHITE );     // plot the new shot
      
      if ( ds.didCollide() )    // collision?
      {
        if ( p.didHitShip( y[i]+8 ) ) active[i] = 3;       // player Ship hit
        else active[i] = 4;                                // wall or player shot hit
      }
      
      if (y[i]+8 > Y27-2) active[i] = 5;                   // shot reached the bottom line?
      continue;
    }
                                                       
    if (timer[i]-- == 0)  // explosion timer != -1 => shot exploding
    {
      ds.drawBitmap(x[i], y[i], AlienShotExploding, 8, 8, SWCOLOR_BLACK);        // delete the explosion after some frames
      active[i] = -1;
    }
  }
}



