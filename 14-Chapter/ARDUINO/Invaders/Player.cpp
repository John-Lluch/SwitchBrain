//
//  Player.cpp
//  Invaders
//
//  Created by Joan Lluch on 7/5/24.
//
//  This is a refactoring of the Space Invaders game clone
//  developed by Carsten Herting in Java/Processing as part of his
//  "Minimal 64x4" Home Computer architecture https://www.youtube.com/@slu467
//  The C++ port was made with the explicit permision of the author


#include "Player.hpp"
#include "SpaceInvaders.hpp"

#include "Text.hpp"

#include "Display.hpp"
extern Display ds;

#include "AlienShots.hpp"
extern AlienShots as;

#include "Aliens.hpp"
extern Aliens a;

#include "Shot.hpp"
extern Shot s;

#define PROGMEM
const uint8_t ShieldImage [] PROGMEM = {
  0b00001111,0b11111111,0b11000000,
  0b00011111,0b11111111,0b11100000,
  0b00111111,0b11111111,0b11110000,
  0b01111111,0b11111111,0b11111000,
  0b11111111,0b11111111,0b11111100,
  0b11111111,0b11111111,0b11111100,
  0b11111111,0b11111111,0b11111100,
  0b11111111,0b11111111,0b11111100,
  0b11111111,0b11111111,0b11111100,
  0b11111111,0b11111111,0b11111100,
  0b11111111,0b11111111,0b11111100,
  0b11111111,0b11111111,0b11111100,
  0b11111110,0b00000011,0b11111100,
  0b11111100,0b00000001,0b11111100,
  0b11111000,0b00000000,0b11111100,
  0b11111000,0b00000000,0b11111100
  };

uint8_t PlayerSprite [] PROGMEM = {
  0b00000000,0b10000000,
  0b00000001,0b11000000,
  0b00000001,0b11000000,
  0b00011111,0b11111100,
  0b00111111,0b11111110,
  0b00111111,0b11111110,
  0b00111111,0b11111110,
  0b00111111,0b11111110,
};

const uint8_t PlayerBlowupSprites [] PROGMEM = {
  0b00000010,0b00000000,
  0b00000000,0b00010000,
  0b00000010,0b10100000,
  0b00010010,0b00000000,
  0b00000001,0b10110000,
  0b01000101,0b10101000,
  0b00011111,0b11100100,
  0b00111111,0b11110101,
 
  0b00010000,0b00000100,
  0b10000010,0b00011001,
  0b00010000,0b11000000,
  0b00000010,0b00000010,
  0b01001011,0b00110001,
  0b00100001,0b11000100,
  0b00011111,0b11110000,
  0b00110111,0b11110010
};


void Player::reset()
{
  shippos = 16;
  timer = T60-1;
  frozen = false;
}


void Player::setup()
{
  reset();
  shippos = 16;
  timer = T60-1;
  print(12, 0, "WAVE", SWCOLOR_WHITE);
  print(13, R2, level, 2, SWCOLOR_WHITE);
  for(int i=0; i<4; i++) ds.drawBitmap(32 + i*(22+24), Y21, ShieldImage, 24, 16, SWCOLOR_RGB_GREEN, SWCOLOR_BLACK);   // paint shieds
  ds.fillRect(0, Y27-2, W, 1, SWCOLOR_RGB_GREEN);         // draw bottom line
  ds.fillRect(0, Y27, 128, 8, SWCOLOR_BLACK);         // show ships left
  print(1, R27, lives, 1, SWCOLOR_WHITE);
  for(int i=0; i<lives-1; i++) ds.drawBitmap(24+(i<<4), Y27, PlayerSprite, 16, 8, SWCOLOR_RGB_GREEN, SWCOLOR_BLACK);  // paint the spare ships
}


void Player::update()
{
  if (timer == -1)                                               // ship controlled by user?
  {
    if (left) shippos = shippos>16 ? shippos-1 : 16;             // control the ship
    if (right) shippos = shippos<W-32 ? shippos+1 : W-32;
    if (fire) s.place();                                         // start a new shot
    ds.drawBitmap(shippos, Y24, PlayerSprite, 16, 8, SWCOLOR_RGB_GREEN, SWCOLOR_BLACK); // paint the ship
    
    if (frozen) return;
    
    if ( a.getTotalAliens() == 0 )                               // all aliens killed? -> next level
    {
      frozen = true; nextLevel();
    }
    return;
  }
  
  if (timer == T60 )                                             // ship finished exploding
  {
    ds.fillRect(shippos, Y24, 16, 8, SWCOLOR_BLACK);             // delete the ships position
    if (--lives > 0)
    {
      print(1, R27, lives, 1, SWCOLOR_WHITE);                    // number of spare lifes
      ds.fillRect(8+(lives<<4), Y27, 16, 8, SWCOLOR_BLACK);      // delete a spare ship
      shippos = 16;
    }
  }
  
  if (timer == 0) frozen = false;
  
  if (timer-- > T60)                           // ship still exploding?
  {
    int n = (ds.frameCount() & 4)>>2;          // alternates every 16 frames between 0 and 1
    ds.drawBitmap(shippos, Y24, PlayerBlowupSprites + (n<<4), 16, 8, SWCOLOR_RGB_GREEN, SWCOLOR_BLACK);  // paint the exploding ship
    return;
  }
  
  if (lives == 0) gameOver();
}


bool Player::didHitShip(int sy)              
{
  if (sy < Y24 || sy >= Y24+8)                 // no need to look for x
    return false;

  frozen = true;
  timer = T60*2;
  return true;
}


void Player::killAll()
{
  frozen = true;
  lives = 1;
  timer = 120;
}

