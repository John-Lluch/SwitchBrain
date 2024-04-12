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


typedef union
{ 
  uint8_t port8[8];
  uint16_t port16[4];
  uint32_t port32[2];
} LAPorts;

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
