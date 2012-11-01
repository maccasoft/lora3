
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.20
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "combase.h"
#include "config.h"
#include "msgbase.h"
#include "tools.h"
#include "user.h"
#include "version.h"

static PSZ MONTHS[] = {
   "January", "February", "March", "April", "Maj", "Juni",
   "July", "August", "September", "October", "November", "December"
};

class TPop
{
public:
   TPop (void);
   ~TPop (void);

   USHORT Delete;
   class  TTcpip *Tcp;

   VOID   FetchMail (PSZ pszUser, PSZ pszPassword);
   VOID   GetLine (VOID);
   USHORT SendCommand (PSZ pszCommand);
   VOID   SetMailBox (PSZ pszUserName);

private:
   CHAR   Response[256];
   CHAR   Temp[128];
   CHAR   MailBox[128];
};

TPop::TPop (void)
{
   Delete = FALSE;
   strcpy (MailBox, "pop3mail");
}

TPop::~TPop (void)
{
}

VOID TPop::FetchMail (PSZ pszUser, PSZ pszPassword)
{
   USHORT i, Total = 0, Current;
   PSZ p;
   struct dosdate_t date;
   struct dostime_t time;
   class MsgBase *Msg = NULL;
   class TCollection Text;

   if ((Msg = new SQUISH (MailBox)) != NULL) {
      printf ("\nFetching mail for user '%s'\n", pszUser);

      GetLine ();
      sprintf (Temp, "USER %s", pszUser);
      if (SendCommand (Temp) == TRUE) {
         sprintf (Temp, "PASS %s", pszPassword);
         if (SendCommand (Temp) == TRUE) {
            if (SendCommand ("STAT") == TRUE)
               Total = (USHORT)atoi (&Response[4]);
            for (Current = 1; Current <= Total; Current++) {
               sprintf (Temp, "RETR %d", Current);
               if (SendCommand (Temp) == TRUE) {
                  Msg->New ();
                  _dos_getdate (&date);
                  _dos_gettime (&time);
                  Msg->Arrived.Day = Msg->Written.Day = date.day;
                  Msg->Arrived.Month = Msg->Written.Month = date.month;
                  Msg->Arrived.Year = Msg->Written.Year = (USHORT)date.year;
                  Msg->Arrived.Hour = Msg->Written.Hour = time.hour;
                  Msg->Arrived.Minute = Msg->Written.Minute = time.minute;
                  Msg->Arrived.Second = Msg->Written.Second = time.second;

                  do {
                     GetLine ();
                     if (!strncmp (Response, "From: ", 6)) {
                        if (strlen (&Response[6]) >= sizeof (Msg->From))
                           Response[6 + sizeof (Msg->From)] = '\0';
                        strcpy (Msg->From, &Response[6]);
                     }
                     else if (!strncmp (Response, "To: ", 4)) {
                        if (strlen (&Response[4]) >= sizeof (Msg->To))
                           Response[4 + sizeof (Msg->To)] = '\0';
                        strcpy (Msg->To, &Response[4]);
                     }
                     else if (!strncmp (Response, "Subject: ", 9)) {
                        if (strlen (&Response[9]) >= sizeof (Msg->Subject))
                           Response[9 + sizeof (Msg->Subject)] = '\0';
                        strcpy (Msg->Subject, &Response[9]);
                     }
                     else if (!strncmp (Response, "Date: ", 6)) {
                        p = strtok (&Response[6], " ");
                        if (p != NULL && !isdigit (p[0]))
                           p = strtok (NULL, " ");
                        if (p != NULL) {
                           Msg->Written.Day = (UCHAR)atoi (p);
                           if ((p = strtok (NULL, " ")) != NULL) {
                              for (i = 0; i < 12; i++)
                                 if (strnicmp (MONTHS[i], p, 3) == 0) {
                                    Msg->Written.Month = (UCHAR)(i + 1);
                                    i = 12;
                                 }
                           }
                           if ((p = strtok (NULL, " ")) != NULL) {
                              Msg->Written.Year = (USHORT)atoi (p);
                              if (Msg->Written.Year >= 80 && Msg->Written.Year < 100)
                                 Msg->Written.Year += 1900;
                              else if (Msg->Written.Year < 80)
                                 Msg->Written.Year += 2000;
                           }
                           if ((p = strtok (NULL, " ")) != NULL) {
                              Msg->Written.Hour = (UCHAR)((p[0] - '0') * 10 + (p[1] - '0'));
                              p += 2;
                              if (p[0] == ':')
                                 p++;
                              Msg->Written.Minute = (UCHAR)((p[0] - '0') * 10 + (p[1] - '0'));
                              p += 2;
                              if (p[0] != '\0') {
                                 if (p[0] == ':')
                                    p++;
                                 if (p[0] != '\0')
                                    Msg->Written.Second = (UCHAR)((p[0] - '0') * 10 + (p[1] - '0'));
                              }
                           }
                        }
                     }
                  } while (Tcp->Carrier () == TRUE && Response[0] != '\0');

                  Text.Clear ();
                  do {
                     GetLine ();
                     if (stricmp (Response, ".")) {
                        if (!stricmp (Response, ".."))
                           Text.Add (".");
                        else
                           Text.Add (Response, (USHORT)(strlen (Response) + 1));
                     }
                  } while (Tcp->Carrier () == TRUE && strcmp (Response, "."));

                  Msg->Write (Msg->Highest () + 1, Text);

                  if (Delete == TRUE) {
                     sprintf (Temp, "DELE %d", Current);
                     SendCommand (Temp);
                  }
               }
            }
            SendCommand ("QUIT");
         }
      }
      delete Msg;
   }
}

