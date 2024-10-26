//
//  Display.hpp
//
//  Created by Joan Lluch on 27/4/24.
//
/*
 Essential low level functions to draw bitmaps, and fill squares
 on a 8 bit computer stile memory Video Ram (VRAM).
*/

#ifndef Display_hpp
#define Display_hpp

#include <stdint.h>
#include <stddef.h>

// set DEPTH 1 for one bit per pixel, 8 pixels per byte, and 8 bits data bus access
// Set DEPTH 4 for 4 bits per pixel, 2 pixels per byte, and 16 bit data bus access
#define DEPTH 4


// Color pallete
#if DEPTH == 1
enum SWColor
{
  SWCOLOR_BLACK = 0,
  SWCOLOR_WHITE = 1,
  
  SWCOLOR_RGB_BLUE     = 1,  // All colors are mapped to black
  SWCOLOR_DARK_GREEN   = 1,
  SWCOLOR_BLUE         = 1,
  SWCOLOR_GREEN        = 1,
  SWCOLOR_LIGHT_BLUE   = 1,
  SWCOLOR_RGB_GREEN    = 1,
  SWCOLOR_CYAN         = 1,
  SWCOLOR_RGB_RED      = 1,
  SWCOLOR_MAGENTA      = 1,
  SWCOLOR_ORANGE       = 1,
  SWCOLOR_PINK         = 1,
  SWCOLOR_LIGHT_ORANGE = 1,
  SWCOLOR_LIGHT_PINK   = 1,
  SWCOLOR_YELLOW       = 1
};

#elif DEPTH == 4
enum SWColor
{                               // RGGB
  SWCOLOR_BLACK        = 0,     // 0000 0 Black
  SWCOLOR_RGB_BLUE     = 1,     // 0001 1 RGB Blue
  SWCOLOR_DARK_GREEN   = 2,     // 0010 2 Dark Green
  SWCOLOR_BLUE         = 3,     // 0011 3 Blue
  SWCOLOR_GREEN        = 4,     // 0100 4 Green
  SWCOLOR_LIGHT_BLUE   = 5,     // 0101 5 Light Blue
  SWCOLOR_RGB_GREEN    = 6,     // 0110 6 RGB Green
  SWCOLOR_CYAN         = 7,     // 0111 7 Cyan
  SWCOLOR_RGB_RED      = 8,     // 1000 8 RGB Red
  SWCOLOR_MAGENTA      = 9,     // 1001 9 Magenta
  SWCOLOR_ORANGE       = 10,    // 1010 10 Orange
  SWCOLOR_PINK         = 11,    // 1011 11 Pink
  SWCOLOR_LIGHT_ORANGE = 12,    // 1100 12 Light Orange
  SWCOLOR_LIGHT_PINK   = 13,    // 1101 13 Light Pink
  SWCOLOR_YELLOW       = 14,    // 1110 14 Yelow
  SWCOLOR_WHITE        = 15,    // 1111 15 White
};
#endif

class Display
{
public:
  bool init();
  void clear(uint16_t color);
  void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
  void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t foreColor, uint16_t backColor, bool aligned=false);
  void drawPoint(int16_t x, int16_t y, uint16_t color);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void fillRectAligned(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void setCursor(int16_t x, int16_t y) { cursor_x = x; cursor_y = y; }
  void print( const char *cstr );
  void print( int num);
  uint16_t getPixel( int16_t x, int16_t y );
  void render();
  
public:
  bool didCollide() { return _collided; }
  uint32_t frameCount() { return _frameCount; }

public:
  void pollEvent();
  bool eventQuit();
  bool eventKeyRight();
  bool eventKeyLeft();
  bool eventKeySpace();

public:
  ~Display();
  
private:
  
  void drawPixel_begin();
  void drawPixel_next(int16_t x, int16_t y, uint16_t color, bool alpha);
  void drawWord_next(int16_t k, int16_t y, uint16_t color);
  void drawPixel_end();
  void drawWord(int16_t k, int16_t y, uint16_t word);
  
private:
  uint32_t _frameCount = 0;
  bool _collided;
  
  uint8_t keysArray;
//  bool hasEvent = false;
  
private:
  int16_t cursor_x;     // x location to start print()ing text
  int16_t cursor_y;     // y location to start print()ing text
#if DEPTH == 1
  uint8_t *vram = NULL;
  uint8_t *ptr = NULL;
  uint8_t vreg = 0;
#elif DEPTH == 4
  uint16_t vram = 0; // fist vram address
  //uint16_t ptr = -1; // current vram address, -1 if not set
  uint16_t vreg = 0;  // vram data register
  bool active;  // vram data register valid 
#endif
  
};

#endif /* Display_hpp */
