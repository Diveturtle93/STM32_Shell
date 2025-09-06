# Shell für den STM32 via UART

The code based on the two original workspaces of [ShareCat](https://github.com/ShareCat/STM32CommandLine) and [mdiepart](https://github.com/mdiepart/ushell-stm32).

#

Die Bibliothek stellt alle Rescourcen für die Kommandozeile zur Verfügung. Dabei können dann
Befehle eingelesen und Informationen ausgegeben werden. Ebenfalls ist eine farbliche
Darstellung von Informationen möglich.

Für ein Passwort kann folgendes Define definiert werden:

```C
#define CLI_PASSWORD
```

Für einen benutzerdefinierten Namen muss folgendes Define definiert werden:

```C
#define CLI_NAME
```