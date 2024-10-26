//
//  Saucer.hpp
//  Invaders
//
//  Created by Joan Lluch on 3/5/24.
//

#ifndef Saucer_hpp
#define Saucer_hpp

#include <stdint.h>

class Saucer
{
public:
  void reset();
  void update();
  void notifyShot() { tries++; };  // count the shots aimed at Saucer
  bool didHitSaucer(int sy);
  
private:
  int x, tries, step;
  int state;               // 0: hidden, 1: active, 3: explosion, 4: points
  int value[9] = { 0, 300, 200, 100, 100, 50, 50, 50, 50 };
  int timer;
};
  
#endif /* Saucer_hpp */
