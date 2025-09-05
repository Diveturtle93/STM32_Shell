# Shell für den STM32 via UART
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