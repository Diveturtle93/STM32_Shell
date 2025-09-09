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
volatile bool cli_tx_isr_flag = false;
UART_HandleTypeDef *huart_shell;
//----------------------------------------------------------------------
// Variablen fuer die Beschreibung der Funktionen
//----------------------------------------------------------------------
const char cli_help_help[]	= "Show commands";
const char cli_clear_help[]	= "Clear the screen";
const char cli_reset_help[]	= "Reboot MCU";
const char cli_log_help[]	= "Controls which logs are displayed."
							  "\n\t\"log show\" to shwo which logs are enabled"
							  "\n\t\"log on/off all\" to enable/disable all logs"
							  "\n\t\"log on/off [CAT1 CAT2 CAT3...]\" to enable/disable the logs for categories [CAT1 CAT2 CAT...]";
//----------------------------------------------------------------------
// Buffer Variablen
//----------------------------------------------------------------------
unsigned char cBuffer;
RingbufferShellTypeDef cli_rx_buf;
ShellCommandTypeDef cli_commands[MAX_COMMAND_NB];
static ShellHistoryTypeDef history;
//----------------------------------------------------------------------

// Variable fuer Log status
//----------------------------------------------------------------------
// Log Kategorien
// Werden ueber CLI_ADDITIONAL_LOG_CATEGORIES definiert
// Beispiel: #define CLI_ADDITIONAL_LOG_CATEGORIES 			X(CAT1, true)
// Fuer eine Kategorie mit dem Name CAT1, Logging aktiv (true)
//----------------------------------------------------------------------
char *cli_logs_names[] = {"SHELL",
#ifdef CLI_ADDITIONAL_LOG_CATEGORIES
	#define X(name, b) #name,
		CLI_ADDITIONAL_LOG_CATEGORIES
	#undef X
#endif
};
//----------------------------------------------------------------------
// Log Status, jedes Bit ist fuer eine Kategorie, Bit = 0 - Logging aus, Bit = 1 - Logging an
//----------------------------------------------------------------------
uint32_t cli_log_stat = 0
#ifdef CLI_ADDITIONAL_LOG_CATEGORIES
	#define X(name, b) | (b << CLI_LOG_##name)
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
void shell_welcome (void);
void cli_disable_log_entry (char *str);
void cli_enable_log_entry (char *str);
//----------------------------------------------------------------------

// Ausgabe auf Console durch printf
//----------------------------------------------------------------------
int _write (int file, char *data, int len)
{
	// Fehler
	if (file != STDOUT_FILENO && file != STDERR_FILENO){
		errno = EBADF;
		return -1;
	}

	// Wenn Password falsch
	if (cli_password_ok == false){
		return len;
	}

	// Status definieren
	HAL_StatusTypeDef status = HAL_OK;

	// Abfrage, ob Daten ueber Interrupt versendet werden
	if (!(SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk))
	{
		// Setze ISR Flag
		cli_tx_isr_flag = true;

		// Schalte Interrupt fuer Uart aus, um Unterbrechungen während des Sendens zu unterbinden
		HAL_NVIC_DisableIRQ(USART2_IRQn);

		// Transmit Daten mit Interrupt Routine
		status = HAL_UART_Transmit_IT(huart_shell, (uint8_t *)data, len);

		// Schalte Interrupt fuer Uart ein, damit Daten ordentlich gesendet werden
		HAL_NVIC_EnableIRQ(USART2_IRQn);

		// Warte bis Flag nicht mehr true ist
		while (cli_tx_isr_flag == true)
		{

		}
	}
	// Ansonsten normal versenden
	else
	{
		// Schalte Interrupt fuer Uart aus, um Unterbrechungen während des Sendens zu unterbinden
		HAL_NVIC_DisableIRQ(USART2_IRQn);

		// Transmit Daten mit Interrupt Routine
		status = HAL_UART_Transmit(huart_shell, (uint8_t *)data, len, 1000);

		// Schalte Interrupt fuer Uart ein, damit Daten ordentlich gesendet werden
		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}

	// Wenn Status OK
	if (status == HAL_OK)
	{
		// Rueckgabe der Datenlaenge die gesendet wurde
		return len;
	}
	else
	{
		// Rueckgabe ohne Datenlaenge
		return 0;
	}
}
//----------------------------------------------------------------------

// Check ob Ausgabe Console ist
//----------------------------------------------------------------------
__attribute__((weak)) int _isatty (int file)
{
	switch(file)
	{
		case STDERR_FILENO:
		case STDIN_FILENO:
		case STDOUT_FILENO:
			return 1;
		default:
			errno = EBADF;
			return 0;
	}
}
//----------------------------------------------------------------------

