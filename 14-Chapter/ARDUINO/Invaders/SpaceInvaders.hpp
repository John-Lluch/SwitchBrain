//
//  SpaceInvaders.hpp
//  Invaders
//
//  Created by Joan Lluch on 27/4/24.
//

#ifndef SpaceInvaders_hpp
#define SpaceInvaders_hpp

// screen size
#define W 224
#define H 224

// common Row positions
#define R2 2      // score
#define R3 3      // moon
#define R4 4      // saucer
#define R15 15    // aliens bottom
#define R21 21    // shield
#define R24 24    // ship
#define R25 25    // earth
#define R27 27    // spare ships

// splash screen rows
#define RS5 5     // play
#define RS13 12   // score table
#define RS25 24   // info

// common Y positions
#define Y3 (R3*8)
#define Y4 (R4*8)
#define Y21 (R21*8)
#define Y24 (R24*8)
#define Y25 (R25*8)
#define Y27 (R27*8)

// comon timer values
#define T12 12    // explosion time
#define T60 60    // player setup time
#define T600 600  // saucer interval

void spaceInvaders_setup();
void spaceInvaders_loop();

extern bool left, right, fire;                    // player's control keys
extern int level;                                 // increase by one to let the aliens spawn 8 pixels down
extern int score;
extern int hiscore;
extern int lives;

extern int random(int r);
extern void gameOver();
extern void nextLevel();

#endif /* SpaceInvaders_hpp */
