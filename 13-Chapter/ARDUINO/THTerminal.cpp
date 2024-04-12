/*
THTerminal

This class maintains a char buffer that stores a list of user typed commands
separated by null terminators.

The command currently being typed goes inserted to the beginning of the 
char buffer.

Previous commands can be recalled with the up and down arrow keys.

Older commands are discarded as the buffer gets full due to new characters
being inserted.
*/

#include <Arduino.h>
#include "THTerminal.h"

//Put here your desired prompt string
#define THTerminalPrompString "Thor % "

//-------------------------------------------------
// Constructor
THTerminal::THTerminal( unsigned long bps, char *buff, const int size ) :
   Bps(bps), _buff(buff), BuffSize(size-1) {}

//-------------------------------------------------
// Debug function, please ignore
static void debugPrint( char ch )
{
  char s[20];
  sprintf( s, " [%d]", ch );
  Serial.print( s );
}

//-------------------------------------------------
// Debug function, please ignore
static void debugPrintSt( char *ch,  int len )
{
  char s[80];
  for ( int i=0 ; i<len ; i++ )
  {
    sprintf( s, " [%d]", ch[i] );
    Serial.print( s );
  }
}

//-------------------------------------------------
// Initialize variables and start serial communications
void THTerminal::init()
{
  _beg = 0;
  _index = 0;
  _end = 0;
  _length = 0;
  _buff[0]= '\0';
  _isFirstKey = true;
  _sequence = none_SQ;
  _flowControl = none_FC;
  _commandEnded = false;
   Serial.begin(Bps);
}

//-------------------------------------------------
// Display promp string
void THTerminal::prompt( void )
{
  // always enable insert mode
  Serial.print( "\e[4h");

  // delete 3 characters so at least terminals 
  // not supporting the above but supporting delete
  // won't look weird
  Serial.print( "\x7F\x7F\x7F");

  // display the prompt
  Serial.print( F(THTerminalPrompString) );
}

//-------------------------------------------------
// Recall next command on the list (terminal arrow up)
bool THTerminal::_recallUp()
{
  if ( _end == _length )
    return false;
  
  _beg = _end + 1;
  while ( _buff[++_end] != '\0' ); 
  _index = _end;
  return true;
}

//-------------------------------------------------
// Recall previous command on the list (terminal arrow down)
bool THTerminal::_recallDown()
{
  if ( _beg <= 0 )
    return false;
  
  _end = _beg - 1;
  _beg = _end - 1;
  while ( _beg >= 0 && _buff[_beg] != '\0' ) --_beg;
  _beg++;
  _index = _end;
  return true;
}

//-------------------------------------------------
// Move current command to the front of the list and insert leading terminator (enter key)
void THTerminal::_storeRecall()
{
  // get the command length and whether there is already a leading terminator
  int len = _end - _beg;
  bool leading = _buff[0] == '\0';

  // will skip leading terminator if pressent
  _index = 0;
  if ( leading ) _index = 1;

  // copy command to the front if not already there
  if ( _beg > 1 )
  {
    int source = _end;
    for ( int i = 0 ; i <= len ; i++ )  // this includes the trailing terminator
      _insert( _buff[source], false );
  }
  
  // insert now a leading terminator if it was not there
  if ( !leading )
    _insert( '\0', false ); // insert a separator

  // update command position so that it is correctly reflected
  _beg = 1;
  _end = _beg+len;
}

//-------------------------------------------------
// Delete chatacter at current position (delete key)
bool THTerminal::_delete()
{
  if ( _index <= _beg )
    return false;
  
  for ( int i = _index ; i < _length ; i++  )
    _buff[i-1] = _buff[i];   // shift characters left

  _index--;
  _buff[--_end] = '\0';  // always add a null terminator
  _buff[--_length] = '\0';  // always add a null terminator
  return true;
}

//-------------------------------------------------
// Insert character at the current position (character key)
// Pass in 'insert' as true to move cursor position to the following character 
bool THTerminal::_insert( char chr, bool insert )
{
  if ( _length >= BuffSize && _end < _length )
    while ( _buff[--_length] != '\0' ) ;

  if ( _length >= BuffSize )
    return false;
  
  for ( int i = _length ; i > _index ; i-- )
    _buff[i] = _buff[i-1];  // shift characters right

  _buff[_index] = chr;      // insert character to the buffer
  _buff[++_length] = '\0';  // always add a null terminator at the end
  if ( insert ) _index++;   // update cursor possition if needed
  _end++;
  return true;
}

