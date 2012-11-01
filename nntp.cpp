
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.6
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/07/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lserver.h"

//static CHAR *WeekDays[] = {
//   "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
//};

static CHAR *Months[] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

TNNTP::TNNTP (void)
{
   Tcp = NULL;
   Msg = NULL;
   Log = NULL;
}

TNNTP::~TNNTP (void)
{
   if (Tcp != NULL)
      delete Tcp;
   if (Msg != NULL)
      delete Msg;
}

VOID TNNTP::GetCommand (VOID)
{
   USHORT len = 0, MaxLen = sizeof (Response);
   CHAR c, *pszResp = Response;

   do {
      if (Tcp->BytesReady () == TRUE) {
         do {
            if ((c = (CHAR)Tcp->ReadByte ()) != '\r') {
               if (c != '\n') {
                  *pszResp++ = c;
                  if (++len >= MaxLen)
                     c = '\r';
               }
            }
         } while (Tcp->RxBytes > 0 && c != '\r');
      }
#if defined(__OS2__)
      DosSleep (1L);
#elif defined(__NT__)
      Sleep (1L);
#endif
   } while (c != '\r' && Tcp->Carrier () == TRUE);
   *pszResp = '\0';
}

VOID TNNTP::DisplayXOver (ULONG XFrom, ULONG XTo)
{
   int lines;
   PSZ p;
   ULONG size;
   time_t mtime;
   struct tm ftm;

   if (XFrom == 0L)
      XFrom = Msg->Lowest ();

   sprintf (Temp, "224 data follows\r\n");
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

   do {
      if (Msg->Read (XFrom) == TRUE) {
         sprintf (Temp, "%lu\t%s\t%s\t", Msg->Current, Msg->Subject, Msg->From);
         Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

         memset (&ftm, 0, sizeof (ftm));
         ftm.tm_mday = Msg->Written.Day;
         ftm.tm_mon = Msg->Written.Month - 1;
         ftm.tm_year = Msg->Written.Year - 1900;
         ftm.tm_hour = Msg->Written.Hour;
         ftm.tm_min = Msg->Written.Minute;
         ftm.tm_sec = Msg->Written.Second;
         mtime = mktime (&ftm);
         memcpy (&ftm, gmtime (&mtime), sizeof (ftm));
         sprintf (Temp, "%d %s %d %d:%02d:%02d GMT\t", ftm.tm_mday, Months[ftm.tm_mon], ftm.tm_year, ftm.tm_hour, ftm.tm_min, ftm.tm_sec);
         Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

         sprintf (Temp, "<%lu@lorabbs.server>\t", Msg->Current);
         Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

         lines = 0;
         size = 0L;
         if ((p = (PSZ)Msg->Text.First ()) != NULL)
            do {
               lines++;
               size += strlen (p) + 1;
            } while ((p = (PSZ)Msg->Text.Next ()) != NULL);

         sprintf (Temp, "\t%lu\t%d\r\n", size, lines);
         Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
      }
      if (XFrom >= XTo)
         break;
   } while (Msg->Next (XFrom) == TRUE);

   sprintf (Temp, ".\r\n");
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   Tcp->UnbufferBytes ();
}

VOID TNNTP::SendHeader (USHORT Termination)
{
   int lines;
   time_t mtime;
   struct tm ftm;

   sprintf (Temp, "From: (%s)\r\n", Msg->From);
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   sprintf (Temp, "Subject: %s\r\n", Msg->Subject);
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   memset (&ftm, 0, sizeof (ftm));
   ftm.tm_mday = Msg->Written.Day;
   ftm.tm_mon = Msg->Written.Month - 1;
   ftm.tm_year = Msg->Written.Year - 1900;
   ftm.tm_hour = Msg->Written.Hour;
   ftm.tm_min = Msg->Written.Minute;
   ftm.tm_sec = Msg->Written.Second;
   mtime = mktime (&ftm);
   memcpy (&ftm, gmtime (&mtime), sizeof (ftm));
   sprintf (Temp, "Date: %d %s %d %d:%02d:%02d GMT\r\n", ftm.tm_mday, Months[ftm.tm_mon], ftm.tm_year, ftm.tm_hour, ftm.tm_min, ftm.tm_sec);
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

   lines = 0;
   if (Msg->Text.First () != NULL)
      do {
         lines++;
      } while (Msg->Text.Next () != NULL);
   sprintf (Temp, "Lines: %d\r\n", lines);
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

   if (Termination == TRUE) {
      sprintf (Temp, ".\r\n");
      Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
      Tcp->UnbufferBytes ();
   }
   else
      Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
}

