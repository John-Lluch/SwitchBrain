//
//  Display.cpp
//
//  Created by Joan Lluch on 27/4/24.
//

#include "Display.hpp"

#include "THExec.h"
extern THExec exe;


//#include <ctime>
#include <assert.h>

//Screen dimension constants
const int WIDTH = 512;
const int HEIGHT = 480;

#if DEPTH == 1
const int BYTEPITCH = (WIDTH + 7) / 8;
#elif DEPTH == 4
const int WORDPITCH = (DEPTH * WIDTH + 15) / 16;
#endif

const int SCREEN_SCALE = 4;
const int SCREEN_WIDTH = WIDTH*SCREEN_SCALE;
const int SCREEN_HEIGHT = HEIGHT*SCREEN_SCALE;


bool Display::init()
{
  _frameCount = 0;      // initialise frame count
  exe.outputRange(4);   // set initial output range                                    
  return true;
}


// Destructor
Display::~Display()
{
}


void Display::clear(uint16_t color)
{
  fillRectAligned(0, 0, WIDTH, HEIGHT, color);
}



// PLEASE REMOVE ME
static void printStamp( const char *ctx )
{
  /*
  struct timespec start;
  clock_gettime( CLOCK_REALTIME, &start );
  
  //char* dt = ctime(&now); // convert it into string.
  printf( "Stamp %s: %ld, %ld\n", ctx, start.tv_sec, start.tv_nsec/1000 );
  */
}


void Display::render()
{
  // update frame count
  _frameCount += 1;
}



#if DEPTH == 1

static void VRAM_Write( uint8_t *ptr, uint8_t s )
{
  *ptr = s; // memory write;
}

static uint8_t VRAM_Read( uint8_t *ptr )
{
  return *ptr; // memory write;
}

static uint8_t *getVRAMPtr( uint8_t *vram, int16_t x, int16_t y )
{
  if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
    return 0;

  return vram + y*BYTEPITCH + x/8 ;
}


void Display::drawPixel_begin()
{
  ptr = 0;
}

void Display::drawPixel_next(int16_t k, int16_t y, uint16_t color, bool alpha)
{
  if ( !(k & 7) || !ptr )
  {
    if ( ptr ) VRAM_Write( ptr, vreg ); // memory write;
    if ( 0 == (ptr = getVRAMPtr( vram, k, y )) ) return;
    vreg = VRAM_Read( ptr ) ; // vram word being processed
  }
  
  if ( alpha == 0 ) return;
  uint8_t m = 0x80u >> (k & 7);  // erase mask to be applied
  uint8_t c = color << (7 - (k & 7)); // color mask to be applied
  _collided = _collided || (vreg & m);
  vreg = (vreg & ~m) | c;
}

void Display::drawPixel_end()
{
  if ( ptr ) VRAM_Write( ptr, vreg ); // memory write;
}

uint16_t Display::getPixel( int16_t x, int16_t y )
{
  uint8_t *ptr = getVRAMPtr(vram, x, y);
  if ( ptr )
  {
    uint8_t m = 0x80u >> (x & 7);  // mask to be applied
    uint8_t sh = 7 - (x & 7); // shift to be applies
    return (*ptr & m) >> sh;
  }
  return 0;
}

#endif


#if DEPTH == 4


// VRAM writes are only performed after a preceding read on the same address,
// since the address is already provided, we only provide here
// the data to be written, send it to the VGA, and restore read mode again
static void VRAM_Write( uint16_t s )
{
  // set data
  exe.writePort16(PORT_W2, s);                                  // data write port W2
  
  // set data write mode before extending output range to avoid contention
  // (may this cause a visual glitch?)
  exe.writeChannel8(PORT_X0, /*bitNumber*/ 0, /*bitValue*/ 1);  // set data write mode
  exe.outputRange(6);                                           // increase output range

  // unset data write mode before reducing output range
  // (this will technically create a bus contention, however with no harmfull effects as both
  // buses contain the same logic levels for the duration of the contention)
  exe.writeChannel8(PORT_X0, /*bitNumber*/ 0, /*bitValue*/ 0);  // cancel data writes
  exe.outputRange(4);                                           // reduce output range
}



