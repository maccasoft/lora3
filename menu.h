
// --------------------------------------------------------------------
// LoraBBS Professional Edition - Version 0.14
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History list:
//    23/04/95 - Initial coding
// --------------------------------------------------------------------

#ifndef _MENU_H
#define _MENU_H

#include "bbs.h"
#include "editor.h"
#include "filebase.h"
#include "files.h"
#include "forum.h"
#include "ftrans.h"
#include "internet.h"
#include "msgbase.h"
#include "msgdata.h"
#include "offline.h"
#include "tools.h"

class DLL_EXPORT TMessage
{
public:
   TMessage (class TBbs *bbs);
   ~TMessage (void);

   USHORT ReadForward, ShowKludges;
   CHAR   From[64], To[64], Subject[128];
   class  MsgBase   *Msg;
   class  TMsgData  *Current;
   class  TLastRead *LastRead;

   VOID   AddOfflineArea (VOID);
   VOID   BriefList (VOID);
   VOID   DeleteMessage (VOID);
   ULONG  GotoNext (class MsgBase *Msg, ULONG ulMsg);
   ULONG  GotoPrevious (class MsgBase *Msg, ULONG ulMsg);
   VOID   ListMessages (VOID);
   VOID   Read (ULONG ulNumber, USHORT fCls);
   VOID   ReadCurrent (USHORT fCls);
   VOID   ReadMessages (VOID);
   VOID   ReadNext (VOID);
   VOID   ReadNonStop (VOID);
   VOID   ReadPrevious (VOID);
   USHORT SelectArea (VOID);
   VOID   SelectNext (VOID);
   VOID   SelectPrevious (VOID);
   VOID   StartMessageQuestion (ULONG ulFirst, ULONG ulLast, USHORT fNewMessages, ULONG &ulMsg, USHORT &fForward);
   VOID   TextListMessages (VOID);
   VOID   TitleListMessages (VOID);

private:
   class  TBbs      *Bbs;
   class  TConfig   *Cfg;
   class  TLanguage *Lang;
   class  TLog      *Log;
   class  TUser     *User;
};


class TMsgEditor : public TEditor
{
public:
   TMsgEditor (class TBbs *bbs);
   ~TMsgEditor (void);

   class  MsgBase *Msg;

   VOID   Forward (VOID);
   VOID   InputSubject (VOID);
   VOID   InputTo (VOID);
   USHORT Modify (VOID);
   USHORT Reply (VOID);
   VOID   Save (VOID);
   USHORT Write (VOID);

private:
   CHAR   To[36];
   CHAR   Subject[72];
   ULONG  Number;

   class  TLog *Log;
};

class DLL_EXPORT TLibrary
{
public:
   TLibrary (class TBbs *bbs);
   ~TLibrary (void);

   class  FileData *Current;

   VOID   AddFiles (VOID);
   VOID   Download (class TFileTag *File = 0);
   USHORT DownloadFile (PSZ pszFile, PSZ pszName, ULONG ulSize);
   VOID   DownloadList (VOID);
   VOID   EditFileData (VOID);
   VOID   FileDetails (class TFile *Data);
   VOID   ListFiles (class TFile *Data = 0);
   VOID   ListDownloadedFiles (VOID);
   VOID   ListRecentFiles (VOID);
   VOID   ManageTagged (VOID);
   VOID   RemoveFiles (VOID);
   VOID   SearchKeyword (VOID);
   VOID   SearchNewFiles (VOID);
   VOID   SearchText (VOID);
   USHORT SelectArea (VOID);
   VOID   SelectNext (VOID);
   VOID   SelectPrevious (VOID);
   VOID   Upload (VOID);

private:
   class  TBbs      *Bbs;
   class  TConfig   *Cfg;
   class  TLanguage *Lang;
   class  TLog      *Log;
   class  TUser     *User;
};

