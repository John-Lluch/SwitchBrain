/*
THExec

Note that this uses direct port control instead of digitalWrite calls so that the code
can run as fast as possible.  This only works on the Arduino Pro Micro hardware 
 
Also see code from TommyPROM PromDevice.cpp and AVR documentation for implementation tips
https://github.com/TomNisbet/TommyPROM/blob/master/HardwareVerify/PromDevice.cpp#L95
https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7766-8-bit-AVR-ATmega16U4-32U4_Datasheet.pdf
*/

#include "THExec.h"

// Convenience i/o bit mask defines
#define SBA (1<<DDC6)
#define OE_BA (1<<DDD7)
#define CK_BA (1<<DDE6)
#define RST_AB (1<<DDB4)
#define CK_OEAB (1<<DDB5)
#define CK_AB (1<<DDB6)
#define SEL ((1<<DDB1) | (1<<DDB2) | (1<<DDB3))
#define SEL_Offset DDB1
#define DATA_LO (0x0f)
#define DATA_HI (0xf0)

// Convenience Direction Registers (DDR) defines
#define DDR_SBA DDRC
#define DDR_OE_BA DDRD
#define DDR_CK_BA DDRE
#define DDR_RST_AB DDRB
#define DDR_CK_OEAB DDRB
#define DDR_CK_AB DDRB
#define DDR_SEL DDRB
#define DDR_DATA_LO DDRD
#define DDR_DATA_HI DDRF

// Convenience Output Registers (PORT) defines
#define PORT_SBA PORTC
#define PORT_OE_BA PORTD
#define PORT_CK_BA PORTE
#define PORT_RST_AB PORTB
#define PORT_CK_OEAB PORTB
#define PORT_CK_AB PORTB
#define PORT_SEL PORTB
#define PORT_DATA_LO PORTD
#define PORT_DATA_HI PORTF

// Convenience Input Registers (PIN) defines
#define PIN_CK_BA PINE
#define PIN_DATA_LO PIND
#define PIN_DATA_HI PINF

// Configure initial values for i/o pins
void THExec::init()
{
  DDR_SBA |= SBA; // SBA set as output
  DDR_OE_BA |= OE_BA; // OE_BA set as output
  DDR_CK_BA |= CK_BA; // CK_BA set as output (this may change)
  DDR_RST_AB |= RST_AB; // RST_AB set as output
  DDR_CK_OEAB |= CK_OEAB; // CK_OEAB set as output
  DDR_CK_AB |= CK_AB; // CK_AB set as output
  DDR_SEL |= SEL; // Sel[0..2] set as output
}

// Configure de data direction register as output
static inline void setDATAModeOut()
{
  DDR_DATA_LO |= DATA_LO;   // we use PORTD0 trhough PORTD3 for data outputs 
  DDR_DATA_HI |= DATA_HI;   // we use PORTF4 through PORTF7 for data outputs
}

// Configure de data direction register as output
static inline void setDATAModeIn()
{
  DDR_DATA_LO &= ~DATA_LO;   // we use PIND0 trhough PORTD3 for data outputs 
  DDR_DATA_HI &= ~DATA_HI;   // we use PINF4 through PORTF7 for data outputs
}

// Convenience functions for seting and reseting control signals
static inline void setSBA() { PORT_SBA |= SBA; }
static inline void resetSBA() { PORT_SBA &= ~SBA; }
static inline void setOE_BA() { PORT_OE_BA |= OE_BA; }
static inline void resetOE_BA() { PORT_OE_BA &= ~OE_BA; }
static inline void pulseCK_BA() { PORT_CK_BA |= CK_BA; PORT_CK_BA &= ~CK_BA; }
static inline void setRST_AB() { PORT_RST_AB |= RST_AB; }
static inline void resetRST_AB() { PORT_RST_AB &= ~RST_AB; }
static inline void pulseCK_OEAB() { PORT_CK_OEAB |= CK_OEAB; PORT_CK_OEAB &= ~CK_OEAB; }
static inline void pulseCK_AB() { PORT_CK_AB |= CK_AB; PORT_CK_AB &= ~CK_AB; }

// Read CK_BA as input
static inline bool inputCK_BA() {
  return (PIN_CK_BA & CK_BA) != 0;
}

// Output byte to the data port register
static inline void writeDATA( uint8_t aByte ) {
  PORT_DATA_LO = (PORT_DATA_LO & ~DATA_LO) | (aByte & DATA_LO);  // move the lower nibble
  PORT_DATA_HI = (PORT_DATA_HI & ~DATA_HI) | (aByte & DATA_HI);  // move the higher nibble
}

// Input byte from the data pin register
static inline uint8_t readDATA() {
  return (PIN_DATA_LO & DATA_LO) | (PIN_DATA_HI & DATA_HI);
}

// Output port selection
static inline void portSEL( uint8_t port ) {
  PORT_SEL = (PORT_SEL & ~SEL) | (((7-port)<<SEL_Offset) & SEL);
}

// Selects the desired range of output ports that we want
// Pass in a number from 0 to 7
// 0 means no outputs (all inputs)
// 7 means all outputs (no inputs)  
void THExec::outputRange( uint8_t range )
{
  resetRST_AB();     // clear previous selection
  setRST_AB();       // enable selection
  while (range-- > 0)
    pulseCK_OEAB(); // select next port
}

