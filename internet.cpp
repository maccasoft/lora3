
// LoraBBS Version 2.99 Free Edition
// Copyright (C) 1987-98 Marco Maccaferri
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

   if (pszServer == NULL || pszServer[0] == '\0') {
      Embedded->Printf ("\n\026\001\017Host name? ");
      Embedded->GetString (Host, (USHORT)(sizeof (Host) - 1), 0);
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

   if (pszServer == NULL || pszServer[0] == '\0') {
      Embedded->Printf ("\n\026\001\017Host name? ");
      Embedded->GetString (Host, (USHORT)(sizeof (Host) - 1), 0);
      pszServer = Host;
   }

   if (Embedded->AbortSession () == FALSE && Host[0] != '\0') {
      if ((Tcp = new TTcpip) != NULL) {
         if (Tcp->ConnectServer (pszServer, usPort) == TRUE) {
            Log->Write ("+Finger => %s", pszServer);

            Embedded->Printf ("\n\026\001\017User name? ");
            Embedded->GetString (String, (USHORT)(sizeof (String) - 1), 0);

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
   CHAR Name[64], *p;
   ULONG Size, Elapsed;

   while (Data->WaitClient () == 0) {
      if (Embedded->AbortSession () == TRUE || Tcp->Carrier () == FALSE)
         return;
   }

   if (pszFile != NULL) {
      if (Log != NULL)
         Log->Write (" Receiving %s", pszFile);
      if ((fp = _fsopen (pszFile, "wb", SH_DENYNO)) == NULL) {
         if ((p = strchr (pszFile, '\0')) != NULL) {
            while (p > pszFile && *p != '\\' && *p != ':' && *p != '/')
               p--;
            strcpy (Name, ++p);
            Name[12] = '\0';
            while ((p = strchr (Name, '.')) != NULL)
               *p = '_';
            if (strlen (Name) > 8)
               Name[8] = '.';
            strcpy (p, Name);
         }
      }
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
      }
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
      User->FileTag->New ();
      strcpy (User->FileTag->Name, pszName);
      strcpy (User->FileTag->Area, "USER");
      strcpy (User->FileTag->Complete, pszFile);
      User->FileTag->Size = Size;
      User->FileTag->DeleteAfter = TRUE;
      User->FileTag->Add ();
      Embedded->Printf ("\x16\x01\x0AThe file %s is tagged for download\n", pszName);
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
   }

   if (pszFile != NULL)
//kj      fp = _fsopen (pszFile, (fBinary == TRUE) ? "rb" : "rt", SH_DENYNO);
          fp = _fsopen (pszFile, "rb", SH_DENYNO);


   Size = 0;
   Elapsed = time (NULL);

   do {
      i = (USHORT)fread (Buffer, 1, sizeof (Buffer), fp);
      Data->BufferBytes (Buffer, i);
      Size += i;
      if (fHash == TRUE && Size != 0L && (Size % 1024L) == 0L)
         Embedded->Putch ((UCHAR)'#');
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

   Binary = Hash = TRUE;
   DataPort = FTPDATA_PORT;

   if (pszServer == NULL || pszServer[0] == '\0') {
      Embedded->Printf ("\n\026\001\017Host name? ");
      Embedded->GetString (Host, (USHORT)(sizeof (Host) - 1), 0);
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

VOID TInternet::IRC (PSZ pszServer, PSZ pszNick, USHORT usPort)
{
   int i;
   USHORT Refresh, noctcp, Loop;
   CHAR Temp[128], Nick[32], Object[64], *tempptr;
   CHAR InLine[128], OutLine[128], *inptr, c, *p;
   CHAR *FromHost;

   if (pszServer == NULL || pszServer[0] == '\0') {
      Embedded->Printf ("\n\026\001\017Host name? ");
      Embedded->GetString (Host, (USHORT)(sizeof (Host) - 1), 0);
      pszServer = Host;
   }
   if (pszServer[0] != '\0' && (pszNick == NULL || pszNick[0] == '\0')) {
      Embedded->Printf ("\026\001\017Nickname? ");
      Embedded->GetString (Nick, (USHORT)(sizeof (Nick) - 1), 0);
      pszNick = Nick;
   }

   if ((p = strchr (pszServer, ':')) != NULL) {
      *p++ = '\0';
      usPort = (USHORT)atoi (p);
   }

   Loop = TRUE;
   if (Loop == TRUE && Embedded->AbortSession () == FALSE && pszServer[0] != '\0' && pszNick[0] != '\0') {
      if ((Tcp = new TTcpip) != NULL) {
         if (Log != NULL)
            Log->Write ("+Opening IRC connection to %s", pszServer);

         if (Tcp->ConnectServer (pszServer, usPort) == TRUE) {
            sprintf (OutLine, "NICK %s\r\n", pszNick);
            Tcp->SendBytes ((UCHAR *)OutLine, (USHORT)strlen (OutLine));
            sprintf (OutLine, "USER %s %s %s :%s\r\n", User->MailBox, Cfg->HostName, pszServer, pszNick);
            Tcp->SendBytes ((UCHAR *)OutLine, (USHORT)strlen (OutLine));

            inptr = InLine;
            strcpy (Object, "*");
            FromHost = "";
            tempptr = Temp;
            Temp[0] = '\0';
            Refresh = TRUE;

            while (Embedded->AbortSession () == FALSE && Tcp->Carrier () == TRUE) {
               while (Tcp->BytesReady () == TRUE) {
                  c = (CHAR)Tcp->ReadByte ();
                  if (c == '\n' || c == '\r') {
                     *inptr = '\0';
                     Refresh = TRUE;
                     inptr = InLine;

                     if (Log != NULL)
                        Log->Write (">  %s", InLine);
                     if (InLine[0] != '\0')
                        Embedded->Printf ("\r\026\001\x07\026\007");

                     // Risponde al comando ping con un pong.
                     if (!strncmp (InLine, "PI", 2)) {
                        memcpy (InLine, "PO", 2);
                        strcat (InLine, "\r\n");
                        Tcp->SendBytes ((UCHAR *)InLine, (USHORT)strlen (InLine));
                     }
                     // Messaggio normale.
                     else if (InLine[0] != ':' && InLine[0] != '\0')
                        Embedded->Printf ("\r%s\n", InLine);
                     // Parsing dei messaggi del server.
                     else {
                        i = 0;
                        if ((p = strtok (InLine, " ")) != NULL)
                           do {
                              Tokens[i++] = p;
                           } while ((p = strtok (NULL, " ")) != NULL);
                        Tokens[i] = NULL;

                        if (i > 1) {
                        if ((p = strchr (Tokens[0], '!')) != NULL) {
                           *p = '\0';
                           FromHost = p + 1;
                        }
                        if (atoi (Tokens[1]) != 0) {
                           switch (atoi (Tokens[1])) {
                              case 352:
                                 Embedded->BufferedPrintf ("\r%-15s %-10s %3s %s@%s", Tokens[3], Tokens[7], Tokens[8], Tokens[4], Tokens[5]);
                                 for (i = 9; Tokens[i] != NULL; i++)
                                    Embedded->BufferedPrintf (" %s", Tokens[i]);
                                 Embedded->Printf ("\n");
                                 break;
                              default:
                                 Embedded->BufferedPrintf ("\r\026\001\007%s", &Tokens[3][1]);
                                 for (i = 4; Tokens[i] != NULL; i++)
                                    Embedded->BufferedPrintf (" %s", Tokens[i]);
                                 Embedded->Printf ("\n");
                                 break;
                           }
                        }
                        else if (Tokens[1][0] == 'P') {
                           if (Tokens[1][1] == 'R') {
                              i = 4;
                              noctcp = 0;
                              if (Tokens[3][1] == '\001') {
                                 while ((p = strchr (Tokens[0], '\001')) != NULL)
                                    *p = ' ';
                                 if (Tokens[3][2] == 'A') {
                                    Embedded->BufferedPrintf ("\r*** ACTION: %s", Tokens[0]);
                                    noctcp = 1;
                                 }
//                                 else {
//                                    while ((p = strchr (Tokens[3], '\001')) != NULL)
//                                       *p = ' ';
//                                    Embedded->BufferedPrintf ("\r*** got CTCP %s from %s", &Tokens[3][2], Tokens[0]);
//                                 }
                              }
                              else {
                                 if (Tokens[2][0] != '#') {
                                    Embedded->BufferedPrintf ("\r*%s*", &Tokens[0][1]);
                                    i = 3;
                                    noctcp = 1;
                                 }
                                 else {
                                    Log->Write (">%s - %s", Object, Tokens[2]);
                                    if (stricmp (Object, Tokens[2]))
                                       Embedded->BufferedPrintf ("\r<%s:%s>", &Tokens[0][1], Tokens[2]);
                                    else
                                       Embedded->BufferedPrintf ("\r<%s>", &Tokens[0][1]);
                                    Embedded->BufferedPrintf ("%s", &Tokens[3][1]);
                                    i = 4;
                                    noctcp = 1;
                                 }
                              }
                              for (; Tokens[i] != NULL; i++)
                                 Embedded->BufferedPrintf (" %s", Tokens[i]);
                              Embedded->Printf ("\n");

                              if (noctcp == 0) {
                                 if (Tokens[3][2] == 'V') {
                                    sprintf (OutLine, "NOTICE %s \001VERSION %s %s *IX :1024 bytes\001\r\n", Tokens[0], NAME, VERSION);
                                    Tcp->SendBytes ((UCHAR *)OutLine, (USHORT)strlen (OutLine));
                                 }
                                 else if (Tokens[3][2] == 'P') {
                                    sprintf (OutLine, "NOTICE %s \001PID %d\001\r\n", Tokens[0], 1);
                                    Tcp->SendBytes ((UCHAR *)OutLine, (USHORT)strlen (OutLine));
                                 }
                                 else if (Tokens[3][2] == 'F') {
                                    sprintf (OutLine, "NOTICE %s \001FINGER %s (%s@%s) Idle %d seconds\001\r\n", Tokens[0], "Unix", User->MailBox, Cfg->HostName, 0);
                                    Tcp->SendBytes ((UCHAR *)OutLine, (USHORT)strlen (OutLine));
                                 }
                                 else if (Tokens[3][2] == 'C') {
                                    sprintf (OutLine, "NOTICE %s \001CLIENTINFO VERSION FINGER PID ERRMSG\001\r\n", Tokens[0]);
                                    Tcp->SendBytes ((UCHAR *)OutLine, (USHORT)strlen (OutLine));
                                 }
                                 else if (Tokens[3][2] == 'E') {
                                    sprintf (OutLine, "NOTICE %s \001%s %s %s %s\001\r\n", Tokens[0], Tokens[3], Tokens[4], Tokens[5], Tokens[6]);
                                    Tcp->SendBytes ((UCHAR *)OutLine, (USHORT)strlen (OutLine));
                                 }
                                 else if (Tokens[3][2] == 'D') {
                                    sprintf (OutLine, "NOTICE %s \001DCC not supported\001\r\n", Tokens[0]);
                                    Tcp->SendBytes ((UCHAR *)OutLine, (USHORT)strlen (OutLine));
                                 }
                                 else {
                                    sprintf (OutLine, "NOTICE %s \001ERRMSG I'm sorry dave, I'm afraid I can't do that\001\r\n", Tokens[0]);
                                    Tcp->SendBytes ((UCHAR *)OutLine, (USHORT)strlen (OutLine));
                                 }
                              }
                           }
                           else {
                              if (!strcmp (Tokens[0], pszNick)) {
                                 if (!strcmp (Object, Tokens[2]))
                                    strcpy (Object, "*");
                                 Embedded->Printf ("\r*** You have left %s\n", Tokens[2]);
                              }
                              else
                                 Embedded->Printf ("\r*** %s (%s) has left channel %s\n", Tokens[0], FromHost, Tokens[2]);
                           }
                        }
                        else if (Tokens[1][0] == 'N') {
                           if (Tokens[1][1] == 'O') {
                              i = 3;
                              if (Tokens[3][1] == '\01')
                                 Embedded->BufferedPrintf ("\r*** CTCP %s reply from %s:", &Tokens[i++][2], Tokens[0]);
                              else if (strchr (Tokens[0], '.') == NULL)
                                 Embedded->BufferedPrintf ("\r\026\001\004-%s-", &Tokens[0][1]);
                              for (; Tokens[i] != NULL; i++)
                                 Embedded->BufferedPrintf (" %s", Tokens[i]);
                              Embedded->Printf ("\n");
                           }
                           else {
                              if (!strcmp (&Tokens[0][1], pszNick)) {
                                 strcpy (Nick, &Tokens[2][1]);
                                 pszNick = Nick;
                                 Embedded->Printf ("\r\026\001\002*** You have changed your nickname to %s\n", pszNick);
                              }
                              else
                                 Embedded->Printf ("\r\026\001\002*** %s is now known as %s\n", &Tokens[0][1], &Tokens[2][1]);
                           }
                        }
                        else if (Tokens[1][0] == 'J') {
                           if (!strcmp (&Tokens[0][1], pszNick)) {
                              strcpy (Object, &Tokens[2][1]);
                              Embedded->Printf ("\r\026\001\002*** Current channel is %s\n", Object);
                              if (Log != NULL)
                                 Log->Write ("+ Current channel is %s\n", Object);
                           }
                           else
                              Embedded->Printf ("\r\026\001\002*** %s (%s) has joined %s\n", &Tokens[0][1], FromHost, &Tokens[2][1]);
                        }
                        else if (Tokens[1][0] == 'Q') {
                           Embedded->BufferedPrintf ("\r*** %s quit", Tokens[0]);
                           for (i = 2; Tokens[i] != NULL; i++)
                              Embedded->BufferedPrintf (" %s", Tokens[i]);
                           Embedded->Printf ("\n");
                        }
                        else if (Tokens[1][0] == 'T') {
                           Embedded->BufferedPrintf ("\r\026\001\001*** %s changed the %s topic to", &Tokens[0][1], Tokens[2]);
                           for (i = 3; Tokens[i] != NULL; i++)
                              Embedded->BufferedPrintf (" %s", Tokens[i]);
                           Embedded->Printf ("\n");
                        }
                        else if (Tokens[1][0] == 'I')
                           Embedded->BufferedPrintf ("\r*** %s invited you to %s\n", Tokens[0], Tokens[3]);
                        else if (Tokens[1][0] == 'M') {
                           Embedded->BufferedPrintf ("\r*** %s changed %s to \"%s", Tokens[0], Tokens[2], Tokens[3]);
                           for (i = 4; Tokens[i] != NULL; i++)
                              Embedded->BufferedPrintf (" %s", Tokens[i]);
                           Embedded->Printf ("\"\n");
                        }
                        else if (Tokens[1][0] == 'K')
                           Embedded->BufferedPrintf ("\r*** %s kicked %s from %s\n", Tokens[0], Tokens[3], Tokens[2]);
                        else if (Tokens[1][0] == 'E') {
                           Embedded->BufferedPrintf ("\r*** ERROR:");
                           for (i = 2; Tokens[i] != NULL; i++)
                              Embedded->BufferedPrintf (" %s", Tokens[i]);
                           Embedded->Printf ("\n");
                        }
                        else
                           Refresh = FALSE;
                        }
                     }
                  }
                  else if (c != '\r' && c != '\n')
                     *inptr++ = c;
               }

               if (Refresh == TRUE) {
                  Embedded->Printf ("\r\026\001\x70\026\007> %s", Temp);
                  Refresh = FALSE;
               }

               if (Embedded->KBHit () == TRUE) {
                  c = (CHAR)Embedded->Getch ();
                  if (c == '\r') {
                     if (Temp[0] == '/') {
                        Embedded->Printf ("\r\026\001\007\026\007%s\n", Temp);
                        if (toupper (Temp[1]) == 'Q') {
                           strcat (Temp, " :lorabbs - exit");
                           Loop = FALSE;
                        }
                        Tcp->BufferBytes ((UCHAR *)&Temp[1], (USHORT)strlen (&Temp[1]));
                     }
                     else {
                        Embedded->Printf ("\r\026\001\007\026\007<%s> %s\n", pszNick, Temp);
                        Tcp->BufferBytes ((UCHAR *)"PRIVMSG ", 8);
                        Tcp->BufferBytes ((UCHAR *)Object, (USHORT)strlen (Object));
                        Tcp->BufferBytes ((UCHAR *)" :", 2);
                        Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                     }
                     Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                     Tcp->UnbufferBytes ();
                     tempptr = Temp;
                     *tempptr = '\0';
                     Refresh = TRUE;
                  }
                  else if (c == 0x08) {
                     if (tempptr > Temp) {
                        Embedded->Printf ("\x08 \x08");
                        tempptr--;
                        *tempptr = '\0';
                     }
                  }
                  else if (c >= ' ') {
                     *tempptr++ = c;
                     *tempptr = '\0';
                     Embedded->Putch (c);
                  }
               }
            }
         }
         else {
            Embedded->Printf ("\n\026\001\014Connection refused.");
            if (Log != NULL)
               Log->Write ("!Connection refused");
         }

         delete Tcp;
      }
   }
}


