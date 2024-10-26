//
//  Aliens.hpp
//  Invaders
//
//  Created by Joan Lluch on 3/5/24.
//

#ifndef Aliens_hpp
#define Aliens_hpp

#include <stdint.h>

class Aliens
{
public:
  void reset();
  void update();
  
  int xpos() { return x;}
  int ypos() { return y;}
  bool isAlive( int i ) { return alive[i]; }
  bool didHitAlien(int sx, int sy);
  void stopAliens();
  int getTotalAliens() { return total; }
  
private:
  bool alive[55];
  int x, y;            // start position of left bottom corner
  int xkill, ykill;    // start position of killed alien
  int num;             // number of alien slot currently moving
  int step;            // alternating between +2 and -2
  int costume;         // alternating between 0 and 1
  int total;
  int timer;
};

#endif /* Aliens_hpp */
