//----------------------------------------------------------------------
// Titel	:	shell_commands.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.09.2025
// Version	:	1.0
// Autor	:	Diveturtle93
// Projekt	:	Shell
// Quelle	:	https://github.com/ShareCat/STM32CommandLine
//				https://github.com/mdiepart/ushell-stm32
//----------------------------------------------------------------------

// Dateiheader definieren
//----------------------------------------------------------------------
#ifndef SRC_SHELL_COMMANDS_H_
#define SRC_SHELL_COMMANDS_H_
//----------------------------------------------------------------------

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der eigenen Include Dateien
//----------------------------------------------------------------------
#include "basicuart.h"
//----------------------------------------------------------------------

// Key Codes definieren
//----------------------------------------------------------------------
#define KEY_UP				"\x1b\x5b\x41"									// [up] key: 0x1b 0x5b 0x41
#define KEY_DOWN			"\x1b\x5b\x42"									// [down] key: 0x1b 0x5b 0x42
#define KEY_RIGHT			"\x1b\x5b\x43"									// [right] key: 0x1b 0x5b 0x43
#define KEY_LEFT			"\x1b\x5b\x44"									// [left] key: 0x1b 0x5b 0x44
#define KEY_ENTER			'\r'											// [enter] key
#define KEY_BACKSPACE		'\b'											// [backspace] key
#define KEY_DEL				'\x7f'											// [DEL] key
#define KEY_DELETE			"\x1b\x5b\x33\x7e"								// [Delete] key
//----------------------------------------------------------------------

/* terminal display-----------------------------------------------------BEGIN */

/*
    @links: http://blog.csdn.net/yangguihao/article/details/47734349
            http://blog.csdn.net/kevinshq/article/details/8179252


    @terminal setting commands:
        \033[0m     reset all
        \033[1m     set high brightness
        \033[4m     underline
        \033[5m     flash
        \033[7m     reverse display
        \033[8m     blanking
        \033[30m    --  \033[37m  set font color
        \033[40m    --  \033[47m  set background color
        \033[nA     cursor up up n lines
        \033[nB     cursor move up n lines
        \033[nC     cursor move right n lines
        \033[nD     cursor left up n lines
        \033[y;xH   set cursor position
        \033[2J     clear all display
        \033[K      clear line
        \033[s      save cursor position
        \033[u      restore cursor position
        \033[?25l   cursor invisible
        \033[?25h   cursor visible


    @background color: 40--49           @font color: 30--39
        40: BLACK                           30: black
        41: RED                             31: red
        42: GREEN                           32: green
        43: YELLOW                          33: yellow
        44: BLUE                            34: blue
        45: PURPLE                          35: purple
        46: CYAN                            36: deep green
        47: WHITE                           37: white
*/

// Commands fuer Schriftfarbe
//----------------------------------------------------------------------
#define CLI_FONT_BLACK		"\033[1;30m"
#define CLI_FONT_L_RED		"\033[0;31m"    /* light red */
#define CLI_FONT_RED		"\033[1;31m"    /* red */
#define CLI_FONT_GREEN		"\033[1;32m"
#define CLI_FONT_YELLOW		"\033[1;33m"
#define CLI_FONT_BLUE		"\033[1;34m"
#define CLI_FONT_PURPLE		"\033[1;35m"
#define CLI_FONT_CYAN		"\033[1;36m"
#define CLI_FONT_WHITE		"\033[1;37m"
#define CLI_FONT_GREY		"\033[1;90m"
#define CLI_FONT_DEFAULT	CLI_FONT_WHITE
//----------------------------------------------------------------------
#define TERMINAL_FONT_BLACK()       printf("\033[1;30m")
#define TERMINAL_FONT_L_RED()       printf("\033[0;31m")    /* light red */
#define TERMINAL_FONT_RED()         printf("\033[1;31m")    /* red */
#define TERMINAL_FONT_GREEN()       printf("\033[1;32m")
#define TERMINAL_FONT_YELLOW()      printf("\033[1;33m")
#define TERMINAL_FONT_BLUE()        printf("\033[1;34m")
#define TERMINAL_FONT_PURPLE()      printf("\033[1;35m")
#define TERMINAL_FONT_CYAN()        printf("\033[1;36m")
#define TERMINAL_FONT_WHITE()       printf("\033[1;37m")
#define TERMINAL_FONT_DEFAULT()	    TERMINAL_FONT_WHITE()
//----------------------------------------------------------------------

