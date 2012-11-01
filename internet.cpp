
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.15
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "internet.h"

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
#define ECHO               1
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

TInternet::TInternet (class TBbs *bbs)
{
   Bbs = bbs;
   Cfg = bbs->Cfg;
   Com = bbs->Com;
   Lang = bbs->Lang;
   Log = bbs->Log;
   Snoop = bbs->Snoop;
   User = bbs->User;
}

TInternet::~TInternet (void)
{
}

VOID TInternet::Telnet (PSZ pszServer, USHORT usPort)
{
   USHORT c, Exit;
   CHAR Temp[30];

   if (pszServer == NULL || pszServer[0] == '\0') {
      Bbs->Printf ("\n\x16\x01\013Enter the host name, or RETURN to exit: \026\001\x1E");
      Bbs->GetString (Temp, (USHORT)(sizeof (Temp) - 1), INP_FIELD);
      pszServer = Temp;
   }

   if (Bbs->AbortSession () == FALSE && Temp[0] != '\0') {
      Bbs->Printf ("\nTrying...");

      if ((Tcp = new TTcpip) != NULL) {
         if (Tcp->ConnectServer (pszServer, usPort) == TRUE) {
            Log->Write ("+Telnet to %s", pszServer);

            Bbs->Printf ("\014Connected to %s.\nEscape character is '^]'\n\n", pszServer);

            Tcp->BufferByte (IAC);
            Tcp->BufferByte (DOTEL);
            Tcp->BufferByte (ECHO);
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
                              if (c == BINARY || c == ECHO && c != SGA)
                                 Tcp->BufferByte (DOTEL);
                              else
                                 Tcp->BufferByte (DONTTEL);
                              Tcp->BufferByte ((UCHAR)c);
                              Tcp->UnbufferBytes ();
                           }
                           else if (c == WONTTEL) {
                              c = Tcp->ReadByte ();
                              if (c != BINARY && c != ECHO && c != SGA) {
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
                              else if (c != BINARY && c != ECHO && c != SGA) {
                                 Tcp->BufferByte (IAC);
                                 Tcp->BufferByte (WONTTEL);
                                 Tcp->BufferByte ((UCHAR)c);
                              }
                              Tcp->UnbufferBytes ();
                           }
                           else if (c == DONTTEL) {
                              c = Tcp->ReadByte ();
                              if (c != BINARY && c != ECHO && c != SGA) {
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
                     }
                  } while (Tcp->RxBytes > 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE && Exit == FALSE);

                  Com->UnbufferBytes ();
                  if (Snoop != NULL)
                     Snoop->UnbufferBytes ();
               }

               if (Com->BytesReady () == TRUE) {
                  do {
                     c = Com->ReadByte ();
                     if (c == 0x1D) {
                        Tcp->UnbufferBytes ();
                        Bbs->Printf ("\n");
                        do {
                           Bbs->Printf ("\x16\x01\013telnet> ");
                           Bbs->GetString (Temp, (USHORT)(sizeof (Temp) - 1), 0);
                           if (!stricmp (Temp, "quit")) {
                              if (Tcp->Carrier () == TRUE)
                                 Tcp->ClosePort ();
                              Exit = TRUE;
                           }
                           else if (!stricmp (Temp, "close")) {
                              Tcp->ClosePort ();
                              Bbs->Printf ("Connection closed.\n");
                           }
                        } while (Temp[0] != '\0' && Bbs->AbortSession () == FALSE);
                     }
                     else {
                        Tcp->BufferByte ((UCHAR)c);
                        if (c == IAC)
                           Tcp->BufferByte ((UCHAR)c);
                     }
                  } while (Com->RxBytes > 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE && Exit == FALSE);

                  Tcp->UnbufferBytes ();
               }
            } while (Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE && Exit == FALSE);

            Log->Write ("+End Telnet");

            Bbs->Printf ("\n");
            Bbs->Printf ("Connection closed.\n");
         }
         delete Tcp;
      }
   }
}

