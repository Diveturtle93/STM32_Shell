//----------------------------------------------------------------------
// Titel	:	shell_queue.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	05.09.2025
// Version	:	1.0
// Autor	:	Diveturtle93
// Projekt	:	Shell
// Quelle	:	https://github.com/ShareCat/STM32CommandLine
//				https://github.com/mdiepart/ushell-stm32
//----------------------------------------------------------------------

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------
#include <string.h>
//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------
#include "main.h"
//----------------------------------------------------------------------

// Einfuegen der eigenen Include Dateien
//----------------------------------------------------------------------
#include "shell_queue.h"
//----------------------------------------------------------------------

// Initialisiere Ringbuffer
//----------------------------------------------------------------------
bool shell_ringbuffer_init (RingbufferShellTypeDef *ring)
{
	ring->head = 0;
	ring->tail = 0;

    memset(ring->PBase, 0, SHELL_RING_LENGTH);

    return true;
}
//----------------------------------------------------------------------

// Zeichen zum Ring hinzufuegen
//----------------------------------------------------------------------
bool shell_addToRingBuffer (RingbufferShellTypeDef *ring, uint8_t *PData)
{
    // Variable definieren
	uint16_t nextEntry;
	nextEntry = (ring->head + 1) % SHELL_RING_LENGTH;
	
	// Fuege Element zum Ring hinzu
    ring->PBase[ring->tail] = *PData;
	
	// Ringbuffer Kopf hochzaehlen
    ring->tail = ((ring->tail) + 1) % SHELL_RING_LENGTH;

    return true;
}
//----------------------------------------------------------------------

// Zeichen von Ring entfernen
//----------------------------------------------------------------------
bool shell_removeFromRingBuffer(RingbufferShellTypeDef *ring, uint8_t *PData)
{
	// Pruefen, ob Zeichen im Ring sind
    if (shell_isRingBufferEmpty(ring))
	{
        return false;
    }
	
	// Kopiere Zeichen
    *PData = ring->PBase[ring->tail];
	
	// Ringbuffer Schwanz hochzaehlen
    ring->tail = (ring->tail + 1) % SHELL_RING_LENGTH;

    return true;
}
//----------------------------------------------------------------------

// Abfrage, ob Ringbuffer leer ist
//----------------------------------------------------------------------
bool shell_isRingBufferEmpty (RingbufferShellTypeDef *ring)
{
	// Wenn Ringpuffer leer
	if (ring->head == ring->tail)
	{
		return true;														// Wenn leer, Rueckgabe True
	}
	else
	{
		return false;														// Wenn leer, Rueckgabe False
	}
}
//----------------------------------------------------------------------