// Default mode is reading from VRAM, 
// we just need to provide the target address and perform a read
static uint16_t VRAM_Read()
{
  // read from VGA memory
  uint16_t s = exe.readPort16(PORT_W2);  // Read port W2
  return s;
}

// // Default mode is reading from VRAM, 
// // we just need to provide the target address and perform a read
// static uint16_t VRAM_Read( uint16_t ptr )
// {
//   // set read address
//   exe.writePort16(PORT_W1, ptr);                                // address to read
//   exe.writeChannel8(PORT_X0, /*bitNumber*/1, /*bitValue*/1);    // set address writes
//   exe.writeChannel8(PORT_X0, /*bitNumber*/1, /*bitValue*/0);    // cancel address writes

//   // read from VGA memory
//   uint16_t s = exe.readPort16(PORT_W2);  // Read port W2

//   return s;
// }

static bool setVRAMPtr( uint16_t vram, int16_t x, int16_t y )
{
    //x += 80;
    //y += 80;
    if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
      return false;

    // set address
    exe.writePort16(PORT_W1, vram + y*WORDPITCH + x/4);           // address to read
    exe.writeChannel8(PORT_X0, /*bitNumber*/1, /*bitValue*/1);    // set address writes
    exe.writeChannel8(PORT_X0, /*bitNumber*/1, /*bitValue*/0);    // cancel address writes

    return true;
}

// static void VRAM_Write( uint16_t *ptr, uint16_t s )
// {
//   // set address
//   exe.writePort16(1, (uint16_t)ptr);                             // address to write
//   exe.writeChannel8(/*port*/0, /*bitNumber*/1, /*bitValue*/1);   // write address
//   exe.writeChannel8(/*port*/0, /*bitNumber*/1, /*bitValue*/0);   // cancel address writes

//   // set data
//   exe.writePort16(2,s);                                          // data to write
  
//   // set data write mode before extending output range to avoid contention
//   // (may this cause a visual glitch?)
//   exe.writeChannel8(/*port*/ 0, /*bitNumber*/ 0, /*bitValue*/ 1);  // set data write mode
//   exe.outputRange(6);                                              // increase output range

//   // unset data write mode before reducing output range
//   // (this will technically create a bus contention, however with no harmfull effects as both
//   // buses contain the same logic levels for the duration of the contention)
//   exe.writeChannel8(/*port*/ 0, /*bitNumber*/ 0, /*bitValue*/ 0);  // cancel data writes
//   exe.outputRange(4);                                              // reduce output range

//   //*ptr = (s >> 8) | (s << 8); // memory write
// }

// static uint16_t VRAM_Read( uint16_t *ptr )
// {
//   exe.writePort16(1, (uint16_t)ptr);                            // address to read
//   exe.writeChannel8(/*port*/0, /*bitNumber*/1, /*bitValue*/1);  // write address
//   exe.writeChannel8(/*port*/0, /*bitNumber*/1, /*bitValue*/0);  // cancel address writes

//   uint16_t s = exe.readPort16(2);
//   return s;

//   // uint16_t s = *ptr;
//   // return (s >> 8) | (s << 8); // memory read
// }

// static uint16_t getVRAMPtr( uint16_t vram, int16_t x, int16_t y )
// {
//   // if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
//   //   return -1;

//   return  vram + y*WORDPITCH + x/4;
//   //return  &vram[y * WORDPITCH + x / 4];
// }


void Display::drawPixel_begin()
{
  active = false;
}


