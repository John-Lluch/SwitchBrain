#ifndef THTerminal_H
#define THTerminal_H

/*
THTerminal

Class to input string commands typed on a Terminal app

Supported features:

* insert character at the current cursor position
* delete character before the current cursor position
* move cursor left
* move cursor right
* up key recalls the previous command in the input list
* down key recalls the following command in the input list

Usage:

Initialize with the terminal baud rate, and a constant char buffer with length.

Call 'processUserInput' asynchronously to start receiving input from the terminal,
this method will only return true upon command completion (i.e. the user pressed enter),
at this point 'c' and 'end' will respectivelly point to the first character
and past the last character of the command string, undefined otherwhise

Method 'buffer' is provided as a convenience to obtain a const pointer to the
command buffer
*/

/*
Com utilitzar amb l'aplicació de Terminal de Mac OS

1. conectar l'Arduino al port USB de l'ordinador
2. a la aplicació Terminal posar (atenció el serial monitor del arduino ha d'estar tancat)

% ls /dev/tty.*
% screen /dev/tty.usbmodem831101 9600

3. això obrira una finestra de terminal VT100 conectada al arduino
4. atenció que el terminador de les comandes es \r no pas \n

5. Per sortir (detach) de la sesió d'screen: tocar ^a d
   Per veure les sesions que hi ha % screen -list
6. Per tornar a entrar (resume) a la sessio posar % screen -r
7. Per matar (kill) una sesió d'screen: tocar ^a k
8. A partir d'aquí l'access des de arduino torna a estar operatiu, i estem en el punt 1
*/

typedef enum { 
  THTerminalModeNormal, 
  THTerminalModePassive, 
  THTerminalModeFile 
} THTerminalMode;

class THTerminal
{
  public:
    THTerminal( unsigned long bps, char * const buff, const int size );
    
  public:
    const char * const buffer() { return _buff; }
    void init();
    bool processUserInput( THTerminalMode mode = THTerminalModeNormal );
    const char *c;
    const char *end;
    
  private:
    bool _append( int numchars );
    bool _insert( char ch, bool insert=true );
    bool _delete();
    bool _recallUp();
    bool _recallDown();
    void _storeRecall();
    
  private:
    void prompt( void );
    void help( void );

  private:
    const int BuffSize;
    const unsigned long Bps;

  private:
    char * const _buff; 
    int _beg;
    int _index;
    int _end;
    int _length;
    bool _isFirstKey;
    bool _commandEnded;
    enum { none_SQ, isEscaping_SQ, isArrow_SQ} _sequence;
    enum { none_FC, XON_FC, XOFF_FC} _flowControl;
};











#endif