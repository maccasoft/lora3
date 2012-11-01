
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/02/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "collect.h"

#pragma pack(1)
typedef struct {
   USHORT Size;                  // Dimensioni della struttura
   CHAR   MenuName[32];          // Nome del menu'
   USHORT Items;                 // Numero di elementi presenti
   UCHAR  Color;                 // Colore del testo normale
   UCHAR  Hilight;               // Colore del testo evidenziato
   CHAR   Prompt[128];           // Stringa da usare come prompt
} HEADER;

#pragma pack(1)
typedef struct {
   USHORT Size;                  // Dimensioni della struttura
   CHAR   Display[128];          // Testo da visualizzare all'utente
   UCHAR  Color;                 // Colore del testo normale
   UCHAR  Hilight;               // Colore del testo evidenziato
   CHAR   Key[16];               // Comando per attivare l'opzione
   USHORT Command;               // Tipo di comando da eseguire (vedi CMD_???)
   CHAR   Argument[128];         // Argomenti al comando
   USHORT Level;                 // Livello di accesso minimo
   ULONG  AccessFlags;           // Flag di accesso
   ULONG  DenyFlags;             // Flag di impedimento all'accesso
   UCHAR  Automatic;
   UCHAR  FirstTime;
} ITEM;

#define MNU_NULL                 0
#define MNU_LOGOFF               1
#define MNU_GOTO                 2
#define MNU_GOSUB                3
#define MNU_RETURN               4
#define MNU_ONLINEUSERS          5
#define MNU_DISPLAY              6
#define MNU_SETLANGUAGE          7
#define MNU_SETPASSWORD          8
#define MNU_RUNEXTERNAL          10
#define MNU_FILEDOWNLOADANY      11
#define MNU_MSGSELECT            13
#define MNU_FILESELECT           14
#define MNU_FILENAMELIST         15
#define MNU_FILEDOWNLOAD         16
#define MNU_FILEUPLOAD           17
#define MNU_FILEDOWNLOADLIST     18
#define MNU_FILEDATELIST         19
#define MNU_FILENEWLIST          20
#define MNU_FILEDELETE           21
#define MNU_FILEKEYWORDLIST      22
#define MNU_FILETEXTLIST         23
#define MNU_SEARCHFILENAME       24
#define MNU_MSGBRIEFLIST         25
#define MNU_MSGTITLELIST         26
#define MNU_MSGREAD              27
#define MNU_MSGWRITE             28
#define MNU_MSGMODIFY            29
#define MNU_MSGUNRECEIVE         30
#define MNU_MSGREADNONSTOP       31
#define MNU_MSGDELETE            32
#define MNU_MSGENTER             33
#define MNU_MSGREPLY             34
#define MNU_MSGINDIVIDUAL        35
#define MNU_MSGLISTNEWAREAS      36
#define MNU_PRESSENTER           37
#define MNU_VERSION              38
#define MNU_MSGFORWARD           39
#define MNU_MSGREADORIGINAL      40
#define MNU_MSGREADREPLY         41
#define MNU_TOGGLEFULLED         42
#define MNU_TOGGLEFULLREAD       43
#define MNU_TOGGLENODISTURB      44
#define MNU_TOGGLEMAILCHECK      45
#define MNU_TOGGLEFILECHECK      46
#define MNU_SETBIRTHDATE         47
#define MNU_TELNET               50
#define MNU_FINGER               51
#define MNU_FTP                  52
#define MNU_GOPHER               53
#define MNU_OLRDOWNLOADASCII     60
#define MNU_OLRDOWNLOADBW        61
#define MNU_OLRDOWNLOADQWK       62
#define MNU_OLRTAGAREA           63
#define MNU_OLRREMOVEAREA        64
#define MNU_OLRVIEWTAGGED        65
#define MNU_OLRUPLOAD            66
#define MNU_OLRDOWNLOADPNT       67
#define MNU_OLRRESTRICTDATE      68
#define MNU_INQUIREPERSONAL      70
#define MNU_INQUIRETEXT          71
#define MNU_MAILREAD             80
#define MNU_MAILDELETE           81
#define MNU_MAILWRITE            82
#define MNU_MAILLIST             83
#define MNU_MSGBACKWARD          84
#define MNU_CLEARSTACK           85
#define MNU_CLEARGOSUB           86
#define MNU_CLEARGOTO            87
#define MNU_RETURNMAIN           88
#define MNU_TOGGLECOLOR          89
#define MNU_TOGGLEANSI           90
#define MNU_TOGGLEAVATAR         91
#define MNU_TOGGLEHOTKEY         92
#define MNU_MAILCHECK            93
#define MNU_SETCOMPANY           94
#define MNU_SETADDRESS           95
#define MNU_SETCITY              96
#define MNU_SETPHONE             97
#define MNU_SETGENDER            98
#define MNU_FILEUPLOADUSER       99
#define MNU_FILEDISPLAY          100
#define MNU_MSGREADCURRENT       101
#define MNU_TOGGLEKLUDGES        102
#define MNU_ADDTAGGED            103
#define MNU_LISTTAGGED           104
#define MNU_DELETETAGGED         105
#define MNU_DELETEALLTAGGED      106
#define MNU_TOGGLEFULLSCREEN     107
#define MNU_TOGGLEIBMCHARS       108
#define MNU_TOGGLEMOREPROMPT     109
#define MNU_TOGGLESCREENCLEAR    110
#define MNU_TOGGLEINUSERLIST     111
#define MNU_SETARCHIVER          112
#define MNU_SETPROTOCOL          113
#define MNU_SETSIGNATURE         114
#define MNU_SETVIDEOMODE         115

class DLL_EXPORT TMenu
{
public:
   TMenu (void);
   ~TMenu (void);

   CHAR   Path[64], Name[32];
   CHAR   AltPath[64];

   CHAR   Prompt[128];
   UCHAR  PromptColor, PromptHilight;

   USHORT Command, Level;
   CHAR   Display[128], Key[16], Argument[128];
   UCHAR  Color, Hilight;
   ULONG  AccessFlags, DenyFlags;
   UCHAR  Automatic, FirstTime;

   VOID   Add (VOID);
   USHORT Check (PSZ pszKey);
   VOID   Delete (VOID);
   USHORT First (VOID);
   VOID   Insert (VOID);
   USHORT Load (PSZ pszName);
   VOID   New (USHORT usNewPrompt = FALSE);
   USHORT Next (VOID);
   USHORT Previous (VOID);
   USHORT Save (PSZ pszName);
   VOID   Update (VOID);

private:
   HEADER Hdr;
   ITEM   Item;
   class  TCollection Items;
};