void Display::drawPixel_next(int16_t x, int16_t y, uint16_t color, bool alpha)
{
  if ( !(x & 3) || !active )
  {
    if ( active ) VRAM_Write( vreg ); // memory write
    if ( false == (active = setVRAMPtr(vram, x, y)) ) return;
    vreg = VRAM_Read() ; // vram word being processed
  }
  
  if ( alpha == 0 ) return;
  uint8_t pos = (4 * (x & 3));  // position of pixel being processed
  uint16_t m = 0xf000u >> pos;  // erase mask to be applied
  uint16_t c = color << (12 - pos); // color mask to be applied
  _collided = _collided || (vreg & m);
  vreg = (vreg & ~m) | c;
}


void Display::drawPixel_end()
{
  if ( active ) VRAM_Write( vreg ); // memory write;
}


void Display::drawWord_next( int16_t x, int16_t y, uint16_t color )
{
  if ( !(x & 3) || !active )
  {
    if ( active ) VRAM_Write( vreg ); // memory write
    if ( false == (active = setVRAMPtr(vram, x, y)) ) return;
    vreg = 0;
  }
  vreg = (vreg<<4) | color;
}


void Display::drawWord( int16_t x, int16_t y, uint16_t word )
{
  if ( false == (active = setVRAMPtr(vram, x, y)) ) return;
  if ( active ) VRAM_Write( word ); // memory write
}


uint16_t Display::getPixel( int16_t x, int16_t y )
{
  if ( false == (active = setVRAMPtr(vram, x, y)) ) return 0;

  uint16_t r = VRAM_Read() ; // vram word being processed
  uint16_t m = 0xf000u >> (4 * (x & 3));  // mask to be applied
  uint16_t sh = 12 - (4 * (x & 3));  // shift to be applied
  return (r & m) >> sh;
}

// uint16_t Display::getPixel( int16_t x, int16_t y )
// {
//   uint16_t ptr = getVRAMPtr(vram, x, y);
//   if ( ptr == -1  ) return 0;
  
//   uint16_t r = VRAM_Read( ptr ) ; // vram word being processed
//   uint16_t m = 0xf000u >> (4 * (x & 3));  // mask to be applied
//   uint16_t sh = 12 - (4 * (x & 3));  // shift to be applied
//   return (r & m) >> sh;
// }

#endif








//void Display::drawPixel(int16_t x, int16_t y, uint16_t color)
//{
//  if (!vram)
//    return;
//
//  if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
//    return;
//
//  uint8_t *ptr = &vram[(x / 8) + y * ((WIDTH + 7) / 8)];
//  uint8_t s = VRAM_Read( ptr ); // Memory read
//  uint8_t m = 0x80 >> (x & 7);  // erase mask to be applied
//  uint8_t c = color << (7 - (x & 7)); // color mask to be applied
//  s &= ~m;
//  s |= c;
//  VRAM_Write( ptr, s ); // memory write;
//}




void Display::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                              int16_t w, int16_t h, uint16_t color)
{
  assert( w>=0 && h>=0 );
  int16_t bByteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t b = 0; // bitmap byte being processed

  _collided = false;
  for (int16_t j = 0; j < h; j++, y++)  // iterate rows
  {
    drawPixel_begin();
    for (int16_t i = 0, k = x ; i < w ; i++, k++)  // iterate whithin a row
    {
      if (i & 7) b <<= 1;
      else b = bitmap[j * bByteWidth + i / 8];
    
      drawPixel_next( k, y, color, (b & 0x80) ); // only set pixel color if bitmap bit was 1
    }
    drawPixel_end();
  }
}


void Display::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                              int16_t w, int16_t h, uint16_t foreColor, uint16_t backColor, bool aligned)
{
  assert( w>=0 && h>=0 );
  int16_t bByteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t b = 0; // bitmap byte being processed

  _collided = false;
  for (int16_t j = 0; j < h; j++, y++)  // iterate rows
  {
    drawPixel_begin();
    for (int16_t i = 0, k = x ; i < w ; i++, k++)  // iterate whithin a row
    {
      if (i & 7) b <<= 1;
      else b = bitmap[j * bByteWidth + i / 8];
      
      uint16_t color = (b & 0x80) ? foreColor : backColor;  // determine color based on bitmap bit
      if (aligned) drawWord_next(k, y, color);
      else drawPixel_next(k, y, color, true);   // set pixel color
    }
    drawPixel_end();
  }
}


