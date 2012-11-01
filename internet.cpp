
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.11
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora.h"

#define SE                 240
#define NOP                241
#define DM                 242
#define BREAK              243
#define IP                 244
#define AO                 245
#define AYT                246
#define EC                 247
#define EL                 248
#define GOAHEAD            249
#define SB                 250
#define WILLTEL            251
#define WONTTEL            252
#define DOTEL              253
#define DONTTEL            254
#define IAC                255

#define BINARY             0
#define IECHO              1
#define RECONNECT          2
#define SGA                3
#define AMSN               4
#define STATUS             5
#define TIMING             6
#define RCTAN              7
#define OLW                8
#define OPS                9
#define OCRD               10
#define OHTS               11
#define OHTD               12
#define OFFD               13
#define OVTS               14
#define OVTD               15
#define OLFD               16
#define XASCII             17
#define LOGOUT             18
#define BYTEM              19
#define DET                20
#define SUPDUP             21
#define SUPDUPOUT          22
#define SENDLOC            23
#define TERMTYPE           24
#define EOR                25
#define TACACSUID          26
#define OUTPUTMARK         27
#define TERMLOCNUM         28
#define REGIME3270         29
#define X3PAD              30
#define NAWS               31
#define TERMSPEED          32
#define TFLOWCNTRL         33
#define LINEMODE           34
#define XDISPLOC           35
#define ENVIRONMENT        36
#define AUTHENTICATION     37
#define DATA_ENCRYPTION    38

TInternet::TInternet (void)
{
   Cfg = NULL;
   Com = Snoop = NULL;
   Log = NULL;
   Embedded = NULL;
   User = NULL;
}

TInternet::~TInternet (void)
{
}