VOID TPop::GetLine (VOID)
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
   } while (c != '\r' && Tcp->Carrier () == TRUE);
   *pszResp = '\0';
}

USHORT TPop::SendCommand (PSZ pszCommand)
{
   USHORT len = 0, MaxLen = sizeof (Response);
   CHAR c, *pszResp = Response;

   Tcp->BufferBytes ((UCHAR *)pszCommand, (USHORT)strlen (pszCommand));
   Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
   Tcp->UnbufferBytes ();

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
   } while (c != '\r' && Tcp->Carrier () == TRUE);
   *pszResp = '\0';

   printf ("   %s\n", Response);

   return ((Response[0] == '+') ? TRUE : FALSE);
}

VOID TPop::SetMailBox (PSZ pszUserName)
{
   class TConfig *Cfg;
   class TUser *User;

   if ((Cfg = new TConfig (".\\")) != NULL) {
      Cfg->Read (0);
      if ((User = new TUser (Cfg->UserFile)) != NULL) {
         if (User->GetData (pszUserName) == TRUE)
            sprintf (MailBox, "%s%s", Cfg->MailSpool, User->MailBox);
         delete User;
      }
      delete Cfg;
   }
}

void main (int argc, char *argv[])
{
   int i;
   USHORT Delete = FALSE;
   class TTcpip *Tcp;
   class TPop *Pop;

   printf ("\nLoraBBS Professional Edition for OS/2 PM. Version %s\n", VERSION);
   printf ("Copyright (c) 1995 by Marco Maccaferri. All rights reserved.\n\n");

   if (argc < 5) {
      printf ("Usage: POP3MAIL [host] [username] [pwd] \"User Name\" [-d]\n\n");
   }
   else {
      for (i = 5; i < argc; i++) {
         if (!stricmp (argv[i], "-d"))
            Delete = TRUE;
      }

      if ((Tcp = new TTcpip) != NULL) {
         if (Tcp->ConnectServer (argv[1], 110) == TRUE) {
            if ((Pop = new TPop) != NULL) {
               Pop->Tcp = Tcp;
               Pop->SetMailBox (argv[4]);
               Pop->Delete = Delete;
               Pop->FetchMail (argv[2], argv[3]);
               delete Pop;
            }
         }
         delete Tcp;
      }
   }
}

