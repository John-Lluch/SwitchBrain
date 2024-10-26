//
//  SpaceInvaders.cpp
//  Invaders
//
//  Created by Joan Lluch on 27/4/24.
//
//  This is a refactoring of the Space Invaders game clone
//  developed by Carsten Herting in Java/Processing as part of his
//  "Minimal 64x4" Home Computer architecture https://www.youtube.com/@slu467
//  The C++ port was made with the explicit permision of the author

#include <stdlib.h>
#include "SpaceInvaders.hpp"

#include "AlienShots.hpp"
#include "Aliens.hpp"
#include "Saucer.hpp"
#include "Shot.hpp"
#include "Player.hpp"
#include "Text.hpp"

bool left, right, fire;        // player's control keys
bool frozen;                   // set to freeze the game for a while, example upon hitting player ship
int level;                     // increase to let the aliens spawn down and fire more shots!
int score;
int hiscore;
int lives;

enum
{
  Splash = 0,
  WaitSetup,
  Setup,
  MainLoop,
  GameOver,
} gamestate;

uint32_t setuptimer;
uint32_t endtimer;

Shot s;                        // shots from the player's ship to aliens
Aliens a;                      // the aliens
AlienShots as;                 // shots from the aliens
Saucer u;                      // saucer ship
Player p;                      // player ship

#include "Display.hpp"
extern Display ds;

extern const uint8_t SaucerSprite [];
extern const uint8_t AlienSprite [];
extern const uint8_t PlayerSprite [];


int random(int r)
{
  int rnd = rand();
  return rnd % r ;
}


void updateScore()
{
  static int last = -1;
  if ( score == last ) return;
  print(2, R2, score, 4, SWCOLOR_WHITE);
  last = score;
}


void gameOverAnimation()
{
  const char *str = ((ds.frameCount() & 31) > 15) ? "@@@GAME@OVER@@@" : "@@@@@@@@@@@@";
  print(6, 7, str, SWCOLOR_WHITE);
}


void spaceInvaders_setup()
{
  left = right = fire = false;                    // player's control keys
  gamestate = Splash;
  hiscore = 7820;
}


void spaceInvaders_loop()
{
  switch (gamestate)
  {
    case Splash:   // draw splash screen
      if ( fire ) break;
      score = 0; level = 0; lives = 3;
      //ds.clear(SWCOLOR_BLACK);
      ds.fillRectAligned(0, 0, W, H, SWCOLOR_BLACK);
      print(2,  0, "SCORE<1>", SWCOLOR_WHITE);
      print(18, 0, "HI;SCORE", SWCOLOR_WHITE);
      print(2,  R2, score, 4, SWCOLOR_WHITE);
      print(22, R2, hiscore, 4, SWCOLOR_WHITE);
      print(12, RS5, "PLAY", SWCOLOR_WHITE);
      print(7,  RS5+3, "SPACE@INVADERS", SWCOLOR_WHITE);
      print(3,  RS13, ":@SCORE@ADVANCE@TABLE@:", SWCOLOR_WHITE);
      print(10, RS13+3, "=@?@MYSTERY", SWCOLOR_WHITE);
      ds.drawBitmap((7<<3)-4, (RS13+3)<<3, SaucerSprite, 24, 8, SWCOLOR_WHITE, SWCOLOR_BLACK);
      print(10, RS13+5, "=@30@POINTS", SWCOLOR_WHITE);
      ds.drawBitmap(7<<3, (RS13+5)<<3, AlienSprite + (5<<4), 16, 8, SWCOLOR_WHITE, SWCOLOR_BLACK);
      print(10, RS13+7, "=@20@POINTS", SWCOLOR_WHITE);
      ds.drawBitmap(7<<3, (RS13+7)<<3, AlienSprite + (3<<4), 16, 8, SWCOLOR_WHITE, SWCOLOR_BLACK);
      print(10, RS13+9, "=@10@POINTS", SWCOLOR_WHITE);
      ds.drawBitmap(7<<3, (RS13+9)<<3, AlienSprite + (1<<4), 16, 8, SWCOLOR_WHITE, SWCOLOR_BLACK);
      print(4, RS25, "<;", SWCOLOR_WHITE);
      print(10, RS25, ";>", SWCOLOR_WHITE);
      ds.drawBitmap(7<<3, RS25<<3, PlayerSprite, 16, 8, SWCOLOR_WHITE, SWCOLOR_BLACK);
      print(1, R27, "@PRESS@<FIRE>@", SWCOLOR_WHITE);
      print(17, R27, "CREDIT@00", SWCOLOR_WHITE);
      gamestate = WaitSetup;
      break;
      
    case WaitSetup:  
      if ( fire ) gamestate = Setup;  // start a new game
      break;
      
    case Setup:  // game level setup
      if ( fire ) break;             // wait for fire button release to start a new game
      ds.fillRectAligned(0, Y3, W, Y27-Y3, SWCOLOR_BLACK);     // clear game area
      level++;                                          // increase level
      p.setup();
      s.reset();
      as.reset();
      u.reset();
      a.reset();
      setuptimer = ds.frameCount();
      endtimer = 0xffff0000;
      gamestate = MainLoop;
      break;
                                      
    case MainLoop:  // MAIN GAME LOOP
      updateScore();
      p.update();                                       // player control
      if (ds.frameCount() > setuptimer + T60 )          // initial delay
      {
        s.update();                                     // propagate player shots
        as.update();                                    // propagate alien shots
        u.update();                                     // propagate saucer
      }
      a.update();                                       // propagate aliens
      if (ds.frameCount() > endtimer + T60 ) gamestate = Setup;
      break;
      
    case GameOver:  // GAME OVER, wait for keypress
      gameOverAnimation();
      if (fire) gamestate = Splash;
      break;
  }
  
  ds.pollEvent();
  left = ds.eventKeyLeft();     // update user controls
  right = ds.eventKeyRight();   //
  fire = ds.eventKeySpace();    //

  ds.render();
}


void nextLevel()
{
  endtimer = ds.frameCount();
}


void gameOver()
{
  if (score > hiscore)
    print(22, R2, (hiscore=score), 4, SWCOLOR_WHITE);

  print(1, R27, "@PRESS@<FIRE>@", SWCOLOR_WHITE);
  gamestate = GameOver;
}


