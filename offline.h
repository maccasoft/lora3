
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.20
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    06/13/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _OFFLINE_H
#define _OFFLINE_H

#include "bbs.h"
#include "config.h"
#include "ftrans.h"
#include "msgbase.h"
#include "msgdata.h"
#include "packer.h"
#include "tools.h"
#include "user.h"

class DLL_EXPORT TOffline
{
public:
   TOffline (class TBbs *bbs);
   virtual ~TOffline (void);

   USHORT Area;
   CHAR   Id[16], Path[128];
   ULONG  Limit;
   ULONG  Current, Total;
   ULONG  Personal, TotalPersonal;
   ULONG  Reply;

   virtual VOID   AddConference (VOID);
   virtual USHORT BuildPath (PSZ pszPath);
   virtual USHORT Create (VOID);
   virtual USHORT Compress (PSZ pszPacket);
   virtual VOID   Display (VOID);
   virtual VOID   Download (PSZ pszFile, PSZ pszName);
   virtual USHORT FetchReply (VOID);
   virtual VOID   PackArea (PSZ pszKey, ULONG &ulLast);
   virtual VOID   PackEMail (ULONG &ulLast);
   virtual USHORT Prescan (VOID);
   virtual VOID   RemoveArea (VOID);
   virtual VOID   Scan (PSZ pszKey, ULONG ulLast);

protected:
   class  TBbs      *Bbs;
   class  TConfig   *Cfg;
   class  TLanguage *Lang;
   class  TLog      *Log;
   class  TUser     *User;

   CHAR   Work[128];
   USHORT BarWidth;
   ULONG  TotalPack;
   class  TMsgData  *MsgArea;
   class  MsgBase   *Msg;
   class  TLastRead NewLR;
};

class DLL_EXPORT TAscii : public TOffline
{
public:
   TAscii (class TBbs *bbs);
   ~TAscii (void);

   USHORT Create (VOID);
   VOID   PackArea (PSZ pszKey, ULONG &ulLast);
   VOID   PackEMail (ULONG &ulLast);
};

#include "bluewave.h"

class DLL_EXPORT TBlueWave : public TOffline
{
public:
   TBlueWave (class TBbs *bbs);
   ~TBlueWave (void);

   USHORT Create (VOID);
   USHORT FetchReply (VOID);
   VOID   PackArea (PSZ pszKey, ULONG &ulLast);
   VOID   PackEMail (ULONG &ulLast);

private:
   INF_HEADER    Inf;
   INF_AREA_INFO AreaInf;
   MIX_REC       Mix;
   FTI_REC       Fti;
   UPI_HEADER    Upih;
   UPI_REC       Upir;
   UPL_HEADER    Uplh;
   UPL_REC       Uplr;
};

typedef struct {
   UCHAR  Msgstat;
   UCHAR  Msgnum[7];
   UCHAR  Msgdate[8];
   UCHAR  Msgtime[5];
   CHAR   MsgTo[25];
   CHAR   MsgFrom[25];
   CHAR   MsgSubj[25];
   UCHAR  Msgpass[12];
   UCHAR  Msgrply[8];
   UCHAR  Msgrecs[6];
   UCHAR  Msglive;
   UCHAR  Msgarealo;
   UCHAR  Msgareahi;
   UCHAR  Msgfiller[3];
} QWKHDR;

typedef union {
   UCHAR  uc[10];
   USHORT ui[5];
   ULONG  ul[2];
   float  f[2];
   double d[1];
} QWKCONV;

class DLL_EXPORT TQWK : public TOffline
{
public:
   TQWK (class TBbs *bbs);
   ~TQWK (void);

   USHORT Create (VOID);
   USHORT FetchReply (VOID);
   VOID   PackArea (PSZ pszKey, ULONG &ulLast);
   VOID   PackEMail (ULONG &ulLast);

private:
   ULONG  Blocks;
   QWKHDR Qwk;

   float  IEEToMSBIN (float f);
};

#endif

