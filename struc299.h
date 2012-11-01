
/////////////////////////////////////////////////////////////////////////
// LoraBBS Professional Edition - Version 3.00.32                      //
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.        //
/////////////////////////////////////////////////////////////////////////

#ifndef _STRUC299_H
#define _STRUC299_H

/////////////////////////////////////////////////////////////////////////
// Index file for message and file areas                               //
/////////////////////////////////////////////////////////////////////////

#ifndef IDX_DELETED
#define IDX_DELETED         0x0001

typedef struct {
   CHAR   Key[16];
   USHORT Level;
   ULONG  AccessFlags;
   ULONG  DenyFlags;
   ULONG  Position;
   USHORT Flags;
} INDEX;

#endif

/////////////////////////////////////////////////////////////////////////
// Message areas structure (msg.dat / msg.idx)                         //
/////////////////////////////////////////////////////////////////////////

// Message base type identification numbers
#define ST_JAM                0
#define ST_SQUISH             1
#define ST_USENET             2
#define ST_FIDO               3
#define ST_ADEPT              4
#define ST_HUDSON             5     // Not implemented
#define ST_GOLDBASE           6     // Not implemented
#define ST_PASSTHROUGH        7

// Special values for origin index
#define OIDX_DEFAULT          0
#define OIDX_RANDOM           -1

typedef struct {
   USHORT Size;                     // Size of this structure
   CHAR   Display[128];             // Area's description
   CHAR   Key[16];                  // Area's name
   USHORT Level;                    // Access level
   ULONG  AccessFlags;              // Access flags
   ULONG  DenyFlags;                // Deny flags
   USHORT WriteLevel;               // Write level
   ULONG  WriteFlags;               // Write flags
   ULONG  DenyWriteFlags;           // Deny write flags
   UCHAR  Age;                      // Minimum age to access the area
   USHORT Storage;                  // Message base type
   CHAR   Path[128];                // Path to message base files
   USHORT Board;                    // Board number (for Goldbase and Hudson)
   USHORT Flags;                    // Flags
   USHORT Group;                    // Group number
   CHAR   EchoMail;                 // TRUE=This is an echomail area
   CHAR   ShowGlobal;               // TRUE=Show this area in global areas list
   CHAR   UpdateNews;               // TRUE=Update messages from newsserver
   CHAR   Offline;                  // TRUE=Can be used with the Offline-reader
   CHAR   MenuName[32];             // Menu name for this area
   CHAR   Moderator[64];            // Name of this area's moderator
   ULONG  Cost;                     // Cost to access this area
   USHORT DaysOld;                  // Number of days to keep messages
   USHORT RecvDaysOld;              // Number of days to keep a received message
   USHORT MaxMessages;              // Maximum number of messages to keep
   ULONG  ActiveMsgs;               // Number of messages in this area
   CHAR   NewsGroup[128];           // Usenet newsgroup name
   ULONG  Highest;                  // Highest message retrived from the newsserver
   CHAR   EchoTag[64];              // EchoMail tag name
   SHORT  OriginIndex;              // Index to the origin line or special value
   CHAR   Origin[80];               // Origin da usare al posto di quella di default
   ULONG  HighWaterMark;            // Highest message number sent
   CHAR   Address[48];              // Address to use when sending echomail messages
   ULONG  FirstMessage;             // First message number in area
   ULONG  LastMessage;              // Last message number in area
} MESSAGE;

/////////////////////////////////////////////////////////////////////////
// EchoMail link file structure (echolink.dat)                         //
/////////////////////////////////////////////////////////////////////////

typedef struct {
   USHORT Free;                     // TRUE=Free record
   ULONG  EchoTag;                  // 32bit CRC of echomail tag name
   USHORT Zone;                     // Zone number
   USHORT Net;                      // Net number
   USHORT Node;                     // Node number
   USHORT Point;                    // Point number
   CHAR   Domain[32];               // Domain name
   UCHAR  SendOnly;                 // TRUE=Node can only send messages
   UCHAR  ReceiveOnly;              // TRUE=Node can only receive messages
   UCHAR  PersonalOnly;             // TRUE=Node receive only messages for the Sysop
   UCHAR  Passive;                  // TRUE=Passive mode (message are not exported)
   UCHAR  Skip;                     // Used internally
} ECHOLINK;

