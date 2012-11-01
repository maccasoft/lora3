
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.5
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora_api.h"

TLog::TLog (void)
{
   fp = NULL;

   Months[0] = "Jan";
   Months[1] = "Feb";
   Months[2] = "Mar";
   Months[3] = "Apr";
   Months[4] = "May";
   Months[5] = "Jun";
   Months[6] = "Jul";
   Months[7] = "Aug";
   Months[8] = "Sep";
   Months[9] = "Oct";
   Months[10] = "Nov";
   Months[11] = "Dec";
}

TLog::~TLog (void)
{
   if (fp != NULL)
      fclose (fp);
}

USHORT TLog::Open (PSZ pszName)
{
   USHORT RetVal = FALSE;

   if (fp != NULL)
      fclose (fp);

   strcpy (FileName, pszName);
   if ((fp = fopen (FileName, "a+t")) != NULL)
      RetVal = TRUE;

   return (RetVal);
}

VOID TLog::Suspend (VOID)
{
   if (fp != NULL) {
      fclose (fp);
      fp = NULL;
   }
}

VOID TLog::Resume (VOID)
{
   if (fp == NULL)
      fp = fopen (FileName, "a+t");
}

VOID TLog::Write (PSZ pszFormat, ...)
{
   va_list arglist;
   time_t t;
   struct tm *timep;

   va_start (arglist, pszFormat);
   vsprintf (Buffer, pszFormat, arglist);
   va_end (arglist);

   t = time (NULL);
   timep = localtime (&t);
   sprintf (Temp, "%c %02d %3s %02d:%02d:%02d %s %s", Buffer[0], timep->tm_mday, Months[timep->tm_mon], timep->tm_hour, timep->tm_min, timep->tm_sec, "LORA", &Buffer[1]);

   if (fp != NULL) {
      fprintf (fp, "%s\n", Temp);
      fflush (fp);
   }
}

VOID TLog::WriteBlank (VOID)
{
   if (fp != NULL) {
      fprintf (fp, "\n");
      fflush (fp);
   }
}