// Commands fuer Hintergrundfarbe
//----------------------------------------------------------------------
#define CLI_BACK_BLACK		"\033[1;40m"
#define CLI_BACK_L_RED		"\033[0;41m"    /* light red */
#define CLI_BACK_RED		"\033[1;41m"    /* red */
#define CLI_BACK_GREEN		"\033[1;42m"
#define CLI_BACK_YELLOW		"\033[1;43m"
#define CLI_BACK_BLUE		"\033[1;44m"
#define CLI_BACK_PURPLE		"\033[1;45m"
#define CLI_BACK_CYAN		"\033[1;46m"
#define CLI_BACK_WHITE		"\033[1;47m"
#define CLI_BACK_DEFAULT	CLI_BACK_BLACK
//----------------------------------------------------------------------
#define TERMINAL_BACK_BLACK()       printf("\033[1;40m")
#define TERMINAL_BACK_L_RED()       printf("\033[0;41m")    /* light red */
#define TERMINAL_BACK_RED()         printf("\033[1;41m")    /* red */
#define TERMINAL_BACK_GREEN()       printf("\033[1;42m")
#define TERMINAL_BACK_YELLOW()      printf("\033[1;43m")
#define TERMINAL_BACK_BLUE()        printf("\033[1;44m")
#define TERMINAL_BACK_PURPLE()      printf("\033[1;45m")
#define TERMINAL_BACK_CYAN()        printf("\033[1;46m")
#define TERMINAL_BACK_WHITE()       printf("\033[1;47m")
#define TERMINAL_BACK_DEFAULT()		TERMINAL_BACK_BLACK()
//----------------------------------------------------------------------

// Zeile bis zum Ende loeschen
//----------------------------------------------------------------------
#define TERMINAL_CLEAR_END()        printf("\033[K")
//----------------------------------------------------------------------

// Terminal loeschen, alles
//----------------------------------------------------------------------
#define TERMINAL_DISPLAY_CLEAR()    printf("\033[2J")
//----------------------------------------------------------------------

// Cursor nach oben bewegen, x Stellen
//----------------------------------------------------------------------
#define TERMINAL_MOVE_UP(x)         do{ if(x>0) printf("\033[%dA", (x)); }while(0)
//----------------------------------------------------------------------

// Cursor nach unten bewegen, x Stellen
//----------------------------------------------------------------------
#define TERMINAL_MOVE_DOWN(x)       do{ if(x>0) printf("\033[%dB", (x)); }while(0)
//----------------------------------------------------------------------

// Cursor nach links bewegen, y Stellen
//----------------------------------------------------------------------
#define TERMINAL_MOVE_LEFT(y)       do{ if(y>0) printf("\033[%dD", (y)); }while(0)
//----------------------------------------------------------------------

// Cursor nach rechts bewegen, y Stellen
//----------------------------------------------------------------------
#define TERMINAL_MOVE_RIGHT(y)      do{ if(y>0) printf("\033[%dC", (y)); }while(0)
//----------------------------------------------------------------------

// Cursor an Position x, y setzen
//----------------------------------------------------------------------
#define TERMINAL_MOVE_TO(x, y)      printf("\033[%d;%dH", (x), (y))
//----------------------------------------------------------------------

// Cursor zuruecksetzen
//----------------------------------------------------------------------
#define TERMINAL_RESET_CURSOR()     printf("\033[H")
//----------------------------------------------------------------------

// Cursor verbergen
//----------------------------------------------------------------------
#define TERMINAL_HIDE_CURSOR()      printf("\033[?25l")
//----------------------------------------------------------------------

// Cursor anzeigen
//----------------------------------------------------------------------
#define TERMINAL_SHOW_CURSOR()      printf("\033[?25h")
//----------------------------------------------------------------------

/* reverse display */
//----------------------------------------------------------------------
#define TERMINAL_HIGHLIGHT()       printf("\033[7m")
#define TERMINAL_UN_HIGHLIGHT()    printf("\033[27m")
//----------------------------------------------------------------------

#endif /* SRC_SHELL_COMMANDS_H_ */
//----------------------------------------------------------------------