#define MNU_NULL              0
#define MNU_LOGOFF            1
#define MNU_GOTO              2
#define MNU_GOSUB             3
#define MNU_RETURN            4
#define MNU_RUN               5
#define MNU_DISPLAY           6
#define MNU_DOWNLOAD          7
#define MNU_UPLOAD            8
#define MNU_FILETAG           9
#define MNU_TELNET            10
#define MNU_FINGER            11
#define MNU_FTP               12
#define MNU_MSGAREA           13
#define MNU_FILEAREA          14
#define MNU_NEWFILES          15
#define MNU_FILETITLES        16
#define MNU_DOWNLOADQWK       17
#define MNU_DOWNLOADBW        18
#define MNU_MSGTAG            19
#define MNU_READNEXT          20
#define MNU_READPREVIOUS      21
#define MNU_READCURRENT       22
#define MNU_SAMEDIRECTION     23
#define MNU_READNONSTOP       24
#define MNU_MSGLIST           25
#define MNU_EDITLIST          26
#define MNU_EDITEDIT          27
#define MNU_EDITINSERT        28
#define MNU_EDITDELETE        29
#define MNU_EDITCONTINUE      30
#define MNU_MSGENTER          31
#define MNU_EDITSAVE          32
#define MNU_EDITABORT         33
#define MNU_EDITTO            34
#define MNU_EDITSUBJECT       35
#define MNU_MAILREAD          36
#define MNU_PRESSENTER        37
#define MNU_VERSION           38
#define MNU_MSGFORWARD        39
#define MNU_MSGINDIVIDUAL     40
#define MNU_READMSG           41
#define MNU_MSGVIEWTAG        42
#define MNU_WHOISON           43
#define MNU_MSGKLUDGES        44
#define MNU_MSGNEXTAREA       45
#define MNU_MSGPREVAREA       46
#define MNU_FILENEXTAREA      47
#define MNU_FILEPREVAREA      48
#define MNU_CHGPASSWORD       49
#define MNU_CHGMORE           50
#define MNU_CHGALIAS          51
#define MNU_CHGPHONE          52
#define MNU_CHGRIP            53
#define MNU_CHGHOTKEYS        54
#define MNU_CHGVIDEO          55
#define MNU_CHGLENGTH         56
#define MNU_FILEKILL          57
#define MNU_MSGDELETE         58
#define MNU_FILENAMELOCATE    59

typedef struct {
   USHORT cbSize;                // Dimensioni della struttura
   CHAR   szMenuName[32];        // Nome del menu'
   USHORT usItems;               // Numero di elementi presenti
   UCHAR  ucColor;               // Colore del testo normale
   UCHAR  ucHilight;             // Colore del testo evidenziato
   CHAR   szPrompt[128];
} MENUHDR;

#define MF_DSPECHOMAIL           0x0001
#define MF_DSPNETMAIL            0x0002
#define MF_DSPLOCAL              0x0004
#define MF_DSPRIP                0x0008
#define MF_DSPNORIP              0x0010
#define MF_AUTOEXEC              0x0020
#define MF_DSPUSRREMOTE          0x0040
#define MF_DSPUSRLOCAL           0x0080
#define MF_ONENTER               0x0100

typedef struct {
   USHORT cbSize;                // Dimensioni della struttura
   CHAR   szDisplay[128];        // Testo da visualizzare all'utente
   UCHAR  ucColor;               // Colore del testo normale
   UCHAR  ucHilight;             // Colore del testo evidenziato
   CHAR   szKey[16];             // Comando per attivare l'opzione
   USHORT usCommand;             // Tipo di comando da eseguire (vedi CMD_???)
   CHAR   szArgument[128];       // Argomenti al comando
   USHORT usLevel;               // Livello di accesso minimo
   ULONG  ulAccessFlags;         // Flag di accesso
   ULONG  ulDenyFlags;           // Flag di impedimento all'accesso
   USHORT Flags;                 // Flags di visualizzazione
} ITEM;

#define MAX_STACK           16

#define MENU_OK             0
#define MENU_RETURN         1
#define MENU_LOGOFF         2

