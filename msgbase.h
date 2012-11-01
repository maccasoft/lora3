
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.5
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _MSGBASE_H
#define _MSGBASE_H

#include "collect.h"
#include "combase.h"
#include "jam.h"

#define MAX_LINE_LENGTH       512

#define ST_JAM                0
#define ST_SQUISH             1
#define ST_USENET             2
#define ST_FIDO               3
#define ST_ADEPT              4
#define ST_HUDSON             5
#define ST_GOLDBASE           6
#define ST_PASSTHROUGH        7

typedef struct {
   UCHAR  Day;
   UCHAR  Month;
   USHORT Year;
   UCHAR  Hour;
   UCHAR  Minute;
   UCHAR  Second;
} MDATE;

class DLL_EXPORT TMsgBase
{
public:
   TMsgBase (void) {};
   virtual ~TMsgBase (void) {};

   ULONG   Id, Current;
   CHAR    From[64];
   CHAR    To[64];
   CHAR    Subject[72];
   UCHAR   Crash, Direct, FileAttach, FileRequest, Hold, Immediate;
   UCHAR   Intransit, KillSent, Local, Private, ReceiptRequest, Received;
   UCHAR   Sent;
   MDATE   Written;
   MDATE   Arrived;
   CHAR    FromAddress[48];
   CHAR    ToAddress[48];
   class   TCollection Text;

   virtual USHORT Add (VOID) = 0;
   virtual USHORT Add (class TMsgBase *MsgBase) = 0;
   virtual USHORT Add (class TCollection &MsgText) = 0;
   virtual VOID   Close (VOID) = 0;
   virtual USHORT Delete (ULONG ulMsg) = 0;
   virtual ULONG  Highest (VOID) = 0;
   virtual USHORT GetHWM (ULONG &ulMsg) = 0;
   virtual USHORT Lock (ULONG ulTimeout = 0) = 0;
   virtual ULONG  Lowest (VOID) = 0;
   virtual ULONG  MsgnToUid (ULONG ulMsg) = 0;
   virtual VOID   New (VOID) = 0;
   virtual USHORT Next (ULONG &ulMsg) = 0;
   virtual ULONG  Number (VOID) = 0;
   virtual VOID   Pack (VOID) = 0;
   virtual USHORT Previous (ULONG &ulMsg) = 0;
   virtual USHORT ReadHeader (ULONG ulMsg) = 0;
   virtual USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79) = 0;
   virtual USHORT Read (ULONG ulMsg, SHORT nWidth = 79) = 0;
   virtual VOID   SetHWM (ULONG ulMsg) = 0;
   virtual ULONG  UidToMsgn (ULONG ulMsg) = 0;
   virtual VOID   UnLock (VOID) = 0;
   virtual USHORT WriteHeader (ULONG ulMsg) = 0;

protected:
   CHAR   szBuff[MAX_LINE_LENGTH + 1];
   CHAR   szLine[MAX_LINE_LENGTH + 1];
   CHAR   szWrp[MAX_LINE_LENGTH + 1];
   CHAR   *pLine, *pBuff;
};

class DLL_EXPORT JAM : public TMsgBase
{
public:
   JAM (void);
   JAM (PSZ pszName);
   ~JAM (void);

   USHORT Add (VOID);
   USHORT Add (class TMsgBase *MsgBase);
   USHORT Add (class TCollection &MsgText);
   VOID   Close (VOID);
   USHORT Delete (ULONG ulMsg);
   USHORT GetHWM (ULONG &ulMsg);
   ULONG  Highest (VOID);
   USHORT Lock (ULONG ulTimeout = 0);
   ULONG  Lowest (VOID);
   ULONG  MsgnToUid (ULONG ulMsg);
   VOID   New (VOID);
   USHORT Next (ULONG &ulMsg);
   ULONG  Number (VOID);
   USHORT Open (PSZ pszName);
   VOID   Pack (VOID);
   USHORT Previous (ULONG &ulMsg);
   USHORT ReadHeader (ULONG ulMsg);
   USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79);
   USHORT Read (ULONG ulMsg, SHORT nWidth = 79);
   VOID   SetHWM (ULONG ulMsg);
   ULONG  UidToMsgn (ULONG ulMsg);
   VOID   UnLock (VOID);
   USHORT WriteHeader (ULONG ulMsg);