/////////////////////////////////////////////////////////////////////////
// File areas structures (file.dat / file.idx)                         //
/////////////////////////////////////////////////////////////////////////

typedef struct {
   USHORT Size;                     // Size of this structure
   CHAR   Display[128];             // Area's description
   CHAR   Key[16];                  // Area's name
   USHORT Level;                    // Access level
   ULONG  AccessFlags;              // Access flags
   ULONG  DenyFlags;                // Deny flags
   USHORT UploadLevel;              // Upload level
   ULONG  UploadFlags;              // Upload flags
   ULONG  UploadDenyFlags;          // Deny upload flags
   USHORT DownloadLevel;            // Download level
   ULONG  DownloadFlags;            // Download flags
   ULONG  DownloadDenyFlags;        // Deny download flags
   UCHAR  Age;                      // Minimum age to access the area
   CHAR   Download[128];            // Path where download files are stored
   CHAR   Upload[128];              // Path where uploaded files are to be stored
   CHAR   CdRom;                    // TRUE=The are is on CDROM
   CHAR   FreeDownload;             // TRUE=No download limits
   CHAR   ShowGlobal;               // TRUE=Show in global lists
   CHAR   MenuName[32];             // Name of the menu file for this area
   CHAR   Moderator[64];            // Name of the manager of this area
   ULONG  Cost;                     // Cost to access this area
   ULONG  ActiveFiles;              // Number of files in this area
   ULONG  UnapprovedFiles;          // Number of unapproved files
   CHAR   EchoTag[64];              // Fileecho (TIC) tag name
   UCHAR  UseFilesBBS;              // Uses the files.bbs instead of the filebase
   UCHAR  DlCost;                   // Cost for download each file
   CHAR   FileList[128];            // Alternative files list
} FILES;

/////////////////////////////////////////////////////////////////////////
// FileBase structures (filebase.dat / filebase.idx)                   //
/////////////////////////////////////////////////////////////////////////

#define DATA_EXT           ".dat"
#define INDEX_EXT          ".idx"

#define FILE_DELETED       0x8000
#define FILE_OFFLINE       0x4000
#define FILE_UNAPPROVED    0x2000
#define FILE_CDROM         0x1000

#define FILEBASE_ID        0x602C789FL

typedef struct {
   ULONG  Id;                       // Must be FILEBASE_ID
   CHAR   Area[32];                 // File area name
   CHAR   Name[32];                 // File name
   CHAR   Complete[128];            // Fully qualified pathfilename
   USHORT Description;              // Description length
   USHORT Uploader;                 // Uploader name length
   CHAR   Keyword[32];              // Optional keywords
   ULONG  Size;                     // Size in bytes
   ULONG  DlTimes;                  // Number of downloads
   ULONG  FileDate;                 // File's creation date and time (seconds since 1970)
   ULONG  UploadDate;               // File's upload date and time
   ULONG  Cost;                     // Cost for download this file
   ULONG  Password;                 // 32bit CRC of file's download password
   USHORT Level;                    // Security level
   ULONG  AccessFlags;              // Access flags
   ULONG  DenyFlags;                // Deny flags
   USHORT Flags;                    // Miscellaneous flags
} FILEDATA;

typedef struct {
   ULONG  Area;                     // 32bit CRC of file's area name
   CHAR   Name[32];                 // File name
   ULONG  UploadDate;               // File's upload date and time
   ULONG  Offset;                   // Offset of the FILEDATA structure
   USHORT Flags;                    // Miscellaneous flags
} FILEINDEX;

/////////////////////////////////////////////////////////////////////////
// Users database structures (user.dat / user.idx)                     //
/////////////////////////////////////////////////////////////////////////