//-------------------------------------------------
// This is the main entry method to this class.
// This method is asynchronous and it is meant to be called repeatedly.
// Returns true after reading an input line up to the carriage return.
// Otherwise it returns false and awaits further calls to continue processing.
//
// The THTerminalModePasive mode will process a string normally 
// up to the carriage return but will not display the prompt. 
// This mode is meant to process multi-line commands
//
// The THTerminalModeFile will still process a string up to the carriage return 
// but it is optimised to process bulk input such as comming from text files,
// This mode disables the command recall buffer so previous commands will not be
// available trhough the up/down keys after using it.
//
// The class also implements software Flow Control (Xon/Xoff) to prevent serial 
// buffer overflow when receiving large files. However, note that this may
// stil fail if the client terminal does not honour control flow characters
// or it is sending data too fast for them to get catched on time, so care
// must be taken on the terminal settings to account for this.

bool THTerminal::processUserInput( THTerminalMode mode )
{
  // display prompt if required
  if ( _commandEnded && mode == THTerminalModeNormal )
    prompt();  

  // obtain how many characters are waiting in the Serial input buffer
  bool ended = false;
  int numchars = Serial.available();

  // enable incoming flow only after the incoming serial buffer is empty 
  if ( _flowControl != XON_FC && numchars == 0 )
  {
    _flowControl = XON_FC;
    Serial.write( '\021' ); // send XON '\x11'
  }

  // process one character at a time
  for ( int i = 0 ; i < numchars ; i++ )
  { 
    char chr = (char)Serial.read();

    // uncomment for debug purposes 
    //debugPrint( chr );

    if ( mode == THTerminalModeFile )
    {
      // end of line
      if ( chr == '\r' || chr == '\n' )
      {
        _length = 0;

        // exit the loop
        ended = true;
        break;  
      }

      // any printable character
      if ( chr >= 32 && chr <= 126 )
      {
        if ( _end >= BuffSize )
          continue;

        _buff[_end++] = chr;
        _buff[_end] = '\0';
        _length = _end;
      }

      continue;
    }

    // THTerminalModeNormal and THTerminalModePassive mode
    // processing starts here

    // *** the first key press just displays help and is ignored
    if ( _isFirstKey )
    {
       _isFirstKey = false;
       help();

       ended = true; // exit the loop
       break;
    }

    // *** process escape sequence
    else if ( _sequence == isEscaping_SQ )
    {
      if ( chr == '[' ) _sequence = isArrow_SQ;  // mark we entered arrow key sequence
      else _sequence = none_SQ;  // unsuported sequence, just reset and ignore
    }

    // *** arrow key
    else if ( _sequence == isArrow_SQ )
    {
      // up or down
      if ( chr == 'A' || chr == 'B' )  
      {
        int len = _index - _beg;
        bool recall = ( chr == 'A' ? _recallUp() : _recallDown() );
        if ( recall )
        {
          if ( len > 0 )   
          {
            // this may be a bug on the VT100 or just intended behaviour?
            // we need to filter for len==0 to prevent undesired cursor left
            Serial.print( "\e[" ); 
            Serial.print( len );  
            Serial.print( 'D' );    // cursor left
          }
          Serial.print( "\e[K" );      // erase to end
          Serial.print( _buff+_beg );  // echo recalled command
        }
      }
      // right
      else if ( chr == 'C' ) 
      {
        if ( _index < _end )  
        {
          _index++;
           Serial.print( "\e[C" );   // cursor right sequence
        }
      }
      // left
      else if ( chr == 'D' ) // left
      {
        if ( _index > _beg )
        {
          _index--;
           Serial.print( "\e[D" );   // cursor left sequence
           //Serial.print( "\eD" );   // cursor left sequence
        }
      }
      // always reset sequence at this point
      _sequence = none_SQ;
    }

    // *** escape key
    else if ( chr == '\e' )
    {
      // mark we entered a escape sequence
      _sequence = isEscaping_SQ;
    }

    // *** return key 
    // (Note that either return or newline is interpreted the same way,
    // this may cause duplicate processing on terminals providing both)
    else if ( chr == '\r' || chr == '\n')
    {
      _storeRecall();
      Serial.print( "\r\n" ); // echo both a carriage return and newline character

      // exit the loop
      ended = true;
      break;  
    }

    // *** printable characters
    else if ( chr >= 32 && chr <= 126 ) 
    {
      if ( _insert( chr ) )
        Serial.print( chr );   // echo the character
    }

    // *** delete key
    else if ( chr == 127 || chr == 8 )
    {
      if ( _delete() )
        Serial.print( "\b\e[P" );   // echo backspace + delete character sequence
    }

    // *** ignore everything else
    else
    {
      // do nothing  
    }

  } // end for

  if ( ended )
  {
    // dissable incoming flow
    _flowControl = XOFF_FC;
    Serial.write( '\023' );   //send XOFF '\x13' 

    // uncomment for debug purposes 
    //debugPrintSt( _buff, _length );

    // update parser variables
    c = _buff + _beg;
    end = _buff + _end;

    // prepare for next user command
    _index = 0;
    _beg = 0;
    _end = 0;
  }

  // will return false if we have not yet an entire line
  _commandEnded = ended;
  return ended;
}