private:
   INT    fdHdr;
   INT    fdJdt;
   INT    fdJdx;
   UCHAR *pSubfield;
   CHAR   BaseName[128];
   JAMHDRINFO jamHdrInfo;
   JAMHDR     jamHdr;
};

// --------------------------------------------------------------------------

typedef struct {
   USHORT Len;                // LENGTH OF THIS STRUCTURE!
   USHORT Rsvd1;              // reserved
   ULONG  NumMsg;             // Number of messages in area
   ULONG  HighMsg;            // Highest msg in area. Same as num_msg
   ULONG  SkipMsg;            // Skip killing first x msgs in area
   ULONG  HighWater;          // Msg# (not umsgid) of HWM
   ULONG  Uid;                // Number of the next UMSGID to use
   CHAR   Base[80];           // Base name of SquishFile
   ULONG  BeginFrame;         // Offset of first frame in file
   ULONG  LastFrame;          // Offset to last frame in file
   ULONG  FreeFrame;          // Offset of first FREE frame in file
   ULONG  LastFreeFrame;      // Offset of last free frame in file
   ULONG  EndFrame;           // Pointer to end of file
   ULONG  MaxMsg;             // Max # of msgs to keep in area
   USHORT KeepDays;           // Max age of msgs in area (SQPack)
   USHORT SzSqhdr;            // sizeof (SQHDR)
   UCHAR  Rsvd2[124];         // Reserved by Squish for future use
} SQBASE;

#define SQHDRID         0xAFAE4453L

#define FRAME_NORMAL    0x00
#define FRAME_FREE      0x01
#define FRAME_RLE       0x02  // not implemented
#define FRAME_LZW       0x03  // not implemented

typedef struct {
   ULONG  Id;                 // sqhdr.id must always equal SQHDRID
   ULONG  NextFrame;
   ULONG  PrevFrame;
   ULONG  FrameLength;
   ULONG  MsgLength;
   ULONG  CLen;
   USHORT FrameType;
   USHORT Rsvd;
} SQHDR;

typedef struct {
   USHORT Zone;
   USHORT Net;
   USHORT Node;
   USHORT Point;
} NADDR;

#define MSGPRIVATE      0x0001
#define MSGCRASH        0x0002
#define MSGREAD         0x0004
#define MSGSENT         0x0008
#define MSGFILE         0x0010
#define MSGFWD          0x0020
#define MSGORPHAN       0x0040
#define MSGKILL         0x0080
#define MSGLOCAL        0x0100
#define MSGHOLD         0x0200
#define MSGXX2          0x0400
#define MSGFRQ          0x0800
#define MSGRRQ          0x1000
#define MSGCPT          0x2000
#define MSGARQ          0x4000
#define MSGURQ          0x8000

#define MSGSCANNED      0x00010000L
#define MSGUID          0x00020000L

#define XMSG_FROM_SIZE  36
#define XMSG_TO_SIZE    36
#define XMSG_SUBJ_SIZE  72

#define MAX_REPLY       9     // Max number of stored replies to one msg

typedef struct {
   ULONG  Attr;
   CHAR   From[XMSG_FROM_SIZE];
   CHAR   To[XMSG_TO_SIZE];
   CHAR   Subject[XMSG_SUBJ_SIZE];
   NADDR  Orig;               // Origination and destination addresses             */
   NADDR  Dest;
   ULONG  DateWritten;        // When user wrote the msg (UTC)
   ULONG  DateArrived;        // When msg arrived on-line (UTC)
   SHORT  UtcOfs;             // Offset from UTC of message writer, in minutes.
   ULONG  ReplyTo;
   ULONG  Replies[MAX_REPLY];
   ULONG  MsgId;
   CHAR   FtscDate[20];       // Obsolete date information
} XMSG;

typedef struct {
   ULONG  Ofs;
   ULONG  MsgId;
   ULONG  Hash;
} SQIDX;

class SQUISH : public TMsgBase
{
public:
   SQUISH (void);
   SQUISH (PSZ pszName);
   ~SQUISH (void);

