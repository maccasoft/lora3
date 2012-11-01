
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.19
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _TOOLS_H
#define _TOOLS_H

typedef struct _lData {
   struct _lData  *Previous;
   struct _lData  *Next;
   PVOID           Value;
   CHAR            Data[1];
} LDATA;

class DLL_EXPORT TCollection
{
public:
   TCollection (void);
   ~TCollection (void);

   USHORT Elements;

   USHORT Add (PVOID lpData);
   USHORT Add (PVOID lpData, USHORT usSize);
   VOID   Clear (VOID);
   PVOID  First (VOID);
   USHORT Insert (PVOID lpData);
   USHORT Insert (PVOID lpData, USHORT usSize);
   PVOID  Last (VOID);
   PVOID  Next (VOID);
   PVOID  Previous (VOID);
   VOID   Remove (VOID);
   USHORT Replace (PVOID lpData);
   USHORT Replace (PVOID lpData, USHORT usSize);
   PVOID  Value (VOID);

private:
   LDATA *List;
};

class DLL_EXPORT TAddress
{
public:
   TAddress (void);
   ~TAddress (void);

   USHORT Zone, Net, Node, Point;
   CHAR   Domain[32];
   CHAR   String[64];

   SHORT  Add (VOID);
   SHORT  Add (PSZ pszAddress);
   SHORT  Add (USHORT usZone, USHORT usNet, USHORT usNode, USHORT usPoint = 0, PSZ pszDomain = "");
   VOID   Clear (VOID);
   SHORT  First (VOID);
   SHORT  Next (VOID);
   VOID   Parse (PSZ pszAddress);

private:
   class  TCollection List;
};

class DLL_EXPORT TCrc
{
public:
    USHORT Crc16 (UCHAR ucByte, USHORT usCrc);
    ULONG  Crc32 (UCHAR ucByte, ULONG ulCrc);
    USHORT StringCrc16 (CHAR *pszString, USHORT usCrc = 0);
    ULONG  StringCrc32 (CHAR *pszString, ULONG ulCrc = 0xFFFFFFFFL);
};

class DLL_EXPORT TLanguage
{
public:
   TLanguage (void);
   TLanguage (PSZ pszLanguage);
   ~TLanguage (void);

   // Sezione generica
   CHAR   Yes, No, Help, NonStopKey, QuitKey, ContinueKey;
   PSZ    DefYesNo, YesDefNo, DefYesNoHelp, YesDefNoHelp;
   PSZ    Months[12];
   PSZ    MoreQuestion, DeleteMoreQuestion;

   // Sezione posta elettronica (E-Mail)
   CHAR   MailFromYouKey, MailToYouKey, WhichMailHelpKey;
   PSZ    ReadMailMenu;

   // Sezione aree messaggi
   CHAR   FirstMessageKey, LastMessageKey, MailStartHelpKey;
   CHAR   NextMessageKey, PreviousMessageKey;
   CHAR   ExitKey, ReplyKey, RereadKey, EraseKey, ForwardKey, CopyKey;
   CHAR   ReadMessageKey;
   PSZ    ReadMessageMenu, EndOfMessagesMenu, ReadWhichMail;
   PSZ    ThreadMenu, NoMessagesInMailBox, NoMessagesFromYou, StartMessageNumber;
   PSZ    StartNewMessages, StartPrompt, EndOfMessages, StartOfMessages;
   PSZ    MessageHdr, MessageFrom, MessageTo, MessageSubject, MessageFooter;
   PSZ    MessageText, MessageQuote, MessageKludge, ReadThisMessage;
   PSZ    MessageToErase, ConfirmedErase, CantErase, EnterPreference;
   PSZ    ModifyNotice, ModifyNumber, MessageOutOfRange, CantModify, EditorMenu;
   PSZ    MessageList, MessageListHeader, MessageNotAvailable, MessageSelect;
   PSZ    NoMessagesInArea;
   // Sezione aree files
   // Sezione login
   PSZ    AskAnsi, AskRealName, AskSex, City, Country, EnterCompanyName;
   PSZ    NameNotFound, PhoneNumber, PleaseReenter, SelectPassword;
   PSZ    StreetAddress, Wrong, WrongPassword;
   // Sezione editor di linea
   PSZ    WhoToSend, NoSuchName, TypeMessageNow, ContinueEntering, InsertAfterLine;
   PSZ    LineOutOfRange, ConfirmedCopy, ConfirmedSend, SendToAnother, SendToWho;
   PSZ    TextList, ChangeLine, CurrentLineReads, TextToChange, NewText;
   PSZ    LineNowReads, EnterSubject, DeleteLine, OkToDelete, RetypeLine;
   PSZ    EnterNewLine, EditorExited, MailWhoToSend, ToAll;
   // Sezione logoff
   PSZ    LogoffWarning, LogoffConfirm;

   PSZ    CheckCharItem (PSZ pszItem);
   PSZ    *CheckStringItem (PSZ pszItem);
   VOID   Default (VOID);
   USHORT Read (PSZ pszLanguage);
   PSZ    TranslateIn (PSZ pszText);
   PSZ    TranslateOut (PSZ pszText);
   USHORT Write (PSZ pszLanguage);

   PSZ  AvailableFileAreas;
   PSZ  AvailableForums;
   PSZ  AvailableMsgAreas;
   PSZ  EnterName;
   PSZ  IsCorrect;
   PSZ  Password;
   PSZ  PleaseAnswer2;
   PSZ  PleaseAnswer3;

   PSZ  ForumList;
   PSZ  ForumListHeader;
   PSZ  ForumNotAvailable;
   PSZ  ForumSelect;
   PSZ  ListKey;

private:
   PSZ    Buffer;
   PSZ    Translate;
};

#define LOG_PLUS        0x01
#define LOG_EXCLAM      0x02
#define LOG_COLON       0x04
#define LOG_STAR        0x08
#define LOG_BLANK       0x10

class DLL_EXPORT TLog
{
public:
   TLog (void);
   ~TLog (void);

   USHORT Display;
   USHORT Level;
   CHAR   Id[16];

   PSZ    Begin, BrainLapsed, Dialing, IsCalling, GivenLevel;
   PSZ    MenuError, MenuNotFound, ModemResponse, NotInList;
   PSZ    ProcessingNode, SeriousError;
   PSZ    UserOffline, End;

   VOID   Close (VOID);
   VOID   Update (VOID);
   USHORT Open (PSZ pszName);
   VOID   Write (PSZ pszFormat, ...);
   VOID   WriteBlank (VOID);

private:
   FILE   *fp;
   short  whLog;
   PSZ    Months[12];
   CHAR   Buffer[512], Temp[512];
   class  TCollection Text;
};

class DLL_EXPORT TTimer
{
public:
   VOID Pause (LONG lHund);
   LONG TimerSet (LONG lHund);
   LONG TimeUp (LONG lEndtime);
};

#endif

