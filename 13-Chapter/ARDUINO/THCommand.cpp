/*
THCommand

This class implements a descent recursive parser to interpret
arbitrary commands issued by the user through a terminal app
*/


#include <Arduino.h>
#include "THCommand.h"
#include "THTerminal.h"
#include "THExec.h"

#define THCommandHelpString "\
\r\n\
-------------------------------------------------\r\n\
\r\n\
THOR\r\n\
\r\n\
64 channel Logic Analyzer\r\n\
and Digital Signal Generator\r\n\
based on an Arduino microcontroller\r\n\
\r\n\
List of available THOR commands:\r\n\
\r\n\
X<port>                      :Byte read from port\r\n\
X<port>=<value>              :Byte write to port\r\n\
X<port>.<channel>            :Bit 0..7 read from channel\r\n\
X<port>.<channel>=<value>    :Bit 0..7 write to channel\r\n\
\r\n\
W<port>                      :Word (16 bit) read from port\r\n\
W<port>=<value>              :Word (16 bit) write to port\r\n\
W<port>.<channel>            :Bit 0..15 read from channel\r\n\
W<port>.<channel>=<value>    :Bit 0..15 write to channel\r\n\
\r\n\
L<port>                      :Long (32 bit) read from port\r\n\
L<port>=<value>              :Long (32 bit) write to port\r\n\
L<port>.<channel>            :Bit 0..31 read from channel\r\n\
L<port>.<channel>=<value>    :Bit 0..31 write to channel\r\n\
\r\n\
range <value>                :Set output port range (0 to 8)\r\n\
poll X<port>                 :Start polling port\r\n\
\r\n\
help                         :Display this list\r\n\
\r\n\
-------------------------------------------------\r\n\
\r\n\
"

static bool printHelp()
{
    return Serial.println( F(THCommandHelpString) );
}

// Print a value with the following format
//    bin:00000011, hex:03, dec:3
//    0b00000011 0x03 3
static void printValue( long value, uint8_t width )
{
  uint32_t v = value;
  //Serial.print( F("bin:") );
  Serial.print( F("0b") );
  for ( uint8_t i=width ; i>0 ; i-- )
    Serial.write( ((v >> (i-1)) & 1) + '0' );

  //Serial.print( F(" hex:") );
  Serial.print( F(" 0x") );
  for ( uint8_t i=width ; i>0 ; i-=4 )
  {
    uint8_t h = (v >> (i-4)) & 0xf;
    Serial.write( h > 9 ? h + ('a'-10) : h + '0' );
  }

  //Serial.print( F(" dec:") );
  Serial.print( F(" ") );
  Serial.println( v );
}

//-------------------------------------------------
// Display help string
void THTerminal::help( void )
{
    printHelp();
}

//-------------------------------------------------
// Constructor
THCommand::THCommand( THTerminal &terminal, THExec &exe ) : 
  PrimitiveParser(terminal.buffer()), _terminal(terminal), _exe(exe) {}

//-------------------------------------------------
// Display error message
void THCommand::error(const __FlashStringHelper * errStr)
{
    Serial.println( errStr ); 
}

//-------------------------------------------------
// Initialize dependendable objects
void THCommand::init()
{
  _terminal.init();
  _exe.init();
}