class DLL_EXPORT TMenu
{
public:
   TMenu (class TBbs *bbs);
   virtual ~TMenu (void);

   class  TBbs      *Bbs;
   class  TCom      *Com;
   class  TCom      *Snoop;
   class  TConfig   *Cfg;
   class  TLanguage *Lang;
   class  TLog      *Log;
   class  TUser     *User;

   virtual VOID   DisplayPrompt (PSZ pszDisplay, UCHAR ucColor, UCHAR ucHilight);
   virtual USHORT ExecuteOption (ITEM *pItem);
   virtual USHORT Run (PSZ pszName);

protected:
   MENUHDR menuhdr;
   ITEM    *item;

   USHORT usDoRead;                       // Flag di lettura del file menu'
   CHAR   szMenu[32];                     // Nome del menu' corrente
   CHAR   szMenuStack[MAX_STACK][32];     // Stack LIFO dei menu'
   USHORT usStackPos;                     // Puntatore allo stack dei menu'
   CHAR   szCommand[128];                 // Linea di comando
   USHORT usFullMenu;                     // TRUE=Visualizza tutto il menu'

   class  TEmbedded  *PS;
   class  TLibrary   *Library;
   class  TMessage   *Message;
   class  TMsgEditor *Editor;

   VOID   DisplayItem (ITEM *pItem);
   VOID   GetCommand (VOID);

   VOID   Gosub (PSZ pszArgument);
   VOID   Goto (PSZ pszArgument);
   USHORT Logoff (VOID);
   VOID   OnlineUsers (VOID);
   USHORT Return (VOID);
   VOID   RunProgram (PSZ pszArgument);
};

class TMenuEmbedded : public TEmbedded
{
public:
   TMenuEmbedded (class TBbs *pBbs);
   ~TMenuEmbedded (void);

   class  TMessage *Message;
   class  TLibrary *Library;

   VOID   BasicControl (UCHAR ucControl);

private:
   class  TLanguage *Lang;
};

class DLL_EXPORT TEMail
{
public:
   TEMail (class TBbs *bbs);
   ~TEMail (void);

   CHAR   Path[128];
   USHORT Storage;

   USHORT Check (VOID);
   VOID   DeleteMessage (VOID);
   ULONG  GotoNext (class MsgBase *Msg, ULONG ulMsg);
   ULONG  GotoPrevious (class MsgBase *Msg, ULONG ulMsg);
   VOID   ReadMessages (VOID);

private:
   class  TBbs      *Bbs;
   class  TConfig   *Cfg;
   class  TLanguage *Lang;
   class  TLog      *Log;
   class  TUser     *User;
};

class DLL_EXPORT TForum : public TMenu
{
public:
   TForum (class TBbs *bbs);
   ~TForum (void);

   USHORT ExecuteOption (ITEM *pItem);
   USHORT Run (PSZ pszName);

   USHORT SelectArea (VOID);
   USHORT SelectLibrary (VOID);

private:
   class  TBbs      *Bbs;
   class  TConfig   *Cfg;
   class  TLanguage *Lang;
   class  TLog      *Log;
   class  TUser     *User;

   class  ForumData Current;
   class  TMsgData  Msg;
   class  FileData  File;
};

class TMailEditor : public TMenu
{
public:
   TMailEditor (class TBbs *bbs);
   ~TMailEditor (void);

   CHAR   To[36];
   CHAR   Subject[72];
   class  MsgBase *Msg;

   USHORT ExecuteOption (ITEM *pItem);
   USHORT Run (PSZ pszName);

   USHORT Copy (USHORT flCheckName = FALSE);
   USHORT Forward (USHORT flCheckName = FALSE);
   VOID   InputSubject (VOID);
   USHORT Modify (VOID);
   USHORT Reply (USHORT flCheckName = FALSE);
   VOID   Save (VOID);
   USHORT Write (USHORT flCheckName = FALSE);

private:
   class  TEditor *Editor;
};

#endif

