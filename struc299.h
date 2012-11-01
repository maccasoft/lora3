
// LoraBBS Version 2.99 Free Edition
// Copyright (C) 1987-98 Marco Maccaferri
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef _STRUC299_H
#define _STRUC299_H

/////////////////////////////////////////////////////////////////////////
// Global definitions                                                  //
/////////////////////////////////////////////////////////////////////////

#define VERSION            "2.99.41"
#define VER_MAJOR          2
#define VER_MINOR          99
#define PRODUCT_ID         0x4E

/////////////////////////////////////////////////////////////////////////
// Index file for message and file areas                               //
/////////////////////////////////////////////////////////////////////////

#ifndef IDX_DELETED
#define IDX_DELETED        0x0001

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
#define ST_JAM             0
#define ST_SQUISH          1
#define ST_USENET          2
#define ST_FIDO            3
#define ST_ADEPT           4
#define ST_HUDSON          5     // Not implemented
#define ST_GOLDBASE        6     // Not implemented
#define ST_PASSTHROUGH     7

// Special values for origin index
#define OIDX_DEFAULT       0
#define OIDX_RANDOM        -1

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
   ULONG  NewsHWM;                  // High water mark for newsgroup areas
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
   USHORT UploadFiles;              // Number of uploaded files
   ULONG  UploadBytes;              // Total size of the uploaded files
   USHORT DownloadFiles;            // Number of downloaded files
   ULONG  DownloadBytes;            // Total size of the downloaded files
   USHORT FilesToday;               // Files downloaded today
   ULONG  BytesToday;               // Size of today downloaded files
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
   USHORT PwdLength;                // Length of text password
   CHAR   PwdText[32];              // Text password, encrypted
} USER;

/////////////////////////////////////////////////////////////////////////
// Tagged message areas for offline reader (msgtags.dat)               //
/////////////////////////////////////////////////////////////////////////

typedef struct {
   UCHAR  Free;                     // TRUE=Record is free
   UCHAR  Tagged;                   // TRUE=Tagged for offline mail packing
   ULONG  UserId;                   // CRC32 of user's name
   CHAR   Area[16];                 // Area name
   ULONG  LastRead;                 // Number of last message read
   ULONG  OlderMsg;                 // Timestamp of the older message to pack
} MSGTAGS;

/////////////////////////////////////////////////////////////////////////
// Files tagged for later download (filetags.dat)                      //
/////////////////////////////////////////////////////////////////////////

typedef struct {
   UCHAR  Free;                     // TRUE=Record is free
   ULONG  UserId;                   // CRC32 of user's name
   USHORT Index;                    // Reserved for internal use
   CHAR   Area[16];                 // Area name
   CHAR   Name[32];                 // Name of tagged file
   ULONG  Size;                     // Size in bytes
   CHAR   Complete[128];            // Complete path and file name
   USHORT DeleteAfter;              // Delete file after transfer
   USHORT CdRom;                    // The file is on a CD-ROM drive
} FILETAGS;

/////////////////////////////////////////////////////////////////////////
// Files and magic names available for file requests (okfile.dat)      //
/////////////////////////////////////////////////////////////////////////

typedef struct {
   USHORT Size;                     // Size of this structure
   CHAR   Name[32];                 // File or magic-name
   CHAR   Path[128];                // Complete pathname to the file
   CHAR   Pwd[32];                  // Optional password
   CHAR   Normal;                   // TRUE=Available for unknown nodes
   CHAR   Known;                    // TRUE=Available for known nodes only
   CHAR   Protected;                // TRUE=Available for password-protected nodes
} OKFILE;

/////////////////////////////////////////////////////////////////////////
// FidoNet address structure (address.dat)                             //
/////////////////////////////////////////////////////////////////////////

typedef struct {
   USHORT Zone;                     // Zone number
   USHORT Net;                      // Net number
   USHORT Node;                     // Node number
   USHORT Point;                    // Point number
   CHAR   Domain[32];               // Domain name
   USHORT FakeNet;                  // 3d point fake net address
} MAILADDRESS;

/////////////////////////////////////////////////////////////////////////
// Main configuration file (config.dat)                                //
/////////////////////////////////////////////////////////////////////////

