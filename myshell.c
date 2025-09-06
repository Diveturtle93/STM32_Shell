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
bool cli_password_ok = false;
volatile bool cli_tx_isr_flag = false
//----------------------------------------------------------------------
// Variablen fuer die Beschreibung der Funktionen
//----------------------------------------------------------------------
const char cli_help_help[]	= "Show commands";
const char cli_clear_help[]	= "Clear the screen";
const char cli_reset_help[]	= "Reboot MCU";
const char cli_log_help[]	= "Controls which logs are displayed."
							  "\n\t\"log show\" to shwo which logs are enabled"
							  "\n\t\"log on/off all\" to enable/disable all logs"
							  "\n\t\"log on/off [CAT1 CAT2 CAT...]\" to enable/disable the logs for categories [CAT1 CAT2 CAT...]";
//----------------------------------------------------------------------
// Buffer Variablen
//----------------------------------------------------------------------
unsigned char cBuffer;
RingbufferShellTypeDef cli_rx_buf;
ShellCommandTypeDef cli_commands[MAX_COMMAND_NB];
static ShellHistoryTypeDef history;

char *cli_logs_names[] = {"SHELL",
#ifdef CLI_ADDITIONAL_LOG_CATEGORIES
	#define X(name, b) #name,
		CLI_ADDITIONAL_LOG_CATEGORIES
	#undef X
#endif
}
//----------------------------------------------------------------------
// Variable fuer Log status
//----------------------------------------------------------------------
uint32_t cli_log_stat = 0
#ifdef CLI_ADDITIONAL_LOG_CATEGORIES
	#define X(name, b) | (b<<CLI_LOG_##name)
		CLI_ADDITIONAL_LOG_CATEGORIES
	#undef X
#endif
;
//----------------------------------------------------------------------

// Interne Funktionen definieren
//----------------------------------------------------------------------
static void cli_history_add (char* buff);
static uint8_t cli_history_show (uint8_t mode, char** p_history);
static void cli_rx_handle (RingbufferShellTypeDef *rx_buff);
static void cli_tx_handle (void);
uint8_t cli_help (int argc, char *argv[]);
uint8_t cli_clear (int argc, char *argv[]);
uint8_t cli_reset (int argc, char *argv[]);
uint8_t cli_log	(int argc, char *argv[]);
void cli_add_command (const char *command, const char *help, uint8_t (*exec)(int argc, char *argv[]));
void greet (void);
void cli_disable_log_entry (char *str);
void cli_enable_log_entry (char *str);
//----------------------------------------------------------------------

// Console initialisieren
//----------------------------------------------------------------------
void cli_init (UART_HandleTypeDef *handle_uart)
{
	// Ringbuffer initialisieren
	shell_ringbuffer_init(&cli_rx_buf);
	
	// Reserviere Speicherplatz fuer Historie
	memset((uint8_t *)&history, 0, sizeof(history));
	
	// Starte UART
	HAL_UART_MspInit(handle_uart);
	HAL_UART_Receive_IT(handle_uart, &cBuffer, 1);
	
	// Willkommensbildschirm ausgeben
#ifndef CLI_PASSWORD
    cli_password_ok = true;
    greet();
#else
    TERMINAL_BACK_DEFAULT();
	TERMINAL_DISPLAY_CLEAR();
	TERMINAL_RESET_CURSOR();
	TERMINAL_FONT_GREEN();
    uartTransmitString("#$ Password");
#endif

	// Commands definieren und in Commandliste speichern
	CLI_ADD_CMD("help", cli_help_help, cli_help);
    CLI_ADD_CMD("cls", cli_clear_help, cli_clear);
    CLI_ADD_CMD("reset", cli_reset_help, cli_reset);
    CLI_ADD_CMD("log", cli_log_help, cli_log);
	
	// Logging ausgeben
	if(CLI_LAST_LOG_CATEGORY > 32){
		// Fehlerausgabe wenn zuviele Commands in Liste geschrieben werden
    	ERR("Too many log categories defined. The max number of log categories that can be user defined is 31.\n");
    }
	
	// Logging
    LOG(CLI_LOG_SHELL, "Command line successfully initialized.\n");
}
//----------------------------------------------------------------------

// Console ausfuehren
//----------------------------------------------------------------------
void cli_run(void)
{
	cli_rx_handle(&cli_rx_buf);
	cli_tx_handle();
}
//----------------------------------------------------------------------

// Empfange Nachricht
//----------------------------------------------------------------------
static void cli_rx_handle (RingbufferShellTypeDef *rx_buf)
{
	
}
//----------------------------------------------------------------------

// Transmit Nachricht
//----------------------------------------------------------------------
static void cli_tx_handle(void)
{
	
}
//----------------------------------------------------------------------

// Command zu Commandliste hinzufügen
//----------------------------------------------------------------------
void cli_add_command (const char *command, const char *help, uint8_t (*exec)(int argc, char *argv[]))
{
	// Commandliste fuellen
	for (uint8_t i = 0; i < MAX_COMMAND_NB; i++)
	{
		if (strcmo(cli_commands[i].pCmd, "") == 0)
		{
			cli_commands[i].pCmd = command;
			cli_commands[i].pFun = exec;
			cli_commands[i].pHelp = help;
			break;
		}
	}
	
	// Logging ausgeben
	if (i == MAX_COMMAND_NB)
	{
		// Fehlerausgabe wenn i == MAX_COMMAND_NB
		ERR("Cannot add command %s, max number of commands "
			"reached. The maximum number of command is set to %d.\n" CLI_FONT_DEFAULT,
			command, MAX_COMMAND_NB);
		NL1();
	}
	
	// Logging
	LOG(CLI_LOG_SHELL, "Command %s added to shell.\n", command);
}
//----------------------------------------------------------------------

// Command in Historie hinzufuegen
//----------------------------------------------------------------------
static void cli_history_add (char* buf)
{

}
//----------------------------------------------------------------------

// Historie anzeigen
//----------------------------------------------------------------------
static uint8_t cli_history_show (uint8_t mode, char** p_history)
{
	return 1;
}
//----------------------------------------------------------------------