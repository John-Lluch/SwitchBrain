#ifndef THCommand_H
#define THCommand_H

/*
THCommand

Class implementing a parser and interpreter of user commands issued from a terminal app.
Inherits from PrimitiveParser

Usage:

Initialize with 'terminal' and 'exe' objects already initialised then call 'init' once.

Call 'next' asyncronously to process user commands. The class will use
the 'terminal' object for user interaction, and the 'exe' object for 
interaction with the logic analyzer 

Refer to method implementations for details on supported commands.
*/

#include "PrimitiveParser.h"

//class THExec;
//class THTerminal;

class THCommand : public PrimitiveParser
{
  public:
    //THCommand( THTerminal &terminal, THExec &exe );
    THCommand();
    
  public:
    void init();
    void next( void );

  private:
    void error(const __FlashStringHelper* errStr);
    bool parseCommand();
    bool continueCommand();

  private: // Miscelaneous commands
    bool parseUPLOAD();
    bool parsePOLL();
    bool parseKITT();
    bool parseVAR();
    bool parseRANGE();
    bool parseTRANSPARENT();

  private: // VGA commands
    bool parseDRAWLINE();
    bool parseFILLRECT();
    bool parseDRAWLINES();
    bool parseFILLRECTS();
    bool parseINVADERS();

  private:
    bool parseVAR( uint8_t &width, uint8_t &port, uint8_t &channel);
    bool continuePOLL();
    bool continueKITT();
    bool continueINVADERS();
    long pollValue;
    uint8_t pollPort;

  private:
    enum { none, upload, poll, kitt, invaders } ongoing = none;
    //THExec &_exe;
    //THTerminal &_terminal;
};


#endif
