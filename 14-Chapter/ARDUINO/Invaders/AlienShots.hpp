//
//  AlienShots.hpp
//  Invaders
//
//  Created by Joan Lluch on 3/5/24.
//
//  This is an adaptation with minor changes of the Space Invaders game clone
//  developed by Carsten Herting in Java/Processing as part of his
//  "Minimal 64x4" Home Computer architecture https://www.youtube.com/@slu467
//  This c++ code was made with his explicit permision.
//

#ifndef AlienShots_hpp
#define AlienShots_hpp

#include <stdint.h>

class AlienShots
{
public:
  void reset();
  void place();
  void update();
  
private:
  static const int N = 3;
  int active[N];    // -1: free, 0..2: shot type, 3:killed, 4: collided, 5: hit bottom
  int shot[N];      // last shot byte index
  int x[N];
  int y[N];         // TOP position of the shot
  int timer[N];
};

#endif /* AlienShots_hpp */