/* https://askubuntu.com/questions/831971/what-type-of-sequences-are-escape-sequences-starting-with-033
he string is actually \033[ and that's not the whole thing.

After that opening bracket comes a series of numbers and symbols. This string is known as an escape sequence and is used to control the console's cursor and text color, among other things.

    non-printing escape sequences have to be enclosed in \[\033[ and \]

If the escape sequence is controlling text color, then it will be followed by an m.

Here's a table for the color sequences:

Black       0;30     Dark Gray     1;30  
Blue        0;34     Light Blue    1;34  
Green       0;32     Light Green   1;32  
Cyan        0;36     Light Cyan    1;36  
Red         0;31     Light Red     1;31  
Purple      0;35     Light Purple  1;35  
Brown       0;33     Yellow        1;33  
Light Gray  0;37     White         1;37   

So, if you want your console prompt to be blue, you would use the following escape sequence (in the filename I'm forgetting):

\[\033[34m\]

(Notice the m)

This escape sequence doesn't only control color, however. It can also control cursor movement. Here's a table/list with the movement codes and how they work:

    Position the Cursor:

    \033[<L>;<C>H

    Or

    \033[<L>;<C>f

    puts the cursor at line L and column C.

    Move the cursor up N lines:

    \033[<N>A

    Move the cursor down N lines:

    \033[<N>B

    Move the cursor forward N columns:

    \033[<N>C

    Move the cursor backward N columns:

    \033[<N>D

    Clear the screen, move to (0,0):

    \033[2J

    Erase to end of line:

    \033[K

    Save cursor position:

    \033[s

    Restore cursor position:

    \033[u

Just be aware that the last two may not work in the terminal emulator you use. Apparently, only xterm and nxterm use those two sequences.

And example using one of these escape sequences: say I want to position my cursor at line 3, column (character) 9. For that, I would use

\[033\[3;9H]

(I am assuming that column 0 is the first position, so that would be the 8th character).

Source: http://www.tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html (also read 6.2)

More general reading: http://ascii-table.com/ansi-escape-sequences.php

Wikipedia: https://en.wikipedia.org/wiki/ANSI_escape_code*/

/* https://gist.github.com/vratiu/9780109

# Reset
Color_Off="\[\033[0m\]"       # Text Reset

# Regular Colors
Black="\[\033[0;30m\]"        # Black
Red="\[\033[0;31m\]"          # Red
Green="\[\033[0;32m\]"        # Green
Yellow="\[\033[0;33m\]"       # Yellow
Blue="\[\033[0;34m\]"         # Blue
Purple="\[\033[0;35m\]"       # Purple
Cyan="\[\033[0;36m\]"         # Cyan
White="\[\033[0;37m\]"        # White

# Bold
BBlack="\[\033[1;30m\]"       # Black
BRed="\[\033[1;31m\]"         # Red
BGreen="\[\033[1;32m\]"       # Green
BYellow="\[\033[1;33m\]"      # Yellow
BBlue="\[\033[1;34m\]"        # Blue
BPurple="\[\033[1;35m\]"      # Purple
BCyan="\[\033[1;36m\]"        # Cyan
BWhite="\[\033[1;37m\]"       # White

# Underline
UBlack="\[\033[4;30m\]"       # Black
URed="\[\033[4;31m\]"         # Red
UGreen="\[\033[4;32m\]"       # Green
UYellow="\[\033[4;33m\]"      # Yellow
UBlue="\[\033[4;34m\]"        # Blue
UPurple="\[\033[4;35m\]"      # Purple
UCyan="\[\033[4;36m\]"        # Cyan
UWhite="\[\033[4;37m\]"       # White

# Background
On_Black="\[\033[40m\]"       # Black
On_Red="\[\033[41m\]"         # Red
On_Green="\[\033[42m\]"       # Green
On_Yellow="\[\033[43m\]"      # Yellow
On_Blue="\[\033[44m\]"        # Blue
On_Purple="\[\033[45m\]"      # Purple
On_Cyan="\[\033[46m\]"        # Cyan
On_White="\[\033[47m\]"       # White

# High Intensty
IBlack="\[\033[0;90m\]"       # Black
IRed="\[\033[0;91m\]"         # Red
IGreen="\[\033[0;92m\]"       # Green
IYellow="\[\033[0;93m\]"      # Yellow
IBlue="\[\033[0;94m\]"        # Blue
IPurple="\[\033[0;95m\]"      # Purple
ICyan="\[\033[0;96m\]"        # Cyan
IWhite="\[\033[0;97m\]"       # White

# Bold High Intensty
BIBlack="\[\033[1;90m\]"      # Black
BIRed="\[\033[1;91m\]"        # Red
BIGreen="\[\033[1;92m\]"      # Green
BIYellow="\[\033[1;93m\]"     # Yellow
BIBlue="\[\033[1;94m\]"       # Blue
BIPurple="\[\033[1;95m\]"     # Purple
BICyan="\[\033[1;96m\]"       # Cyan
BIWhite="\[\033[1;97m\]"      # White

# High Intensty backgrounds
On_IBlack="\[\033[0;100m\]"   # Black
On_IRed="\[\033[0;101m\]"     # Red
On_IGreen="\[\033[0;102m\]"   # Green
On_IYellow="\[\033[0;103m\]"  # Yellow
On_IBlue="\[\033[0;104m\]"    # Blue
On_IPurple="\[\033[10;95m\]"  # Purple
On_ICyan="\[\033[0;106m\]"    # Cyan
On_IWhite="\[\033[0;107m\]"   # White
*/

