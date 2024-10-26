//
//  Shot.hpp
//  Invaders
//
//  Created by Joan Lluch on 3/5/24.
//

#ifndef Shot_hpp
#define Shot_hpp

#include <stdint.h>

class Shot
{
public:
  void reset();
  void place();
  void update();

private:
  int x, y;
  int state;  // 0: no shot, 1: active, 3: killed, 4: collided, 5: hit top
  int timer;
};

#endif /* Shot_hpp */
