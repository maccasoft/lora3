
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.08
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "tools.h"

#if defined(__DOS__)
#include "cxl.h"
#endif

TLog::TLog (void)
{
   fp = NULL;
   strcpy (Id, "LORA");
   Months[0] = "Jan";
   Months[1] = "Feb";
   Months[2] = "Mar";
   Months[3] = "Apr";
   Months[4] = "Maj";
   Months[5] = "Jun";
   Months[6] = "Jul";
   Months[7] = "Aug";
   Months[8] = "Sep";
   Months[9] = "Oct";
   Months[10] = "Nov";
   Months[11] = "Dec";
   Level = 0xFFFFU;

   Begin = "+Begin, v%s (Ch. %d)";
   BrainLapsed = ":User temporarily brain-lapsed '%s'";
   Dialing = ":Dialing %s (%s)";
   End = "+End";
   GivenLevel = ":Given %d mins. (%u, %s)";
   IsCalling = "+%s is calling";
   MenuError = "!MNU ERR: ID=%u MNU=%s";
   MenuNotFound = "!Menu '%s' not found. Aborting session";
   ModemResponse = ":%s";
   NotInList = "+%s isn't in user list";
   ProcessingNode = "*Processing %s - %s";
   SeriousError = ">Serious error in file %s, line %d";
   UserOffline = "+%s off-line. Calls=%ld, Len=%ld";

   Display = TRUE;
   Text.Clear ();

   if ((whLog = wopen (1, 0, 12, 45, 1, YELLOW|_BLACK, LGREY|_BLACK)) != 0)
      prints (1, 2, YELLOW|_BLACK, "LOG");
}

TLog::~TLog (void)
{
   short wh;

   Close ();

   if (whLog != 0) {
      wh = whandle ();
      wactiv (whLog);
      wclose ();
      if (whLog != wh)
         wactiv (wh);
   }
}

USHORT TLog::Open (PSZ pszName)
{
   if (fp != NULL)
      Close ();

   if ((fp = fopen (pszName, "at")) == NULL)
      return (FALSE);

   return (TRUE);
}

VOID TLog::Close (VOID)
{
   if (fp != NULL) {
      WriteBlank ();
      fclose (fp);
      fp = NULL;
   }
}

VOID TLog::Update (VOID)
{
#if defined(__DOS__)
   short wh, i;
   PSZ p;

   if (Display == TRUE && whLog != 0) {
      wh = whandle ();
      wactiv (whLog);
      wcclear (LGREY|_BLACK);
      i = 9;
      if ((p = (PSZ)Text.Last ()) != NULL) {
         wprintsf (i--, 0, YELLOW|_BLACK, p);
         if ((p = (PSZ)Text.Previous ()) != NULL)
            do {
               wprintsf (i--, 0, LCYAN|_BLACK, p);
            } while ((p = (PSZ)Text.Previous ()) != NULL);
      }
      wactiv (wh);
   }
#endif
}

VOID TLog::Write (PSZ pszFormat, ...)
{
   va_list arglist;
   short wh;
   CHAR c, *p;
   time_t t;
   struct tm *timep;

   c = pszFormat[0];
   if (c == '>' || (c == '+' && (Level & LOG_PLUS)) || (c == '!' && (Level & LOG_EXCLAM)) || (c == ':' && (Level & LOG_COLON)) || (c == '*' && (Level & LOG_STAR)) || (c == ' ' && (Level & LOG_BLANK))) {
      va_start (arglist, pszFormat);
      vsprintf (Buffer, pszFormat, arglist);
      va_end (arglist);

      t = time (NULL);
      timep = localtime (&t);

      if (fp != NULL) {
         fprintf (fp, "%c %02d %3s %02d:%02d:%02d %s %s\n", Buffer[0], timep->tm_mday, Months[timep->tm_mon], timep->tm_hour, timep->tm_min, timep->tm_sec, Id, &Buffer[1]);
         fflush (fp);
      }

      if (Display == TRUE && whLog != 0) {
#if defined(__DOS__)
	 wh = whandle ();
	 wactiv (whLog);
    if ((p = (PSZ)Text.Last ()) != NULL)
      wprintsf (9, 0, LCYAN|_BLACK, p);
	 Buffer[44] = '\0';
	 wscroll (1, D_UP);
	 wprintsf (9, 0, YELLOW|_BLACK, "%c %02d:%02d %s", Buffer[0], timep->tm_hour, timep->tm_min, &Buffer[1]);
	 wactiv (wh);
#endif
      }

      sprintf (Temp, "%c %02d:%02d %s", Buffer[0], timep->tm_hour, timep->tm_min, &Buffer[1]);
      if (Text.Elements >= 10) {
         Text.First ();
         Text.Remove ();
      }
      Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
   }
}

VOID TLog::WriteBlank (VOID)
{
   if (fp != NULL) {
      fprintf (fp, "\n");
      fflush (fp);
   }
}