// Console initialisieren
//----------------------------------------------------------------------
void cli_init (UART_HandleTypeDef *handle_uart)
{
	// Uart Handler uebergeben
	huart_shell = handle_uart;

	// Ringbuffer initialisieren
	shell_ringbuffer_init(&cli_rx_buf);
	
	// Reserviere Speicherplatz fuer Historie
	memset((uint8_t *)&history, 0, sizeof(history));

	// Commandliste loeschen und auf Null setzen
    for (uint8_t j = 0; j < MAX_COMMAND_NB; j++)
    {
    	cli_commands[j].pCmd = "";
    	cli_commands[j].pFun = NULL;
    }
	
	// Starte UART
	HAL_UART_MspInit(handle_uart);
	HAL_UART_Receive_IT(handle_uart, &cBuffer, 1);
	
	// Willkommensbildschirm ausgeben
#ifndef CLI_PASSWORD
    cli_password_ok = true;
    shell_welcome();
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
	if (CLI_LAST_LOG_CATEGORY > 32)
	{
		// Fehlerausgabe wenn CLI_LAST_LOG_CATEGORY > 32
    	ERR("Too many log categories defined. The max number of log categories that can be user defined is 31.\n");
    }
	else
	{
		// Logging
		LOG(CLI_LOG_SHELL, "Command line successfully initialized.\n");
	}
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
static void cli_rx_handle(RingbufferShellTypeDef *rx_buf)
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
	// Variable definieren
	uint8_t i = 0;

	// Commandliste fuellen
	for (; i < MAX_COMMAND_NB; i++)
	{
		if (strcmp(cli_commands[i].pCmd, "") == 0)
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
	else
	{
		// Logging
		LOG(CLI_LOG_SHELL, "Command %s added to shell.\n", command);
	}
}
//----------------------------------------------------------------------

// Willkommensbildschirm fuer Console
//----------------------------------------------------------------------
__weak void shell_welcome (void)
{
	NL1();
	TERMINAL_BACK_DEFAULT(); /* set terminal background color: black */
	TERMINAL_DISPLAY_CLEAR();
	TERMINAL_RESET_CURSOR();
	TERMINAL_FONT_BLUE();
	printf("###################################################################################################\n");
	printf("###################################################################################################\n");
	printf("###################################################################################################\n");
	printf("###################################################################################################\n");
	printf("###########     ###    ##  ###  ##      ##      ##  ##  ##     ###      ##  ######      ###########\n");
	printf("###########  ##  ###  ###  ###  ##  ########  ####  ##  ##  ##  ####  ####  ######  ###############\n");
	printf("###########  ##  ###  ###  ###  ##  ########  ####  ##  ##  ##  ####  ####  ######  ###############\n");
	printf("###########  ##  ###  ###  ###  ##    ######  ####  ##  ##     #####  ####  ######    #############\n");
	printf("###########  ##  ###  ####  #  ###  ########  ####  ##  ##  #  #####  ####  ######  ###############\n");
	printf("###########  ##  ###  ####  #  ###  ########  ####  ##  ##  ##  ####  ####  ######  ###############\n");
	printf("###########     ###    ####   ####      ####  #####    ###  ##  ####  ####      ##      ###########\n");
	printf("###################################################################################################\n");
	printf("###################################################################################################\n");
	printf("###################################################################################################\n");
	printf("###################################################################################################\n");
	NL2();
	TERMINAL_FONT_DEFAULT();
	PRINT_CLI_NAME();
	TERMINAL_SHOW_CURSOR();
}
//----------------------------------------------------------------------

// Command in Historie hinzufuegen
//----------------------------------------------------------------------
static void cli_history_add (char* buf)
{
	// Variablen definieren
	uint16_t len;
	uint8_t index = history.latest;

	// Wenn buf = 0
	if (NULL == buf)
	{
		// Beenden
		return;
	}

	// Laenge ermitteln
	len = strlen((const char *)buf);

	// Wenn len groesser als MAX_LINE_LENGTH
	if (len >= MAX_LINE_LENGTH)
	{
		// Beenden
		return;
	}

	// Wenn index ungleich 0 ist
	if (0 != index)
	{
		// index runterzaehlen
		index--;
	}
	// Ansonsten
	else
	{
		// Zum hoechsten Historieeintrag springen
		index = HISTORY_MAX - 1;
	}

	// Wenn aktuelle Befehl nicht mit letztem Historieeintrag uebereinstimmt
	if (0 != memcmp(history.cmd[index], buf, len))
	{
		// Speicher reservieren und Daten kopieren
		memset((void *)history.cmd[history.latest], 0x00, MAX_LINE_LENGTH);
		memcpy((void *)history.cmd[history.latest], (const void *)buf, len);

		// count < HISTORY_MAX
		if (history.count < HISTORY_MAX)
		{
			// count hochzaehlen
			history.count++;
		}

		// latest hochzaehlen
		history.latest++;

		// Wenn latest >= HISTORY_MAX, dann Elemente ueberschreiben
		if (history.latest >= HISTORY_MAX)
		{
			// Erstes Element der History ueberschrieben
			history.latest = 0;
		}
	}

	// History show deaktivieren
	history.show = 0;
}
//----------------------------------------------------------------------

// Historie anzeigen
//----------------------------------------------------------------------
static uint8_t cli_history_show (uint8_t mode, char** p_history)
{
	// Variablen definieren
	uint8_t err = true;
	uint8_t num;
	uint8_t index;

	// Abfrage, ob Historie = 0 ist
	if (0 == history.count)
	{
		// Rueckgabe Fehler
		return err;
	}

	// Abfrage, ob Mode = true ist
	if (true == mode)
	{
		// Wenn show < count ist
		if (history.show < history.count)
		{
			// show hochzaehlen
			history.show++;
		}
	}
	// Wenn mode nicht true ist
	else
	{
		// Wenn show > 1 ist
		if (1 < history.show)
		{
			// show runterzaehlen
			history.show--;
		}
	}

	// Daten abpeichern
	num = history.show;
	index = history.latest;

	// Solange num nicht 0 ist
	while (num)
	{
		// Wenn index ungleich 0 ist
		if (0 != index)
		{
			// index runterzaehöen
			index--;
		}
		// Ansonsten
		else
		{
			// Zum hoechsten Historieeintrag springen
			index = HISTORY_MAX - 1;
		}

		// Num runterzaehlen
		num--;
	}

	// Command zurueckschrieben in *p_history
	err = false;
	*p_history = history.cmd[index];

	return err;
}
//----------------------------------------------------------------------

// Receive Interrupt Funktion
//----------------------------------------------------------------------
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart)
{
	//shell_addToRingBuffer(&cli_rx_buff, &cBuffer);
	HAL_UART_Receive_IT(huart, &cBuffer, 1);
}
//----------------------------------------------------------------------

