#include <Arduino.h>
#include "THCommand.h"
#include "THTerminal.h"
#include "THExec.h"
#include "Display.hpp"

// Create a buffer for command processing
#define THCommandBuffSize 82
char buffer[THCommandBuffSize];

// Create terminal and exe instance variables
THTerminal terminal( 9600, buffer, THCommandBuffSize );
THExec exe;

// Create command instance variable
//THCommand command( terminal, exe );
THCommand command;

Display ds;

// Setup code
void setup() 
{
   command.init();
   delay(5000);
}

// Loop code
void loop() 
{
  command.next();
}