VOID TInternet::Telnet (PSZ pszServer, USHORT usPort)
{
   USHORT c, Exit;
   CHAR Temp[30];

   if (ValidateKey ("bbs", NULL, NULL) == KEY_BASIC) {
      Embedded->Printf ("\n\x16\x01\015Sorry, command avalable only in the ADVANCED and PROFESSIONAL version\006\007\006\007");
      return;
   }

   if (pszServer == NULL || pszServer[0] == '\0') {
      Embedded->Printf ("\n\x16\x01\013Enter the host name, or RETURN to exit: \026\001\x1E");
      Embedded->GetString (Host, (USHORT)(sizeof (Host) - 1), INP_FIELD);
      pszServer = Host;
   }

   if (Embedded->AbortSession () == FALSE && Host[0] != '\0') {
      Embedded->Printf ("\nTrying...");

      if ((Tcp = new TTcpip) != NULL) {
         if (Tcp->ConnectServer (pszServer, usPort) == TRUE) {
            Log->Write ("+Telnet to %s", pszServer);

            Embedded->Printf ("\014Connected to %s.\nEscape character is '^]'\n\n", pszServer);

            Tcp->BufferByte (IAC);
            Tcp->BufferByte (DOTEL);
            Tcp->BufferByte (IECHO);
            Tcp->BufferByte (IAC);
            Tcp->BufferByte (DOTEL);
            Tcp->BufferByte (BINARY);
            Tcp->UnbufferBytes ();

            Exit = FALSE;

            do {
               if (Tcp->BytesReady () == TRUE) {
                  do {
                     if ((c = Tcp->ReadByte ()) == IAC) {
                        if ((c = Tcp->ReadByte ()) != IAC) {
                           if (c == WILLTEL) {
                              c = Tcp->ReadByte ();
                              Tcp->BufferByte (IAC);
                              if (c == BINARY || c == IECHO && c != SGA)
                                 Tcp->BufferByte (DOTEL);
                              else
                                 Tcp->BufferByte (DONTTEL);
                              Tcp->BufferByte ((UCHAR)c);
                              Tcp->UnbufferBytes ();
                           }
                           else if (c == WONTTEL) {
                              c = Tcp->ReadByte ();
                              if (c != BINARY && c != IECHO && c != SGA) {
                                 Tcp->BufferByte (IAC);
                                 Tcp->BufferByte (DONTTEL);
                                 Tcp->BufferByte ((UCHAR)c);
                                 Tcp->UnbufferBytes ();
                              }
                           }
                           else if (c == DOTEL) {
                              c = Tcp->ReadByte ();
                              if (c == TERMTYPE) {
                                 Tcp->BufferByte (IAC);
                                 Tcp->BufferByte (WILLTEL);
                                 Tcp->BufferByte (TERMTYPE);
                              }
                              else if (c != BINARY && c != IECHO && c != SGA) {
                                 Tcp->BufferByte (IAC);
                                 Tcp->BufferByte (WONTTEL);
                                 Tcp->BufferByte ((UCHAR)c);
                              }
                              Tcp->UnbufferBytes ();
                           }
                           else if (c == DONTTEL) {
                              c = Tcp->ReadByte ();
                              if (c != BINARY && c != IECHO && c != SGA) {
                                 Tcp->BufferByte (IAC);
                                 Tcp->BufferByte (WONTTEL);
                                 Tcp->BufferByte ((UCHAR)c);
                                 Tcp->UnbufferBytes ();
                              }
                           }
                           else if (c == SB) {
                              if ((c = Tcp->ReadByte ()) == TERMTYPE) {
                                 if ((c = Tcp->ReadByte ()) == 1) {
                                    Tcp->BufferByte (IAC);
                                    Tcp->BufferByte (SB);
                                    Tcp->BufferByte (TERMTYPE);
                                    Tcp->BufferByte (0);
                                    Tcp->BufferBytes ((UCHAR *)"VT100", 5);
                                    Tcp->BufferByte (IAC);
                                    Tcp->BufferByte (SE);
                                    Tcp->UnbufferBytes ();
                                 }
                              }
                           }
                        }
                        else {
                           Com->BufferByte ((UCHAR)c);
                           if (Snoop != NULL)
                              Snoop->BufferByte ((UCHAR)c);
                        }
                     }
                     else {
                        Com->BufferByte ((UCHAR)c);
                        if (Snoop != NULL)
                           Snoop->BufferByte ((UCHAR)c);
                        if (c == 0x0A) {
                           Com->BufferByte (13);
                           if (Snoop != NULL)
                              Snoop->BufferByte (13);
                        }
                     }
                  } while (Tcp->RxBytes > 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE && Exit == FALSE);

                  Com->UnbufferBytes ();
                  if (Snoop != NULL)
                     Snoop->UnbufferBytes ();
               }

               if (Com->BytesReady () == TRUE) {
                  do {
                     c = Com->ReadByte ();
                     if (c == 0x1D) {
                        Tcp->UnbufferBytes ();
                        Embedded->Printf ("\n");
                        do {
                           Embedded->Printf ("\x16\x01\013telnet> ");
                           Embedded->Input (Temp, (USHORT)(sizeof (Temp) - 1), 0);
                           if (!stricmp (Temp, "quit")) {
                              if (Tcp->Carrier () == TRUE)
                                 Tcp->ClosePort ();
                              Exit = TRUE;
                           }
                           else if (!stricmp (Temp, "close")) {
                              Tcp->ClosePort ();
                              Embedded->Printf ("Connection closed.\n");
                           }
                        } while (Temp[0] != '\0' && Embedded->AbortSession () == FALSE);
                     }
                     else {
                        Tcp->BufferByte ((UCHAR)c);
                        if (c == IAC)
                           Tcp->BufferByte ((UCHAR)c);
                     }
                  } while (Com->RxBytes > 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE && Exit == FALSE);

                  Tcp->UnbufferBytes ();
               }

#if defined(__OS2__)
               DosSleep (1L);
#elif defined(__NT__)
               Sleep (1L);
#endif
            } while (Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE && Exit == FALSE);

            Log->Write ("+End Telnet");

            Embedded->Printf ("\n");
            Embedded->Printf ("\026\001\007Connection closed.\n");
         }
         Tcp->ClosePort ();
         delete Tcp;
      }
   }
}