/* https://cis106.com/bash/ANSI_escape_sequences/
\033[0m 	Reset / Normal
\033[1m 	Bold / Increased intensity
\033[2m 	Faint / Decreased intensity (may not display)
\033[3m 	Italicized (not supported in some terminals)
\033[4m 	Underlined
\033[5m 	Blink (often disabled or not supported)
\033[7m 	Inverted / Reverse video
\033[8m 	Hidden (conceal text)
\033[9m 	Strikethrough text (not widely supported)
\033[10m 	Primary font (not widely used)
\033[11m-19m 	Alternate fonts 1-9 (rarely supported)
\033[21m 	Bold off (reset bold)
\033[22m 	Normal color (no bold, no underlined)
\033[23m 	Not italicized (reset italic)
\033[24m 	Not underlined (remove underline)
\033[25m 	Blink off (remove blink)
\033[27m 	Reverse off (reset reverse video)
\033[28m 	Reveal (unhide text)
\033[29m 	Reset strikethrough (remove strikethrough effect)
\033[30m 	Set text color to black
\033[31m 	Set text color to red
\033[32m 	Set text color to green
\033[33m 	Set text color to yellow
\033[34m 	Set text color to blue
\033[35m 	Set text color to magenta
\033[36m 	Set text color to cyan
\033[37m 	Set text color to white
\033[38m 	Set custom text color (ANSI 256 colors or RGB values)
\033[39m 	Reset text color to default
\033[40m 	Set background color to black
\033[41m 	Set background color to red
\033[42m 	Set background color to green
\033[43m 	Set background color to yellow
\033[44m 	Set background color to blue
\033[45m 	Set background color to magenta
\033[46m 	Set background color to cyan
\033[47m 	Set background color to white
\033[48m 	Set custom background color (ANSI 256 colors or RGB)
\033[49m 	Reset background color to default
\033[50m 	Fraktur (not widely supported)
\033[51m 	Framed (rarely supported)
\033[52m 	Encircled (rarely supported)
*/

