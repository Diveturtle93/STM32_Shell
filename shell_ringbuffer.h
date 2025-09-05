//----------------------------------------------------------------------
// Titel	:	shell_ringbuffer.h
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
#ifndef SHELL_RINGBUFFER_H_
#define SHELL_RINGBUFFER_H_
//----------------------------------------------------------------------

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------
#include "main.h"
//----------------------------------------------------------------------

// Definiere Debug Symbols
//----------------------------------------------------------------------
#ifdef DEBUG
//	#define DEBUG_SHELL
#endif
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
#ifndef SHELL_RING_LENGTH
	#define SHELL_RING_LENGTH 32											// Laenge ist default auf 32 Zeichen gesetzt
#endif
//----------------------------------------------------------------------

// Typedefine definieren
//----------------------------------------------------------------------
typedef struct ringbuffer
{
	size_t		head;														//
	size_t 		tail;														//
	uint8_t		PBase[SHELL_RING_LENGTH];									//
} RingbufferShellTypeDef;
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
bool shell_ringbuffer_init (RingbufferShellTypeDef *ring);					//
bool shell_addToRingBuffer (RingbufferShellTypeDef *ring, uint8_t *PData);	//
bool shell_removeFromRingBuffer (RingbufferShellTypeDef *ring, uint8_t *PData);	//
bool shell_isRingBufferEmpty (RingbufferShellTypeDef *ring);				//
//----------------------------------------------------------------------

#endif /* SHELL_RINGBUFFER_H_ */
//----------------------------------------------------------------------
