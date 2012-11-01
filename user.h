
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _USER_H
#define _USER_H

#include "tools.h"

typedef struct {
   CHAR   Key[16];             // Nome di riferimento
   CHAR   Description[32];     // Descrizione del livello
   USHORT Level;
   ULONG  Flags;
   ULONG  DenyFlags;
   ULONG  ExpireDays;          // Numero di giorni di validita'
   CHAR   ExpireClass[16];     // Classe di limitazione dopo la scadenza
   USHORT ExpireLevel;         // Livello da selezionare dopo la scadenza
   ULONG  ExpireFlags;
   ULONG  ExpireDenyFlags;
   USHORT CallTimeLimit;       // Tempo limite per chiamata
   USHORT DayTimeLimit;        // Tempo limite giornaliero
   USHORT WeekTimeLimit;       // Tempo limite settimanale
   USHORT MonthTimeLimit;      // Tempo limite mensile
   ULONG  YearTimeLimit;       // Tempo limite annuale
   ULONG  CallDownloadLimit;   // Limite di download per chiamata
   ULONG  DayDownloadLimit;    // Limite di download giornaliero
   ULONG  WeekDownloadLimit;   // Limite di download settimanale
   ULONG  MonthDownloadLimit;  // Limite di download mensile
   ULONG  YearDownloadLimit;   // Limite di download annuale
   USHORT InactivityTime;
   USHORT ShowLevel;           // Livello per vedere questo livello nella lista
   ULONG  ShowFlags;
   ULONG  ShowDenyFlags;
} LIMITS;

class DLL_EXPORT TLimits
{
public:
   TLimits (void);
   TLimits (PSZ pszDataPath);
   ~TLimits (void);

   CHAR   Key[16];             // Nome di riferimento
   CHAR   Description[32];     // Descrizione del livello
   USHORT Level;
   ULONG  Flags, DenyFlags;
   ULONG  ExpireDays;          // Numero di giorni di validita'
   CHAR   ExpireClass[16];     // Classe di limitazione dopo la scadenza
   USHORT ExpireLevel;         // Livello da selezionare dopo la scadenza
   ULONG  ExpireFlags;
   ULONG  ExpireDenyFlags;
   USHORT CallTimeLimit;       // Tempo limite per chiamata
   USHORT DayTimeLimit;        // Tempo limite giornaliero
   USHORT WeekTimeLimit;       // Tempo limite settimanale
   USHORT MonthTimeLimit;      // Tempo limite mensile
   ULONG  YearTimeLimit;       // Tempo limite annuale
   ULONG  CallDownloadLimit;   // Limite di download per chiamata
   ULONG  DayDownloadLimit;    // Limite di download giornaliero
   ULONG  WeekDownloadLimit;   // Limite di download settimanale
   ULONG  MonthDownloadLimit;  // Limite di download mensile
   ULONG  YearDownloadLimit;   // Limite di download annuale
   USHORT InactivityTime;
   USHORT ShowLevel;           // Livello per vedere questo livello nella lista
   ULONG  ShowFlags;
   ULONG  ShowDenyFlags;

   USHORT Add (VOID);
   VOID   Clear (VOID);
   VOID   Delete (VOID);
   USHORT First (VOID);
   USHORT Next (VOID);
   USHORT Previous (VOID);
   USHORT Read (PSZ pszName, USHORT fCloseFile = TRUE);
   USHORT Update (VOID);

private:
   int    fdDat;
   CHAR   DataPath[64];
   CHAR   Temp[64];
};

// Identification code for the USER.Id field
#define USER_ID                0x4141

// Definitions for the USER.System field
#define NONE                   0
#define IBM_PC                 1
#define MAC                    2
#define APPLE                  3
#define AMIGA                  4
#define ATARI                  5
#define RADIO_SHACK            6
#define CPM                    7

// Definitions for the UINDEX.Flags field
#define IDX_DELETED            0x0001

typedef struct {
   USHORT Id;                  // USER_ID
   USHORT Size;                // Size of the record (complete with lastreads and tags)
   CHAR   Name[48];            // Name or nickname
   ULONG  Password;            // Password as a 32bit CRC
   CHAR   RealName[48];        // Real name
   CHAR   Company[36];         // Company name (if allowed)
   CHAR   Address1[48];        // First line of the address
   CHAR   Address2[48];        // Second line of the address
   CHAR   Address3[48];        // Third line of the address
   CHAR   DayPhone[26];        // Daytime phone number
   CHAR   Ansi;                // TRUE=User can display ANSI
   CHAR   Avatar;              // TRUE=User can display AVATAR
   CHAR   Color;               // TRUE=User can display color informations
   CHAR   HotKey;              // TRUE=User can use hot keyed menus
   CHAR   ReadHeader;          // TRUE=Read header then request to continue
   CHAR   System;              // System type (see above)
   CHAR   Sex;                 // M=Male, F=Female (no other sex allowed at this writing)
   ULONG  BirthDate;           // Birthdate as a Unix-style timestamp
   USHORT ScreenHeight;        // User's screen width
   USHORT ScreenWidth;         // User's screen height
   USHORT Level;               // Level
   ULONG  AccessFlags;         // Access flags
   ULONG  DenyFlags;           // Deny access flags
   ULONG  CreationDate;        // Record's creation date as a Unix-style timestamp
   ULONG  LastCall;            // User's last call date/time as a Unix-style timestamp
   CHAR   MailBox[32];         // User's mailbox name
   CHAR   LimitClass[16];      // Class of limits
   ULONG  TotalCalls;          // Number of calls to the system
   ULONG  TodayTime;           // Number of minutes on-line today
   ULONG  WeekTime;            // Number of minutes on-line this week
   ULONG  MonthTime;           // Number of minutes on-line this month
   ULONG  YearTime;            // Number of minutes on-line this year
   CHAR   Language[16];        // Currently selected language (if any)
   CHAR   FtpHost[48];         // Host name for FTP transfers
   CHAR   FtpName[32];         // User name for FTP transfers
   CHAR   FtpPwd[32];          // Password for FTP transfers
   USHORT LastReads;           // Number of lastreads that follows
   USHORT FileTags;            // Number of files tagged
} USER;