//-------------------------------------------------
// Helper to Parse any of the commands to read/write, port or channels 
bool THCommand::parseVAR( uint8_t &width, uint8_t &port, uint8_t &channel )
{
  width = 0;
  if      ( parseChar( 'x' ) || parseChar( 'X' ) ) width = 8;
  else if ( parseChar( 'w' ) || parseChar( 'W' ) ) width = 16;
  else if ( parseChar( 'l' ) || parseChar( 'L' ) ) width = 32;

  if ( width != 0 )
  {
    long thePort;
    if ( parseUnsigned( thePort ) )
    {
      // range error check
      if ( thePort >= 0 && ( 
            (width == 8  && thePort < 8) ||
            (width == 16 && thePort < 4) ||
            (width == 32 && thePort < 2) ) )
      {  
        port = thePort;
        skipSP();
        if ( parseChar( '.' ) )  // bit access ahead
        {
          long theChannel;
          skipSP();
          if ( parseUnsigned( theChannel ) )
          {
            // range error check
            if ( theChannel >= 0 && ( 
                  (width == 8  && theChannel < 8) ||
                  (width == 16 && theChannel < 16) ||
                  (width == 32 && theChannel < 32) ) )
            {  
              channel = theChannel;
              return true;
            } else error( F("Channel out of range") ); 
          } else error( F("Expecting channel number after '.'") );
        } 
        else
        {
          channel = 0xff;
          return true;
        }
      } else error( F("Port out of range") );
    } else error( F("Missing port number") );
  }
  return false;
}


//-------------------------------------------------
// Parse any of the commands to read/write, port or channels 
bool THCommand::parseVAR()
{
  uint8_t width, port, channel;
  if ( parseVAR( width, port, channel ) )
  {
    skipSP();
    if ( parseChar( '=' ) ) // it's a write
    {
      long writeValue;
      skipSP();
      if ( parseAnyBaseInt( writeValue ) )
      {
        if ( channel != 0xff ) 
          switch ( width )  // channel writes
          {
            case 8: _exe.writeChannel8( port, channel, writeValue ); break;
            case 16: _exe.writeChannel16( port, channel, writeValue ); break;
            case 32: _exe.writeChannel32( port, channel, writeValue ); break;
          }
        else
          switch ( width ) // port writes
          {
            case 8: _exe.writePort8( port, writeValue ); break;
            case 16: _exe.writePort16( port, writeValue ); break;
            case 32: _exe.writePort32( port, writeValue ); break;
          }
        return true;
      } else error( F("Expecting value after '='") );
    }

    else // it's a read
    {      
      long readValue = 0;
      if ( channel != 0xff )
        switch ( width )  // channel reads
        {
          case 8: readValue = _exe.readChannel8( port, channel ); break;
          case 16: readValue = _exe.readChannel16( port, channel ); break;
          case 32: readValue = _exe.readChannel32( port, channel ); break;
        }    
      else
        switch ( width ) // port reads
        {
          case 8: readValue = _exe.readPort8( port ); break;
          case 16: readValue = _exe.readPort16( port ); break;
          case 32: readValue = _exe.readPort32( port ); break;
        }
      printValue( readValue, width );
      return true;
    }
  }
  return false;
}

//-------------------------------------------------
bool THCommand::parseTRANSPARENT()
{
  if ( parseString( "transparent" ) )
  {  
    skipSP();
    long port;
    if ( parseAnyBaseInt( port ) )
    {
      _exe.setTransparent( port );
      return true;
    } 
    else error ( F("Missing port number") );
  }
  return false;
}

//-------------------------------------------------
bool THCommand::parseUPLOAD()
{
  if ( ongoing == none )
  {
    if ( parseString( "upload" ) )
    {  
      skipSP();
      long addr;
      if ( parseAnyBaseInt( addr ) )
      {
        Serial.print( F("Executing Upload at address: ") );
        Serial.println( addr );
        ongoing = upload;
        return true;
      } 
      else error ( F("Missing upload address") );
    }
  }
  
  else if ( ongoing == upload )
  {
    skipSP();
    if ( eol() )
    {
      ongoing = none;
      return true;
    }

    const char *token;
    int len;
    if ( parseToken( token, len ) )
    {
      char s[len+1];
      for ( int i= 0 ; i<len ; i++ ) s[i] = token[i];
      s[len] = '\0';
      Serial.print( F("Upload is loading: ") );
      Serial.println( s );
      return true;
    }
  }
  return false;
}

