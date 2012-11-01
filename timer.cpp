
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "tools.h"

#define PER_WEEK    60480000L
#define PER_DAY      8640000L
#define PER_HOUR      360000L
#define PER_MINUTE      6000L
#define PER_SECOND       100L

LONG TTimer::TimerSet (LONG lHund)
{
   LONG l;
   struct dostime_t dt;

   _dos_gettime (&dt);
   l = (dt.minute % 60) * PER_MINUTE + (dt.second % 60) * PER_SECOND + dt.hsecond;
   l += lHund;

   return (l);
}

LONG TTimer::TimeUp (LONG lEndtime)
{
   LONG l;
   struct dostime_t dt;

   _dos_gettime (&dt);
   l = (dt.minute % 60) * PER_MINUTE + (dt.second % 60) * PER_SECOND + dt.hsecond;
   if (l < (lEndtime - 65536L))
      l += PER_HOUR;

   return ((l - lEndtime) >= 0L);
}

VOID TTimer::Pause (LONG lHund)
{
   LONG Endtime;

   Endtime = TimerSet (lHund);
   while (!TimeUp (Endtime))
      ;
}

