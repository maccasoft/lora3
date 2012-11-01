
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _CONFIG_H
#define _CONFIG_H

#include "tools.h"

typedef struct {
   CHAR   Key[16];
   CHAR   Description[32];
   CHAR   File[16];
   CHAR   MenuPath[48];
   CHAR   MiscPath[48];
} LANGUAGE;

class DLL_EXPORT TLangCfg
{
public:
   TLangCfg (void);
   ~TLangCfg (void);

   CHAR   Key[16];
   CHAR   Description[32];
   CHAR   File[16];
   CHAR   MenuPath[48];
   CHAR   MiscPath[48];

   VOID   Add (VOID);
   VOID   Add (LANGUAGE *lpLanguage);
   USHORT Check (PSZ pszKey);
   VOID   Clear (VOID);
   USHORT First (VOID);
   USHORT Next (VOID);
   VOID   Remove (VOID);
   VOID   Update (VOID);

private:
   class TCollection Data;
};

#define Q_NO         0
#define Q_YES        1
#define Q_REQUIRED   2

#define CH_LOCAL     1           // Console Locale
#define CH_MODEM     2           // Modem
#define CH_SERIAL    3           // Seriale NULL-modem
#define CH_NPIPE     4           // OS/2 Named Pipe
#define CH_TELNET    5           // Telnet (richiede TCP/IP)
#define CH_NETBIOS   6           // Server NetBIOS
#define CH_IPX       7           // Server IPX

typedef struct {
   USHORT Size;
   USHORT Type;                  // Tipo di connessione
   USHORT Channel;               // Numero del primo canale da occupare
   USHORT MaxChannels;           // Massimo numero di connessioni disponibili
   ULONG  Cost;                  // Costo del collegamento su questo canale
   // -- Serial port channel specifics
   CHAR   Device[32];            // Nome del device di comunicazione
   ULONG  Speed;                 // Velocita' massima
   // -- Modem channel specifics
   CHAR   Initialize[3][64];     // Stringhe di inizializzazione del modem
   CHAR   Answer[64];            // Comando per la risposta
   CHAR   Hangup[64];            // Comando per effettuare l'hangup
   CHAR   OffHook[64];           // Comando per occupare la linea
   CHAR   Dial[64];              // Comando per iniziare una chiamata
   // -- Named Pipe server channel specifics
   CHAR   PipeName[64];          // Nome del PIPE da utilizzare
   // -- Telnet socket channel specifics
   USHORT Port;
   // -- Generic configuration
   USHORT NewLevel;              // Livello per i nuovi utenti
   CHAR   NewLimitClass[16];
   ULONG  NewAccessFlags;        // Flags per i nuovi utenti
   ULONG  NewDenyFlags;          // Flags di inibizione per i nuovi utenti
   USHORT RookieCalls;
   USHORT MinimumAge;
   CHAR   AskRealName;
   CHAR   AskCompany;
   CHAR   AskAddress;
   CHAR   AskZipCity;
   CHAR   AskState;
   CHAR   AskPhone;
   CHAR   AskSex;
   USHORT InactivityTime;
   USHORT LoginTime;
   CHAR   Logo[16];
   ULONG  Calls;
} CHANNEL;

#define LOG_PLUS        0x01
#define LOG_EXCLAM      0x02
#define LOG_COLON       0x04
#define LOG_STAR        0x08
#define LOG_BLANK       0x10

typedef struct {
   USHORT Size;                  // Dimensione della struttura
   CHAR   SystemName[64];        // Nome del sistema
   CHAR   SysopName[48];         // Nome del Sysop
   CHAR   SystemPath[48];        // Path comune a tutti i canali
   CHAR   MiscPath[48];          // Path dove cercare i file di testo
   CHAR   MenuPath[48];
   CHAR   UserFile[48];          // Path dove cercare il database utenti
   CHAR   HomePath[48];          // User's home directory
   CHAR   LogPath[48];           // Path dove memorizzare il log dei canali
   USHORT LogLevel;              // Livello di informazioni nel log
   CHAR   MailSpool[48];         // Directory dove memorizzare le mailbox
   CHAR   SentMail[48];          // Directory per i messaggi inviati
   USHORT FidoAddresses;         // Numero di indirizzo FidoNet definiti
   CHAR   HostName[48];          // Nome dell'host internet
   CHAR   NewsServer[48];        // News server di default
   CHAR   SMTPServer[48];        // Mail server (SMPT) di default
   CHAR   Inbound[48];
   CHAR   InboundProt[48];
   CHAR   InboundKnown[48];
   CHAR   Outbound[64];
   CHAR   BbsId[16];
   CHAR   CityState[32];
   CHAR   PhoneNumber[32];
   CHAR   FirstMenu[16];         // First menu to display (default TOP.MNU)
   CHAR   Bulletin[16];          // OFFLINE: Bulletin file
   CHAR   Welcome[16];           // OFFLINE: Welcome file
   USHORT BaseNode;
   CHAR   TempPath[48];
   UCHAR  PwdAttempts;
   USHORT MaxOfflineMsgs;
   ULONG  TotalCalls;
   ULONG  MaxBankTime;
   USHORT MaxInactiveUser;
   CHAR   FirstMessageArea[16];
   CHAR   FirstFileArea[16];
   UCHAR  FtpServer;
   UCHAR  Pop3Server;
   USHORT FidoNodelists;         // Numero di nodelist definite
   USHORT Packers;               // Numero di compattatori definiti
   USHORT Languages;             // Number of languages installed
} CONFIG;