VOID TInternet::Finger (PSZ pszServer, USHORT usPort)
{
   CHAR String[24], c;

   if (ValidateKey ("bbs", NULL, NULL) == KEY_BASIC) {
      Embedded->Printf ("\n\x16\x01\015Sorry, command avalable only in the ADVANCED and PROFESSIONAL version\006\007\006\007");
      return;
   }

   if (pszServer == NULL || pszServer[0] == '\0') {
      Embedded->Printf ("\n\x16\x01\013Enter the host name, or RETURN to exit: \026\001\x1E");
      Embedded->GetString (Host, (USHORT)(sizeof (Host) - 1), INP_FIELD);
      pszServer = Host;
   }

   if (Embedded->AbortSession () == FALSE && Host[0] != '\0') {
      if ((Tcp = new TTcpip) != NULL) {
         if (Tcp->ConnectServer (pszServer, usPort) == TRUE) {
            Log->Write ("+Finger => %s", pszServer);

            Embedded->Printf ("\n\x16\x01\013Enter a user name, or RETURN to list online users: \026\001\x1E");
            Embedded->Input (String, (USHORT)(sizeof (String) - 1), INP_FIELD);

            Tcp->SendBytes ((UCHAR *)String, (USHORT)strlen (String));
            Tcp->SendBytes ((UCHAR *)"\r\n", 2);

            Embedded->Printf ("\n\x16\x01\x07");

            while (Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE) {
               if (Tcp->BytesReady () == TRUE) {
                  c = (CHAR)Tcp->ReadByte ();
                  if (Com != NULL)
                     Com->BufferByte ((UCHAR)c);
                  if (Snoop != NULL)
                     Snoop->BufferByte ((UCHAR)c);
               }
#if defined(__OS2__)
               DosSleep (1L);
#elif defined(__NT__)
               Sleep (1L);
#endif
            }

            if (Com != NULL)
               Com->UnbufferBytes ();
            if (Snoop != NULL)
               Snoop->UnbufferBytes ();
         }

         Tcp->ClosePort ();
         delete Tcp;
      }
   }
}