// void Display::drawBitmapQuick(int16_t x, int16_t y, const uint8_t bitmap[],
//                               int16_t w, int16_t h, uint16_t foreColor, uint16_t backColor)
// {
//   assert( w>=0 && h>=0 );
//   int16_t bByteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
//   uint8_t b = 0; // bitmap byte being processed

//   _collided = false;
//   for (int16_t j = 0; j < h; j++, y++)  // iterate rows
//   {
//     drawPixel_begin();
//     for (int16_t i = 0, k = x ; i < w ; i++, k++)  // iterate whithin a row
//     {
//       if (i & 7) b <<= 1;
//       else b = bitmap[j * bByteWidth + i / 8];
      
//       uint16_t color = (b & 0x80) ? foreColor : backColor;  // determine color based on bitmap bit
//       drawWord_next(k, y, color);
//     }
//     drawPixel_end();
//   }
// }














void Display::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  assert( w>=0 && h>=0 );
  
  _collided = false;
  for ( int16_t j = 0 ; j < h ; j++, y++ )  // iterate rows
  {
    drawPixel_begin();
    for (int16_t k = x; k < x+w ; k++)  // iterate whithin a row
    {
      drawPixel_next(k, y, color, true);
    }
    drawPixel_end();
  }
}

void Display::fillRectAligned(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  assert( w>=0 && h>=0 );
  
  uint16_t word = (color<<12) | (color<<8) | (color<<4) | color;
  for ( int16_t j = 0 ; j < h ; j++, y++ )  // iterate rows
  {
    for (int16_t k = x; k < x+w ; k+=4)  // iterate whithin a row
    {
      drawWord(k, y, word);
    }
  }
}

void Display::drawPoint(int16_t x, int16_t y, uint16_t color)
{
  _collided = false;
  drawPixel_begin();
  drawPixel_next( x, y, color, true );
  drawPixel_end();
}


#define _swap_int16_t(a, b) { int16_t t = a; a = b;  b = t; }

void Display::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  if (x0 == x1)
  {
    if (y0 > y1) _swap_int16_t(y0, y1);
    fillRect(x0, y0, 1, y1 - y0 + 1, color);
    return;
  }
  
  if (y0 == y1)
  {
    if (x0 > x1) _swap_int16_t(x0, x1);
    fillRect(x0, y0, x1 - x0 + 1, 1, color);
    return;
  }
  
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep)
  {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1)
  {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep = -1;
  if (y0 < y1) ystep = 1;

  for (; x0 <= x1; x0++)
  {
    if (steep) drawPoint(y0, x0, color);
    else drawPoint(x0, y0, color);
    
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}



void Display::print( const char *cstr )
{
  // TO DO
  //printf( "Print at x:%d, y:%d : %s\n", cursor_x, cursor_y, cstr ) ;
}

void Display::print( int num)
{
  // TO DO
  //printf( "Print at x:%d, y:%d : %d\n", cursor_x, cursor_y, num ) ;
}




#if DEPTH == 1


#elif DEPTH == 4


#endif


void Display::pollEvent()
{
  keysArray = exe.readPort8(PORT_X7);
}

bool Display::eventQuit()
{
  return false;
}

bool Display::eventKeyRight()
{
  // read from channel X7
  return 0 != (keysArray & 0b100);
}

bool Display::eventKeyLeft()
{
  // read from channel X7
  return 0 != (keysArray & 0b010);
}

bool Display::eventKeySpace()
{
  // read from channel X7
  return 0 != (keysArray & 0b001);
}
