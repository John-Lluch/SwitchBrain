#ifndef THExec_H
#define THExec_H

/*
THExec

Class to implement the interface with the Logic Analyzer.

This is highly dynamic as new features can be
implemented while new uses are found for it.

Refer to method implementations for details.
*/

#include <avr/io.h>

// 8 bit ports
#define PORT_X0 0 
#define PORT_X1 1 
#define PORT_X2 2 
#define PORT_X3 3
#define PORT_X4 4
#define PORT_X5 5
#define PORT_X6 6
#define PORT_X7 7

// 16 bit ports
#define PORT_W0 0 
#define PORT_W1 1 
#define PORT_W2 2 
#define PORT_W3 3

// 32 bit ports
#define PORT_L0 0 
#define PORT_L1 1

// cached port data
typedef union
{ 
  uint8_t port8[8];
  uint16_t port16[4];
  uint32_t port32[2];
} LAPorts;

// THExex
class THExec
{
  public:
    void init();

  public:
    void outputRange( uint8_t range );
    void writeChannel8( uint8_t port, uint8_t bitNumber, bool bitValue );
    void writePort8( uint8_t port, uint8_t value );
    void writePort16( uint8_t portW, uint16_t valueW );
    void writePort32( uint8_t portL, uint32_t valueL );
    
    inline void writeChannel16( uint8_t port, uint8_t bitNumber, bool bitValue ) {
      writeChannel8( port*2 + bitNumber/8, bitNumber%8, bitValue );
    }
    
    inline void writeChannel32( uint8_t port, uint8_t bitNumber, bool bitValue ) {
      writeChannel8( port*4 + bitNumber/8, bitNumber%8, bitValue );
    }

    bool readChannel8( uint8_t port, uint8_t bitNumber );
    uint8_t readPort8( uint8_t port );
    uint16_t readPort16( uint8_t portW );
    uint32_t readPort32( uint8_t portL );
    
    inline bool readChannel16( uint8_t port, uint8_t bitNumber ) {
      return readChannel8( port*2 + bitNumber/8, bitNumber%8 );
    }

    inline bool readChannel32( uint8_t port, uint8_t bitNumber ) {
      return readChannel8( port*4 + bitNumber/8, bitNumber%8 );
    }

    void setTransparent( uint8_t port );

  private:
    LAPorts thePorts;
};



#endif
