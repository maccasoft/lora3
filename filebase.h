
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.20
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _FILEBASE_H
#define _FILEBASE_H

#include "tools.h"

#define DATA_EXT        ".DAT"
#define INDEX_EXT       ".IDX"

#define FILE_DELETED    0x8000
#define FILE_OFFLINE    0x4000

typedef struct {
   CHAR   Area[32];
   CHAR   Name[32];
   CHAR   Complete[128];
   USHORT Description;
   USHORT Uploader;
   CHAR   Keyword[32];
   ULONG  Size;
   ULONG  DlTimes;
   ULONG  FileDate;
   ULONG  UploadDate;
   ULONG  Cost;
   ULONG  Password;
   USHORT Level;
   ULONG  AccessFlags;
   ULONG  DenyFlags;
   USHORT Flags;
} FILEDATA;

typedef struct {
   ULONG  Area;
   CHAR   Name[32];
   ULONG  UploadDate;
   ULONG  Offset;
   USHORT Flags;
} FILEINDEX;

typedef struct {
   UCHAR  Day;
   UCHAR  Month;
   USHORT Year;
   UCHAR  Hour;
   UCHAR  Minute;
} FBDATE;

class DLL_EXPORT TFile : public TCrc
{
public:
   TFile (void);
   TFile (PSZ pszPath, PSZ pszArea);
   ~TFile (void);

   CHAR   Area[32], Name[32], Complete[128];
   FBDATE UplDate, Date;
   USHORT Level;
   PSZ    Uploader;
   CHAR   Keyword[32];
   ULONG  Size, DlTimes;
   ULONG  Cost, Password, AccessFlags, DenyFlags;
   ULONG  UploadDate, FileDate;
   class  TCollection *Description;

   USHORT Add (VOID);
   VOID   Clear (VOID);
   VOID   Close (VOID);
   VOID   Delete (VOID);
   USHORT First (VOID);
   USHORT Next (VOID);
   USHORT Open (PSZ pszPath, PSZ pszArea);
   USHORT Previous (VOID);
   USHORT Read (PSZ pszFile);
   USHORT Replace (VOID);
   USHORT ReplaceHeader (VOID);
   VOID   SearchFile (PSZ pszFile);
   VOID   SearchKeyword (PSZ pszKeyword);
   VOID   SearchText (PSZ pszText);
   VOID   SortByDate (ULONG ulDate = 0L);
   VOID   SortByDownload (VOID);
   VOID   SortByName (VOID);

private:
   int    fdIdx;
   int    fdDat;
   CHAR   szArea[32];
   CHAR   fUploader;
   PSZ    pszMemUploader;
   ULONG  LastPos;
   ULONG  LastDate;
   class  TCollection *List;
};

#endif

