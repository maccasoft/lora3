
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _FORUM_H
#define _FORUM_H

#include "tools.h"

typedef struct {
   USHORT Size;             // Dimensioni della struttura
   CHAR   Display[128];     // Testo da visualizzare all'utente
   CHAR   Key[16];          // Nome del forum (per la selezione)
   USHORT Level;            // Livello di accesso minimo
   ULONG  AccessFlags;      // Flag di accesso
   ULONG  DenyFlags;        // Flag di impedimento all'accesso
   UCHAR  Age;              // Eta' minima per accedere al forum
   CHAR   Location[128];    // Location of this forum's private files
   USHORT Flags;            // Flags (non definiti)
   CHAR   MenuName[32];     // Nome del menu' per questo forum
   CHAR   Moderator[64];    // Nome del moderatore (Sysop) del forum
   ULONG  Cost;             // Costo al minuto per l'accesso al forum
   ULONG  ActiveMsgs;       // Numero di messaggi totali
   ULONG  ActiveFiles;      // Numero di files totali
   USHORT Msgs;
   USHORT Files;
   CHAR   AreaKey[1][16];   // Aree appartenenti al forum
} FORUM;

#ifndef IDX_DELETED
#define IDX_DELETED         0x0001

typedef struct {
   CHAR   Key[32];
   USHORT Level;
   ULONG  AccessFlags;
   ULONG  DenyFlags;
   ULONG  Position;
   USHORT Size;
   USHORT Flags;
} INDEX;

#endif

class DLL_EXPORT ForumData
{
public:
   ForumData (void);
   ~ForumData (void);

   CHAR   Display[128];
   CHAR   Key[16];
   USHORT Level;
   ULONG  AccessFlags;
   ULONG  DenyFlags;
   UCHAR  Age;
   CHAR   Location[128];
   USHORT Flags;
   CHAR   MenuName[32];
   CHAR   Moderator[64];
   ULONG  Cost;
   ULONG  ActiveMsgs;
   ULONG  ActiveFiles;
   class  TCollection Msg;
   class  TCollection File;

   USHORT IsValid;
   USHORT Deleted;

   USHORT Add (VOID);
   VOID   Delete (VOID);
   USHORT First (VOID);
   USHORT IsNext (VOID);
   USHORT IsPrevious (VOID);
   USHORT Next (VOID);
   VOID   Pack (VOID);
   USHORT Previous (VOID);
   USHORT Read (PSZ pszName, USHORT fCloseFile = TRUE);
   USHORT ReRead (VOID);
   VOID   Reset (VOID);
   USHORT Update (VOID);

private:
   int    fdDat;
   int    fdIdx;

   VOID   Class2Struct (FORUM *Forum);
   VOID   Struct2Class (FORUM *Forum);
};

#endif