#define CONFIG_VERSION     2942

#define NO                 0
#define YES                1
#define ASK                2
#define AUTO               2
#define REQUIRED           2
#define PROMPT             3

typedef struct {
   USHORT Size;
   USHORT Version;

   CHAR   Device[32];               // Communication Device (COM2, COM3, etc.)
   ULONG  Speed;                    // DTE Maximum Speed
   USHORT LockSpeed;
   CHAR   Initialize[3][48];        // Initialization Strings
   CHAR   Answer[48];               // Command to Answer an Incoming Call
   CHAR   Dial[48];                 // Command to Dial to Another Modem
   CHAR   Hangup[48];               // Command to Hangup the Line
   CHAR   OffHook[48];              // Command to Bring the Modem OffHook
   USHORT DialTimeout;
   USHORT CarrierDropTimeout;
   USHORT StripDashes;
   CHAR   FaxMessage[48];
   CHAR   FaxCommand[64];

   CHAR   SystemName[64];
   CHAR   SysopName[48];
   CHAR   Location[48];
   CHAR   Phone[32];
   CHAR   NodelistFlags[64];

   USHORT NewUserLevel;
   ULONG  NewUserFlags;
   ULONG  NewUserDenyFlags;
   CHAR   NewUserLimits[16];

   UCHAR  LoginType;
   UCHAR  UseAnsi;
   UCHAR  AskAlias;
   UCHAR  AskCompanyName;
   UCHAR  AskAddress;
   UCHAR  AskCity;
   UCHAR  AskPhoneNumber;
   UCHAR  AskGender;

   CHAR   LogFile[64];

   CHAR   SystemPath[64];
   CHAR   UserFile[64];
   CHAR   NormalInbound[64];
   CHAR   KnownInbound[64];
   CHAR   ProtectedInbound[64];
   CHAR   Outbound[64];
   CHAR   NodelistPath[64];
   CHAR   UsersHomePath[64];
   CHAR   MenuPath[64];
   CHAR   LanguageFile[64];
   CHAR   TextFiles[64];
   CHAR   SchedulerFile[64];

   CHAR   MainMenu[32];

   CHAR   HostName[48];
   CHAR   NewsServer[48];
   CHAR   MailServer[48];
   CHAR   PopServer[48];

   USHORT FakeNet;

   USHORT MailStorage;
   CHAR   MailPath[64];
   USHORT NetMailStorage;
   CHAR   NetMailPath[64];
   USHORT BadStorage;
   CHAR   BadPath[64];
   USHORT DupeStorage;
   CHAR   DupePath[64];

   USHORT TelnetServer;
   USHORT TelnetPort;
   USHORT FtpServer;
   USHORT FtpPort;
   USHORT WebServer;
   USHORT WebPort;
   USHORT SMTPServer;
   USHORT SMTPPort;
   USHORT POP3Server;
   USHORT POP3Port;
   USHORT NNTPServer;
   USHORT NNTPPort;

   USHORT WaZoo;
   USHORT EMSI;
   USHORT Janus;

   USHORT NewAreasStorage;
   CHAR   NewAreasPath[64];
   USHORT NewAreasLevel;
   ULONG  NewAreasFlags;
   ULONG  NewAreasDenyFlags;
   USHORT NewAreasWriteLevel;
   ULONG  NewAreasWriteFlags;
   ULONG  NewAreasDenyWriteFlags;

   UCHAR  Ansi;
   UCHAR  IEMSI;

   UCHAR  ImportEmpty;
   UCHAR  ReplaceTear;
   CHAR   TearLine[32];
   UCHAR  ForceIntl;
   UCHAR  Secure;
   UCHAR  KeepNetMail;
   UCHAR  TrackNetMail;

   CHAR   MailOnly[64];
   CHAR   EnterBBS[64];

   CHAR   ImportCmd[64];
   CHAR   ExportCmd[64];
   CHAR   SinglePassCmd[64];
   CHAR   PackCmd[64];
   CHAR   NewsgroupCmd[64];

   UCHAR  UseSinglePass;
   UCHAR  SeparateNetMail;

   CHAR   AreasBBS[64];
   UCHAR  UseAreasBBS;
   UCHAR  UpdateAreasBBS;

   CHAR   AfterCallerCmd[64];
   CHAR   AfterMailCmd[64];

   CHAR   Ring[32];
   UCHAR  ZModemTelnet;
   UCHAR  EnablePPP;
   USHORT PPPTimeLimit;
   CHAR   PPPCmd[64];

   CHAR   TempPath[64];
   CHAR   OLRPacketName[16];
   USHORT OLRMaxMessages;

   UCHAR  ExternalFax;
   UCHAR  FaxFormat;
   CHAR   FaxPath[64];
   CHAR   AfterFaxCmd[64];
   CHAR   FaxAlertNodes[64];
   CHAR   FaxAlertUser[64];

   UCHAR  ReloadLog;
   UCHAR  MakeProcessLog;
   USHORT RetriveMaxMessages;

   UCHAR  UseAvatar;
   UCHAR  UseColor;
   UCHAR  UseFullScreenEditor;
   UCHAR  UseHotKey;
   UCHAR  UseIBMChars;
   UCHAR  AskLines;
   UCHAR  UsePause;
   UCHAR  UseScreenClear;
   UCHAR  AskBirthDate;
   UCHAR  AskMailCheck;
   UCHAR  AskFileCheck;

   UCHAR  ExternalEditor;
   CHAR   EditorCmd[64];

   CHAR   HudsonPath[64];
   CHAR   GoldPath[64];
   USHORT BadBoard;
   USHORT DupeBoard;
   USHORT MailBoard;
   USHORT NetMailBoard;

   UCHAR  UseFullScreenReader;
   UCHAR  UseFullScreenLists;
   UCHAR  UseFullScreenAreaLists;

   UCHAR  AreafixActive;
   UCHAR  AllowRescan;
   UCHAR  CheckZones;
   UCHAR  RaidActive;
   CHAR   AreafixNames[64];
   CHAR   AreafixHelp[64];
   CHAR   RaidNames[64];
   CHAR   RaidHelp[64];
   CHAR   NewTicPath[64];           // Path for new TIC file areas

   UCHAR  TextPasswords;            // Uses text-mode (encrypted) password
} CONFIG;