   USHORT Add (VOID);
   USHORT Add (class TMsgBase *MsgBase);
   USHORT Add (class TCollection &MsgText);
   VOID   Close (VOID);
   USHORT Delete (ULONG ulMsg);
   USHORT GetHWM (ULONG &ulMsg);
   ULONG  Hash (PSZ f);
   ULONG  Highest (VOID);
   USHORT Lock (ULONG ulTimeout = 0);
   ULONG  Lowest (VOID);
   ULONG  MsgnToUid (ULONG ulMsg);
   VOID   New (VOID);
   USHORT Next (ULONG &ulMsg);
   ULONG  Number (VOID);
   USHORT Open (PSZ pszName);
   VOID   Pack (VOID);
   USHORT Previous (ULONG &ulMsg);
   USHORT ReadHeader (ULONG ulMsg);
   USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79);
   USHORT Read (ULONG ulMsg, SHORT nWidth = 79);
   VOID   SetHWM (ULONG ulMsg);
   ULONG  UidToMsgn (ULONG ulMsg);
   VOID   UnLock (VOID);
   USHORT WriteHeader (ULONG ulMsg);

private:
   FILE   *fpDat, *fpIdx;
   USHORT Locked;
   SQBASE SqBase;
   SQIDX  SqIdx, *pSqIdx;
   SQHDR  SqHdr;
   XMSG   XMsg;
};

// --------------------------------------------------------------------------

class DLL_EXPORT USENET : public TMsgBase
{
public:
   USENET (void);
   USENET (PSZ pszServer, PSZ pszGroup);
   ~USENET (void);

   CHAR   HostName[32], Organization[64];
   CHAR   NewsGroup[64], User[32];
   CHAR   ProgramID[32];
   CHAR   Error[128];

   USHORT Add (VOID);
   USHORT Add (class TMsgBase *MsgBase);
   USHORT Add (class TCollection &MsgText);
   VOID   Close (VOID);
   USHORT Delete (ULONG ulMsg);
   ULONG  Highest (VOID);
   USHORT GetHWM (ULONG &ulMsg);
   USHORT Lock (ULONG ulTimeout = 0);
   ULONG  Lowest (VOID);
   ULONG  MsgnToUid (ULONG ulMsg);
   VOID   New (VOID);
   USHORT Next (ULONG &ulMsg);
   ULONG  Number (VOID);
   USHORT Open (PSZ pszServer, PSZ pszGroup);
   VOID   Pack (VOID);
   USHORT Previous (ULONG &ulMsg);
   USHORT ReadHeader (ULONG ulMsg);
   USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79);
   USHORT Read (ULONG ulMsg, SHORT nWidth = 79);
   VOID   SetHWM (ULONG ulMsg);
   ULONG  UidToMsgn (ULONG ulMsg);
   VOID   UnLock (VOID);
   USHORT WriteHeader (ULONG ulMsg);

private:
   CHAR   szBuffer[1024];
   ULONG  ulHighest;
   ULONG  ulFirst;
   ULONG  ulTotal;
   ULONG  LastReaded;
   class  TTcpip *Tcp;

   USHORT GetResponse (PSZ pszResponse, USHORT usMaxLen);
};

// --------------------------------------------------------------------------

typedef struct {
   USHORT OrigNode;
   USHORT DestNode;
   USHORT Year;
   USHORT Month;
   USHORT Day;
   USHORT Hour;
   USHORT Minute;
   USHORT Second;
   USHORT Rate;
   USHORT Version;
   USHORT OrigNet;
   USHORT DestNet;
   UCHAR  ProductL;
   UCHAR  Serial;
   CHAR   Password[8];
   USHORT OrigZone;
   USHORT DestZone;
   USHORT Auxnet;
   USHORT CWValidation;
   UCHAR  ProductH;
   UCHAR  Revision;
   USHORT Capability;
   USHORT OrigZone2;
   USHORT DestZone2;
   USHORT OrigPoint;
   USHORT DestPoint;
   UCHAR  Filler[4];
} PKT2HDR;

typedef struct {
   USHORT OrigNode;
   USHORT DestNode;
   USHORT OrigPoint;
   USHORT DestPoint;
   CHAR   Reserved[8];
   USHORT SubVersion;
   USHORT Version;
   USHORT OrigNet;
   USHORT DestNet;
   UCHAR  Product;
   UCHAR  Serial;
   CHAR   Password[8];
   USHORT OrigZone;
   USHORT DestZone;
   CHAR   OrigDomain[8];
   CHAR   DestDomain[8];
   UCHAR  Filler[4];
} PKT22HDR;

