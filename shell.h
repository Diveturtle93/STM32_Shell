//----------------------------------------------------------------------
// Titel	:	shell.h
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
#ifndef SRC_SHELL_H_
#define SRC_SHELL_H_
//----------------------------------------------------------------------

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>
#include <string.h>
//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------
#include "main.h"
//----------------------------------------------------------------------

// Einfuegen der eigenen Include Dateien
//----------------------------------------------------------------------
#include "shell_commands.h"
#include "shell_ringbuffer.h"
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
#define CLI_ENABLE				true										// Einschalten der Commandzeile
#define HISTORY_MAX				10											// Maximale Anzahl der Commands in der Historie
#define MAX_COMMAND_NB			32											// Maximale Anzahl der definierbaren Commands
#define MAX_ARGC				8											// Maximale Anzahl Argumente
#define MAX_LINE_LEN			80											// Maximale laenge der Commandzeile
//----------------------------------------------------------------------
// Auswahl ob CLI ausgeschaltet ist
//----------------------------------------------------------------------
#ifndef CLI_DISABLE
    #define CLI_INIT(...)       cli_init(__VA_ARGS__)						// Funktion mit Argumenten einlesen
    #define CLI_RUN(...)        cli_run(__VA_ARGS__)						// Funktion mit Argumenten einlesen
	#define CLI_ADD_CMD(...)	cli_add_command(__VA_ARGS__)				// Funktion mit Argumenten einlesen
#else
    #define CLI_INIT(...)       ;											// Funktion ueberspringen
    #define CLI_RUN(...)        ;											// Funktion ueberspringen
	#define CLI_ADD_CMD(...)	;											// Funktion ueberspringen
#endif
//----------------------------------------------------------------------

// Vordefinierte Befehle fuer Fehler, Log, Debug, 
#define ERR(fmt, ...)  do {												\
                            fprintf(stderr,								\
								CLI_FONT_RED							\
								"[ERROR] %s:%d: "fmt					\
								CLI_FONT_DEFAULT,						\
                                __FILE__, __LINE__, ##__VA_ARGS__);		\
                        }while(0)

#define LOG(LOG_CAT, fmt, ...)											\
						if((1<<LOG_CAT)&cli_log_stat) {					\
                            printf(CLI_FONT_CYAN						\
								"[%s]: "fmt								\
								CLI_FONT_DEFAULT,						\
								cli_logs_names[LOG_CAT],				\
								##__VA_ARGS__);							\
                        }

#define DBG(fmt, ...)  do {												\
                            printf(CLI_FONT_YELLOW						\
							"[Debug] %s:%d: "fmt						\
							CLI_FONT_DEFAULT,							\
                                __FILE__, __LINE__, ##__VA_ARGS__);		\
                        } while(0)

#define DIE(fmt, ...)   do {											\
                            TERMINAL_FONT_RED();						\
                            TERMINAL_HIGHLIGHT();						\
                            fprintf(stderr,								\
								"### DIE ### %s:%d: "fmt,				\
                                __FILE__, __LINE__, ##__VA_ARGS__);		\
                        } while(1) /* infinite loop */
//----------------------------------------------------------------------

// Print Leerzeilen
//----------------------------------------------------------------------
#define NL1()					do { printf("\n"); } while(0)
#define NL2()					do { printf("\n\n"); } while(0)
#define NL3()					do { printf("\n\n\n"); } while(0)
//----------------------------------------------------------------------

#define STRING(s) #s
#define XSTRING(s) STRING(s)

#ifdef CLI_NAME
	#define PRINT_CLI_NAME()	do { printf(CLI_FONT_DEFAULT"\n"XSTRING(CLI_NAME)"$ "); } while(0)
#else
	#define PRINT_CLI_NAME()	do { printf(CLI_FONT_DEFAULT"\n#$ "); } while(0)
#endif

enum cli_log_categories {
	CLI_LOG_SHELL = 0,

#ifdef CLI_ADDITIONAL_LOG_CATEGORIES
	#define X(name, b) CLI_LOG_##name,
	CLI_ADDITIONAL_LOG_CATEGORIES
	#undef X
#endif

	CLI_LAST_LOG_CATEGORY,
};

extern char *cli_logs_names[];
extern uint32_t cli_log_stat;

// ... definieren
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
void cli_init (UART_HandleTypeDef *handle_uart);
void cli_run (void);
void cli_add_command (const char *command, const char *help, uint8_t (*exec)(int argc, char *argv[]));
//----------------------------------------------------------------------

#endif /* SRC_SHELL_H_ */
//----------------------------------------------------------------------
