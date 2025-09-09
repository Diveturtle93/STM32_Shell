# Shell für den STM32 via UART

The code based on the two original workspaces of [ShareCat](https://github.com/ShareCat/STM32CommandLine)
and [mdiepart](https://github.com/mdiepart/ushell-stm32).

#

## 1. Einführung [(English)](#introduction)

Dieses Projekt ist eine Kommandozeilen-Schnittstelle (CLI), die entwickelt wurde, um eine Shell
auf einem STM32-Gerät zu implementieren. Es verwendet die generischen Teile der Hardware
Abstraction Layer (HAL)-Schnittstellen und sollte daher mit den meisten ARM-Mikrocontrollern
von STMicroelectronics kompatibel sein.

### 1.1 Funktionen

* Unterstützt farbige Ausgaben und im Allgemeinen die meisten Funktionen, die man von einem
VT100-Terminal erwarten kann.
* Befehlshistorie mit bis zu 10 Einträgen (über Pfeiltasten ↑ und ↓ abrufbar), um zuvor
eingegebene Befehle erneut aufzurufen.
* Möglichkeit, eigene Befehle hinzuzufügen.
* Vorimplementierte Befehle: help, reset, cls.
* Makros LOG, DBG, ERR, um schnell Debug-Ausgaben zu erzeugen und deren Position im Code anzuzeigen.
* Passwortschutz
* Implementiert die notwendigen Funktionen, um stdio-Funktionen wie gewohnt mit der Shell zu
verwenden (d.h. `printf` gibt Text im Terminal aus).

## 2. Installation

### 2.1 Konfiguration des Projekts (CubeMX)

Um die Shell verwenden zu können, muss das CubeMX-Projekt korrekt konfiguriert sein. Der UART,
den du verwenden möchtest, muss aktiviert sein, ebenso wie die globalen Interrupts für diesen
UART. Die Parameter des UARTs (wie Baudrate, Wortlänge, Parität, ...) sind dabei egal, solange
du in der auf deinem Computer verwendeten Software dieselben Parameter einstellst.

### 2.2 Installation der Dateien

Um die Shell in ein bestehendes Projekt zu integrieren, kopiere einfach die Quellcodedateien
in dein Projekt.

Wenn die STM32CubeIDE verwendet wird, können diese direkt in die Ordner `Inc` und `Src` gelegt
werden.

## 3. Erste Schritte mit der Shell

### 3.1 Header-Datei einbinden

Um die Shell verwenden zu können, musst du zuerst die entsprechende Header-Datei einbinden. In
diesem Beispiel wird ein einfaches „Hello, World!“-Beispiel in der `main`-Funktion erstellt.
Dafür musst du folgende Zeile am Anfang deiner `main.c`-Datei einfügen:

```c
#include "../shell/inc/sys_command_line.h"
```

Wenn deine Datei mit CubeMX generiert wurde, solltest du diese Zeile zwischen die Kommentare
`/* USER CODE BEGIN Includes */` und `/* USER CODE END Includes */` setzen.

### 3.2 Starten der CLI

* Danach muss die CLI initialisiert werden. Dies muss nach der Initialisierung des UART geschehen.

In diesem Beispiel fügst du folgende Zeile ein:

```c
CLI_INIT(&huart1);	
```

und zwar zwischen den Kommentaren `/* USER CODE BEGIN 2 */` und `/* USER CODE END 2 */`.

Um die Eingaben der Benutzer zu verarbeiten, musst du außerdem die Zeile `CLI_RUN();` in die 
Hauptschleife `(while=` deines Programms einfügen.

### 3.3 Anpassen der Shell

####Shell-Name

Die Shell kann einen Namen haben, der am Anfang jeder Eingabezeile angezeigt wird, z.B.:

```
shell_name$ help
[help]
show commands

[cls]
clear the screen

[...]

shell_name$
```

Um einen Namen zu definieren, füge folgende Zeile in deine `main.h` ein:

```c
#define CLI_NAME shell_name
```

Wenn diese Zeile nicht vorhanden ist, wird kein Name angezeigt und stattdessen ein `#` verwendet.

#### Passwort

Die Shell kann mit einem Passwort geschützt werden. Ist ein Passwort definiert, zeigt die Shell
keine Ausgaben an und akzeptiert keine Befehle, bis das korrekte Passwort eingegeben und mit
Enter bestätigt wurde.

Um ein Passwort zu definieren, füge folgende Zeile in deine `main.h` ein:

```c
#define CLI_PASSWORD myPassword
```

Wenn diese Zeile fehlt, startet die Shell ohne Passwortabfrage.

#### LOG-Kategorien

Es können zusätzliche Log-Kategorien definiert werden, die in der Shell verwendet werden können.
Dazu fügst du z.B. folgende Definition ein:

```c
#define CLI_ADDITIONAL_LOG_CATEGORIES \
	X(CAT1, true) \
	X(CAT2, false) \
	X(CAT3, true) \
```

Dies erzeugt automatisch die Log-Kategorien `CLI_LOG_CAT1`, `CLI_LOG_CAT2`, `CLI_LOG_CAT3`.

Die Kategorien `CAT1` und `CAT3` sind standardmäßig aktiviert (Logs werden angezeigt), `CAT2` ist deaktiviert.

Du kannst dann z.B. so loggen:

```c
LOG(CLI_LOG_CAT2, "My log line");
// Will print
[CAT2]: My log line\n
```

Diese Kategorien können zur Laufzeit mit dem Befehl `log` ein- oder ausgeschaltet werden.

### 3.4 Neue Befehle hinzufügen

Um der Shell neue Befehle hinzuzufügen, verwende die Funktion:

```c
CLI_ADD_CMD(const char *command, const char *help, uint8_t (*exec)(int argc, char *argv[]) )
```

Parameter:

* `*command`: Ein C-String mit dem Befehl, der in der Shell eingegeben werden soll.
* `*help`: Ein C-String, der angezeigt wird, wenn `help` eingegeben wird.
* `*exec`: Zeiger auf die Funktion, die beim Eingeben des Befehls ausgeführt wird.

  Diese Funktion muss folgendermaßen aussehen:

  ```c
  uint8_t foo (int argc, char *argv[]);
  ```

  Die übergebenen Argumente werden in `argv[]` gespeichert und deren Anzahl in `argc`. Die
  Funktion muss entweder `EXIT_SUCCESS` oder `EXIT_FAILURE` zurückgeben (diese sind in
  `stdlib.h` definiert).

###### Beispiel:

Wenn du folgenden Befehl registrierst:

```
CLI_ADD_CMD("my_command", "My first command", my_command);
...
uint8_t my_command (int argc, char *argv[]){
...
}
```

und dann in der Shell folgendes eingibst:

```
my_command arg1 arg2 arg3	
```

Dann wird `my_command` mit folgenden Werten aufgerufen:

```
argc = 4
argv[0] = "my_command"
argv[1] = "arg1"
argv[2] = "arg2"
argv[3] = "arg3"
```

### 3.5 Konfiguration des Clients

Die Zeilenenden bestehen nur aus einem Line Feed (LF, "\n"). Das bedeutet, dass dein
Terminalprogramm so konfiguriert sein muss, dass es bei jedem empfangenen (LF, "\n") automatisch
einen Carriage Return ("\r") hinzufügt.

Putty:
In der Registerkarte "Terminal" gibt es die Option "Implicit CR in every LF", die aktiviert werden
muss.

TeraTerm:
Gehe zu "Setup → Terminal...", dann zum Tab "Terminal".
Dort muss `Receive = LF` und `Transmit = CR` eingestellt werden.

## 4. Besondere Hinweise bei der Verwendung der Shell

###Verwendung von Ausgaben in Interrupt-Service-Routinen (ISRs)

Beim Drucken mit den bereitgestellten Makros oder der Funktion `printf` wird die Standardbibliothek
stdio.h verwendet. Das bedeutet, dass Ausgaben gepuffert werden können und erst angezeigt werden,
wenn der Puffer voll ist oder ein Zeilenumbruch (`\n`) erfolgt. Daher kann es passieren, dass manche
Ausgaben sofort angezeigt werden, während andere verzögert erscheinen.

Wenn die Ausgabe den Puffer leert (flush), dauert dies deutlich länger – etwa 0,1ms pro Zeichen.
Deshalb wird davon abgeraten, Ausgaben aus Interrupts heraus zu machen.

Außerdem:
Um zu vermeiden, dass der USART-Interrupt eine höhere Priorität als andere ISRs bekommt, verwendet
das Flushen aus Interrupts keinen weiteren Interrupt, um das Ende der Übertragung zu erkennen. Das
bedeutet, dass das Terminal möglicherweise Zeichen nicht korrekt empfängt, wenn viel Text aus einem
Interrupt gesendet wird – selbst wenn der USART-Interrupt die höchste Priorität hat.

TL;DR: Vermeide Ausgaben aus Interrupts. Wenn unbedingt nötig, halte sie kurz und verwende sie nur
zu Debug-Zwecken.

### Verwendung von `PRINTF_COLOR`

`PRINTF_COLOR` ist aus Gründen der Abwärtskompatibilität noch im Code enthalten, sollte aber nicht
mehr verwendet werden, da es veraltet ist.

Stattdessen sollten farbige Ausgaben so erfolgen: `printf(CLI_FONT_RED"My red number: %d.
"CLI_FONT_DEFAULT, myNumber);`

## 5. TODO

* Einige kleinere Bugs müssen noch behoben werden.

---

## 1. Introduction

This project is a command line interface (CLI) created to implement a shell on an STM32 device.
It uses the Generic parts of the Hardware Abstraction Layer (HAL) interfaces and should thus be
compatible with most ARM microcontrollers from ST Microelectronics.

### 1.1 Features

* Supports colored outputs and, more generally most functionality you can expect from a VT100
terminal.
* 10 commands deep history (using the up and down arrows) to recall previously entered commands.
* possibility to add your own commands
* Pre-implemented commands : help, reset, cls.
* LOG, DBG, ERR macros to quickly print debug statements and display their location in the code.
* Password protection
* Implements the required functions to use `stdio` functions as usual, with the shell (i.e.
`printf` will print text on the terminal).

## 2. Installation

### 2.1 Configuring the project (CubeMX)

In order to use the shell, the CubeMX project must be configured correctly. The UART that you are
gonna use must be enabled and the global interrupts for that UART must be enabled too. The
parameters of the UART (such as baudrate, Word length, Parity, ...) do not matter as long as you
use the same parameters in the software you use on your computer.

### 2.2 Installing the files

To install the shell into an existing project, simply copy the source files into your project.

If you are using STM32CubeIDE this could be done directly in the `Inc` and `Src` folder.

## 3. Using the Shell for the first time

### 3.1 Include File

In order to use the shell you must first include the correct header files. In this example a simple
"Hello, World !" example will be created in the main function. For that you need to add the line 

```c
#include "../shell/inc/sys_command_line.h"
```

at the top of your main file. If your file was generated using CubeMX, you need to put between the
comments `/* USER CODE BEGIN Includes */ ` and `/* USER CODE END Includes */`.

### 3.2 Running the CLI

* Then the CLI needs to be initialized. This must be done after the UART is initialized.

In this example, add the line 

```c
CLI_INIT(&huart1);	
```

between the comments ` /* USER CODE BEGIN 2 */` and `/* USER CODE END 2 */`.

* And finally in order to process the commands add the line `CLI_RUN();` inside of the main loop
of your program.

### 3.3 Customizing the shell

#### Shell Name

The shell can have a name which is displayed at the beginning of each line like so:

```
shell_name$ help
[help]
show commands

[cls]
clear the screen

[...]

shell_name$
```

In order to define a name, add a line like the following to your `main.h` file:

```c
#define CLI_NAME shell_name
```

If there is no such line then no name is defined and the name in the previous example will be
replaced by `#`.

#### Password

The shell can be password protected. When using a password, the shell won't display any text and no
command will be taken until the password is correctly entered (you must press 'enter' once you
typed the password).

To define a password, add a line like the following to your `main.h` file:

```c
#define CLI_PASSWORD myPassword
```

If there is no such line then no password is defined and the shell starts up immediately.

#### LOG categories

Additional log categories can be defined to be used with the shell.
For that you need to add a line like the following:

```c
#define CLI_ADDITIONAL_LOG_CATEGORIES \
	X(CAT1, true) \
	X(CAT2, false) \
	X(CAT3, true) \
```

This define will automatically create the log categories `CLI_LOG_CAT1`, `CLI_LOG_CAT2`,
`CLI_LOG_CAT3`. The categories CAT1 and CAT3 will be enabled by default (the logs will be displayed)
while CAT2 will not be enabled at startup (logs won't be displayed).
When calling the log function, use it like this:
 
```c
LOG(CLI_LOG_CAT2, "My log line");
// Will print
[CAT2]: My log line\n
```

The various categories can be enabled and disabled at run time using the command `log`.

### 3.4 Adding new commands

In order to add a new command to the shell, use the function 

```c
CLI_ADD_CMD(const char *command, const char *help, uint8_t (*exec)(int argc, char *argv[]) )
```

Where :

* `*command` is a c string containing the command to enter in the shell to execute the corresponding
function,
* `*help` is a c string that will be displayed when using the command `help`
* `*exec` is a pointer to a function that will be executed when the corresponding command is entered.
The prototype of the function needs to be 

  ```c
  uint8_t foo (int argc, char *argv[]);
  ```

  The parameters in the line calling the command will be placed in `*argv[]` and the number of
  arguments will be placed in `argc`.

  The function must then return `EXIT_SUCCESS` if it executed successfully or  `EXIT_FAILURE` if a
  problem happened. These two macros are defined in `stdlib.h`.

###### Example:

If you add the following command: 

```
CLI_ADD_CMD("my_command", "My first command", my_command);
...
uint8_t my_command (int argc, char *argv[]){
...
}
```

and then you enter the following line in the shell:

```
my_command arg1 arg2 arg3	
```

you will have the function `my_function` called with:

```
argc = 4
argv[0] = "my_command"
argv[1] = "arg1"
argv[2] = "arg2"
argv[3] = "arg3"
```

### 3.5 Client configuration

The line termination is a line feed (LF, "\n"), that means that you will need to enable a setting in
your client software that adds an implicit carriage return (CR, "\r") at each line feed (LF, "\n")
received.

In Putty, the setting is located in the "Terminal" tab and is called "Implicit CR in every LF".

In TeraTerm, the setting is located in the "Setup -> Terminal...". Than there is the "Terminal" tab.
Now the option Receive and Transmit must be set. `Receive = LF` and `Transmit = CR`.

## 4. Special consideration when using the shell

### Using print statements in interrupt requests

When printing using provided macros or `printf` function, the standard `stdio.h` library is used. This
implies that text can be buffered and won't be printed to the shell unless the buffer is full or a
newline is printed. This makes it so that some print statements will flush the buffer while some won't.
If the print statement does flush the buffer, it  will take significantly more time (0.1ms per
character written). Thus, printing from within interrupt requests is not recommended.

Also, in order to avoid having to put the USART interrupt request at a higher pre-emptive priority than
the rest of the ISRs, flushing the buffer from within an interrupt will not use an interrupt to detect
when the transfer is complete. This implies that the terminal may not be able to read characters
properly if a lot of text is printed from within interrupts even if the USART interrupt is the highest
priority ISR.

TL;DR: Avoid printing text from interrupts. Keep text short and only for debug purposes.

### Using `PRINTF_COLOR`

`PRINTF_COLOR` is kept in the code for backward compatibility but should not be used anymore and have
been deprecated. Prefer using statements like `printf(CLI_FONT_RED"My red number: %d."CLI_FONT_DEFAULT,
myNumber);`

## 5. TODO

- Fix a few bugs here and there