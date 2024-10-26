/*
PrimitiveParser
*/

#include "PrimitiveParser.h"

//-------------------------------------------------
// Constructor
PrimitiveParser::PrimitiveParser( const char * const source ) : s(source)
{
  c = s;  // first character
  end = s;  // last character
}

//-------------------------------------------------
// Skip Spaces and Tabs
bool PrimitiveParser::skipSP()
{
  while ( c < end && (*c == ' ' || *c == '\t') )
    c += 1;

  return true;
}

//-------------------------------------------------
// Parse single character
bool PrimitiveParser::parseChar( char ch )
{ 
  if ( c < end && *c == ch ) 
    return c += 1, true;
  return false;
}

//-------------------------------------------------
// Returns true at end of line
bool PrimitiveParser::eol()
{ 
  return ( c == end );
}

//-------------------------------------------------
// Helper function to parse arbitrary tokens. Underscores and dots are allowed as part of
// the token name, but the token shall not start with a number or a dot
bool PrimitiveParser::_parseToken()
{
  if ( c < end && ( 
      ( *c >= 'a' && *c <= 'z') || ( *c >= 'A' && *c <= 'Z') || 
      ( *c == '_' ) ) ) {
 
    c += 1;
    while ( c < end && ( 
      ( *c >= 'a' && *c <= 'z') || ( *c >= 'A' && *c <= 'Z') ||
      ( *c >= '0' && *c <= '9') || ( *c == '_' ) || ( *c == '.' ) ) ) {
        c += 1;
    }
    return true;
  }      
  return false;
}

//-------------------------------------------------
// Parse a Token. Return address and lenght by reference
bool PrimitiveParser::parseToken( const char *&token, int &len )
{
  const char *svc = c;
  if ( _parseToken() )
  {
    token = svc;
    len = c - svc;
    return true;
  }
  return false;
}

//-------------------------------------------------
// Parse a raw string. Returns true if the string is immediatelly identified
// regardless of what's next
bool PrimitiveParser::parseRawString( const char *cStr )
{
  const char *svc = c;
  while ( c < end && *cStr != '\0' && *c == *cStr ) 
  {
    c += 1;
    cStr += 1;
    continue;
  }
  
  if ( *cStr != '\0' ) 
  {
    c = svc ;
    return false;
  }
  
  return true;
}

//-------------------------------------------------
// Parse a concrete token. Returns true if the token is identified
// and the character after the token is not a valid token continuation character
bool PrimitiveParser::parseString( const char *cStr )
{
  const char *svc = c;  
  if ( !parseRawString( cStr ) )
    return false;

  if ( c < end && ( 
        ( *c >= 'a' && *c <= 'z') || ( *c >= 'A' && *c <= 'Z') ||
        ( *c >= '0' && *c <= '9') || ( *c == '_' ) || ( *c == '.' ) ) )
  {
    c = svc;
    return false;
  }
    
  return true;
}

//-------------------------------------------------
// Parse any integer with base prefix
bool PrimitiveParser::parseAnyBaseInt( long &value )
{
  int base = 10;
  if ( c < end && *c == '0' )
  {
    c += 1;
    if      ( c < end && (*c == 'b' || *c == 'B' )) c += 1, base = 2;
    else if ( c < end && (*c == 'x' || *c == 'X' )) c += 1, base = 16;
    else c -= 1, base = 8; // this will reevaluate the '0' character
  }

  long result = 0;
  if ( parseUnsigned( result, base ) )
  {
    value = result;
    return true;
  }

  return false;
}

//-------------------------------------------------
// Parse an integer expressed in base 1, 8, 16, 10. Copy into referenced long if succesful
bool PrimitiveParser::parseInteger( long &value, int base )
{
  const char *svc = c;
  bool minus = false;
  if ( c < end && *c == '-' )
  {
    minus = true;
    c += 1;
  }

  long result;
  if ( parseUnsigned( result, base ) )
  {
    value = ( minus ? -result : result );
    return true;
  }

  c = svc;
  return false;
}

//-------------------------------------------------
// Parse an integer expressed in base 2, 8, 16, 10. Copy into referenced long if succesful
bool PrimitiveParser::parseUnsigned( long &value, int base )
{
  auto lowerCharacter = []( char ch ) -> int
  {
    if ( ch <= '9' ) return ch - '0';
    if ( ch <= 'F' ) return 10 + ch - 'A';
    if ( ch <= 'f' ) return 10 + ch - 'a';
    return 0;
  };

  auto checkRange = []( char ch, int base ) -> bool
  {
    if ( (base == 10 || base == 16) && (ch >= '0' && ch <= '9' ) ) return true;
    if ( base == 8 && (ch >= '0' && ch <= '7') ) return true;
    if ( base == 2 && (ch >= '0' && ch <= '1') ) return true;
    if ( base == 16 && ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F' )) ) return true;
    return false;
  };
  
  if ( c < end && checkRange( *c, base ) )
  {
    long result = lowerCharacter( *c );
    c += 1;
      
    while ( c < end && checkRange( *c, base ) )
    {
       result = result*base + lowerCharacter( *c );
       c += 1;
    }
      
 //   value = ( minus ? -result : result );
    value = result;
    return true;
  }
  
//  c = svc;
  return false;
}



// //-------------------------------------------------
// bool PrimitiveParser::parseInt( long &value )
// {
//  char *begPtr = c;
//  char *endPtr = NULL;
//  value = strtol( c, &endPtr, 0 );
//  if ( endPtr > begPtr ) return c = endPtr, true; 
//  return false;
// }