VOID TInternet::VModem (PSZ pszServer, USHORT usPort)
{
   USHORT c;
   CHAR Temp[30];

   if (pszServer == NULL || pszServer[0] == '\0') {
      Bbs->Printf ("\n\x16\x01\013Enter the host name, or RETURN to exit: \026\001\x1E");
      Bbs->GetString (Temp, (USHORT)(sizeof (Temp) - 1), INP_FIELD);
      pszServer = Temp;
   }

   if (Bbs->AbortSession () == FALSE && Temp[0] != '\0') {
      if ((Tcp = new TTcpip) != NULL) {
         if (Tcp->ConnectServer (pszServer, usPort) == TRUE) {
            Log->Write ("+VModem => %s", pszServer);

            Bbs->Printf ("\nRINGING\n");

            Bbs->Printf ("\nCONNECT 57600/ARQ/VMP\n");

            do {
               if (Tcp->BytesReady () == TRUE) {
                  c = Tcp->ReadByte ();
                  Bbs->Putch ((UCHAR)c);
                  Log->Write ("+OUT: %02X", c);
               }
               if (Bbs->KBHit () == TRUE) {
                  c = Bbs->Getch ();
                  Tcp->SendByte ((UCHAR)c);
               }
            } while (Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
         }
         delete Tcp;
      }
   }
}

VOID TInternet::Finger (PSZ pszServer, USHORT usPort)
{
   CHAR Temp[30], String[24], c;

   if (pszServer == NULL || pszServer[0] == '\0') {
      Bbs->Printf ("\n\x16\x01\013Enter the host name, or RETURN to exit: \026\001\x1E");
      Bbs->GetString (Temp, (USHORT)(sizeof (Temp) - 1), INP_FIELD);
      pszServer = Temp;
   }

   if (Bbs->AbortSession () == FALSE && Temp[0] != '\0') {
      if ((Tcp = new TTcpip) != NULL) {
         if (Tcp->ConnectServer (pszServer, usPort) == TRUE) {
            Log->Write ("+Finger => %s", pszServer);

            Bbs->Printf ("\n\x16\x01\013Enter a user name, or RETURN to list online users: \026\001\x1E");
            Bbs->GetString (String, (USHORT)(sizeof (String) - 1), INP_FIELD);

            Tcp->SendBytes ((UCHAR *)String, (USHORT)strlen (String));
            Tcp->SendBytes ((UCHAR *)"\r\n", 2);

            Bbs->Printf ("\n\x16\x01\x07");

            while (Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE) {
               if (Tcp->BytesReady () == TRUE) {
                  c = (CHAR)Tcp->ReadByte ();
                  Bbs->Putch ((UCHAR)c);
               }
            }
         }

         delete Tcp;
      }
   }
}

USHORT TInternet::GetResponse (PSZ pszResponse, USHORT usMaxLen)
{
   USHORT retVal = FALSE, len = 0;
   CHAR c, *pszResp = pszResponse;

   do {
      if (Tcp->BytesReady () == TRUE) {
         if ((c = (CHAR)Tcp->ReadByte ()) != '\r') {
            if (c != '\n') {
               *pszResp++ = c;
               if (++len >= usMaxLen)
                  c = '\r';
            }
         }
      }
      if (Bbs->KBHit () == TRUE) {
         if (Bbs->Getch () == CTRLC) {
            pszResponse[0] = '\0';
            break;
         }
      }
   } while (c != '\r' && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

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
VOID TInternet::FTP_GET (PSZ pszFile, PSZ pszName, USHORT fHash, USHORT fBinary)
{
   FILE *fp;
   USHORT Len, Counter;
   UCHAR Buffer[512];
   ULONG Size, Elapsed;

   while (Data->WaitClient () == 0) {
      if (Bbs->AbortSession () == TRUE || Tcp->Carrier () == FALSE)
         return;
   }

   if (pszFile != NULL) {
      fp = fopen (pszFile, "wb");
      Log->Write (" Receiving %s", pszFile);
   }

   Size = 0;
   Counter = 0;
   Elapsed = time (NULL);

// ----------------------------------------------------------------------
// Bytes receiving loop. This loop runs until the remote host drops the
// connection, signalling that the transmission is ended.
// ----------------------------------------------------------------------
   while (Data->Carrier () == TRUE && Bbs->AbortSession () == FALSE) {
      if (Data->BytesReady () == TRUE) {
         Len = Data->ReadBytes (Buffer, sizeof (Buffer));
         Size += Len;
// ----------------------------------------------------------------------
// This is a failsafe routine, if a file was request but the fopen fails,
// each byte is received but nothing is saved (I think that's should be
// called failsafe...).
// ----------------------------------------------------------------------
         if (pszFile == NULL) {
//            if (fBinary == FALSE && c == '\n')
//               Com->BufferByte ('\r');
//            Com->BufferByte ((CHAR)c);
            Com->BufferBytes (Buffer, Len);
            if (Snoop != NULL)
               Snoop->BufferBytes (Buffer, Len);
         }
         else if (fp != NULL) {
//            if (fBinary == FALSE && c == '\n')
//               fputc ('\r', fp);
//            fputc (c, fp);
            fwrite (Buffer, Len, 1, fp);
         }
// ----------------------------------------------------------------------
// Hash mark printing after each 1024 bytes packet received.
// ----------------------------------------------------------------------
         Counter += Len;
         if (fHash == TRUE && Counter >= 1024) {
            Bbs->Putch ((UCHAR)'#');
            Counter -= 1024;
         }
      }
   }

   Com->UnbufferBytes ();
   if (Snoop != NULL)
      Snoop->UnbufferBytes ();

   if (fHash == TRUE && Size >= 1024L)
      Bbs->Printf ("\n");
   if (pszFile != NULL && fp != NULL)
      fclose (fp);

   if ((Elapsed = time (NULL) - Elapsed) == 0L)
      Elapsed = 1L;
   Bbs->Printf ("%lu bytes received in %lu seconds (%lu bytes/sec.).\n", Size, Elapsed, Size / Elapsed);
   if (pszFile != NULL)
      Log->Write ("+Received %s, %lu bytes", pszFile, Size);

   if (pszFile != NULL && pszName != NULL) {
      User->FileTag->New ();
      strcpy (User->FileTag->Name, pszName);
      strcpy (User->FileTag->Library, "FTP");
      strcpy (User->FileTag->Complete, pszFile);
      User->FileTag->Size = Size;
      User->FileTag->DeleteAfter = TRUE;
      User->FileTag->Add ();
      Bbs->Printf ("\x16\x01\x0A%5d. The file %s in the %s Library\n", User->FileTag->Index, pszName, User->FileTag->Library);
      Log->Write (":Tagged file %s, library %s", pszName, User->FileTag->Library);
   }
}

VOID TInternet::FTP_MGET (PSZ pszFile)
{
   FILE *fp;
   USHORT i;
   CHAR Temp[128], File[128], *p;

// ----------------------------------------------------------------------
// Delete the existing nlst.tmp file in order to prevent wrong requests
// in case the directory listings is not retrived correctly.
// ----------------------------------------------------------------------
   sprintf (Temp, "%s%s\\nlst.tmp", Cfg->HomePath, User->MailBox);
   unlink (Temp);

// ----------------------------------------------------------------------
// Set the data type to ASCII in order to receive the list of file
// names to receive in the Multiple GET command.
// ----------------------------------------------------------------------
   Tcp->SendBytes ((UCHAR *)"TYPE A\r\n", 8);
   do {
      i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
   } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

   if (Bbs->AbortSession () == FALSE && (Data = new TTcpip) != NULL) {
// ----------------------------------------------------------------------
// Retrive the file names that matches the name requested by the user.
// The NSLT command is used because we only need the name of the files.
// ----------------------------------------------------------------------
      Data->Initialize (DataPort);

      sprintf (Temp, "PORT %ld,%ld,%ld,%ld,%u,%u\r\n", (Tcp->HostID & 0xFF000000L) >> 24, (Tcp->HostID & 0xFF0000L) >> 16, (Tcp->HostID & 0xFF00L) >> 8, (Tcp->HostID & 0xFFL), (DataPort & 0xFF00) >> 8, DataPort & 0xFF);
      Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
      do {
         i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
      } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
      if (i == 200) {
         if ((p = strtok (pszFile, " ")) != NULL) {
            sprintf (Temp, "NLST %s\r\n", p);
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else
            Tcp->SendBytes ((UCHAR *)"NLST\r\n", 6);
         do {
            i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
         } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
         if (i == 150) {
            sprintf (Temp, "%s%s\\nlst.tmp", Cfg->HomePath, User->MailBox);
            FTP_GET (Temp, NULL, FALSE, FALSE);
            do {
               i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
            } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
         }
      }

      DataPort++;
      if (DataPort > FTPDATA_PORT + 1024)
         DataPort = FTPDATA_PORT;

      delete Data;
   }

   sprintf (Temp, "%s%s\\nlst.tmp", Cfg->HomePath, User->MailBox);
   if ((fp = fopen (Temp, "rt")) != NULL) {
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
      } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

      while (Bbs->AbortSession () == FALSE && fgets (File, sizeof (File) - 1, fp) != NULL) {
// ----------------------------------------------------------------------
// This is the file request/receive loop that run until every file was
// retrived or the user drops the carrier (why wasting time, network
// resources and disk space if the user is gone ???).
// ----------------------------------------------------------------------
         if ((p = strchr (File, 0x0A)) != NULL)
            *p = '\0';

         if ((Data = new TTcpip) != NULL) {
            Data->Initialize (DataPort);

            sprintf (Temp, "PORT %ld,%ld,%ld,%ld,%u,%u\r\n", (Tcp->HostID & 0xFF000000L) >> 24, (Tcp->HostID & 0xFF0000L) >> 16, (Tcp->HostID & 0xFF00L) >> 8, (Tcp->HostID & 0xFFL), (DataPort & 0xFF00) >> 8, DataPort & 0xFF);
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            do {
               i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
               Bbs->Printf ("\x16\x01\x07%s\n", Temp);
            } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
            if (i == 200) {
               sprintf (Temp, "RETR %s\r\n", File);
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               do {
                  i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                  Bbs->Printf ("\x16\x01\x07%s\n", Temp);
               } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
               if (i == 150) {
                  Log->Write ("+%s", &Temp[4]);
                  sprintf (Temp, "%s%s\\%s", Cfg->HomePath, User->MailBox, File);
                  FTP_GET (Temp, File, Hash, Binary);
                  do {
                     i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                     Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                  } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
               }
            }

            DataPort++;
            if (DataPort > FTPDATA_PORT + 1024)
               DataPort = FTPDATA_PORT;

            delete Data;
         }
      }

      fclose (fp);

      sprintf (Temp, "%s%s\\nlst.tmp", Cfg->HomePath, User->MailBox);
      unlink (Temp);
   }
}

VOID TInternet::FTPsend (class TTcpip *Data, PSZ pszFile, USHORT fHash, USHORT fBinary)
{
   FILE *fp;
   USHORT i;
   UCHAR Temp[512];
   ULONG Size, Elapsed;

   while (Data->WaitClient () == 0)
      ;

   if (pszFile != NULL)
      fp = fopen (pszFile, (fBinary == TRUE) ? "rb" : "rt");

   Size = 0;
   Elapsed = time (NULL);

   do {
      i = (USHORT)fread (Temp, 1L, sizeof (Temp), fp);
      Data->BufferBytes (Temp, i);
      Size += i;
      if (fHash == TRUE && Size != 0L && (Size % 1024L) == 0L)
         Bbs->Putch ((UCHAR)'#');
   } while (Data->Carrier () == TRUE && i == sizeof (Temp));

   if (Data->Carrier () == TRUE)
      Data->UnbufferBytes ();

   if (fHash == TRUE && Size >= 1024L)
      Bbs->Printf ("\n");
   if (pszFile != NULL && fp != NULL)
      fclose (fp);

   if ((Elapsed = time (NULL) - Elapsed) == 0L)
      Elapsed = 1L;
   Bbs->Printf ("%lu bytes sent in %lu seconds (%lu bytes/sec.).\n", Size, Elapsed, Size / Elapsed);
}

VOID TInternet::FTP (PSZ pszServer, USHORT usPort)
{
   USHORT i;
   CHAR Temp[128], Cmd[50], *p, *Local, *Old, *New;

   Binary = Hash = TRUE;
   DataPort = FTPDATA_PORT;

   if (pszServer == NULL || pszServer[0] == '\0') {
      Bbs->Printf ("\n\x16\x01\013Enter the host name, or RETURN to exit: \026\001\x1E");
      Bbs->GetString (Temp, 29, INP_FIELD);
      pszServer = Temp;
   }

   if (Bbs->AbortSession () == FALSE && Temp[0] != '\0') {
      if ((Tcp = new TTcpip) != NULL) {
         if (Tcp->ConnectServer (pszServer, usPort) == TRUE) {
            Log->Write ("+FTP to %s", pszServer);

            do {
               i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
               Bbs->Printf ("\x16\x01\x07%s\n", Temp);
            } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

            if (i == 220) {
               Bbs->Printf ("\x16\x01\013Username: ");
               Bbs->GetString (Cmd, (USHORT)(sizeof (Cmd) - 1), 0);
               sprintf (Temp, "USER %s\r\n", Cmd);
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               if (Bbs->AbortSession () == FALSE)
                  do {
                     i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                     Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                  } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

               if (i == 331) {
                  Bbs->Printf ("\x16\x01\013Password: ");
                  Bbs->GetString (Cmd, (USHORT)(sizeof (Cmd) - 1), INP_PWD);
                  sprintf (Temp, "PASS %s\r\n", Cmd);
                  Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                  if (Bbs->AbortSession () == FALSE)
                     do {
                        i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                        Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                     } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
               }
            }

            if (i == 230) {
               while (Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE) {
                  Bbs->Printf ("\x16\x01\013ftp> ");
                  Bbs->GetString (Cmd, (USHORT)(sizeof (Cmd) - 1), 0);

                  if ((p = strtok (Cmd, " ")) != NULL) {
                     if (!stricmp (p, "?") || !stricmp (p, "help"))
                        Bbs->ReadFile ("ftphelp");
                     else if (!stricmp (p, "account")) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           sprintf (Temp, "ACCT %s\r\n", p);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                        }
                     }
                     else if (!stricmp (p, "ascii")) {
                        Binary = FALSE;
                        Bbs->Printf ("\x16\x01\007200 Type set to A.\n");
                     }
                     else if (!strnicmp (p, "bin", 3) || !stricmp (p, "image")) {
                        Binary = TRUE;
                        Bbs->Printf ("\x16\x01\007200 Type set to I.\n");
                     }
                     else if (!stricmp (p, "cd")) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           sprintf (Temp, "CWD %s\r\n", p);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                        }
                     }
                     else if (!stricmp (p, "cdup")) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           Tcp->SendBytes ((UCHAR *)"CDUP\r\n", 6);
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                        }
                     }
                     else if (!strnicmp (p, "del", 3)) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           sprintf (Temp, "DELE %s\r\n", p);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                        }
                     }
                     else if (!stricmp (p, "dir") || !stricmp (p, "ls")) {
                        Tcp->SendBytes ((UCHAR *)"TYPE A\r\n", 8);
                        do {
                           i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                        } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

                        if ((Data = new TTcpip) != NULL) {
                           Data->Initialize (DataPort);

                           sprintf (Temp, "PORT %ld,%ld,%ld,%ld,%u,%u\r\n", (Tcp->HostID & 0xFF000000L) >> 24, (Tcp->HostID & 0xFF0000L) >> 16, (Tcp->HostID & 0xFF00L) >> 8, (Tcp->HostID & 0xFFL), (DataPort & 0xFF00) >> 8, DataPort & 0xFF);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                           if (i == 200) {
                              if ((p = strtok (NULL, " ")) != NULL) {
                                 sprintf (Temp, "LIST %s\r\n", p);
                                 Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              }
                              else
                                 Tcp->SendBytes ((UCHAR *)"LIST\r\n", 6);
                              do {
                                 i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                 Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                              } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              if (i == 150) {
                                 FTP_GET (NULL, NULL, FALSE, FALSE);
                                 do {
                                    i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                    Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                                 } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              }
                           }

                           DataPort++;
                           if (DataPort > FTPDATA_PORT + 1024)
                              DataPort = FTPDATA_PORT;

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
                        } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

                        if ((Data = new TTcpip) != NULL) {
                           Data->Initialize (DataPort);

                           sprintf (Temp, "PORT %ld,%ld,%ld,%ld,%u,%u\r\n", (Tcp->HostID & 0xFF000000L) >> 24, (Tcp->HostID & 0xFF0000L) >> 16, (Tcp->HostID & 0xFF00L) >> 8, (Tcp->HostID & 0xFFL), (DataPort & 0xFF00) >> 8, DataPort & 0xFF);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                           if (i == 200 && (p = strtok (NULL, " ")) != NULL) {
                              sprintf (Temp, "RETR %s\r\n", p);
                              Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              do {
                                 i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                 Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                              } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              if (i == 150) {
                                 Log->Write ("+%s", &Temp[4]);
                                 Local = p;
                                 if ((p = strtok (NULL, " ")) != NULL)
                                    Local = p;
                                 sprintf (Temp, "%s%s\\%s", Cfg->HomePath, User->MailBox, Local);
                                 FTP_GET (Temp, Local, Hash, Binary);
                                 do {
                                    i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                    Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                                 } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              }
                           }

                           DataPort++;
                           if (DataPort > FTPDATA_PORT + 1024)
                              DataPort = FTPDATA_PORT;

                           delete Data;
                        }
                     }
                     else if (!stricmp (p, "hash")) {
                        if (Hash == TRUE) {
                           Hash = FALSE;
                           Bbs->Printf ("\x16\x01\x07Hash printing off.\n");
                        }
                        else {
                           Hash = TRUE;
                           Bbs->Printf ("\x16\x01\x07Hash mark printing on (1024 bytes/hash mark).\n");
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
                              Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                        }
                     }
                     else if (!stricmp (p, "put") || !stricmp (p, "stor")) {
                        if (Binary == TRUE)
                           Tcp->SendBytes ((UCHAR *)"TYPE I\r\n", 8);
                        else
                           Tcp->SendBytes ((UCHAR *)"TYPE A\r\n", 8);
                        do {
                           i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                        } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);

                        if ((Data = new TTcpip) != NULL) {
                           Data->Initialize (DataPort);

                           sprintf (Temp, "PORT %ld,%ld,%ld,%ld,%u,%u\r\n", (Tcp->HostID & 0xFF000000L) >> 24, (Tcp->HostID & 0xFF0000L) >> 16, (Tcp->HostID & 0xFF00L) >> 8, (Tcp->HostID & 0xFFL), (DataPort & 0xFF00) >> 8, DataPort & 0xFF);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                           if (i == 200 && (p = strtok (NULL, " ")) != NULL) {
                              sprintf (Temp, "STOR %s\r\n", p);
                              Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              do {
                                 i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                 Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                              } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              if (i == 150) {
                                 Log->Write ("+%s", &Temp[4]);
                                 Local = p;
                                 if ((p = strtok (NULL, " ")) != NULL)
                                    Local = p;
                                 sprintf (Temp, "%s%s\\%s", Cfg->HomePath, User->MailBox, Local);
                                 FTPsend (Data, Temp, Hash, Binary);
                                 do {
                                    i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                    Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                                 } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              }
                           }

                           DataPort++;
                           if (DataPort > FTPDATA_PORT + 1024)
                              DataPort = FTPDATA_PORT;

                           delete Data;
                        }
                     }
                     else if (!stricmp (p, "pwd")) {
                        Tcp->SendBytes ((UCHAR *)"PWD\r\n", 5);
                        do {
                           i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                           Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                        } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                     }
                     else if (!stricmp (p, "quit") || !stricmp (p, "bye")) {
                        Tcp->SendBytes ((UCHAR *)"QUIT\r\n", 6);
                        do {
                           GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                           Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                        } while (Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                     }
                     else if (!stricmp (p, "remotehelp")) {
                        Tcp->SendBytes ((UCHAR *)"HELP\r\n", 6);
                        do {
                           i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                           Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                        } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                     }
                     else if (!strnicmp (p, "ren", 3)) {
                        if ((Old = strtok (NULL, " ")) != NULL) {
                           if ((New = strtok (NULL, " ")) != NULL) {
                              sprintf (Temp, "RNFR %s\r\n", Old);
                              Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              do {
                                 i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                 Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                              } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                              sprintf (Temp, "RNTO %s\r\n", New);
                              Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              do {
                                 i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                                 Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                              } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                           }
                        }
                     }
                     else if (!stricmp (p, "rmdir") || !stricmp (p, "rd")) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           sprintf (Temp, "RMD %s\r\n", p);
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           do {
                              i = GetResponse (Temp, (USHORT)(sizeof (Temp) - 1));
                              Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
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
                              Bbs->Printf ("\x16\x01\x07%s\n", Temp);
                           } while (i == 0 && Bbs->AbortSession () == FALSE && Tcp->Carrier () == TRUE);
                        }
                     }
                  }
               }
            }
            Log->Write ("+End FTP");
         }

         delete Tcp;
      }
   }
}



