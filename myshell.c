//----------------------------------------------------------------------
// Titel	:	shell.c
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
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------
#include "main.h"
//----------------------------------------------------------------------

// Einfuegen der eigenen Include Dateien
//----------------------------------------------------------------------
#include "shell.h"
//----------------------------------------------------------------------

// Typedefines definieren
//----------------------------------------------------------------------
// Buffer fuer aktuelle Zeile
//----------------------------------------------------------------------
typedef struct
{
	uint8_t buf[MAX_LINE_LENGTH];
	uint8_t length;
} ShellHandleTypeDef;
//----------------------------------------------------------------------
// Command Speicher
//----------------------------------------------------------------------
typedef struct
{
	const char *pCmd;
	const char *pHelp;
	uint8_t (*pFun)(int argc, char *argv[]);
} ShellCommandTypeDef;
//----------------------------------------------------------------------
// History Speicher
//----------------------------------------------------------------------
typedef struct
{
	char cmd[HISTORY_MAX][MAX_LINE_LENGTH];
	uint8_t count;
	uint8_t latest;
	uint8_t show;
} ShellHistoryTypeDef;
//----------------------------------------------------------------------

// Globale Variablen definieren
//----------------------------------------------------------------------