typedef struct {
   USHORT Deleted;                  // Record is deleted
   ULONG  NameCrc;                  // User's name 32bit CRC
   ULONG  RealNameCrc;              // User's real name 32bit CRC
   ULONG  Position;
} UINDEX;

typedef struct {
   USHORT Size;                     // Size of the record
   CHAR   Name[48];                 // Name or nickname
   ULONG  Password;                 // Password as a 32bit CRC
   CHAR   RealName[48];             // Real name
   CHAR   Company[36];              // Company name (if allowed)
   CHAR   Address[48];              // First line of the address
   CHAR   City[48];                 // Second line of the address
   CHAR   DayPhone[26];             // Daytime phone number
   CHAR   Ansi;                     // TRUE=User can display ANSI
   CHAR   Avatar;                   // TRUE=User can display AVATAR
   CHAR   Color;                    // TRUE=User can display color informations
   CHAR   HotKey;                   // TRUE=User can use hot keyed menus
   CHAR   System;                   // System type (see above)
   CHAR   Sex;                      // M=Male, F=Female (no other sex allowed at this writing)
   UCHAR  FullEd;                   // TRUE=Use fullscreen editor
   UCHAR  FullReader;               // TRUE=Use fullscreen reader
   UCHAR  NoDisturb;                // TRUE=Do not disturb
   UCHAR  AccessFailed;             // TRUE=Last access attempt failed
   USHORT ScreenHeight;             // User's screen width
   USHORT ScreenWidth;              // User's screen height
   USHORT Level;                    // Level
   ULONG  AccessFlags;              // Access flags
   ULONG  DenyFlags;                // Deny access flags
   ULONG  CreationDate;             // Record's creation date as a Unix-style timestamp
   ULONG  LastCall;                 // User's last call date/time as a Unix-style timestamp
   CHAR   MailBox[32];              // User's mailbox name
   CHAR   LimitClass[16];           // Class of limits
   ULONG  TotalCalls;               // Number of calls to the system
   ULONG  TodayTime;                // Number of minutes on-line today
   ULONG  WeekTime;                 // Number of minutes on-line this week
   ULONG  MonthTime;                // Number of minutes on-line this month
   ULONG  YearTime;                 // Number of minutes on-line this year
   CHAR   Language[16];             // Currently selected language (if any)
   CHAR   FtpHost[48];              // Host name for FTP transfers
   CHAR   FtpName[32];              // User name for FTP transfers
   CHAR   FtpPwd[32];               // Password for FTP transfers
   CHAR   LastMsgArea[16];          // Last message area visited
   CHAR   LastFileArea[16];         // Last file area visited
   USHORT UploadFiles;
   ULONG  UploadBytes;
   USHORT DownloadFiles;
   ULONG  DownloadBytes;
   USHORT FilesToday;
   ULONG  BytesToday;
   UCHAR  ImportPOP3Mail;           // TRUE=Import e-mail from POP3 server
   UCHAR  UseInetAddress;           // TRUE=Use the personal Inet address
   CHAR   InetAddress[64];          // Personal E-Mail address
   CHAR   Pop3Pwd[32];              // Password for POP3 mail server
   CHAR   Archiver[16];             // Default archiver
   CHAR   Protocol[16];             // Default protocol
   CHAR   Signature[64];            // Personal signature
   CHAR   FullScreen;               // TRUE=Full screen enhancements
   CHAR   IBMChars;                 // TRUE=Can display IBM characters
   CHAR   MorePrompt;               // TRUE=Display More? prompt at each page
   CHAR   ScreenClear;              // TRUE=Send screen clear codes
   CHAR   InUserList;               // TRUE=Display in user list
   CHAR   Kludges;                  // TRUE=Display message's kludge lines
   CHAR   MailCheck;                // TRUE=Mail check at logon
   CHAR   NewFileCheck;             // TRUE=Check for new files at logon
   UCHAR  BirthDay;                 // Day of birthdate
   UCHAR  BirthMonth;               // Month of birthdate
   USHORT BirthYear;                // Year of birthdate
   ULONG  LastPwdChange;            // Time of last password change
} USER;

#endif