/////////////////////////////////////////////////////////////////////////
// Line-specific configuration options (channel.dat)                   //
/////////////////////////////////////////////////////////////////////////

typedef struct {
   USHORT TaskNumber;
   CHAR   Device[32];               // Communication Device (COM2, COM3, etc.)
   ULONG  Speed;                    // DTE Maximum Speed
   USHORT LockSpeed;
   CHAR   Initialize[3][48];        // Initialization Strings
   CHAR   Answer[48];               // Command to Answer an Incoming Call
   CHAR   Dial[48];                 // Command to Dial to Another Modem
   CHAR   Hangup[48];               // Command to Hangup the Line
   CHAR   OffHook[48];              // Command to Bring the Modem OffHook
   USHORT DialTimeout;
   USHORT CarrierDropTimeout;
   USHORT StripDashes;
   CHAR   FaxMessage[48];
   CHAR   FaxCommand[64];
   CHAR   SchedulerFile[64];
   CHAR   MainMenu[32];
   CHAR   Ring[32];
   UCHAR  ManualAnswer;
   UCHAR  LimitedHours;
   USHORT StartTime;
   USHORT EndTime;
   CHAR   CallIf[64];
   CHAR   DontCallIf[64];
} CHANNEL;

/////////////////////////////////////////////////////////////////////////
// FidoNet nodes definitions (nodes.dat)                               //
/////////////////////////////////////////////////////////////////////////

