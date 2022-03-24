;#ifndef __MESSAGES_H__
;#define __MESSAGES_H__
;


LanguageNames =
    (
        English = 0x0409:Messages_ENU
        German  = 0x0407:Messages_GER
    )


;////////////////////////////////////////
;// Eventlog categories
;//
;// These always have to be the first entries in a message file
;//

MessageId       = 1
SymbolicName    = CATEGORY_ONE
Severity		= Success
Language        = English
First category event
.
Language        = German
Ereignis erster Kategorie
.

MessageId       = +1
SymbolicName    = CATEGORY_TWO
Severity		= Success
Language        = English
Second category event
.
Language        = German
Ereignis zweiter Kategorie
.

;////////////////////////////////////////
;// Events
;//

MessageId       = +1
SymbolicName    = EVENT_STARTED_BY
Language        = English
The app %1 has been started successfully by user %2
.
Language        = German
Der Benutzer %2 konnte das Programm %1 erfolgreich starten
.

MessageId       = +1
SymbolicName    = EVENT_BACKUP
Language        = English
You should backup your data regulary!
.
Language        = German
Sie sollten Ihre Daten regelm‰ﬂig sichern!
.




;////////////////////////////////////////
;// Additional messages
;//

MessageId       = 1000
SymbolicName    = IDS_HELLO
Language        = English
Hello World!
.
Language        = German
Hallo, Welt!
.

MessageId       = +1
SymbolicName    = IDS_GREETING
Language        = English
Hello %1. How do you do?
.
Language        = German
Hallo %1. Wie geht es Ihnen?
.


;
;#endif  //__MESSAGES_H__
;