USHORT TInternet::GetResponse (PSZ pszResponse, USHORT usMaxLen)
{
   USHORT retVal = FALSE, len = 0;
   CHAR c, *pszResp = pszResponse;

   do {
      if (Tcp->BytesReady () == TRUE)
         do {
            if ((c = (CHAR)Tcp->ReadByte ()) != '\r') {
               if (c != '\n') {
                  *pszResp++ = c;
                  if (++len >= usMaxLen)
                     c = '\r';
               }
            }
         } while (Tcp->BytesReady () == TRUE && c != '\r');

#if defined(__OS2__)
      DosSleep (1L);
#elif defined(__NT__)
      Sleep (1L);
#endif
   } while (c != '\r' && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

   *pszResp = '\0';
   if (pszResponse[3] == ' ')
      retVal = (USHORT)atoi (pszResponse);

   return (retVal);
}

// ----------------------------------------------------------------------
// Waits for an incoming connection for the FTP file transfer. This
// routine is used to receive both ASCII and BINARY files for real files
// and directory listings.
// ----------------------------------------------------------------------
VOID TInternet::FTP_GET (PSZ pszFile, PSZ pszName, USHORT fHash)
{
   FILE *fp;
   USHORT Len, Counter;
   ULONG Size, Elapsed;

   while (Data->WaitClient () == 0) {
      if (Embedded->AbortSession () == TRUE || Tcp->Carrier () == FALSE)
         return;
#if defined(__OS2__)
      DosSleep (1L);
#elif defined(__NT__)
      Sleep (1L);
#endif
   }

   if (pszFile != NULL) {
      fp = _fsopen (pszFile, "wb", SH_DENYNO);
      if (Log != NULL)
         Log->Write (" Receiving %s", pszFile);
   }

   Size = 0;
   Counter = 0;
   Elapsed = time (NULL);

// ----------------------------------------------------------------------
// Bytes receiving loop. This loop runs until the remote host drops the
// connection, signalling that the transmission is ended.
// ----------------------------------------------------------------------
   do {
      while (Data->BytesReady () == TRUE && Embedded->AbortSession () == FALSE) {
         Len = Data->ReadBytes (Buffer, sizeof (Buffer));
         Size += Len;
// ----------------------------------------------------------------------
// This is a failsafe routine, if a file was request but the fopen fails,
// each byte is received but nothing is saved (I think that's should be
// called failsafe...).
// ----------------------------------------------------------------------
         if (pszFile == NULL) {
            Com->BufferBytes (Buffer, Len);
            if (Snoop != NULL)
               Snoop->BufferBytes (Buffer, Len);
         }
         else if (fp != NULL)
            fwrite (Buffer, Len, 1, fp);
// ----------------------------------------------------------------------
// Hash mark printing after each 1024 bytes packet received.
// ----------------------------------------------------------------------
         Counter += Len;
         if (fHash == TRUE && Counter >= 1024) {
            Embedded->Putch ((UCHAR)'#');
            Counter -= 1024;
         }

#if defined(__OS2__)
         DosSleep (1L);
#elif defined(__NT__)
         Sleep (1L);
#endif
      }

#if defined(__OS2__)
      DosSleep (1L);
#elif defined(__NT__)
      Sleep (1L);
#endif
   } while (Data->Carrier () == TRUE && Embedded->AbortSession () == FALSE);

   Com->UnbufferBytes ();
   if (Snoop != NULL)
      Snoop->UnbufferBytes ();

   if (fHash == TRUE && Size >= 1024L)
      Embedded->Printf ("\n");
   if (pszFile != NULL && fp != NULL)
      fclose (fp);

   if ((Elapsed = time (NULL) - Elapsed) == 0L)
      Elapsed = 1L;
   Embedded->Printf ("%lu bytes received in %lu seconds (%lu bytes/sec.).\n", Size, Elapsed, Size / Elapsed);
   if (pszFile != NULL && Log != NULL)
      Log->Write ("+Received %s, %lu bytes", pszFile, Size);

   if (pszFile != NULL && pszName != NULL && User != NULL) {
//      Embedded->Printf ("\n\x16\x01\012You have just tagged:\n\n");
      User->FileTag->New ();
      strcpy (User->FileTag->Name, pszName);
      strcpy (User->FileTag->Area, "USER");
      strcpy (User->FileTag->Complete, pszFile);
      User->FileTag->Size = Size;
      User->FileTag->DeleteAfter = TRUE;
      User->FileTag->Add ();
//      Embedded->Printf ("\x16\x01\x0A%5d. The file %s in the %s Library\n", User->FileTag->Index, pszName, User->FileTag->Area);
      if (Log != NULL)
         Log->Write (":Tagged file %s, library %s", pszName, User->FileTag->Area);
   }
}

VOID TInternet::FTP_MGET (PSZ pszFile)
{
   FILE *fp;
   USHORT i;
   CHAR File[128], *p;

// ----------------------------------------------------------------------
// Delete the existing nlst.tmp file in order to prevent wrong requests
// in case the directory listings is not retrived correctly.
// ----------------------------------------------------------------------
   sprintf (Temp, "%s%s\\", Cfg->UsersHomePath, User->MailBox);
   BuildPath (Temp);
   strcat (Temp, "nlst.tmp");
   unlink (Temp);

// ----------------------------------------------------------------------
// Set the data type to ASCII in order to receive the list of file
// names to receive in the Multiple GET command.
// ----------------------------------------------------------------------
   Tcp->SendBytes ((UCHAR *)"TYPE A\r\n", 8);
   do {
      i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
   } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

   if (Embedded->AbortSession () == FALSE && (Data = new TTcpip) != NULL) {
// ----------------------------------------------------------------------
// Retrive the file names that matches the name requested by the user.
// The NSLT command is used because we only need the name of the files.
// ----------------------------------------------------------------------
      Data->Initialize (DataPort);

      sprintf (Temp, "PORT %lu,%lu,%lu,%lu,%u,%u\r\n", (Data->HostID & 0xFF000000L) >> 24, (Data->HostID & 0xFF0000L) >> 16, (Data->HostID & 0xFF00L) >> 8, Data->HostID & 0xFFL, (DataPort & 0xFF00) >> 8, DataPort & 0xFF);
      Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
      do {
         i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
      } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
      if (i == 200) {
         if ((p = strtok (pszFile, " ")) != NULL) {
            sprintf (Temp, "NLST %s\r\n", p);
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else
            Tcp->SendBytes ((UCHAR *)"NLST\r\n", 6);
         do {
            i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
         } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
         if (i == 150) {
#if defined(__LINUX__)
            sprintf (Temp, "%s%s/nlst.tmp", Cfg->UsersHomePath, User->MailBox);
#else
            sprintf (Temp, "%s%s\\nlst.tmp", Cfg->UsersHomePath, User->MailBox);
#endif
            FTP_GET (Temp, NULL, FALSE);
            do {
               i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
            } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
         }
      }

      DataPort += 2;
      if (DataPort > FTPDATA_PORT + 1024)
         DataPort = FTPDATA_PORT;

      Data->ClosePort ();
      delete Data;
   }

#if defined(__LINUX__)
   sprintf (Temp, "%s%s/nlst.tmp", Cfg->UsersHomePath, User->MailBox);
#else
   sprintf (Temp, "%s%s\\nlst.tmp", Cfg->UsersHomePath, User->MailBox);
#endif
   if ((fp = _fsopen (Temp, "rt", SH_DENYNO)) != NULL) {
// ----------------------------------------------------------------------
// If the file names list was successfully retrived we can read one line
// at a time and request the corresponding file from the remote host.
// ----------------------------------------------------------------------
      if (Binary == TRUE)
         Tcp->SendBytes ((UCHAR *)"TYPE I\r\n", 8);
      else
         Tcp->SendBytes ((UCHAR *)"TYPE A\r\n", 8);
      do {
         i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
      } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

      while (Embedded->AbortSession () == FALSE && fgets (File, sizeof (File) - 1, fp) != NULL) {
// ----------------------------------------------------------------------
// This is the file request/receive loop that run until every file was
// retrived or the user drops the carrier (why wasting time, network
// resources and disk space if the user is gone ???).
// ----------------------------------------------------------------------
         if ((p = strchr (File, 0x0A)) != NULL)
            *p = '\0';

         if ((Data = new TTcpip) != NULL) {
            Data->Initialize (DataPort);

            sprintf (Temp, "PORT %lu,%lu,%lu,%lu,%u,%u\r\n", (Data->HostID & 0xFF000000L) >> 24, (Data->HostID & 0xFF0000L) >> 16, (Data->HostID & 0xFF00L) >> 8, Data->HostID & 0xFFL, (DataPort & 0xFF00) >> 8, DataPort & 0xFF);
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            do {
               i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
               Embedded->Printf ("\x16\x01\x07%s\n", Temp);
            } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
            if (i == 200) {
               sprintf (Temp, "RETR %s\r\n", File);
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               do {
                  i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                  Embedded->Printf ("\x16\x01\x07%s\n", Temp);
               } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
               if (i == 150) {
                  Log->Write ("+%s", &Temp[4]);
#if defined(__LINUX__)
                  sprintf (Temp, "%s%s/%s", Cfg->UsersHomePath, User->MailBox, File);
#else
                  sprintf (Temp, "%s%s\\%s", Cfg->UsersHomePath, User->MailBox, File);
#endif
                  FTP_GET (Temp, File, Hash);
                  do {
                     i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                     Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                  } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
               }
            }

            DataPort += 2;
            if (DataPort > FTPDATA_PORT + 1024)
               DataPort = FTPDATA_PORT;

            Data->ClosePort ();
            delete Data;
         }
      }

      fclose (fp);

#if defined(__LINUX__)
      sprintf (Temp, "%s%s/nlst.tmp", Cfg->UsersHomePath, User->MailBox);
#else
      sprintf (Temp, "%s%s\\nlst.tmp", Cfg->UsersHomePath, User->MailBox);
#endif
      unlink (Temp);
   }
}

VOID TInternet::FTP_PUT (PSZ pszFile, USHORT fHash, USHORT fBinary)
{
   FILE *fp;
   USHORT i;
   ULONG Size, Elapsed;

   while (Data->WaitClient () == 0) {
      if (Embedded->AbortSession () == TRUE || Tcp->Carrier () == FALSE)
         return;
#if defined(__OS2__)
      DosSleep (1L);
#elif defined(__NT__)
      Sleep (1L);
#endif
   }

   if (pszFile != NULL)
      fp = _fsopen (pszFile, (fBinary == TRUE) ? "rb" : "rt", SH_DENYNO);

   Size = 0;
   Elapsed = time (NULL);

   do {
      i = (USHORT)fread (Buffer, 1, sizeof (Buffer), fp);
      Data->BufferBytes (Buffer, i);
      Size += i;
      if (fHash == TRUE && Size != 0L && (Size % 1024L) == 0L)
         Embedded->Putch ((UCHAR)'#');
#if defined(__OS2__)
      DosSleep (1L);
#elif defined(__NT__)
      Sleep (1L);
#endif
   } while (Data->Carrier () == TRUE && i == sizeof (Buffer) && Embedded->AbortSession () == FALSE);

   if (Embedded->AbortSession () == FALSE) {
      if (Data->Carrier () == TRUE)
         Data->UnbufferBytes ();

      if (fHash == TRUE && Size >= 1024L)
         Embedded->Printf ("\n");
      if (pszFile != NULL && fp != NULL)
         fclose (fp);

      if ((Elapsed = time (NULL) - Elapsed) == 0L)
         Elapsed = 1L;
      Embedded->Printf ("%lu bytes sent in %lu seconds (%lu bytes/sec.).\n", Size, Elapsed, Size / Elapsed);
   }

   Data->ClosePort ();
}

VOID TInternet::FTP (PSZ pszServer, USHORT usPort)
{
   USHORT i;
   CHAR *p, *Local, *Old, *New;

   if (ValidateKey ("bbs", NULL, NULL) == KEY_BASIC) {
      Embedded->Printf ("\n\x16\x01\015Sorry, command avalable only in the ADVANCED and PROFESSIONAL version\006\007\006\007");
      return;
   }

   Binary = Hash = TRUE;
   DataPort = FTPDATA_PORT;

   if (pszServer == NULL || pszServer[0] == '\0') {
      Embedded->Printf ("\n\x16\x01\013Enter the host name, or RETURN to exit: \026\001\x1E");
      Embedded->GetString (Host, (USHORT)(sizeof (Host) - 1), INP_FIELD);
      pszServer = Host;
   }

   if (Embedded->AbortSession () == FALSE && Host[0] != '\0') {
      if ((Tcp = new TTcpip) != NULL) {
         if (Tcp->ConnectServer (pszServer, usPort) == TRUE) {
            if (Log != NULL)
               Log->Write ("+Opening FTP connection to %s", pszServer);

            do {
               i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
               Embedded->Printf ("\x16\x01\x07%s\n", Temp);
            } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

            if (i == 220) {
               Embedded->Printf ("\x16\x01\013Username: ");
               Embedded->GetString (Cmd, (USHORT)(sizeof (Cmd) - 1), 0);
               sprintf (Temp, "USER %s\r\n", Cmd);
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               if (Log != NULL)
                  Log->Write ("   %s", Temp);
               if (Embedded->AbortSession () == FALSE)
                  do {
                     i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                     Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                  } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

               if (i == 331) {
                  Embedded->Printf ("\x16\x01\013Password: ");
                  Embedded->GetString (Cmd, (USHORT)(sizeof (Cmd) - 1), INP_PWD);
                  sprintf (Temp, "PASS %s\r\n", Cmd);
                  Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                  if (Log != NULL)
                     Log->Write ("   %s", Temp);
                  if (Embedded->AbortSession () == FALSE)
                     do {
                        i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                        Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                     } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
               }
            }

            if (i == 230) {
               while (Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE) {
                  Embedded->Printf ("\x16\x01\013ftp> ");
                  Embedded->GetString (Cmd, (USHORT)(sizeof (Cmd) - 1), 0);

                  if ((p = strtok (Cmd, " ")) != NULL) {
                     if (!stricmp (p, "?") || !stricmp (p, "help"))
                        Embedded->DisplayFile ("ftphelp");
                     else if (!stricmp (p, "account")) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           sprintf (Temp, "ACCT %s\r\n", p);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                        }
                     }
                     else if (!stricmp (p, "ascii")) {
                        Binary = FALSE;
                        Embedded->Printf ("\x16\x01\007200 Type set to A.\n");
                     }
                     else if (!strnicmp (p, "bin", 3) || !stricmp (p, "image")) {
                        Binary = TRUE;
                        Embedded->Printf ("\x16\x01\007200 Type set to I.\n");
                     }
                     else if (!stricmp (p, "cd")) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           sprintf (Temp, "CWD %s\r\n", p);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                        }
                     }
                     else if (!stricmp (p, "cdup")) {
                        Tcp->SendBytes ((UCHAR *)"CDUP\r\n", 6);
                        do {
                           i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                           Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                        } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                     }
                     else if (!strnicmp (p, "del", 3)) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           sprintf (Temp, "DELE %s\r\n", p);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                        }
                     }
                     else if (!stricmp (p, "dir") || !stricmp (p, "ls")) {
                        Tcp->SendBytes ((UCHAR *)"TYPE A\r\n", 8);
                        do {
                           i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                        } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

                        if ((Data = new TTcpip) != NULL) {
                           Data->Initialize (DataPort);

                           sprintf (Temp, "PORT %lu,%lu,%lu,%lu,%u,%u\r\n", (Data->HostID & 0xFF000000L) >> 24, (Data->HostID & 0xFF0000L) >> 16, (Data->HostID & 0xFF00L) >> 8, Data->HostID & 0xFFL, (DataPort & 0xFF00) >> 8, DataPort & 0xFF);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                           if (i == 200) {
                              if ((p = strtok (NULL, " ")) != NULL) {
                                 sprintf (Temp, "LIST %s\r\n", p);
                                 Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              }
                              else
                                 Tcp->SendBytes ((UCHAR *)"LIST\r\n", 6);
                              do {
                                 i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                 Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                              } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              if (i == 150) {
                                 FTP_GET (NULL, NULL, FALSE);
                                 do {
                                    i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                    Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                                 } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              }
                           }

                           DataPort += 2;
                           if (DataPort > FTPDATA_PORT + 1024)
                              DataPort = FTPDATA_PORT;

                           Data->ClosePort ();
                           delete Data;
                        }
                     }
                     else if (!stricmp (p, "get") || !stricmp (p, "retr")) {
                        if (Binary == TRUE)
                           Tcp->SendBytes ((UCHAR *)"TYPE I\r\n", 8);
                        else
                           Tcp->SendBytes ((UCHAR *)"TYPE A\r\n", 8);
                        do {
                           i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                        } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

                        if ((Data = new TTcpip) != NULL) {
                           Data->Initialize (DataPort);

                           sprintf (Temp, "PORT %lu,%lu,%lu,%lu,%u,%u\r\n", (Data->HostID & 0xFF000000L) >> 24, (Data->HostID & 0xFF0000L) >> 16, (Data->HostID & 0xFF00L) >> 8, Data->HostID & 0xFFL, (DataPort & 0xFF00) >> 8, DataPort & 0xFF);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                           if (i == 200 && (p = strtok (NULL, " ")) != NULL) {
                              sprintf (Temp, "RETR %s\r\n", p);
                              Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              do {
                                 i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                 Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                              } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              if (i == 150) {
                                 Local = p;
                                 if ((p = strtok (NULL, " ")) != NULL)
                                    Local = p;
#if defined(__LINUX__)
                                 sprintf (Temp, "%s%s/", Cfg->UsersHomePath, User->MailBox);
#else
                                 sprintf (Temp, "%s%s\\", Cfg->UsersHomePath, User->MailBox);
#endif
                                 BuildPath (Temp);
                                 strcat (Temp, Local);
                                 FTP_GET (Temp, Local, Hash);
                                 do {
                                    i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                    Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                                 } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              }
                           }

                           DataPort += 2;
                           if (DataPort > FTPDATA_PORT + 1024)
                              DataPort = FTPDATA_PORT;

                           Data->ClosePort ();
                           delete Data;
                        }
                     }
                     else if (!stricmp (p, "hash")) {
                        if (Hash == TRUE) {
                           Hash = FALSE;
                           Embedded->Printf ("\x16\x01\x07Hash printing off.\n");
                        }
                        else {
                           Hash = TRUE;
                           Embedded->Printf ("\x16\x01\x07Hash mark printing on (1024 bytes/hash mark).\n");
                        }
                     }
                     else if (!stricmp (p, "mget")) {
                        if ((p = strtok (NULL, " ")) != NULL)
                           FTP_MGET (p);
                     }
                     else if (!stricmp (p, "mkdir") || !stricmp (p, "md")) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           sprintf (Temp, "MKD %s\r\n", p);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                        }
                     }
                     else if (!stricmp (p, "put") || !stricmp (p, "stor")) {
                        if (Binary == TRUE)
                           Tcp->SendBytes ((UCHAR *)"TYPE I\r\n", 8);
                        else
                           Tcp->SendBytes ((UCHAR *)"TYPE A\r\n", 8);
                        do {
                           i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                        } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

                        if ((Data = new TTcpip) != NULL) {
                           Data->Initialize (DataPort);

                           sprintf (Temp, "PORT %lu,%lu,%lu,%lu,%u,%u\r\n", (Data->HostID & 0xFF000000L) >> 24, (Data->HostID & 0xFF0000L) >> 16, (Data->HostID & 0xFF00L) >> 8, Data->HostID & 0xFFL, (DataPort & 0xFF00) >> 8, DataPort & 0xFF);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                           if (i == 200 && (p = strtok (NULL, " ")) != NULL) {
                              sprintf (Temp, "STOR %s\r\n", p);
                              Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              do {
                                 i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                 Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                              } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              if (i == 150) {
                                 Local = p;
                                 if ((p = strtok (NULL, " ")) != NULL)
                                    Local = p;
                                 sprintf (Temp, "%s%s\\%s", Cfg->UsersHomePath, User->MailBox, Local);
                                 FTP_PUT (Temp, Hash, Binary);
                                 do {
                                    i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                    Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                                 } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              }
                           }

                           DataPort += 2;
                           if (DataPort > FTPDATA_PORT + 1024)
                              DataPort = FTPDATA_PORT;

                           Data->ClosePort ();
                           delete Data;
                        }
                     }
                     else if (!stricmp (p, "pwd")) {
                        Tcp->SendBytes ((UCHAR *)"PWD\r\n", 5);
                        do {
                           i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                           Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                        } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                     }
                     else if (!stricmp (p, "quit") || !stricmp (p, "bye")) {
                        Tcp->SendBytes ((UCHAR *)"QUIT\r\n", 6);
                        do {
                           GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                           Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                        } while (Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                     }
                     else if (!stricmp (p, "remotehelp")) {
                        Tcp->SendBytes ((UCHAR *)"HELP\r\n", 6);
                        do {
                           i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                           Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                        } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                     }
                     else if (!strnicmp (p, "ren", 3)) {
                        if ((Old = strtok (NULL, " ")) != NULL) {
                           if ((New = strtok (NULL, " ")) != NULL) {
                              sprintf (Temp, "RNFR %s\r\n", Old);
                              Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              do {
                                 i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                 Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                              } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              sprintf (Temp, "RNTO %s\r\n", New);
                              Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              do {
                                 i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                 Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                              } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                           }
                        }
                     }
                     else if (!stricmp (p, "rmdir") || !stricmp (p, "rd")) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           sprintf (Temp, "RMD %s\r\n", p);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                        }
                     }
                     else if (!stricmp (p, "site")) {
                        if ((p = strtok (NULL, "")) != NULL) {
                           while (*p == ' ')
                              p++;
                           sprintf (Temp, "SITE %s\r\n", p);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                        }
                     }
                     else if (!stricmp (p, "SYST")) {
                        Tcp->SendBytes ((UCHAR *)"SYST\r\n", 6);
                        do {
                           i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                           Embedded->Printf ("\x16\x01\x07%s\n", Temp);
                        } while (i == 0 && Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                     }
                  }
               }
            }
            Log->Write (":Closing FTP connection");
         }

         Tcp->ClosePort ();
         delete Tcp;
      }
   }

   sprintf (Temp, "%s%s", Cfg->UsersHomePath, User->MailBox);
   rmdir (Temp);
}


