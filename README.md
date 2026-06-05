# STM32 Shell
 
Eine in C implementierte Kommandozeilen-Schnittstelle (CLI) für STM32-Mikrocontroller über UART.
Die Bibliothek basiert auf den ursprünglichen Projekten von
[ShareCat](https://github.com/ShareCat/STM32CommandLine) und
[mdiepart](https://github.com/mdiepart/ushell-stm32) und wurde weiterentwickelt.
 
## Beschreibung
 
Dieses Projekt stellt eine vollständige Shell-Implementierung für STM32-Mikrocontroller bereit,
die über eine UART-Schnittstelle bedient wird. Sie verwendet ausschließlich generische HAL-Interfaces
von STMicroelectronics und ist daher mit den meisten ARM-Mikrocontrollern der STM32-Familie
kompatibel. Eigene Befehle lassen sich einfach registrieren; `printf` gibt Text direkt im Terminal aus.
 
## Funktionsumfang
 
- VT100-Terminalunterstützung mit farbiger Ausgabe (Schrift- und Hintergrundfarben)
- Befehlshistorie mit bis zu 10 Einträgen (Abruf über Pfeiltasten ↑ / ↓)
- Bis zu 32 frei definierbare Befehle mit bis zu 8 Argumenten
- Vorimplementierte Befehle: `help`, `reset`, `cls`, `log`
- Debug-Makros `LOG`, `DBG`, `ERR` und `DIE` mit automatischer Ausgabe von Dateiname und Zeilennummer
- Optionaler Passwortschutz beim Start
- Konfigurierbarer Shell-Name in der Eingabezeile
- Frei definierbare Log-Kategorien, zur Laufzeit ein- und ausschaltbar
- Vollständige `stdio`-Integration: `printf` schreibt direkt ins Terminal

## Dateien
 
| Datei                  | Beschreibung                                                        |
|------------------------|---------------------------------------------------------------------|
| `shell.h`              | Hauptheader: API-Definitionen, Makros, Konstanten, Log-Kategorien   |
| `shell.c`              | Kernimplementierung: Eingabeverarbeitung, Befehlsparser, Historie   |
| `shell_commands.h`     | VT100-Farbcodes, Cursor-Steuerung, Tastencodes                      |
| `shell_ringbuffer.h`   | Ringpuffer-Header für UART-Empfang                                  |
| `shell_ringbuffer.c`   | Ringpuffer-Implementierung für UART-Empfang                         |
| `myshell.c`            | Beispielintegration mit eigenen Befehlen                            |
 
## Konfigurationsparameter
 
Die folgenden Konstanten sind in `shell.h` definiert und können angepasst werden:
 
| Konstante        | Standardwert | Beschreibung                              |
|------------------|--------------|-------------------------------------------|
| `HISTORY_MAX`    | 10           | Maximale Anzahl Einträge in der Historie  |
| `MAX_COMMAND_NB` | 32           | Maximale Anzahl registrierbarer Befehle   |
| `MAX_ARGC`       | 8            | Maximale Anzahl Argumente pro Befehl      |
| `MAX_LINE_LEN`   | 80           | Maximale Länge der Eingabezeile           |
 
## API
 
```c
void cli_init(UART_HandleTypeDef *handle_uart);                              // Shell initialisieren
void cli_run(void);                                                          // Shell in der Hauptschleife aufrufen
void cli_add_command(const char *command, const char *help,
                     uint8_t (*exec)(int argc, char *argv[]));               // Befehl registrieren
```
 
Die Makros `CLI_INIT`, `CLI_RUN` und `CLI_ADD_CMD` leiten auf diese Funktionen weiter und können über
`#define CLI_DISABLE` global deaktiviert werden.
 
## Verwendung
 
### 1. CubeMX konfigurieren
 
Den gewünschten UART aktivieren und die globalen Interrupts für diesen UART einschalten. Baudrate und
weitere Parameter können frei gewählt werden, müssen aber im Terminalprogramm übereinstimmen.
 
### 2. Dateien einbinden
 
Alle Quelldateien in das STM32-Projekt kopieren. Bei Verwendung der STM32CubeIDE direkt in die Ordner
`Inc` und `Src`.
 
### 3. Header einbinden
 
```c
#include "shell.h"
```
 
### 4. Shell starten
 
Nach der UART-Initialisierung (zwischen `/* USER CODE BEGIN 2 */` und `/* USER CODE END 2 */`):
 
```c
CLI_INIT(&huart1);
```
 
In der Hauptschleife (`while (1)`):
 
```c
CLI_RUN();
```
 
### 5. Eigene Befehle registrieren
 
```c
CLI_ADD_CMD("my_command", "Beschreibung des Befehls", my_command_func);
 
uint8_t my_command_func(int argc, char *argv[]) {
    // argv[0] = "my_command", argv[1] = erstes Argument, ...
    return EXIT_SUCCESS;
}
```
 
## Anpassungsmöglichkeiten
 
### Shell-Name
 
Ohne Definition wird `#` als Prompt verwendet. Mit folgendem Eintrag in `main.h`:
 
```c
#define CLI_NAME mein_geraet
```
 
erscheint der Prompt als `mein_geraet$ `.
 
### Passwortschutz
 
```c
#define CLI_PASSWORD meinPasswort
```
 
Die Shell zeigt keine Ausgaben und akzeptiert keine Befehle, bis das korrekte Passwort eingegeben wurde.
 
### Log-Kategorien
 
```c
#define CLI_ADDITIONAL_LOG_CATEGORIES \
    X(SENSOR,  true)  \
    X(NETWORK, false) \
```
 
Erzeugt automatisch `CLI_LOG_SENSOR` (aktiv) und `CLI_LOG_NETWORK` (inaktiv). Verwendung:
 
```c
LOG(CLI_LOG_SENSOR, "Temperatur: %d Grad\n", temp);
```
 
Zur Laufzeit können Kategorien mit dem Befehl `log` ein- und ausgeschaltet werden.
 
### Farbige Ausgabe
 
```c
printf(CLI_FONT_RED "Fehler: %d" CLI_FONT_DEFAULT, fehlercode);
```
 
Verfügbare Farben: `CLI_FONT_BLACK`, `CLI_FONT_RED`, `CLI_FONT_GREEN`, `CLI_FONT_YELLOW`,
`CLI_FONT_BLUE`, `CLI_FONT_PURPLE`, `CLI_FONT_CYAN`, `CLI_FONT_WHITE`, `CLI_FONT_GREY`
 
## Terminalkonfiguration
 
Die Zeilenenden bestehen nur aus einem Line Feed (`\n`). Das Terminalprogramm muss so eingestellt werden,
dass es bei jedem empfangenen LF automatisch einen Carriage Return ergänzt.
 
- **PuTTY**: Terminal → „Implicit CR in every LF" aktivieren
- **TeraTerm**: Setup → Terminal → Receive: `LF`, Transmit: `CR`

## Hinweise
 
Ausgaben aus Interrupt-Service-Routinen (ISRs) sind technisch möglich, werden aber nicht empfohlen.
Das Leeren des `stdio`-Puffers dauert ca. 0,1 ms pro Zeichen und kann die Interrupt-Verarbeitung stören.
ISR-Ausgaben sollten daher nur zu Debugzwecken und möglichst kurz gehalten werden.
 
## Abhängigkeiten
 
- `main.h` – STM32 HAL
- [`basicuart.h`](https://github.com/Diveturtle93/STM32_Basicuart) – UART-Sende- und Empfangsfunktionen

## Quellen
 
Dieses Projekt basiert auf:
- [ShareCat/STM32CommandLine](https://github.com/ShareCat/STM32CommandLine)
- [mdiepart/ushell-stm32](https://github.com/mdiepart/ushell-stm32)

## Lizenz
 
Dieses Projekt steht unter der [GPL-3.0 Lizenz](LICENSE).