typedef struct {
   USHORT Version;
   USHORT OrigNode;
   USHORT DestNode;
   USHORT OrigNet;
   USHORT DestNet;
   USHORT Attrib;
   USHORT Cost;
} MSGHDR;

class PACKET : public TMsgBase
{
public:
   PACKET (void);
   PACKET (PSZ pszName);
   ~PACKET (void);

   CHAR   Password[16];
   MDATE  Date;

   USHORT Add (VOID);
   USHORT Add (class TMsgBase *MsgBase);
   USHORT Add (class TCollection &MsgText);
   VOID   Close (VOID);
   USHORT Delete (ULONG ulMsg);
   USHORT GetHWM (ULONG &ulMsg);
   ULONG  Highest (VOID);
   VOID   Kill (VOID);
   USHORT Lock (ULONG ulTimeout = 0);
   ULONG  Lowest (VOID);
   ULONG  MsgnToUid (ULONG ulMsg);
   VOID   New (VOID);
   USHORT Next (ULONG &ulMsg);
   ULONG  Number (VOID);
   USHORT Open (PSZ pszName, USHORT doScan = TRUE);
   VOID   Pack (VOID);
   USHORT Previous (ULONG &ulMsg);
   USHORT ReadHeader (ULONG ulMsg);
   USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79);
   USHORT Read (ULONG ulMsg, SHORT nWidth = 79);
   VOID   SetHWM (ULONG ulMsg);
   ULONG  UidToMsgn (ULONG ulMsg);
   VOID   UnLock (VOID);
   USHORT WriteHeader (ULONG ulMsg);

private:
   PKT2HDR  pkt2Hdr;
   PKT22HDR pkt22Hdr;
   MSGHDR   msgHdr;
   FILE   *fp;
   CHAR   LastRead, Line[256];
   CHAR   FileName[128], WasScanned;
   ULONG  TotalMsgs;
   class  TCollection Index;

   USHORT GetLine (VOID);
};

// --------------------------------------------------------------------------

typedef struct {
   CHAR   From[36];
   CHAR   To[36];
   CHAR   Subject[72];
   CHAR   Date[20];
   USHORT TimesRead;
   USHORT DestNode;
   USHORT OrigNode;
   USHORT Cost;
   USHORT OrigNet;
   USHORT DestNet;
   USHORT BinDate[4];
   USHORT Reply;
   USHORT Attrib;
   USHORT Up;
} FIDOMSG;

class FIDOSDM : public TMsgBase
{
public:
   FIDOSDM (void);
   FIDOSDM (PSZ pszName);
   ~FIDOSDM (void);

   USHORT Add (VOID);
   USHORT Add (class TMsgBase *MsgBase);
   USHORT Add (class TCollection &MsgText);
   VOID   Close (VOID);
   USHORT Delete (ULONG ulMsg);
   USHORT GetHWM (ULONG &ulMsg);
   ULONG  Highest (VOID);
   USHORT Lock (ULONG ulTimeout = 0);
   ULONG  Lowest (VOID);
   ULONG  MsgnToUid (ULONG ulMsg);
   VOID   New (VOID);
   USHORT Next (ULONG &ulMsg);
   ULONG  Number (VOID);
   USHORT Open (PSZ pszName);
   VOID   Pack (VOID);
   USHORT Previous (ULONG &ulMsg);
   USHORT ReadHeader (ULONG ulMsg);
   USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79);
   USHORT Read (ULONG ulMsg, SHORT nWidth = 79);
   VOID   SetHWM (ULONG ulMsg);
   ULONG  UidToMsgn (ULONG ulMsg);
   VOID   UnLock (VOID);
   USHORT WriteHeader (ULONG ulMsg);

private:
   FILE   *fp;
   CHAR   BasePath[128];
   CHAR   LastFile[128];
   ULONG  TotalMsgs;
   FIDOMSG msgHdr;
   class  TCollection Index;
};

// --------------------------------------------------------------------------

#define MSGDELETED  0x0001  /* deleted message,         */

