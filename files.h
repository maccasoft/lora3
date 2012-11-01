
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.04
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _FILEDAT_H
#define _FILEDAT_H

#include "tools.h"

typedef struct {
   USHORT Size;                // Dimensioni della struttura
   CHAR   Display[128];        // Descrizione dell'area
   CHAR   Key[16];             // Nome dell'area
   USHORT Level;               // Livello di accesso
   ULONG  AccessFlags;         // Flags di accesso
   ULONG  DenyFlags;           // Flags di inibizione dell'accesso
   USHORT UploadLevel;         // Livello di upload
   ULONG  UploadFlags;         // Flags di upload
   ULONG  UploadDenyFlags;     // Flags di inibizione dell'upload
   USHORT DownloadLevel;       // Livello di download
   ULONG  DownloadFlags;       // Flags di download
   ULONG  DownloadDenyFlags;   // Flags di inibizione al download
   UCHAR  Age;                 // Eta' minima per l'accesso all'area
   CHAR   Download[128];       // Path da cui prelevare i files
   CHAR   Upload[128];         // Path per l'upload
   CHAR   CdRom;               // TRUE=I files sono su un CD-ROM
   CHAR   FreeDownload;        // TRUE=Non si considerano i limiti di download
   CHAR   ShowGlobal;          // TRUE=Show this area in global areas list
   CHAR   MenuName[32];        // Nome del menu' per quest'area
   CHAR   Moderator[64];       // Nome del gestore dell'area
   ULONG  Cost;                // Costo per l'accesso all'area
   ULONG  ActiveFiles;         // Numero di files presenti
   ULONG  UnapprovedFiles;     // Numero di files da approvare
   CHAR   EchoTag[64];         // Nome dell'area se echofile (Fidonet)
   ADDR   Feeder;              // Feeder FidoNet
   USHORT Forwards;            // Numero di forward
   ADDR   Forward[1];          // Nodi a cui inviare l'area
} FILES;

#ifndef IDX_DELETED
#define IDX_DELETED          0x0001

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

class DLL_EXPORT FileData
{
public:
   FileData (void);
   FileData (PSZ pszDataPath);
   ~FileData (void);

   CHAR   Display[128], Key[16];
   USHORT Level;
   ULONG  AccessFlags, DenyFlags;
   USHORT UploadLevel;
   ULONG  UploadFlags, UploadDenyFlags;
   USHORT DownloadLevel;
   ULONG  DownloadFlags, DownloadDenyFlags;
   UCHAR  Age;
   CHAR   Download[128];
   CHAR   Upload[128];
   CHAR   CdRom, FreeDownload, ShowGlobal;
   CHAR   MenuName[32];
   CHAR   Moderator[64];
   ULONG  Cost;
   ULONG  ActiveFiles, UnapprovedFiles;
   CHAR   EchoTag[64];
   ADDR   Feeder;
   class  TCollection *Forward;

   USHORT IsValid;
   USHORT Deleted;

   USHORT Add (VOID);
   VOID   Delete (VOID);
   USHORT First (VOID);
   USHORT IsNext (VOID);
   USHORT IsPrevious (VOID);
   USHORT Last (VOID);
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
   CHAR   DataPath[128];

   VOID   Class2Struct (FILES *File);
   VOID   Struct2Class (FILES *File);
};

#endif