VOID TNNTP::SendBody (VOID)
{
   CHAR *p;

   if ((p = (CHAR *)Msg->Text.First ()) != NULL)
      do {
         if (*p != 0x01 && strncmp (p, "SEEN-BY: ", 9)) {
            if (!strcmp (p, "."))
               Tcp->BufferBytes ((UCHAR *)"..", 2);
            else
               Tcp->BufferBytes ((UCHAR *)p, (USHORT)strlen (p));
            Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
         }
      } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

   sprintf (Temp, ".\r\n");
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   Tcp->UnbufferBytes ();
}

VOID TNNTP::Run (VOID)
{
   USHORT EndRun = FALSE, Found;
   CHAR *p;
   ULONG Number, XFrom, XTo;
   class TMsgData *Data;

   sprintf (Temp, "200 LoraBBS NNTP server ready (posting ok)\r\n");
   Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

   while (Tcp->Carrier () == TRUE && EndRun == FALSE) {
      GetCommand ();
      if (Log != NULL)
         Log->Write (":NNTP: %s", Response);

      if ((p = strtok (Response, " ")) != NULL) {
         if (!stricmp (p, "QUIT")) {
            sprintf (Temp, "205 goodbye\r\n");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            EndRun = TRUE;
         }
         else if (!stricmp (p, "LIST")) {
            if ((p = strtok (NULL, " ")) == NULL) {
               sprintf (Temp, "215 list of newsgroups follows\r\n");
               Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
                  if (Data->First () == TRUE)
                     do {
                        if (Data->NewsGroup[0] != '\0' && Data->Storage != ST_USENET) {
                           sprintf (Temp, "%s %05lu %05lu y\r\n", Data->NewsGroup, Data->LastMessage, Data->FirstMessage);
                           Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                        }
                     } while (Data->Next () == TRUE);
                  delete Data;
               }
               sprintf (Temp, ".\r\n");
               Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               Tcp->UnbufferBytes ();
            }
            else if (!stricmp (p, "overview.fmt")) {
               sprintf (Temp, "215 Order of fields in overview database.\r\n");
               Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               sprintf (Temp, "Subject:\r\nFrom:\r\nDate:\r\nMessage-ID:\r\nReferences:\r\nBytes:\r\nLines:\r\nXref:full\r\n.\r\n");
               Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               Tcp->UnbufferBytes ();
            }
            else {
               sprintf (Temp, "500 command not recognized.\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            }
         }
         else if (!stricmp (p, "GROUP")) {
            Found = FALSE;
            Number = 0L;
            if ((p = strtok (NULL, " ")) != NULL) {
               if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
                  if (Data->First () == TRUE)
                     do {
                        if (Data->NewsGroup[0] != '\0' && Data->Storage != ST_USENET) {
                           if (!stricmp (p, Data->NewsGroup)) {
                              if (Msg != NULL)
                                 delete Msg;
                              Msg = NULL;
                              if (Data->Storage == ST_JAM)
                                 Msg = new JAM (Data->Path);
                              else if (Data->Storage == ST_SQUISH)
                                 Msg = new SQUISH (Data->Path);
                              else if (Data->Storage == ST_FIDO)
                                 Msg = new FIDOSDM (Data->Path);
                              else if (Data->Storage == ST_ADEPT)
                                 Msg = new ADEPT (Data->Path);
                              if (Msg != NULL) {
                                 sprintf (Temp, "211 %lu %lu %lu %s group selected\r\n", Msg->Number (), Msg->Lowest (), Msg->Highest (), Data->NewsGroup);
                                 Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                                 Found = TRUE;
                              }
                              break;
                           }
                        }
                     } while (Data->Next () == TRUE);
                  delete Data;
               }
            }
            if (Found == FALSE) {
               sprintf (Temp, "411 no such news group\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            }
         }
         else if (!stricmp (p, "XOVER")) {
            if (Msg == NULL) {
               sprintf (Temp, "412 Not in a newsgroup\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            }
            else if ((p = strtok (NULL, "")) != NULL) {
               XTo = 0L;
               if ((p = strtok (p, " -")) != NULL)
                  XFrom = atol (p);
               if ((p = strtok (NULL, " -")) != NULL)
                  XTo = atol (p);
               else
                  XTo = Msg->Highest ();
               DisplayXOver (XFrom, XTo);
            }
            else
               DisplayXOver (Number, Number);
         }
         else if (!stricmp (p, "STAT")) {
            if ((p = strtok (NULL, " ")) != NULL && Msg != NULL) {
               if (Msg->Read (atol (p)) == TRUE)
                  sprintf (Temp, "223 %lu <%lu@lorabbs.server> article retrived - statistics only\r\n", Msg->Current, Msg->Current);
               else
                  sprintf (Temp, "423 no such article number in this group\r\n");
            }
            else {
               if (Msg == NULL)
                  sprintf (Temp, "412 no newsgroup has been selected\r\n");
               else
                  sprintf (Temp, "423 no such article number in this group\r\n");
            }
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else if (!stricmp (p, "ARTICLE")) {
            if ((p = strtok (NULL, " ")) != NULL && Msg != NULL) {
               if (Msg->Read (atol (p)) == TRUE) {
                  sprintf (Temp, "220 %lu <%lu@lorabbs.server> article retrived - text follows\r\n", Msg->Current, Msg->Current);
                  Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

                  SendHeader (FALSE);
                  SendBody ();
               }
               else {
                  sprintf (Temp, "420 no such article number in this group\r\n");
                  Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               }
            }
            else {
               if (Msg == NULL)
                  sprintf (Temp, "412 no newsgroup has been selected\r\n");
               else
                  sprintf (Temp, "420 no such article number in this group\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            }
         }
         else if (!stricmp (p, "NEXT")) {
            if (Msg != NULL) {
               Number = Msg->Current;
               if (Msg->Next (Number) == TRUE) {
                  Msg->Read (Number);
                  sprintf (Temp, "223 %lu <%lu@lorabbs.server> article retrived - statistics only\r\n", Msg->Current, Msg->Current);
               }
               else
                  sprintf (Temp, "423 no such article number in this group\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            }
         }
         else if (!stricmp (p, "HEAD")) {
            if (Msg != NULL) {
               Found = TRUE;
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (atoi (p) != Msg->Current)
                     Found = Msg->Read ((USHORT)atoi (p));
               }
               if (Found == TRUE) {
                  sprintf (Temp, "221 %lu <%lu@lorabbs.server> article retrived - head follows\r\n", Msg->Current, Msg->Current);
                  Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                  SendHeader (TRUE);
               }
               else {
                  sprintf (Temp, "421 no such article number in this group\r\n");
                  Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               }
            }
         }
         else if (!stricmp (p, "BODY")) {
            if (Msg != NULL) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (atoi (p) != Msg->Current)
                     Msg->Read ((USHORT)atoi (p));
               }
               sprintf (Temp, "222 %lu <%lu@lorabbs.server> article retrived - body follows\r\n", Msg->Current, Msg->Current);
               Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               SendBody ();
            }
         }
         else if (!stricmp (p, "NEWGROUPS")) {
            sprintf (Temp, "231 list of new newsgroups follows\r\n.\r\n");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else if (!stricmp (p, "NEWNEWS")) {
            sprintf (Temp, "230 list of new articles by message-id follows\r\n.\r\n");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
//         else if (!stricmp (p, "POST")) {
//         }
         else {
            sprintf (Temp, "500 command not recognized.\r\n");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
      }

#if defined(__OS2__)
      DosSleep (1L);
#elif defined(__NT__)
      Sleep (1L);
#endif
   }
}


