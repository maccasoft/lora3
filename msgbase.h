
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _MSGBASE_H
#define _MSGBASE_H

#include "combase.h"
#include "tools.h"
#include "jam.h"

typedef struct {
   UCHAR  Day;
   UCHAR  Month;
   USHORT Year;
   UCHAR  Hour;
   UCHAR  Minute;
   UCHAR  Second;
} MDATE;

class DLL_EXPORT MsgBase
{
public:
   MsgBase (void) {};
   virtual ~MsgBase (void) {};

   ULONG   Id;
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
   virtual USHORT Add (class TCollection &MsgText) = 0;
   virtual VOID   Close (VOID) = 0;
   virtual USHORT Delete (ULONG ulMsg) = 0;
   virtual ULONG  Highest (VOID) = 0;
   virtual USHORT Lock (ULONG ulTimeout = 0) = 0;
   virtual ULONG  Lowest (VOID) = 0;
   virtual VOID   New (VOID) = 0;
   virtual USHORT Next (ULONG &ulMsg) = 0;
   virtual ULONG  Number (VOID) = 0;
   virtual VOID   Pack (VOID) = 0;
   virtual USHORT Previous (ULONG &ulMsg) = 0;
   virtual USHORT ReadHeader (ULONG ulMsg) = 0;
   virtual USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79) = 0;
   virtual USHORT Read (ULONG ulMsg, SHORT nWidth = 79) = 0;
   virtual VOID   UnLock (VOID) = 0;
   virtual USHORT WriteHeader (ULONG ulMsg) = 0;
   virtual USHORT Write (ULONG ulMsg) = 0;
   virtual USHORT Write (ULONG ulMsg, class TCollection &MsgText) = 0;
};

class DLL_EXPORT JAM : public MsgBase
{
public:
   JAM (void);
   JAM (PSZ pszName);
   ~JAM (void);

   USHORT Add (VOID);
   USHORT Add (class TCollection &MsgText);
   VOID   Close (VOID);
   USHORT Delete (ULONG ulMsg);
   ULONG  Highest (VOID);
   USHORT Lock (ULONG ulTimeout = 0);
   ULONG  Lowest (VOID);
   VOID   New (VOID);
   USHORT Next (ULONG &ulMsg);
   ULONG  Number (VOID);
   USHORT Open (PSZ pszName);
   VOID   Pack (VOID);
   USHORT Previous (ULONG &ulMsg);
   USHORT ReadHeader (ULONG ulMsg);
   USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79);
   USHORT Read (ULONG ulMsg, SHORT nWidth = 79);
   VOID   UnLock (VOID);
   USHORT WriteHeader (ULONG ulMsg);
   USHORT Write (ULONG ulMsg);
   USHORT Write (ULONG ulMsg, class TCollection &MsgText);

private:
   INT    fdHdr;
   INT    fdJdt;
   INT    fdJdx;
   UCHAR *pSubfield;
   CHAR   BaseName[128];
   JAMHDRINFO  jamHdrInfo;
   JAMHDR      jamHdr;
};

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

#define XMSG_FROM_SIZE  36
#define XMSG_TO_SIZE    36
#define XMSG_SUBJ_SIZE  72

#define MAX_REPLY       10    // Max number of stored replies to one msg

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
   CHAR   FtscDate[20];       // Obsolete date information
} XMSG;

typedef struct {
   ULONG  Ofs;
   ULONG  MsgId;
   ULONG  Hash;
} SQIDX;

class DLL_EXPORT SQUISH : public MsgBase
{
public:
   SQUISH (void);
   SQUISH (PSZ pszName);
   ~SQUISH (void);

   USHORT Add (VOID);
   USHORT Add (class TCollection &MsgText);
   VOID   Close (VOID);
   USHORT Delete (ULONG ulMsg);
   ULONG  Highest (VOID);
   USHORT Lock (ULONG ulTimeout = 0);
   ULONG  Lowest (VOID);
   VOID   New (VOID);
   USHORT Next (ULONG &ulMsg);
   ULONG  Number (VOID);
   USHORT Open (PSZ pszName);
   VOID   Pack (VOID);
   USHORT Previous (ULONG &ulMsg);
   USHORT ReadHeader (ULONG ulMsg);
   USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79);
   USHORT Read (ULONG ulMsg, SHORT nWidth = 79);
   VOID   UnLock (VOID);
   USHORT WriteHeader (ULONG ulMsg);
   USHORT Write (ULONG ulMsg);
   USHORT Write (ULONG ulMsg, class TCollection &MsgText);

private:
   INT    fdHdr;
   INT    fdIdx;
   CHAR   BaseName[128];
   SQBASE SqBase;
};

class DLL_EXPORT USENET : public MsgBase
{
public:
   USENET (void);
   USENET (PSZ pszServer, PSZ pszGroup);
   ~USENET (void);

   CHAR   HostName[32];
   CHAR   NewsGroup[64];
   CHAR   Organization[64];
   CHAR   ProgramID[32];

   USHORT Add (VOID);
   USHORT Add (class TCollection &MsgText);
   VOID   Close (VOID);
   USHORT Delete (ULONG ulMsg);
   ULONG  Highest (VOID);
   USHORT Lock (ULONG ulTimeout = 0);
   ULONG  Lowest (VOID);
   VOID   New (VOID);
   USHORT Next (ULONG &ulMsg);
   ULONG  Number (VOID);
   USHORT Open (PSZ pszServer, PSZ pszGroup);
   VOID   Pack (VOID);
   USHORT Previous (ULONG &ulMsg);
   USHORT ReadHeader (ULONG ulMsg);
   USHORT Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth = 79);
   USHORT Read (ULONG ulMsg, SHORT nWidth = 79);
   VOID   UnLock (VOID);
   USHORT WriteHeader (ULONG ulMsg);
   USHORT Write (ULONG ulMsg);
   USHORT Write (ULONG ulMsg, class TCollection &MsgText);

private:
   CHAR   szBuffer[1024];
   ULONG  ulHighest;
   ULONG  ulFirst;
   ULONG  ulTotal;
   ULONG  LastReaded;
   class  TTcpip *Tcp;

   USHORT GetResponse (PSZ pszResponse, USHORT usMaxLen);
};

#endif