typedef struct {
   CHAR   Address[64];              // Node's address
   CHAR   SystemName[64];           // System name
   CHAR   SysopName[48];            // Sysop name
   CHAR   Location[48];             // Location of this node (city/state)
   ULONG  Speed;                    // Maximum connection speed
   ULONG  MinSpeed;                 // Minimum connection speed allowed
   CHAR   Phone[48];                // Phone number
   CHAR   Flags[48];                // Nodelist-type flags
   CHAR   DialCmd[32];              // Special dialing command for this node
   CHAR   SessionPwd[32];           // Mail session password
   CHAR   AreaMgrPwd[32];           // AreaManager (Echomail and Tic) password
   CHAR   OutPktPwd[9];             // Outbound packet password
   CHAR   InPktPwd[9];              // Inbound packet password
   CHAR   TicPwd[32];               // Password for TIC maintenance
   UCHAR  RemapMail;                // TRUE=Remap mail directed to SysopName
   UCHAR  UsePkt22;                 // TRUE=Use mail packet type 2.2
   UCHAR  CreateNewAreas;           // TRUE=Can create new echomail areas
   CHAR   NewAreasFilter[128];      // Filter for new echomail areas
   CHAR   Packer[16];               // Compresso to use for this node
   UCHAR  ImportPOP3Mail;           // TRUE=Import e-mail from POP3 server
   UCHAR  UseInetAddress;           // TRUE=Use the personal Inet address
   CHAR   InetAddress[64];          // Personal E-Mail address
   CHAR   Pop3Pwd[32];              // Password for POP3 mail server
   USHORT Level;                    // Access level for echomail areas
   ULONG  AccessFlags;              // Access flags for echomail areas
   ULONG  DenyFlags;                // Deny flags for echomail areas
   USHORT TicLevel;                 // Access level for TIC areas
   ULONG  TicAccessFlags;           // Access flags for TIC areas
   ULONG  TicDenyFlags;             // Deny flags for TIC areas
   UCHAR  LinkNewEcho;              // TRUE=Link to new echo areas
   UCHAR  EchoMaint;                // TRUE=Can do remote echomail maintenance
   UCHAR  ChangeEchoTag;            // TRUE=Can change echomail tags
   UCHAR  NotifyAreafix;            // TRUE=Notify of echomail changes
   UCHAR  CreateNewTic;             // TRUE=Can create new TIC areas
   UCHAR  LinkNewTic;               // TRUE=Link to new TIC areas
   UCHAR  TicMaint;                 // TRUE=Can do remote TIC maintenance
   UCHAR  ChangeTicTag;             // TRUE=Can change TIC tags
   UCHAR  NotifyRaid;               // TRUE=Notify of TIC changes
   CHAR   MailerAka[48];
   CHAR   EchoAka[48];
   CHAR   TicAka[48];
   CHAR   NewTicFilter[128];        // Filter for new Tic areas
} NODES;

/////////////////////////////////////////////////////////////////////////
// Compressors definition (packer.dat)                                 //
/////////////////////////////////////////////////////////////////////////

#define OS_DOS             0x0001
#define OS_OS2             0x0002
#define OS_WINDOWS         0x0004
#define OS_LINUX           0x0008

typedef struct {
   CHAR   Key[16];
   CHAR   Display[32];
   CHAR   PackCmd[128];
   CHAR   UnpackCmd[128];
   CHAR   Id[32];
   LONG   Position;
   USHORT OS;
} PACKER;

/////////////////////////////////////////////////////////////////////////
// Event scheduler records (events.dat)                                //
/////////////////////////////////////////////////////////////////////////

#define DAY_SUNDAY         0x01
#define DAY_MONDAY         0x02
#define DAY_TUESDAY        0x04
#define DAY_WEDNESDAY      0x08
#define DAY_THURSDAY       0x10
#define DAY_FRIDAY         0x20
#define DAY_SATURDAY       0x40

typedef struct {
   CHAR   Label[32];                // Reference label
   UCHAR  Hour;
   UCHAR  Minute;
   USHORT WeekDays;
   USHORT Length;                   // Duration of this event (if applicable)
   USHORT LastDay;
   UCHAR  Dynamic;
   UCHAR  Force;                    // Force execution of this event
   UCHAR  MailOnly;
   UCHAR  ForceCall;                // TRUE=Force a call to the node specified
   CHAR   Address[32];              // Fidonet address to call
   UCHAR  SendNormal;
   UCHAR  SendCrash;
   UCHAR  SendDirect;
   UCHAR  SendImmediate;
   USHORT CallDelay;
   UCHAR  StartImport;
   UCHAR  StartExport;
   UCHAR  ExportMail;
   UCHAR  ImportNormal;
   UCHAR  ImportKnown;
   UCHAR  ImportProtected;
   CHAR   RouteCmd[64];
   CHAR   Command[128];
   USHORT MaxCalls;
   USHORT MaxConnects;
   UCHAR  AllowRequests;
   UCHAR  MakeRequests;
   UCHAR  ProcessTIC;
   UCHAR  ClockAdjustment;
   UCHAR  Completed;                // If dynamic, tells that the event has been completed
   UCHAR  Dummy[127];
} EVENT;

