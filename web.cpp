
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.6
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    13/06/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lserver.h"

TWeb::TWeb (void)
{
   Cfg = NULL;
   Log = NULL;
   Tcp = NULL;
   Location[0] = '\0';
}

TWeb::~TWeb (void)
{
}

static CHAR *WeekDays[] = {
   "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
   "Friday", "Saturday"
};

static CHAR *Months[] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

VOID TWeb::GetRequest (VOID)
{
   USHORT len = 0;
   CHAR c, *pszResp = Request;

   do {
      if (Tcp->BytesReady () == TRUE) {
         if ((c = (CHAR)Tcp->ReadByte ()) != '\r') {
            if (c != '\n') {
               *pszResp++ = c;
               if (++len >= sizeof (Request) - 1)
                  c = '\r';
            }
         }
      }
#if defined(__OS2__)
      else
         DosSleep (1L);
#elif defined(__NT__)
      else
         Sleep (1L);
#endif
   } while (c != '\r' && Tcp->Carrier () == TRUE);

   *pszResp = '\0';
}

VOID TWeb::Run (VOID)
{
   CHAR *p, Found;
   struct stat statbuf;
   class TUser *User;

   GetRequest ();
   while (Tcp->BytesReady () == TRUE)
      Tcp->ReadByte ();

   if ((p = strchr (Request, '\r')) != NULL)
      *p = '\0';
   if ((p = strchr (Request, '\n')) != NULL)
      *p = '\0';
   if (Log != NULL)
      Log->Write (":  %s", Request);

   if ((p = strchr (Request, ' ')) != NULL) {
      while (*p == ' ')
         *p++ = '\0';
      if (!stricmp (Request, "GET")) {
         strcpy (Request, p);
         if ((p = strchr (Request, ' ')) != NULL) {
            while (*p == ' ')
               *p++ = '\0';
         }
         if (Request[0] == '\0' || !strcmp (Request, "/") || !strcmp (Request, "\\")) {
            sprintf (Temp, "%slogo.htm", Cfg->TextFiles);
            if (stat (Temp, &statbuf) != 0)
               sprintf (Temp, "%slogo.html", Cfg->TextFiles);
            if (stat (Temp, &statbuf) != 0)
               URLNotFound (Request);
            else
               SendFile (Temp, "text/html");
         }
         else if (Request[0] == '/' || Request[0] == '\\') {
            if (Request[1] == '~') {
               Found = FALSE;
               if ((p = strchr (&Request[2], '/')) != NULL)
                  *p = '\0';
               if ((User = new TUser (Cfg->UserFile)) != NULL) {
                  if (User->First () == TRUE)
                     do {
                        if (!stricmp (User->MailBox, &Request[2])) {
                           if (p != NULL)
                              *p = '/';
                           strcpy (Request, &Request[strlen (User->MailBox) + 2]);
                           if (Request[0] == '\0' || !strcmp (Request, "/") || !strcmp (Request, "\\")) {
                              sprintf (Temp, "%s%s\\index.htm", Cfg->UsersHomePath, User->MailBox);
                              if (Request[0] == '\0')
                                 sprintf (Location, "http://%s/~%s/", Tcp->HostIP, User->MailBox);
                              if (stat (Temp, &statbuf) != 0)
                                 sprintf (Temp, "%s%s\\index.html", Cfg->UsersHomePath, User->MailBox);
                              SendFile (Temp, "text/html");
                           }
                           else if (Request[0] == '/' || Request[0] == '\\') {
                              sprintf (Temp, "%s%s\\%s", Cfg->UsersHomePath, User->MailBox, &Request[1]);
                              if (stat (Temp, &statbuf) != 0)
                                 sprintf (Temp, "%s%s\\%s", Cfg->UsersHomePath, User->MailBox, &Request[1]);
                              SendFile (Temp);
                           }
                           Found = TRUE;
                           break;
                        }
                     } while (User->Next () == TRUE);
                  delete User;
               }
               if (Found == FALSE)
                  URLNotFound (Request);
            }
            else {
               sprintf (Temp, "%s%s", Cfg->TextFiles, &Request[1]);
               if (Temp[strlen (Temp) - 1] != '/' && Temp[strlen (Temp) - 1] != '\\')
                  strcat (Temp, "\\");
               strcat (Temp, "index.htm");
               if (stat (Temp, &statbuf) == 0)
                  SendFile (Temp);
               else {
                  strcat (Temp, "l");
                  if (stat (Temp, &statbuf) == 0)
                     SendFile (Temp);
                  else {
                     sprintf (Temp, "%s%s", Cfg->TextFiles, &Request[1]);
                     if (stat (Temp, &statbuf) != 0)
                        sprintf (Temp, "%s%s", Cfg->TextFiles, &Request[1]);
                     SendFile (Temp);
                  }
               }
            }
         }
         else
            URLNotFound (Request);
      }
   }
}

