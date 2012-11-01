
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.04
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _MSGDATA_H
#define _MSGDATA_H

#include "tools.h"

#define ST_JAM              0
#define ST_SQUISH           1
#define ST_USENET           2

typedef struct {
   USHORT Size;             // Dimensioni della struttura
   CHAR   Display[128];     // Descrizione dell'area
   CHAR   Key[16];          // Nome dell'area
   USHORT Level;            // Livello di accesso
   ULONG  AccessFlags;      // Flags di accesso
   ULONG  DenyFlags;        // Flags di inibizione
   USHORT WriteLevel;       // Livello di scrittura
   ULONG  WriteFlags;       // Flags di scrittura
   ULONG  DenyWriteFlags;   // Flags di inibizione alla scrittura
   UCHAR  Age;              // Eta' minima per l'accesso all'area
   USHORT Storage;          // Tipo di base messaggi
   CHAR   Path[128];        // Path in cui risiedono i messaggi
   USHORT Board;            // Numero della board (per le basi messaggi che lo richiedono)
   USHORT Flags;            // Flags
   USHORT Group;            // Group number
   CHAR   EchoMail;         // TRUE=This is an echomail area
   CHAR   ShowGlobal;       // TRUE=Show this area in global areas list
   CHAR   UpdateNews;       // TRUE=Update messages from newsserver
   CHAR   Offline;          // TRUE=Can be used with the Offline-reader
   CHAR   MenuName[32];     // Nome del menu' per quest'area
   CHAR   Moderator[64];    // Nome del gestore dell'area
   ULONG  Cost;             // Costo per l'accesso all'area
   USHORT DaysOld;          // Numero massimo di giorni per conservare un messaggio
   USHORT RecvDaysOld;      // Numero massimo di giorni per i messaggi ricevuti
   USHORT MaxMessages;      // Numero massimo di messaggi da conservare
   ULONG  ActiveMsgs;       // Numero di messaggi presenti
   CHAR   NewsGroup[128];   // Nome dell'USENET newsgroup associato
   ULONG  Highest;          // Highest message retrived from the newsserver
   CHAR   EchoTag[64];      // Nome dell'area se echomail (Fidonet)
   CHAR   Origin[64];       // Origin da usare al posto di quella di default
   ADDR   Feeder;           // Feeder FidoNet
   USHORT Forwards;         // Numero di forward
   ADDR   Forward[1];       // Nodi a cui inviare l'area
} MESSAGE;

#ifndef IDX_DELETED
#define IDX_DELETED         0x0001

typedef struct {
   CHAR   Key[16];
   USHORT Level;
   ULONG  AccessFlags;
   ULONG  DenyFlags;
   ULONG  Position;
   USHORT Size;
   USHORT Flags;
} INDEX;

#endif

class DLL_EXPORT TMsgData
{
public:
   TMsgData (void);
   TMsgData (PSZ pszDataPath);
   ~TMsgData (void);

   CHAR   Key[16];
   CHAR   Display[128];
   USHORT Level, WriteLevel;
   ULONG  AccessFlags, DenyFlags, WriteFlags, DenyWriteFlags;
   UCHAR  Age;
   USHORT Storage;
   CHAR   Path[128];
   USHORT Board;
   USHORT Flags, Group;
   CHAR   EchoMail, ShowGlobal, UpdateNews, Offline;
   CHAR   MenuName[32], Moderator[64];
   ULONG  Cost;
   USHORT DaysOld, RecvDaysOld, MaxMessages;
   ULONG  ActiveMsgs;
   CHAR   NewsGroup[128], EchoTag[64], Origin[64];
   ULONG  Highest;
   class  TAddress Feeder;
   class  TAddress Forward;

   USHORT IsValid;
   USHORT Deleted;

   USHORT Add (VOID);
   VOID   Delete (VOID);
   USHORT First (VOID);
   USHORT IsNext (VOID);
   USHORT IsPrevious (VOID);
   USHORT Last (VOID);
   VOID   New (VOID);
   USHORT Next (VOID);
   VOID   Pack (VOID);
   USHORT Previous (VOID);
   USHORT Read (PSZ pszName, USHORT fCloseFile = TRUE);
   USHORT ReRead (VOID);
   USHORT Update (VOID);

private:
   int    fdDat;
   int    fdIdx;
   CHAR   DataPath[128];

   VOID   Class2Struct (MESSAGE *Msg);
   VOID   Struct2Class (MESSAGE *Msg);
};

#endif