// Sets port to transparent mode
void THExec::setTransparent( uint8_t port )
{
  resetOE_BA();           // disable data bus
  portSEL( port );        // select port
  setDATAModeIn();        // enable Arduino reads
  resetSBA();             // set transparent mode
  setOE_BA();             // enable data bus
}

// Write bit on port
void THExec::writeChannel8( uint8_t port, uint8_t bitNumber, bool bitValue )
{
  uint8_t mask = 1 << bitNumber;
  uint8_t aByte = -bitValue;  // this will set all ones if bitValue is 1
  uint8_t value = (thePorts.port8[port] & ~mask) | (aByte & mask) ;  // move the higher nibble
  writePort8( port, value );
}

// Write 8 bit port
void THExec::writePort8( uint8_t port, uint8_t value )
{
  thePorts.port8[port] = value;

  resetOE_BA();           // disable data bus
  setDATAModeOut();       // enable Arduino writes

  portSEL( port );        // select port
  writeDATA( value );     // write value
  pulseCK_AB();           // clock value into B register
}

// Write 16 bit port
void THExec::writePort16( uint8_t portW, uint16_t value )
{
  thePorts.port16[portW] = value;
  uint8_t portLow = portW << 1;
  uint8_t portHig = portLow + 1;

  resetOE_BA();           // disable data bus
  setDATAModeOut();       // enable Arduino writes

  portSEL( portLow );     // select port
  writeDATA( thePorts.port8[portLow] );     // write value
  pulseCK_AB();           // clock value into B register

  portSEL( portHig );     // select port
  writeDATA( thePorts.port8[portHig] );     // write value
  pulseCK_AB();           // clock value into B register
}

// Write 32 bit port
void THExec::writePort32( uint8_t portL, uint32_t value )
{
  thePorts.port32[portL] = value;
  uint8_t portLowL = portL << 2;
  uint8_t portLowH = portLowL + 1;
  uint8_t portHigL = portLowL + 2;
  uint8_t portHigH = portLowL + 3;

  resetOE_BA();           // disable data bus
  setDATAModeOut();       // enable Arduino writes

  portSEL( portLowL );     // select port
  writeDATA( thePorts.port8[portLowL] );     // write value
  pulseCK_AB();           // clock value into B register

  portSEL( portLowH );     // select port
  writeDATA( thePorts.port8[portLowH] );     // write value
  pulseCK_AB();           // clock value into B register

  portSEL( portHigL );     // select port
  writeDATA( thePorts.port8[portHigL] );     // write value
  pulseCK_AB();           // clock value into B register

  portSEL( portHigH );     // select port
  writeDATA( thePorts.port8[portHigH] );     // write value
  pulseCK_AB();           // clock value into B register
}

// Read bit on port
bool THExec::readChannel8( uint8_t port, uint8_t bitNumber )
{
  uint8_t mask = 1 << bitNumber;
  uint8_t value = readPort8( port );
  return 0 != (value & mask);
}

// Read 8 bit port
uint8_t THExec::readPort8( uint8_t port )
{
  setDATAModeIn();         // enable Arduino reads
  pulseCK_BA();            // clock B ports into the A register
  setSBA();                // set latched mode
  resetOE_BA();            // disable data bus
  portSEL( port );         // select port
  setOE_BA();              // enable data bus
  uint8_t readData = readDATA();

  thePorts.port8[port] = readData;
  return thePorts.port8[port];
}

// Read 16 bit port
uint16_t THExec::readPort16( uint8_t portW )
{
  uint8_t portLow = portW << 1;
  uint8_t portHig = portLow + 1;

  setDATAModeIn();         // enable Arduino reads
  pulseCK_BA();            // clock B ports into the A register
  setSBA();                // set latched mode

  resetOE_BA();            // disable data bus
  portSEL( portLow );      // select port
  setOE_BA();              // enable data bus
  uint8_t readDataLow = readDATA();

  resetOE_BA();            // disable data bus
  portSEL( portHig );      // select port
  setOE_BA();              // enable data bus
  uint8_t readDataHig = readDATA();

  thePorts.port8[portLow] = readDataLow;
  thePorts.port8[portHig] = readDataHig;
  return thePorts.port16[portW];
}

// Read 32 bit port
uint32_t THExec::readPort32( uint8_t portL )
{
  uint8_t portLowL = portL << 2;
  uint8_t portLowH = portLowL + 1;
  uint8_t portHigL = portLowL + 2;
  uint8_t portHigH = portLowL + 3;

  setDATAModeIn();         // enable Arduino reads
  pulseCK_BA();            // clock B ports into the A register
  setSBA();                // set latched mode

  resetOE_BA();            // disable data bus
  portSEL( portLowL );      // select port
  setOE_BA();              // enable data bus
  uint8_t readDataLowL = readDATA();

  resetOE_BA();            // disable data bus
  portSEL( portLowH );      // select port
  setOE_BA();              // enable data bus
  uint8_t readDataLowH = readDATA();

  resetOE_BA();            // disable data bus
  portSEL( portHigL );      // select port
  setOE_BA();              // enable data bus
  uint8_t readDataHigL = readDATA();

  resetOE_BA();            // disable data bus
  portSEL( portHigH );      // select port
  setOE_BA();              // enable data bus
  uint8_t readDataHigH = readDATA();

  thePorts.port8[portLowL] = readDataLowL;
  thePorts.port8[portLowH] = readDataLowH;
  thePorts.port8[portHigL] = readDataHigL;
  thePorts.port8[portHigH] = readDataHigH;
  return thePorts.port32[portL];
}
