
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.6
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    02/07/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lserver.h"

static CHAR *WeekDays[] = {
   "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static CHAR *Months[] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

TPOP3::TPOP3 (void)
{
   Tcp = NULL;
   User = NULL;
   Msg = NULL;
   Log = NULL;
}

TPOP3::~TPOP3 (void)
{
   if (Tcp != NULL)
      delete Tcp;
   if (User != NULL)
      delete User;
   if (Msg != NULL)
      delete Msg;
}

VOID TPOP3::GetCommand (VOID)
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

VOID TPOP3::Run (VOID)
{
   USHORT EndRun, Validated, Index, Found;
   CHAR *p;
   ULONG Number, Size;
   MAILINDEX *MailIdx;
   time_t mtime;
   struct tm ftm;

   EndRun = FALSE;
   Validated = FALSE;

   sprintf (Temp, "+OK LoraBBS POP3 server ready\r\n");
   Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

   User = new TUser (Cfg->UserFile);

   while (Tcp->Carrier () == TRUE && EndRun == FALSE) {
      GetCommand ();

      if ((p = strtok (Response, " ")) != NULL) {
         if (!stricmp (p, "QUIT")) {
            if (Validated == TRUE && Msg != NULL) {
               if ((MailIdx = (MAILINDEX *)MailDrop.First ()) != NULL)
                  do {
                     if (MailIdx->Deleted == TRUE)
                        Msg->Delete (MailIdx->Number);
                  } while ((MailIdx = (MAILINDEX *)MailDrop.Next ()) != NULL);
            }
            sprintf (Temp, "+OK LoraBBS POP3 server signing off\r\n");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            EndRun = TRUE;
         }
         else if (!stricmp (p, "USER")) {
            Validated = FALSE;
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Response, strlwr (p));
               Response[0] = (CHAR)toupper (Response[0]);
               while ((p = strchr (Response, '.')) != NULL) {
                  *p++ = ' ';
                  *p = (CHAR)toupper (*p);
               }
               if (User->GetData (Response) == TRUE)
                  sprintf (Temp, "+OK %s is welcome here\r\n", Response);
               else
                  sprintf (Temp, "-ERR never heard of %s\r\n", Response);
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            }
         }
         else if (!stricmp (p, "PASS")) {
            Validated = FALSE;
            if ((p = strtok (NULL, " ")) != NULL) {
               if (User->CheckPassword (p) == TRUE) {
                  MailDrop.Clear ();

                  if (Cfg->MailStorage == ST_JAM)
                     Msg = new JAM (Cfg->MailPath);
                  else if (Cfg->MailStorage == ST_SQUISH)
                     Msg = new SQUISH (Cfg->MailPath);
                  else if (Cfg->MailStorage == ST_FIDO)
                     Msg = new FIDOSDM (Cfg->MailPath);
                  else if (Cfg->MailStorage == ST_ADEPT)
                     Msg = new ADEPT (Cfg->MailPath);

                  Index = 0;
                  Size = 0L;

                  if (Msg != NULL) {
                     Number = Msg->Lowest ();
                     do {
                        if (Msg->Read (Number) == TRUE && !stricmp (User->Name, Msg->To)) {
                           Idx.Index = ++Index;
                           Idx.Number = Number;
                           Idx.Deleted = FALSE;
                           Idx.Size = 0L;
                           Idx.Size += strlen (Msg->From);
                           Idx.Size += strlen (Msg->To);
                           Idx.Size += strlen (Msg->Subject);
                           if ((p = (CHAR *)Msg->Text.First ()) != NULL)
                              do {
                                 Idx.Size += strlen (p) + 1;
                              } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);
                           Size += Idx.Size;
                           MailDrop.Add (&Idx, sizeof (Idx));
                        }
                     } while (Msg->Next (Number) == TRUE);
                  }

                  sprintf (Temp, "+OK maildrop locked and ready\r\n");
                  Validated = TRUE;
               }
               else
                  sprintf (Temp, "-ERR invalid password\r\n");
            }
            else
               sprintf (Temp, "-ERR invalid password\r\n");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else if (!stricmp (p, "NOOP")) {
            sprintf (Temp, "+OK\r\n");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else if (Validated == TRUE) {
            if (!stricmp (p, "STAT")) {
               sprintf (Temp, "+OK %u %lu\r\n", Index, Size);
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            }
            else if (!stricmp (p, "LIST")) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  Found = FALSE;
                  if ((MailIdx = (MAILINDEX *)MailDrop.First ()) != NULL)
                     do {
                        if (MailIdx->Index == (USHORT)atoi (p)) {
                           sprintf (Temp, "+OK %u %lu\r\n", MailIdx->Index, MailIdx->Size);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           Found = TRUE;
                           break;
                        }
                     } while ((MailIdx = (MAILINDEX *)MailDrop.Next ()) != NULL);
                  if (Found == FALSE) {
                     sprintf (Temp, "-ERR no such message\r\n");
                     Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                  }
               }
               else {
                  sprintf (Temp, "+OK %u messages (%lu octects)\r\n", Index, Size);
                  Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                  if ((MailIdx = (MAILINDEX *)MailDrop.First ()) != NULL)
                     do {
                        sprintf (Temp, "%u %lu\r\n", MailIdx->Index, MailIdx->Size);
                        Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                     } while ((MailIdx = (MAILINDEX *)MailDrop.Next ()) != NULL);
                  Tcp->BufferBytes ((UCHAR *)".\r\n", 3);
                  Tcp->UnbufferBytes ();
               }
            }
            else if (!stricmp (p, "RETR")) {
               Found = FALSE;
               if ((p = strtok (NULL, " ")) != NULL) {
                  if ((MailIdx = (MAILINDEX *)MailDrop.First ()) != NULL)
                     do {
                        if (MailIdx->Index == (USHORT)atoi (p)) {
                           sprintf (Temp, "+OK %lu octects\r\n", MailIdx->Index, MailIdx->Size);
                           Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           Found = TRUE;
                           break;
                        }
                     } while ((MailIdx = (MAILINDEX *)MailDrop.Next ()) != NULL);
               }
               if (Found == FALSE) {
                  sprintf (Temp, "-ERR no such message\r\n");
                  Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               }
               else {
                  Msg->Read (MailIdx->Number);

                  memset (&ftm, 0, sizeof (ftm));
                  ftm.tm_mday = Msg->Written.Day;
                  ftm.tm_mon = Msg->Written.Month - 1;
                  ftm.tm_year = Msg->Written.Year - 1900;
                  ftm.tm_hour = Msg->Written.Hour;
                  ftm.tm_min = Msg->Written.Minute;
                  ftm.tm_sec = Msg->Written.Second;
                  mtime = mktime (&ftm);
                  memcpy (&ftm, gmtime (&mtime), sizeof (ftm));
                  sprintf (Temp, "Date: %s %d %s %d %d:%02d:%02d GMT\r\n", WeekDays[ftm.tm_wday], ftm.tm_mday, Months[ftm.tm_mon], ftm.tm_year % 100, ftm.tm_hour, ftm.tm_min, ftm.tm_sec);
                  Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                  sprintf (Temp, "From: %s <%s>\r\n", Msg->From, Msg->FromAddress);
                  Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                  sprintf (Temp, "To: %s <%s>\r\n", Msg->To, Msg->ToAddress);
                  Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                  sprintf (Temp, "Subject: %s\r\n", Msg->Subject);
                  Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

                  sprintf (Temp, "\r\n");
                  Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

                  sprintf (Temp, "\r\n");
                  if ((p = (CHAR *)Msg->Text.First ()) != NULL)
                     do {
                        if (!strcmp (p, "."))
                           Tcp->BufferBytes ((UCHAR *)"..", 2);
                        else
                           Tcp->BufferBytes ((UCHAR *)p, (USHORT)strlen (p));
                        Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                     } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

                  Tcp->BufferBytes ((UCHAR *)".\r\n", 3);

                  Tcp->UnbufferBytes ();
               }
            }
            else if (!stricmp (p, "DELE")) {
               Found = FALSE;
               if ((p = strtok (NULL, " ")) != NULL) {
                  if ((MailIdx = (MAILINDEX *)MailDrop.First ()) != NULL)
                     do {
                        if (MailIdx->Index == (USHORT)atoi (p)) {
                           if (MailIdx->Deleted == TRUE)
                              sprintf (Temp, "-ERR message %lu already deleted\r\n", MailIdx->Index);
                           else
                              sprintf (Temp, "+OK message %lu deleted\r\n", MailIdx->Index);
                           Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           MailIdx->Deleted = TRUE;
                           Found = TRUE;
                           break;
                        }
                     } while ((MailIdx = (MAILINDEX *)MailDrop.Next ()) != NULL);
               }
               if (Found == FALSE)
                  sprintf (Temp, "-ERR no such message\r\n");
               else
                  sprintf (Temp, "+OK\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            }
            else {
               sprintf (Temp, "-ERR command not implemented.\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            }
         }
         else {
            sprintf (Temp, "-ERR command not implemented.\r\n");
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