/* https://bash-hackers.gabe565.com/scripting/terminalcodes/
Terminal codes (ANSI/VT100) introduction¶

Terminal (control) codes are used to issue specific commands to your terminal. This can be related to switching colors or positioning the cursor, i.e. anything that can't be done by the application itself.
How it technically works¶

A terminal control code is a special sequence of characters that is printed (like any other text). If the terminal understands the code, it won't display the character-sequence, but will perform some action. You can print the codes with a simple echo command.

Note: I see codes referenced as "Bash colors" sometimes (several "Bash tutorials" etc...): That's a completely incorrect definition.
The tput command¶

Because there's a large number of different terminal control languages, usually a system has an intermediate communication layer. The real codes are looked up in a database for the currently detected terminal type and you give standardized requests to an API or (from the shell) to a command.

One of these commands is tput. Tput accepts a set of acronyms called capability names and any parameters, if appropriate, then looks up the correct escape sequences for the detected terminal in the terminfo database and prints the correct codes (the terminal hopefully understands).
The codes¶

In this list I'll focus on ANSI/VT100 control codes for the most common actions - take it as quick reference. The documentation of your terminal or the terminfo database is always the preferred source when something is unclear! Also the tput acronyms are usually the ones dedicated for ANSI escapes!

I listed only the most relevant codes, of course, any ANSI terminal understands many more! But let's keep the discussion centered on common shell scripting ;-)

If I couldn't find a matching ANSI escape, you'll see a :?: as the code. Feel free to mail me or fix it.

The ANSI codes always start with the ESC character. (ASCII 0x1B or octal 033) This isn't part of the list, but you should avoid using the ANSI codes directly - use the tput command!

All codes that can be used with tput can be found in terminfo(5). (on OpenBSD at least) See OpenBSD's terminfo(5) under the Capabilities section. The cap-name is the code to use with tput. A description of each code is also provided.
General useful ASCII codes¶

The Ctrl-Key representation is simply associating the non-printable characters from ASCII code 1 with the printable (letter) characters from ASCII code 65 ("A"). ASCII code 1 would be ^A (Ctrl-A), while ASCII code 7 (BEL) would be ^G (Ctrl-G). This is a common representation (and input method) and historically comes from one of the VT series of terminals.
Name 	decimal 	octal 	hex 	C-escape 	Ctrl-Key 	Description
BEL 	7 	007 	0x07 	\a 	^G 	Terminal bell
BS 	8 	010 	0x08 	\b 	^H 	Backspace
HT 	9 	011 	0x09 	\t 	^I 	Horizontal TAB
LF 	10 	012 	0x0A 	\n 	^J 	Linefeed (newline)
VT 	11 	013 	0x0B 	\v 	^K 	Vertical TAB
FF 	12 	014 	0x0C 	\f 	^L 	Formfeed (also: New page NP)
CR 	13 	015 	0x0D 	\r 	^M 	Carriage return
ESC 	27 	033 	0x1B 	<none> 	^[ 	Escape character
DEL 	127 	177 	0x7F 	<none> 	<none> 	Delete character
Cursor handling¶
ANSI 	terminfo equivalent 	Description
[ ; H
[ ; f 	cup 	Home-positioning to X and Y coordinates
:!: it seems that ANSI uses 1-1 as home while tput uses 0-0
[ H 	home 	Move cursor to home position (0-0)
7 	sc 	Save current cursor position
8 	rc 	Restore saved cursor position
:?: most likely a normal code like \b 	cub1 	move left one space (backspace)
VT100 [ ? 25 l 	civis 	make cursor invisible
VT100 [ ? 25 h 	cvvis 	make cursor visible
Erasing text¶
ANSI 	terminfo equivalent 	Description
[ K
[ 0 K 	el 	Clear line from current cursor position to end of line
[ 1 K 	el1 	Clear line from beginning to current cursor position
[ 2 K 	el2:?: 	Clear whole line (cursor position unchanged)
General text attributes¶
ANSI 	terminfo equivalent 	Description
[ 0 m 	sgr0 	Reset all attributes
[ 1 m 	bold 	Set "bright" attribute
[ 2 m 	dim 	Set "dim" attribute
[ 3 m 	smso 	Set "standout" attribute
[ 4 m 	set smul unset rmul :?: 	Set "underscore" (underlined text) attribute
[ 5 m 	blink 	Set "blink" attribute
[ 7 m 	rev 	Set "reverse" attribute
[ 8 m 	invis 	Set "hidden" attribute
Foreground coloring¶
ANSI 	terminfo equivalent 	Description
[ 3 0 m 	setaf 0 	Set foreground to color #0 - black
[ 3 1 m 	setaf 1 	Set foreground to color #1 - red
[ 3 2 m 	setaf 2 	Set foreground to color #2 - green
[ 3 3 m 	setaf 3 	Set foreground to color #3 - yellow
[ 3 4 m 	setaf 4 	Set foreground to color #4 - blue
[ 3 5 m 	setaf 5 	Set foreground to color #5 - magenta
[ 3 6 m 	setaf 6 	Set foreground to color #6 - cyan
[ 3 7 m 	setaf 7 	Set foreground to color #7 - white
[ 3 9 m 	setaf 9 	Set default color as foreground color
Background coloring¶
ANSI 	terminfo equivalent 	Description
[ 4 0 m 	setab 0 	Set background to color #0 - black
[ 4 1 m 	setab 1 	Set background to color #1 - red
[ 4 2 m 	setab 2 	Set background to color #2 - green
[ 4 3 m 	setab 3 	Set background to color #3 - yellow
[ 4 4 m 	setab 4 	Set background to color #4 - blue
[ 4 5 m 	setab 5 	Set background to color #5 - magenta
[ 4 6 m 	setab 6 	Set background to color #6 - cyan
[ 4 7 m 	setab 7 	Set background to color #7 - white
[ 4 9 m 	setab 9 	Set default color as background color

*/