typedef struct {
   CHAR    MajorVersion; /* Minor revision number of this message style */
   CHAR    MinorVersion; /* Minor revision number of this message style */
   USHORT  StructLen;    /* The length of this data structure */
   CHAR    from[60];     /* Who the message is from */
   CHAR    to[60];       /* Who the message is to */
   CHAR    subj[70];     /* The subject of the message */
   CHAR    date[35];     /* Date the message was written */
   CHAR    indate[4];    /* Import date */
   ULONG   msgnum;       /* Current message number */
   ULONG   timesread;    /* Number of times the message has been read */
   time_t  timerecv;     /* Time user received this message */
   ULONG   length;       /* Length of message stored in .Text Data file */
   LONG    start;        /* Pointer to starting byte in .Text Data file */
   ULONG   Extra1;       /* Extra space  Was going to be for reply */
   ULONG   Extra2;       /* Extra space  linking instead came up with */
   ULONG   Extra3;       /* Extra space  a better method */
   USHORT  o_zone;       /* Messages origin zone */
   USHORT  o_net;        /* Messages origin net */
   USHORT  o_node;       /* Messages origin node */
   USHORT  o_point;      /* Messages origin point */
   USHORT  d_zone;       /* Messages destination zone */
   USHORT  d_net;        /* Messages destination net */
   USHORT  d_node;       /* Messages destination node  */
   USHORT  d_point;      /* Messages destination point */
   USHORT  cost;         /* Cost to send this message */
   USHORT  fflags;       /* FidoNet related flags */
   USHORT  xflags;       /* XBBS related flags */
   ULONG   iflags;       /* Internet related flags */
   ULONG   oflags;       /* Other network related flags */
} ADEPTDATA, *PADEPTDATA;

typedef struct {
   SHORT to;                /* Checksum of the to field */
   SHORT from;              /* Checksum of the from field */
   SHORT subj;              /* Checksum of the subject field */
   LONG  msgidcrc;          /* 32-bit CRC of the MSGID */
   LONG  msgidserialno;     /* MSGID Serial Number */
   LONG  replycrc;          /* REPLYID 32-bit CRC */
   LONG  replyserialno;     /* REPLYID Serial Numver */
} ADEPTINDEXES, *PADEPTINDEXES;

class ADEPT : public TMsgBase
{
public:
   ADEPT (void);
   ADEPT (PSZ pszName);
   ~ADEPT (void);

   USHORT Add (VOID);
   USHORT Add (class TMsgBase *MsgBase);
   USHORT Add (class TCollection &MsgText);
   VOID   Close (VOID);
   USHORT Delete (ULONG ulMsg);
   USHORT GetHWM (ULONG &ulMsg);
   ULONG  Highest (VOID);
   USHORT Lock (ULONG ulTimeout = 0);
   ULONG  Lowest (VOID);
   ULONG  MsgnToUid (ULONG ulMsg);
   VOID   New (VOID);
   USHORT Next (ULONG &ulMsg);
   ULONG  Number (VOID);
   USHORT Open (PSZ pszName);
   VOID   Pack (VOID);
   USHORT Previous (ULONG &ulMsg);
   USHORT ReadHeader (ULONG ulMsg);
   USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79);
   USHORT Read (ULONG ulMsg, SHORT nWidth = 79);
   VOID   SetHWM (ULONG ulMsg);
   ULONG  UidToMsgn (ULONG ulMsg);
   VOID   UnLock (VOID);
   USHORT WriteHeader (ULONG ulMsg);

private:
   int    fdHdr;
   int    fdIdx;
   int    fdTxt;
   CHAR   BaseName[128];
   ULONG  TotalMsgs;
   ADEPTDATA Data;
};

// --------------------------------------------------------------------------

class PASSTHR : public TMsgBase
{
public:
   PASSTHR (void);
   ~PASSTHR (void);

   USHORT Add (VOID);
   USHORT Add (class TMsgBase *MsgBase);
   USHORT Add (class TCollection &MsgText);
   VOID   Close (VOID);
   USHORT Delete (ULONG ulMsg);
   USHORT GetHWM (ULONG &ulMsg);
   ULONG  Highest (VOID);
   USHORT Lock (ULONG ulTimeout = 0);
   ULONG  Lowest (VOID);
   ULONG  MsgnToUid (ULONG ulMsg);
   VOID   New (VOID);
   USHORT Next (ULONG &ulMsg);
   ULONG  Number (VOID);
   VOID   Pack (VOID);
   USHORT Previous (ULONG &ulMsg);
   USHORT ReadHeader (ULONG ulMsg);
   USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79);
   USHORT Read (ULONG ulMsg, SHORT nWidth = 79);
   VOID   SetHWM (ULONG ulMsg);
   ULONG  UidToMsgn (ULONG ulMsg);
   VOID   UnLock (VOID);
   USHORT WriteHeader (ULONG ulMsg);
};