// Transmit Interrupt Funktion
//----------------------------------------------------------------------
void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart)
{
	cli_tx_isr_flag = false;
}
//----------------------------------------------------------------------

// Ausgabe Hilfe
//----------------------------------------------------------------------
uint8_t cli_help (int argc, char *argv[])
{
	// Abfrage ob Argument eins ist
	if (argc == 1)
	{
		// Ausgabe fuer jeden Command
	    for (uint8_t i = 0; i < MAX_COMMAND_NB; i++)
	    {
	    	// Wenn Command ungleich ""
	    	if (strcmp(cli_commands[i].pCmd, "") != 0)
	    	{
	    		// Ausgabe Command
		    	printf("[%s]", cli_commands[i].pCmd);
		    	NL1();

		    	// Wenn Hilfe fuer Command vorhanden
		        if (cli_commands[i].pHelp)
		        {
		        	// Ausgabe Hilfe fuer Command
		            printf(cli_commands[i].pHelp);
		            NL2();
		        }
	    	}
	    }

	    // Erfolgreich beenden der Funktion
	    return EXIT_SUCCESS;
	}
	// Wenn Argumente = 2
	else if (argc == 2)
	{
		// Ausgabe fuer jeden Command
	    for (uint8_t i = 0; i < MAX_COMMAND_NB; i++)
	    {
	    	// Wenn Command mit zweitem Argument uebereinstimmt
	    	if (strcmp(cli_commands[i].pCmd, argv[1]) == 0)
	    	{
	    		// Command ausgeben
		    	printf("[%s]", cli_commands[i].pCmd);
		    	NL1();

		    	// Hilfe fuer Command ausgeben
	    		printf(cli_commands[i].pHelp);
	    		NL1();

	    	    // Erfolgreich beenden der Funktion
	    		return EXIT_SUCCESS;
	    	}
	    }

	    // Ausgabe kein Command gefunden
	    printf("No help found for command %s.", argv[1]);
	    NL1();
	    return EXIT_FAILURE;
	}
	// Ansonsten
	else
	{
		// Ausgabe Fehler
		printf("Command \"%s\" takes at most 1 argument.", argv[0]);
		NL1();
		return EXIT_FAILURE;
	}

	// Wenn Funktion bisher nicht erfolgreich beendet wurde mit Fehler beenden
    return EXIT_FAILURE;
}
//----------------------------------------------------------------------