VOID TWeb::SendFile (PSZ File, PSZ MimeType)
{
   int fd;
   CHAR *p;
   USHORT i;
   struct dosdate_t date_t;
   struct dostime_t time_t;
   struct stat statbuf;
   struct tm *ftm;

   if (MimeType == NULL) {
      MimeType = "application/octet-stream";
      if ((p = strchr (File, '.')) != NULL) {
         if (!stricmp (p, ".js") || !stricmp (p, ".ls") || !stricmp (p, ".mocha"))
            MimeType = "application/x-javascript";
         else if (!stricmp (p, ".exe") || !stricmp (p, ".bin"))
            MimeType = "application/octet-stream";
         else if (!stricmp (p, ".tar"))
            MimeType = "application/x-tar";
         else if (!stricmp (p, ".zip"))
            MimeType = "application/x-zip-compressed";
         else if (!stricmp (p, ".sit"))
            MimeType = "application/x-stuffit";
         else if (!stricmp (p, ".hqx"))
            MimeType = "application/mac-binhex40";
         else if (!stricmp (p, ".avi"))
            MimeType = "video/x-msvideo";
         else if (!stricmp (p, ".mov") || !stricmp (p, ".qt"))
            MimeType = "video/quicktime";
         else if (!stricmp (p, ".mpeg") || !stricmp (p, ".mpg") || !stricmp (p, ".mpe"))
            MimeType = "video/mpeg";
         else if (!stricmp (p, ".wav"))
            MimeType = "audio/x-wav";
         else if (!stricmp (p, ".aif") || !stricmp (p, ".aiff") || !stricmp (p, ".aifc"))
            MimeType = "audio/x-aiff";
         else if (!stricmp (p, ".au") || !stricmp (p, ".snd"))
            MimeType = "audio/basic";
         else if (!stricmp (p, ".gif"))
            MimeType = "image/gif";
         else if (!stricmp (p, ".jpg") || !stricmp (p, ".jpeg"))
            MimeType = "image/jpeg";
         else if (!stricmp (p, ".htm") || !stricmp (p, ".html"))
            MimeType = "text/html";
         else if (!stricmp (p, ".txt") || !stricmp (p, ".text"))
            MimeType = "text/plain";
      }
   }

   while ((p = strchr (File, '/')) != NULL)
      *p = '\\';

   if ((fd = open (File, O_RDONLY|O_BINARY)) != -1) {
      if (Location[0] != '\0')
         sprintf (Temp, "HTTP/1.0 302 Found\r\n");
      else
         sprintf (Temp, "HTTP/1.0 200 Ok\r\n");
      Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

      _dos_getdate (&date_t);
      _dos_gettime (&time_t);
      sprintf (Temp, "Date: %s, %d-%s-%d %d:%02d:%02d GMT\r\n", WeekDays[date_t.dayofweek], date_t.day, Months[date_t.month - 1], date_t.year % 100, time_t.hour, time_t.minute, time_t.second);
      Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

      sprintf (Temp, "Server: %s/%s\r\n", NAME, VERSION);
      Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

      sprintf (Temp, "MIME-version: 1.0\r\n");
      Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

      if (Location[0] != '\0') {
         sprintf (Temp, "Location: %s\r\n", Location);
         Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
      }

      sprintf (Temp, "Content-type: %s\r\n", MimeType);
      Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

      fstat (fd, &statbuf);
      ftm = gmtime (&statbuf.st_mtime);
      sprintf (Temp, "Last-modified: %s, %d-%s-%d %d:%02d:%02d GMT\r\n", WeekDays[ftm->tm_wday], ftm->tm_mday, Months[ftm->tm_mon], ftm->tm_year % 100, ftm->tm_hour, ftm->tm_min, ftm->tm_sec);
      Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

      sprintf (Temp, "Content-length: %lu\r\n", statbuf.st_size);
      Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

      Tcp->BufferBytes ((UCHAR *)"\r\n", 2);

      if (Location[0] == '\0') {
         do {
            i = (USHORT)read (fd, Temp, sizeof (Temp));
            Tcp->BufferBytes ((UCHAR *)Temp, i);
         } while (i == sizeof (Temp));
      }

      Tcp->UnbufferBytes ();

      close (fd);
   }
   else
      URLNotFound (File);
}

VOID TWeb::URLNotFound (PSZ URL)
{
   struct dosdate_t date_t;
   struct dostime_t time_t;

   sprintf (Temp, "HTTP/1.0 404 Not Found\r\n");
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

   _dos_getdate (&date_t);
   _dos_gettime (&time_t);
   sprintf (Temp, "Date: %s, %d-%s-%d %d:%02d:%02d GMT\r\n", WeekDays[date_t.dayofweek], date_t.day, Months[date_t.month - 1], date_t.year % 100, time_t.hour, time_t.minute, time_t.second);
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

   sprintf (Temp, "Server: %s/%s\r\n", NAME, VERSION);
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

   sprintf (Temp, "MIME-version: 1.0\r\n");
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

   sprintf (Temp, "Content-type: %s\r\n", "text/html");
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

   sprintf (Temp, "<HEAD><TITLE>404 Not Found</TITLE></HEAD>\r\n");
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   sprintf (Temp, "<BODY><H1>404 Not Found</H1>\r\n");
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   sprintf (Temp, "The requested URL %s was not found on this server.<P>\r\n", URL);
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   sprintf (Temp, "</BODY>\r\n");
   Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

   Tcp->UnbufferBytes ();
}