// --------------------------------------------------------------------------

class DLL_EXPORT COMBO : public TMsgBase
{
public:
   COMBO (void);
   ~COMBO (void);

   USHORT Add (VOID);
   USHORT Add (class TMsgBase *MsgBase);
   USHORT Add (class TCollection &MsgText);
   USHORT Add (PSZ pszPath, USHORT usStorage, ULONG ulNumber);
   VOID   Close (VOID);
   USHORT Delete (ULONG ulMsg);
   USHORT GetHWM (ULONG &ulMsg);
   ULONG  Highest (VOID);
   USHORT Lock (ULONG ulTimeout = 0);
   ULONG  Lowest (VOID);
   ULONG  MsgnToUid (ULONG ulMsg);
   VOID   New (VOID);
   USHORT Next (ULONG &ulMsg);
   ULONG  Number (VOID);
   VOID   Pack (VOID);
   USHORT Previous (ULONG &ulMsg);
   USHORT ReadHeader (ULONG ulMsg);
   USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79);
   USHORT Read (ULONG ulMsg, SHORT nWidth = 79);
   VOID   SetHWM (ULONG ulMsg);
   ULONG  UidToMsgn (ULONG ulMsg);
   VOID   UnLock (VOID);
   USHORT WriteHeader (ULONG ulMsg);

private:
   CHAR   LastPath[64];
   ULONG  TotalMsgs, NextNumber;
   class  TMsgBase *Msg;
   class  TCollection Index;
};

// --------------------------------------------------------------------------

class DLL_EXPORT INETMAIL : public TMsgBase
{
public:
   INETMAIL (void);
   INETMAIL (PSZ pszServer, PSZ pszUser, PSZ pszPwd);
   ~INETMAIL (void);

   CHAR   HostName[64], SMTPHostName[64];
   CHAR   Error[128];

   USHORT Add (VOID);
   USHORT Add (class TMsgBase *MsgBase);
   USHORT Add (class TCollection &MsgText);
   VOID   Close (VOID);
   USHORT Delete (ULONG ulMsg);
   ULONG  Highest (VOID);
   USHORT GetHWM (ULONG &ulMsg);
   USHORT Lock (ULONG ulTimeout = 0);
   ULONG  Lowest (VOID);
   ULONG  MsgnToUid (ULONG ulMsg);
   VOID   New (VOID);
   USHORT Next (ULONG &ulMsg);
   ULONG  Number (VOID);
   USHORT Open (PSZ pszServer, PSZ pszUser, PSZ pszPwd);
   VOID   Pack (VOID);
   USHORT Previous (ULONG &ulMsg);
   USHORT ReadHeader (ULONG ulMsg);
   USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79);
   USHORT Read (ULONG ulMsg, SHORT nWidth = 79);
   VOID   SetHWM (ULONG ulMsg);
   ULONG  UidToMsgn (ULONG ulMsg);
   VOID   UnLock (VOID);
   USHORT WriteHeader (ULONG ulMsg);

private:
   CHAR   szBuffer[1024];
   ULONG  TotalMsgs;
   ULONG  LastReaded;
   class  TTcpip *Tcp;

   USHORT GetResponse (PSZ pszResponse, USHORT usMaxLen);
};

// --------------------------------------------------------------------------

#define MAX_DUPES    2000

typedef struct {
   CHAR   EchoTag[64];
   USHORT Position;
   ULONG  Dupes[MAX_DUPES];
} DUPEDATA;

typedef struct {
   CHAR   EchoTag[64];
   ULONG  Position;
} DUPEIDX;

class DLL_EXPORT TDupes
{
public:
   TDupes (void);
   TDupes (PSZ pszDataPath);
   ~TDupes (void);

   VOID   Add (PSZ pszEchoTag, class TMsgBase *Msg);
   USHORT Check (PSZ pszEchoTag, class TMsgBase *Msg);
   VOID   Delete (VOID);
   ULONG  GetEID (class TMsgBase *Msg);
   USHORT Load (PSZ pszEchoTag);
   VOID   Save (VOID);

private:
   CHAR   DataFile[128];
   CHAR   IndexFile[128];
   DUPEDATA dd;
};

#endif

