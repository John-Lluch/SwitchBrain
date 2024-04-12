#include <Arduino.h>
#include "SBCommand.h"
#include "SBTerminal.h"
#include "SBExec.h"

// Create a buffer for command processing
#define SBCommandBuffSize 82
char buffer[SBCommandBuffSize];

// Create terminal and exe instance variables
SBTerminal terminal( 9600, buffer, SBCommandBuffSize );
SBExec exe;

// Create command instance variable
SBCommand command( terminal, exe );

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
