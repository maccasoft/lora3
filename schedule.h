
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.09
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    06/06/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "config.h"
#include "msgbase.h"
#include "msgdata.h"
#include "tools.h"
#include "user.h"

class DLL_EXPORT TMaint
{
public:
   TMaint (class TConfig *lpCfg, class TLog *lpLog);
   ~TMaint (void);

   VOID   DeleteMessages (VOID);
   VOID   UpdateNewsgroups (VOID);
   VOID   Users (VOID);

private:
   class  TConfig *Cfg;
   class  TLog    *Log;
};

typedef struct {
   CHAR   Label[32];          // Reference label
   UCHAR  Day;                // Execution day
   UCHAR  Month;              // Execution month
   USHORT Year;               // Execution year
   UCHAR  Hour;               // Execution hour
   UCHAR  Minute;             // Execution minute
   USHORT Length;             // Duration of this event (if applicable)
   UCHAR  Force;              // Force execution of this event
   UCHAR  RepDay;             // Days until next execution
   UCHAR  RepMonth;           // Months until next execution
   USHORT RepYear;            // Years until next execution
   UCHAR  RepHour;            // Hours until next execution
   UCHAR  RepMinute;          // Minutes until next execution
   USHORT Channel;            // Channel affected

   UCHAR  Started;            // TRUE=Event has started

   UCHAR  CallNode;           // TRUE=Call the next node in queue (or the one specified)
   UCHAR  ForceCall;          // TRUE=Force a call to the node specified
   CHAR   Address[32];        // Fidonet address to call
   USHORT Delay;              // Delay between each call;
   USHORT Retries;            // Outgoing calls retries
   USHORT Failures;           // Outgoing calls failures

   UCHAR  ImportMail;         // TRUE=Import mail during this event
   UCHAR  ExportMail;         // TRUE=Export mail during this event
   UCHAR  StartImport;        // TRUE=Import mail at the beginning of this event
   UCHAR  StartExport;        // TRUE=Export mail at the beginning of this event
   UCHAR  EndImport;          // TRUE=Import mail at the end of this event
   UCHAR  EndExport;          // TRUE=Export mail at the end of this event

   UCHAR  UserMaint;          // TRUE=Performs the user's maintenance
   UCHAR  MessageMaint;       // TRUE=Performs the message maintenance
   UCHAR  UpdateNewsgroups;   // TRUE=Update the usenet newsgroups defined
   CHAR   External[128];      // External maintenance command
} SCHED;

class DLL_EXPORT TScheduler
{
public:
   TScheduler (void);
   TScheduler (PSZ pszDataPath);
   ~TScheduler (void);

   CHAR   Label[32];
   UCHAR  Day, Month;
   USHORT Year, Length;
   UCHAR  Hour, Minute;
   UCHAR  Force, RepDay, RepMonth;
   USHORT RepYear;
   UCHAR  RepHour, RepMinute;
   USHORT Channel;
   UCHAR  Started;

   UCHAR  CallNode, ForceCall;
   CHAR   Address[32];
   USHORT Delay, Retries, Failures;

   UCHAR  ImportMail, ExportMail, StartImport;
   UCHAR  StartExport, EndImport, EndExport;

   UCHAR  UserMaint;
   UCHAR  MessageMaint;
   UCHAR  UpdateNewsgroups;
   CHAR   External[128];

   VOID   Add (VOID);
   USHORT CheckNext (VOID);
   VOID   Delete (VOID);
   VOID   Executed (USHORT flUpdate = TRUE);
   USHORT First (VOID);
   VOID   New (VOID);
   USHORT Next (VOID);
   USHORT Previous (VOID);
   USHORT ReadCurrent (USHORT usChannel = 0);
   USHORT ShouldExecute (VOID);
   LONG   TimeToNext (VOID);
   VOID   Update (VOID);

private:
   int    fd;
   CHAR   DataFile[128];
   SCHED  Temp;

   VOID   Struct2Class (VOID);
   VOID   Class2Struct (VOID);
};

#endif