//-------------------------------------------------
// Parse the POLL command
bool THCommand::parsePOLL()
{
  if ( ongoing == none )
  {
    if ( parseString( "poll" ) )
    {  
      uint8_t width, port, channel;
      skipSP();
      if ( parseVAR( width, port, channel ) )
      {
        ongoing = poll;
        pollPort = port;
        return continuePOLL();
      } 
      else error ( F("Missing upload address") );
    }
  }
  
  else if ( ongoing == poll )
  {
    skipSP();
    if ( eol() )
    {
      ongoing = none;
      return true;
    }
  }
  return false;
}

//-------------------------------------------------
bool THCommand::continuePOLL()
{
  if ( ongoing == poll )
  {
    long value = _exe.readPort8( pollPort );
    if ( value != pollValue )
    {
        printValue( value, 8 );
        pollValue = value;
    }
    return true;
  }
  return false;
}

//-------------------------------------------------
// Parse the KITT command
bool THCommand::parseKITT()
{
  if ( ongoing == none )
  {
    if ( parseString( "kitt" ) )
    {  
      Serial.println( F("Executing KITT") );
      ongoing = kitt;
      return true;
    }
  }
  
  else if ( ongoing == kitt )
  {
    skipSP();
    if ( eol() )
    {
      ongoing = none;
      return true;
    }
  }
  return false;
}

//-------------------------------------------------
bool THCommand::continueKITT()
{
  static unsigned long last = 0;
  static uint32_t kittValue = 0xf;
  static bool back = false;
  if ( ongoing == kitt )
  {
    unsigned long time = millis();
    if ( time - last > 50 )
    {
      last = time;
      _exe.writePort32(0, kittValue);
      kittValue = back ? (kittValue>>1) : (kittValue<<1);
      if ( kittValue == 0x7 ) back = false, kittValue=0xf;
      if ( kittValue == 0xe0000000 ) back = true, kittValue=0xf0000000;
    }
    return true;
  }
  return false;
}

//-------------------------------------------------
bool THCommand::parseRANGE()
{
  if ( parseString( "range" ) )
  {  
    skipSP();
    long range;
    if ( parseUnsigned( range ) )
    {
      if ( range >=0 && range <= 8)  // valid ranges are 0 to 8
      {
        _exe.outputRange( range );
        return true;
      } else error ( F("Invalid range value") );
    }
    else error ( F("Missing range value") );
  }
  return false;
}

//-------------------------------------------------
// Commands that depend on terminal input
bool THCommand::parseCommand()
{ 
  if ( parseUPLOAD() ) return true;
  if ( parsePOLL() ) return true;
  if ( parseKITT() ) return true;
  if ( parseVAR() ) return true;
  if ( parseRANGE() ) return true;
  if ( parseTRANSPARENT() ) return true;
  if ( eol() ) return true;
  if ( parseString( "help" ) ) 
    return printHelp();
  return false;
}

//-------------------------------------------------
// Continuation of actions that are independent of terminal input 
bool THCommand::continueCommand()
{
  if ( continuePOLL() ) return true;
  if ( continueKITT() ) return true;
  return true;
}

//-------------------------------------------------
// This is the starting method for this class.
// Call it repeatedly to get input strings from a terminal app
// and get them interpreted by the parser
void THCommand::next(void)
{
  // Attempt to process user input
  THTerminalMode mode = THTerminalModeNormal;
  if ( ongoing == upload ) mode = THTerminalModeFile;
  if ( ongoing == poll || ongoing == kitt ) mode = THTerminalModePassive;
  if ( _terminal.processUserInput( mode ) )
  {
    // We have an user input line ready,
    // so parse it
    c = _terminal.c;          // current character index
    end = _terminal.end;      // last character in source

    skipSP();
    if ( parseCommand() )
    {
      skipSP();
      if ( eol() ); // parse is ok if nothing else after valid command
      else error( F("Extra characters after valid command ignored") );
    }
    else error( F("Unknown Command") );
  }

  // some actions require ongoing processing even without any 
  // termimal input, so give them the opportunity to do so 
  else
  {
    if ( continueCommand() );
    else error( F("Cound not continue command") );
  }
}


