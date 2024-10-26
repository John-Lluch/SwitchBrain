//
//  Player.hpp
//  Invaders
//
//  Created by Joan Lluch on 7/5/24.
//

#ifndef Player_hpp
#define Player_hpp

#include <stdint.h>

class Player
{
public:
  void reset();
  void setup();
  void update();
  bool didHitShip(int sy);
  void killAll();
  //void freeze() { frozen = true; }
  bool isFrozen() { return frozen; }
  int getGunPos() { return shippos+8; }
  
private:
  int x;
  int state;               // 0: hidden, 1: active, 3: explosion, 4: points
  int shippos;             // horizontal position of the ship sprite
  int timer;
  bool frozen;
};

#endif /* Player_hpp */