typedef struct {
   CHAR   Key[16];             // Area name
   ULONG  Number;              // Last packed message number
   UCHAR  QuickScan;
   UCHAR  PersonalOnly;
   UCHAR  ExcludeOwn;
} LASTREAD;

typedef struct {
   ULONG  NameCrc;             // User's name 32bit CRC
   ULONG  RealNameCrc;         // User's real name 32bit CRC
   ULONG  Position;            // Record's position inside the .DAT file
   USHORT Size;                // Record's size
   USHORT Flags;               // Various flags (see above)
} UINDEX;

class DLL_EXPORT TLastRead
{
public:
   TLastRead (void);
   ~TLastRead (void);

   CHAR   Key[16];
   ULONG  Number;
   UCHAR  QuickScan, PersonalOnly, ExcludeOwn;

   VOID   Add (VOID);
   VOID   Clear (VOID);
   USHORT First (VOID);
   USHORT Next (VOID);
   VOID   New (VOID);
   USHORT Read (PSZ pszKey);
   VOID   Remove (VOID);
   VOID   Update (VOID);

private:
   class TCollection Data;
};

typedef struct {
   USHORT Index;
   CHAR   Name[32];
   CHAR   Library[16];
   ULONG  Size;
   CHAR   Complete[128];
   USHORT DeleteAfter;
} FTAGS;

class DLL_EXPORT TFileTag
{
public:
   TFileTag (void);
   ~TFileTag (void);

   USHORT TotalNumber;
   ULONG  TotalSize;
   USHORT Index, DeleteAfter;
   CHAR   Name[32], Library[16];
   CHAR   Complete[128];
   ULONG  Size;

   USHORT Add (VOID);
   USHORT Check (PSZ pszName);
   VOID   Clear (VOID);
   USHORT First (VOID);
   VOID   New (VOID);
   USHORT Next (VOID);
   VOID   Reindex (VOID);
   VOID   Remove (PSZ pszName = NULL);
   USHORT Select (USHORT usIndex);

private:
   USHORT LastIndex;
   class  TCollection Tags;
};

class DLL_EXPORT TUser : public TCrc
{
public:
   TUser (void);
   TUser (PSZ pszUserFile);
   ~TUser (void);

   CHAR   Name[48];
   ULONG  Password;
   CHAR   RealName[48];
   CHAR   Company[36];
   CHAR   Address1[48], Address2[48], Address3[48];
   CHAR   DayPhone[26];
   CHAR   Ansi, Avatar, Color, HotKey, ReadHeader, System, Sex;
   ULONG  BirthDate;
   USHORT ScreenHeight, ScreenWidth;
   USHORT Level;
   ULONG  AccessFlags, DenyFlags;
   ULONG  CreationDate;
   ULONG  LastCall, TotalCalls;
   ULONG  TodayTime, WeekTime, MonthTime, YearTime;
   CHAR   MailBox[32];
   CHAR   LimitClass[16], Language[16];
   CHAR   FtpHost[48], FtpName[32], FtpPwd[32];

   USHORT Valid;
   USHORT Validate;
   class  TLastRead *LastRead;
   class  TFileTag  *FileTag;

   USHORT Add (USHORT usAddSize = 0);
   USHORT CheckPassword (PSZ pszPassword);
   VOID   Clear (VOID);
   USHORT Delete (VOID);
   USHORT Exist (PSZ pszName, USHORT fCheckRealName = TRUE);
   USHORT First (USHORT fComplete = TRUE);
   USHORT GetData (PSZ pszName, USHORT fCheckRealName = TRUE);
   PSZ    GetMailBox (PSZ pszName, USHORT fCheckRealname = TRUE);
   USHORT IsNext (VOID);
   USHORT IsPrevious (VOID);
   USHORT Next (USHORT fComplete = TRUE);
   VOID   Pack (VOID);
   VOID   SetPassword (PSZ pszPassword);
   USHORT Previous (USHORT fComplete = TRUE);
   USHORT Update (VOID);

private:
   int    fdDat, fdIdx;
   CHAR   TempMailBox[32], DatFile[64], IdxFile[64];
   ULONG  LastPosition;
};

#endif