// Loesche Ausgabe
//----------------------------------------------------------------------
uint8_t cli_clear (int argc, char *argv[])
{
	// Abfrage ob Argumente mehr als eins ist
	if (argc != 1)
	{
		// Wenn keine Argumente vorhanden
		printf("command \"%s\" does not take any argument.", argv[0]);
		NL1();
		return EXIT_FAILURE;
	}

	// Terminal auf Default Einstellungen setzen
    TERMINAL_BACK_DEFAULT();
    TERMINAL_FONT_DEFAULT();

    // Reset Cursor
    TERMINAL_RESET_CURSOR();

    // Loesche Bildschirm
    TERMINAL_DISPLAY_CLEAR();

    return EXIT_SUCCESS;
}
//----------------------------------------------------------------------

// Reset MCU
//----------------------------------------------------------------------
uint8_t cli_reset (int argc, char *argv[])
{
	// Abfrage ob Argumente mehr als eins ist
	if (argc > 1)
	{
		// Wenn zu wenige Argumente im Befehl stehen
		printf("Command \"%s\" takes no argument.", argv[0]);
		NL1();
		return EXIT_FAILURE;
	}

	// Ausgabe Shell
	NL1();
	printf("[END]: System Rebooting");
	NL1();

	// Resete MCU
	HAL_NVIC_SystemReset();
	return EXIT_SUCCESS;
}
//----------------------------------------------------------------------

// Logging ein und ausschalten
//----------------------------------------------------------------------
uint8_t cli_log (int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Command %s takes at least one argument. Use \"help %s\" for usage.\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}

	if (strcmp(argv[1], "on") == 0)
	{
		if (argc < 3)
		{
			printf("Command %s on takes at least 3 arguments.\n", argv[0]);

			return EXIT_FAILURE;
		}

		if (strcmp(argv[2], "all") == 0)
		{
			cli_log_stat = 0xFFFFFFFF;
			printf("All logs enabled.\n");

			return EXIT_SUCCESS;
		}
		else
		{
			for (uint8_t i = 2; i < argc; i++)
			{
				cli_enable_log_entry(argv[i]);
			}

			return EXIT_SUCCESS;
		}
	}
	else if (strcmp(argv[1], "off") == 0)
	{
		printf("Turning off all logs\n");
		if (argc < 3)
		{
			printf("Command %s on takes at least 3 arguments.\n", argv[0]);

			return EXIT_FAILURE;
		}

		if (strcmp(argv[2], "all") == 0)
		{
			cli_log_stat = 0;
			printf("All logs disabled.\n");

			return EXIT_SUCCESS;
		}
		else
		{
			for (uint8_t i = 2; i < argc; i++)
			{
				cli_disable_log_entry(argv[i]);
			}

			return EXIT_SUCCESS;
		}
	}
	else if (strcmp(argv[1], "show") == 0)
	{
		for (uint8_t i = 0; i < CLI_LAST_LOG_CATEGORY; i++)
		{
			printf("%16s:\t", cli_logs_names[i]);

			if (cli_log_stat & (1 << i))
			{
				printf(CLI_FONT_GREEN"Enabled"CLI_FONT_DEFAULT"\n");
			}
			else
			{
				printf(CLI_FONT_RED"Disabled"CLI_FONT_DEFAULT"\n");
			}
		}

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}
//----------------------------------------------------------------------

// Logging ausschalten
//----------------------------------------------------------------------
void cli_disable_log_entry (char *str)
{
	// Durch gehen fuer jede Kategorie
	for (unsigned int i = 0; i < CLI_LAST_LOG_CATEGORY; i++)
	{
		// Stimmt Kategorie aus Array mit String ueberein
		if (strcmp(str, cli_logs_names[i]) == 0)
		{
			// Deaktiviere Logging fuer Kategorie
			printf("LOG disabled for category %s.\n", str);
			cli_log_stat &= ~(1<<i);
		}
	}
}
//----------------------------------------------------------------------

// Logging einschalten
//----------------------------------------------------------------------
void cli_enable_log_entry (char *str)
{
	// Durch gehen fuer jede Kategorie
	for (unsigned int i = 0; i < CLI_LAST_LOG_CATEGORY; i++)
	{
		// Stimmt Kategorie aus Array mit String ueberein
		if (strcmp(str, cli_logs_names[i]) == 0)
		{
			// Aktiviere Logging fuer Kategorie
			printf("LOG enabled for category %s.\n", str);
			cli_log_stat |= (1<<i);
		}
	}
}
//----------------------------------------------------------------------