/////////////////////////////////////////////////////////////////////////
// External file transfer protocols (protocol.dat)                     //
/////////////////////////////////////////////////////////////////////////

typedef struct {
   USHORT Size;
   CHAR   Key[16];
   CHAR   Description[64];
   UCHAR  Active;
   UCHAR  Batch;
   UCHAR  DisablePort;
   UCHAR  ChangeToUploadPath;
   CHAR   DownloadCmd[64];
   CHAR   UploadCmd[64];
   CHAR   LogFileName[64];
   CHAR   CtlFileName[64];
   CHAR   DownloadCtlString[32];
   CHAR   UploadCtlString[32];
   CHAR   DownloadKeyword[32];
   CHAR   UploadKeyword[32];
   USHORT FileNamePos;
   USHORT SizePos;
   USHORT CpsPos;
} PROTOCOL;

/////////////////////////////////////////////////////////////////////////
// Nodelist flags mapping for dialing (nodeflag.dat)                   //
/////////////////////////////////////////////////////////////////////////

typedef struct {
   USHORT Size;                     // Size of this structure
   CHAR   Flags[64];                // Flags handled by this entry (comma-separated)
   CHAR   Cmd[64];                  // Dialing command
} NODEFLAGS;

/////////////////////////////////////////////////////////////////////////
// Translation table structure (cost.dat)                              //
/////////////////////////////////////////////////////////////////////////

#define DAY_SUNDAY         0x01
#define DAY_MONDAY         0x02
#define DAY_TUESDAY        0x04
#define DAY_WEDNESDAY      0x08
#define DAY_THURSDAY       0x10
#define DAY_FRIDAY         0x20
#define DAY_SATURDAY       0x40

#define MAXCOST            7

typedef struct {
   USHORT Size;                     // Size of this structure
   CHAR   Location[48];             // Name of the location this data is referring to
   CHAR   Search[32];               // Phone number prefix to translate
   CHAR   Traslate[64];             // Translated prefix
   struct {
      USHORT Days;                  // Week days
      USHORT Start;                 // Starting time
      USHORT Stop;                  // Ending time
      USHORT CostFirst;             // Cost for TimeFirst seconds of conversation
      USHORT TimeFirst;             // Seconds for CostFirst
      USHORT Cost;                  // Cost for Time seconds of conversation or fraction
      USHORT Time;                  // Time for Cost
   } Cost[MAXCOST];
} TRANSLATION;

/////////////////////////////////////////////////////////////////////////
// User limits structure (limits.dat)                                  //
/////////////////////////////////////////////////////////////////////////

typedef struct {
   USHORT Size;                     // Size of this structure
   CHAR   Key[16];                  // Name of the user-limit
   CHAR   Description[32];          // Description
   USHORT Level;                    // Numeric level
   ULONG  Flags;                    // Access flags
   ULONG  DenyFlags;                // Deny flags
   USHORT CallTimeLimit;            // Time limit (in minutes) per call
   USHORT DayTimeLimit;             // Time limit (in minutes) per day
   USHORT DownloadLimit;            // Download limit (in KB) per day
   USHORT DownloadAt2400;           // Download limit (in KB) per day, speed up to 2400 bps
   USHORT DownloadAt9600;           // Download limit (in KB) per day, speed up to 9600 bps
   USHORT DownloadAt14400;          // Download limit (in KB) per day, speed up to 14400 bps
   USHORT DownloadAt28800;          // Download limit (in KB) per day, speed up to 28800 bps
   USHORT DownloadAt33600;          // Download limit (in KB) per day, speed up to 33600 bps
   USHORT DownloadRatio;            // Maximum Download/Upload ratio
   USHORT RatioStart;               // Ratio start after RatioStart KB of downloads
   ULONG  DownloadSpeed;            // Minimum speed at which download is possible
   CHAR   FreeSpace[64];            // Space for future extensions
} LIMITS;

#endif