typedef struct {
   CHAR   NodeList[16];          // Nome della nodelist
   CHAR   NodeDiff[16];          // Nome della nodediff (differenza dalla precedente)
   CHAR   ArchiveList[16];       // Nome dell'archivio contenente la nodelist
   CHAR   ArchiveDiff[16];       // Nome dell'archivio contenente la nodediff
   USHORT DiffDays;              // Numero di giorni tra una nodelist e una nodediff valida
   USHORT Flags;                 // Flags vari
} NODELIST;

#if defined(__OS2__) || defined(__WINDOWS__) || defined(__NT__)
class _export TConfig
#else
class TConfig
#endif
{
public:
   TConfig (PSZ pszConfig, PSZ pszChannels);
   TConfig (PSZ pszConfig, PSZ pszChannels, USHORT usChannel);
   TConfig (PSZ pszDataPath);
   ~TConfig (void);

   // -- Channel specific parameters
   USHORT Type, Channel, MaxChannels;
   ULONG  Cost;
   CHAR   Device[32];
   ULONG  Speed;
   CHAR   Initialize[3][64];
   CHAR   Answer[64], Hangup[64], OffHook[64], Dial[64];
   CHAR   PipeName[64];
   USHORT Port;

   // -- Generic channel configuration
   USHORT NewLevel;
   CHAR   NewLimitClass[16];
   ULONG  NewAccessFlags, NewDenyFlags;
   USHORT RookieCalls, MinimumAge;
   CHAR   AskRealName, AskCompany, AskAddress, AskZipCity;
   CHAR   AskState, AskPhone, AskSex;
   USHORT InactivityTime, LoginTime;
   CHAR   Logo[16];
   ULONG  Calls;

   // -- Generic system configuration parameters
   CHAR   SystemName[64], SysopName[48], SystemPath[48], MiscPath[48];
   CHAR   MenuPath[48], UserFile[48], HomePath[48], LogPath[48];
   USHORT LogLevel;
   CHAR   MailSpool[48], SentMail[48];
   USHORT FidoAddresses;
   CHAR   HostName[48], NewsServer[48], SMTPServer[48], POP3Server[48];
   CHAR   Inbound[48], InboundProt[48], InboundKnown[48];
   CHAR   Outbound[48];
   CHAR   BbsId[16], CityState[32], PhoneNumber[32];
   CHAR   Bulletin[16], Welcome[16], FirstMenu[16];
   USHORT BaseNode;
   CHAR   CommonPath[48], TempPath[48];
   UCHAR  PwdAttempts;
   USHORT MaxOfflineMsgs;
   UCHAR  MinAge, MaxAge;
   ULONG  TotalCalls, MaxBankTime;
   USHORT MaxInactiveUser;
   CHAR   FirstMessageArea[16], FirstFileArea[16];
   UCHAR  FtpServer, Pop3Server;
   class  TAddress    Address;
   class  TCollection NodeList;
   class  TLangCfg    Language;

   VOID   Clear (VOID);
   VOID   ClearChannel (VOID);
   USHORT ReadFirstChannel (VOID);
   USHORT ReadNextChannel (VOID);
   USHORT Read (USHORT usChannel, PSZ pszConfig = NULL, PSZ pszChannels = NULL);
   USHORT Remove (USHORT usChannel, PSZ pszChannels = NULL);
   USHORT Write (USHORT usChannel, PSZ pszConfig = NULL, PSZ pszChannels = NULL);

private:
   USHORT LastChannel;
   CHAR   Config[64];
   CHAR   Channels[64];
};

#endif

