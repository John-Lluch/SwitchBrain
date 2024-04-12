#ifndef PrimitiveParser_H
#define PrimitiveParser_H

/*
PrimitiveParser

Base class for the quick implementation of text based parsers.

Derived clases are responsible to initialize the class by
passing a pointer to the source text in ascii or utf8 representation.

Use by calling class methods that identify patterns in the source file. 

Class methods may have no return value or return a boolean value to signal success in simple cases.
Any pattern information being identified is returned as a reference to the appropiate data type.

Functions with only advance the character index if they successfully identify patterns

Non private properties are directly accessible
*/


class PrimitiveParser
{

  private:
    const char * const s;      // source string

  protected:
    const char *c;             // current character index
    const char *end;           // Last character in source

  protected:
    PrimitiveParser( const char * const source );
    void skipSP();
    bool eol();
    bool parseChar( char ch );
    bool parseRawString( const char *cStr );
    bool parseString( const char *cStr );
    bool parseInteger( long &value, int base = 10 );
    bool parseUnsigned( long &value, int base = 10 );
    bool parseAnyBaseInt( long &value );
    bool parseToken( const char *&token, int &len );

  private:
    bool _done;           
    long _value;
    bool _parseToken();
};

#endif
