
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "ftrans.h"

#define CAN          ('X'&037)

#define ZFILE        4              /* File name from sender */
#define ZEOF         11             /* End of file */
#define ZCOMPL       15             /* Request is complete */

typedef struct {
   ULONG ulSize;
   ULONG ulTime;
   CHAR  szName[17];
   CHAR  szProgram[15];
   CHAR  fNoAcks;
   CHAR  cbFiller[87];
} ZEROBLK;

TTransfer::TTransfer (void)
{
   Com = NULL;
   Log = NULL;
   EndRun = Hangup = FALSE;
   Progress = NULL;
   Device[0] = '\0';

   PktSize = 128;
   Soh = 0x01;
   DoCrc = FALSE;
   UseAck = TRUE;
   FinalName[0] = '\0';
   PktNumber = 0;
   Task = 1;
}

TTransfer::~TTransfer (void)
{
}

PSZ TTransfer::Receive1kXModem (PSZ pszPath)
{
   UseAck = TRUE;
   return (ReceiveXFile (pszPath));
}

USHORT TTransfer::ReceivePacket (UCHAR *lpBuffer)
{
   SHORT c;
   USHORT i, crc, recvcrc;
   UCHAR checksum;

   checksum = 0;
   crc = 0;

   if ((c = TimedRead (100)) == -1 || c != PktNumber)
      return (FALSE);
   if ((c = TimedRead (100)) == -1 || c != (UCHAR)(PktNumber ^ 0xFF))
      return (FALSE);

   if (DoCrc == TRUE) {
      for (i = 0; i < PktSize; i++, lpBuffer++) {
         if ((c = TimedRead (100)) == -1)
            return (FALSE);
         *lpBuffer = (UCHAR)c;
         crc = Crc16 (*lpBuffer, crc);
      }
      if ((c = TimedRead (100)) == -1)
         return (FALSE);
      recvcrc = (USHORT)(c << 8);
      if ((c = TimedRead (100)) == -1)
         return (FALSE);
      recvcrc = (USHORT)(recvcrc | c);
      if (recvcrc != crc)
         return (FALSE);
   }
   else {
      for (i = 0; i < PktSize; i++, lpBuffer++) {
         if ((c = TimedRead (100)) == -1)
            return (FALSE);
         *lpBuffer = (UCHAR)c;
         checksum += *lpBuffer;
      }
      if ((c = TimedRead (100)) == -1 || c != checksum)
         return (FALSE);
   }

   return (TRUE);
}

PSZ TTransfer::ReceiveXFile (PSZ pszFile)
{
   int fd;
   SHORT c, errs, fStop = FALSE, fStart = FALSE;
   UCHAR *buffer;
   PSZ pszReturn = NULL;

   if ((fd = sopen (pszFile, O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE)) == -1)
      return (NULL);

   if ((buffer = (UCHAR *)malloc (1024)) != NULL) {
      errs = 0;
      DoCrc = TRUE;
      PktNumber = 1;

      while (AbortSession () == FALSE && fStop == FALSE) {
         if ((c = TimedRead (1000)) != -1) {
            switch (c) {
               case SOH:
                  PktSize = 128;
                  if (ReceivePacket (buffer) == TRUE) {
                     if (UseAck == TRUE)
                        Com->BufferByte (ACK);
                     write (fd, buffer, PktSize);
                     errs = 0;
                     fStart = TRUE;
                     PktNumber++;
                  }
                  else {
                     Com->BufferByte (NAK);
                     if (++errs >= 10) {
                        Com->BufferBytes ((UCHAR *)"\x18\x18\x18\x18\x18\x08\x08\x08\x08\x08", 10);
                        fStop = TRUE;
                     }
                  }
                  break;

               case STX:
                  PktSize = 1024;
                  if (ReceivePacket (buffer) == TRUE) {
                     if (UseAck == TRUE)
                        Com->BufferByte (ACK);
                     write (fd, buffer, PktSize);
                     errs = 0;
                     fStart = TRUE;
                     PktNumber++;
                  }
                  else {
                     Com->BufferByte (NAK);
                     if (++errs >= 10) {
                        Com->BufferBytes ((UCHAR *)"\x18\x18\x18\x18\x18\x08\x08\x08\x08\x08", 10);
                        fStop = TRUE;
                     }
                  }
                  break;

               case EOT:
                  if (UseAck == TRUE)
                     Com->BufferByte (ACK);
                  fStop = TRUE;
                  pszReturn = pszFile;
                  break;

               case CAN:
                  if (TimedRead (100) == CAN) {
                     Com->BufferBytes ((UCHAR *)"\x18\x18\x18\x18\x18\x08\x08\x08\x08\x08", 10);
                     fStop = TRUE;
                  }
                  break;
            }
         }
         else {
            errs++;
            if (fStart == FALSE && errs < 5)
               Com->BufferByte ('C');
            else {
               if (errs >= 10) {
                  Com->BufferBytes ((UCHAR *)"\x18\x18\x18\x18\x18\x08\x08\x08\x08\x08", 10);
                  fStop = TRUE;
               }
               Com->BufferByte (NAK);
               if (fStart == FALSE)
                  DoCrc = FALSE;
            }
         }
      }

      free (buffer);
   }

   close (fd);

   return (pszReturn);
}

PSZ TTransfer::ReceiveXModem (PSZ pszPath)
{
   UseAck = TRUE;
   return (ReceiveXFile (pszPath));
}

PSZ TTransfer::ReceiveZModem (PSZ pszPath)
{
   PSZ retVal = NULL;

   if (ZInitReceiver () == ZFILE) {
      if (ZReceiveFile (pszPath) == ZEOF)
         retVal = Pathname;
   }

   return (retVal);
}

USHORT TTransfer::Send1kXModem (PSZ pszFile)
{
   Soh = STX;
   PktSize = 1024;
   PktNumber = 1;
   UseAck = TRUE;

   return (SendXFile (pszFile));
}

USHORT TTransfer::SendASCIIDump (PSZ pszFile)
{
   FILE *fp;
   int c;
   USHORT fRet, nChars;

   fRet = FALSE;
   nChars = 0;

   if ((fp = _fsopen (pszFile, "rb", SH_DENYNO)) != NULL) {
      Com->BufferBytes ((UCHAR *)"\r\n", 2);

      while (AbortSession () == FALSE) {
         if (nChars == 0) {
            if ((c = fgetc (fp)) == EOF) {
               fRet = TRUE;
               break;
            }
            Com->BufferByte ((UCHAR)c);
         }

         if (Com->BytesReady () == TRUE) {
            if ((c = Com->ReadByte ()) == 8) {
               Com->BufferBytes ((UCHAR *)"\x08 \x08", 3);
               nChars--;
            }
            else if (c < 32)
               break;
            else {
               Com->SendByte ((UCHAR)c);
               nChars++;
            }
         }
      }

      fclose (fp);
      Com->UnbufferBytes ();
   }

   return (fRet);
}

SHORT TTransfer::SendPacket (UCHAR *lpBuffer)
{
   USHORT i, crc;
   UCHAR checksum;

   checksum = 0;
   crc = 0;

   Com->BufferByte (Soh);

   Com->BufferByte (PktNumber);
   Com->BufferByte ((UCHAR)(PktNumber ^ 0xFF));

   if (DoCrc == TRUE) {
      for (i = 0; i < PktSize; i++, lpBuffer++) {
         crc = Crc16 (*lpBuffer, crc);
         Com->BufferByte (*lpBuffer);
      }
      Com->BufferByte ((UCHAR)(crc >> 8));
      Com->BufferByte ((UCHAR)(crc & 0xFF));
   }
   else {
      for (i = 0; i < PktSize; i++, lpBuffer++) {
         checksum += *lpBuffer;
         Com->BufferByte (*lpBuffer);
      }
      Com->BufferByte (checksum);
   }

   Com->UnbufferBytes ();

   return (AbortSession ());
}

USHORT TTransfer::SendXFile (PSZ pszFile)
{
   int fd;
   SHORT c, errs;
   USHORT fRet, fStarted;
   UCHAR *buffer;

   DoCrc = FALSE;
   errs = 0;
   fRet = FALSE;
   fStarted = FALSE;

   if ((buffer = (UCHAR *)malloc (PktSize)) == NULL)
      return (FALSE);

   if ((fd = sopen (pszFile, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) == -1) {
      free (buffer);
      return (FALSE);
   }

   memset (buffer, 26, PktSize);
   read (fd, buffer, PktSize);

   if (Progress != NULL) {
      strcpy (Progress->TxFileName, pszFile);
      Progress->TxBlockSize = PktSize;
      Progress->TxSize = filelength (fd);
      Progress->Begin ();
   }

   while (AbortSession () == FALSE) {
      if ((c = TimedRead (1000)) != -1) {
         if (c == 'C' || c == NAK) {
            if (c == 'C')
               DoCrc = TRUE;
            SendPacket (buffer);
            errs++;
            fStarted = TRUE;
         }
         if (c == ACK || UseAck == FALSE) {
            PktNumber++;
            if (Progress != NULL) {
               Progress->TxPosition += Progress->TxBlockSize;
               Progress->Update ();
            }
            errs = 0;
            memset (buffer, 26, PktSize);
            if (read (fd, buffer, PktSize) == 0) {
               Com->SendByte ((UCHAR)EOT);
               while (AbortSession () == FALSE) {
                  if ((c = TimedRead (100)) != -1) {
                     if (c == ACK) {
                        fRet = TRUE;
                        break;
                     }
                     else if (c == 'C' || c == NAK)
                        Com->SendByte ((UCHAR)EOT);
                  }
                  else if (++errs > 10) {
                     Com->SendByte ((UCHAR)EOT);
                     Com->SendBytes ((UCHAR *)"\x18\x18\x18\x18\x18\x08\x08\x08\x08\x08", 10);
                     break;
                  }
               }
               break;
            }
            SendPacket (buffer);
            fStarted = TRUE;
         }
         if (c == CAN) {
            if (TimedRead (100) == CAN)
               break;
         }
      }
      else {
         if (++errs > 10) {
            Com->SendBytes ((UCHAR *)"\x18\x18\x18\x18\x18\x08\x08\x08\x08\x08", 10);
            break;
         }
         else if (fStarted == TRUE)
            SendPacket (buffer);
      }
   }

   if (Progress != NULL) {
      Progress->End ();
      delete Progress;
   }

   free (buffer);
   close (fd);

   return (fRet);
}

USHORT TTransfer::SendXModem (PSZ pszFile)
{
   Soh = SOH;
   PktSize = 128;
   PktNumber = 1;
   UseAck = TRUE;

   return (SendXFile (pszFile));
}

USHORT TTransfer::SendYModem (PSZ pszFile)
{
   short c;
   USHORT fZeroSent, fDone, fRet, errs;
   CHAR ZeroBlock[128];
   PSZ p;
   struct stat statbuf;

   Soh = SOH;
   PktSize = 128;
   PktNumber = 0;
   fZeroSent = FALSE;
   fDone = FALSE;
   fRet = FALSE;
   errs = 0;

   p = strchr (pszFile, '\0');
   while (p > pszFile && *p != ':' && *p != '\\' && *p != '/')
      p--;
   if (*p == ':' || *p == '\\' || *p == '/')
      p++;
   stat (pszFile, &statbuf);

   memset (ZeroBlock, 0, sizeof (ZeroBlock));
   sprintf (ZeroBlock, "%s%c%ld %ld", p, '\0', statbuf.st_size, statbuf.st_mtime);

   while (fDone == FALSE && AbortSession () == FALSE) {
      if ((c = TimedRead (1000)) != -1) {
         if (c == 'C' || c == NAK) {
            if (c == 'C')
               DoCrc = TRUE;
            SendPacket ((UCHAR *)ZeroBlock);
            fZeroSent = TRUE;
         }
         if (c == ACK && fZeroSent == TRUE) {
            Soh = STX;
            PktSize = 1024;
            PktNumber = 1;
            UseAck = TRUE;
            fRet = SendXFile (pszFile);
            fDone = TRUE;
         }
      }
      else if (++errs >= 10)
         fDone = TRUE;
   }

   return (fRet);
}

USHORT TTransfer::SendYModemG (PSZ pszFile)
{
   short c;
   USHORT fZeroSent, fDone, fRet, errs;
   CHAR ZeroBlock[128];
   PSZ p;
   struct stat statbuf;

   Soh = SOH;
   PktSize = 128;
   PktNumber = 0;
   fZeroSent = FALSE;
   fDone = FALSE;
   fRet = FALSE;
   errs = 0;

   p = strchr (pszFile, '\0');
   while (p > pszFile && *p != ':' && *p != '\\' && *p != '/')
      p--;
   if (*p == ':' || *p == '\\' || *p == '/')
      p++;
   stat (pszFile, &statbuf);

   memset (ZeroBlock, 0, sizeof (ZeroBlock));
   sprintf (ZeroBlock, "%s%c%ld %ld", p, '\0', statbuf.st_size, statbuf.st_mtime);

   while (fDone == FALSE && AbortSession () == FALSE) {
      if ((c = TimedRead (1000)) != -1) {
         if (c == 'C' || c == NAK) {
            if (fZeroSent == TRUE)
               fDone = TRUE;
            else {
               if (c == 'C')
                  DoCrc = TRUE;
               SendPacket ((UCHAR *)ZeroBlock);
               fZeroSent = TRUE;
            }
         }
         if (c == ACK && fZeroSent == TRUE) {
            Soh = STX;
            PktSize = 1024;
            PktNumber = 1;
            UseAck = TRUE;
            fRet = SendXFile (pszFile);
            fDone = TRUE;
         }
      }
      else if (++errs >= 10)
         fDone = TRUE;
   }

   return (fRet);
}

USHORT TTransfer::SendZModem (PSZ pszFile, PSZ pszName)
{
   USHORT RetVal = FALSE, i;

   Maxblklen = 1024;
   if (FileSent == 0 && pszFile == NULL)
      ZInitSender (TRUE);
   else
      ZInitSender (FALSE);
   if (pszFile != NULL) {
      if ((i = ZSendFile (pszFile, pszName)) == OK || i == ZSKIP) {
         RetVal = TRUE;
         FileSent++;
      }
   }
   else
      ZEndSender ();

   return (RetVal);
}

USHORT TTransfer::SendZModem8K (PSZ pszFile, PSZ pszName)
{
   USHORT RetVal = FALSE, i;

   Maxblklen = KSIZE;
   if (FileSent == 0 && pszFile == NULL)
      ZInitSender (TRUE);
   else
      ZInitSender (FALSE);
   if (pszFile != NULL) {
      if ((i = ZSendFile (pszFile, pszName)) == OK || i == ZSKIP) {
         RetVal = TRUE;
         FileSent++;
      }
   }
   else
      ZEndSender ();

   return (RetVal);
}

VOID TTransfer::Janus (PSZ pszPath)
{
#if !defined(__LINUX__)
   class TJanus *Jan;

   if ((Jan = new TJanus) != NULL) {
      strcpy (Jan->RxPath, pszPath);
      Jan->Com = Com;
      Jan->Log = Log;
      Jan->TxQueue = &TxQueue;
      Jan->RxQueue = &RxQueue;
      Jan->Transfer ();
      delete Jan;
   }
#endif
}

VOID TTransfer::RunExternalProtocol (USHORT Download, PSZ Cmd, class TProtocol *Protocol)
{
   FILE *fp;
   USHORT i, Batch = FALSE, Found = FALSE;
   CHAR Command[128], Temp[128], Control[128], *p;
   ULONG Cps;

   if (TxQueue.TotalFiles > 1L)
      Batch = TRUE;

   if (Protocol != NULL) {
      if (Protocol->First () == TRUE)
         do {
            if (Protocol->Active == TRUE && (Batch == FALSE || Protocol->Batch == TRUE)) {
               if (!stricmp (Protocol->Key, Cmd)) {
                  Found = TRUE;
                  break;
               }
            }
         } while (Protocol->Next () == TRUE);
   }
   if (Found == TRUE) {
      if (Protocol->LogFileName[0] != '\0') {
         strcpy (Command, Protocol->LogFileName);
         if (strstr (Command, "%k") != NULL) {
            sprintf (Temp, "%u", Task);
            strsrep (Command, "%k", Temp);
         }
         unlink (Command);
      }
      if (Protocol->CtlFileName[0] != '\0') {
         strcpy (Control, Protocol->CtlFileName);
         if (strstr (Control, "%k") != NULL) {
            sprintf (Temp, "%u", Task);
            strsrep (Control, "%k", Temp);
         }

         if ((fp = fopen (Control, "wt")) != NULL) {
            if (TxQueue.First () == TRUE)
               do {
                  strcpy (Command, Protocol->DownloadCtlString);
                  if (strstr (Command, "%1") != NULL)
                     strsrep (Command, "%1", TxQueue.Complete);
                  fprintf (fp, "%s\n", Command);
               } while (TxQueue.Next () == TRUE);
            fclose (fp);
         }
      }
      if (Download == TRUE) {
         strcpy (Command, Protocol->DownloadCmd);
         if (strstr (Command, "%k") != NULL) {
            sprintf (Temp, "%u", Task);
            strsrep (Command, "%k", Temp);
         }
         if (strstr (Command, "%P") != NULL) {
#if defined(__LINUX__)
            strsrep (Command, "%P", Device);
#else
            sprintf (Temp, "%u", atoi (&Device[3]));
            strsrep (Command, "%P", Temp);
#endif
         }
         if (strstr (Command, "%b") != NULL) {
            sprintf (Temp, "%lu", Speed);
            strsrep (Command, "%b", Temp);
         }
         if (strstr (Command, "%1") != NULL) {
            if (TxQueue.First () == TRUE)
               strsrep (Command, "%1", TxQueue.Complete);
            else
               strsrep (Command, "%1", "");
         }
         if (strstr (Command, "%2") != NULL)
            strsrep (Command, "%2", Control);
         if (Log != NULL)
            Log->Write (":Running %s", Command);
         RunExternal (Command, 0);
         if (Log != NULL)
            Log->Write (":Returned from external protocol");
      }
      if (Protocol->CtlFileName[0] != '\0')
         unlink (Control);
      if (Protocol->LogFileName[0] != '\0') {
         strcpy (Command, Protocol->LogFileName);
         if (strstr (Command, "%k") != NULL) {
            sprintf (Temp, "%u", Task);
            strsrep (Command, "%k", Temp);
         }

         if ((fp = fopen (Command, "rt")) != NULL) {
            while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
               if (Temp[strlen (Temp) - 1] == '\n')
                  Temp[strlen (Temp) - 1] = '\0';
               if ((p = strtok (Temp, " ")) != NULL) {
                  if (!strcmp (p, Protocol->DownloadKeyword)) {
                     i = 1;
                     Found = FALSE;
                     while ((p = strtok (NULL, " ")) != NULL) {
                        i++;
                        if (Protocol->FileNamePos == i) {
                           if (TxQueue.First () == TRUE)
                              do {
                                 if (!stricmp (p, TxQueue.Complete) || !stricmp (p, TxQueue.Name)) {
                                    Found = TRUE;
                                    break;
                                 }
                              } while (TxQueue.Next () == TRUE);
                        }
                        if (Protocol->CpsPos == i)
                           Cps = (ULONG)atoi (p);
                     }
                     if (Found == TRUE) {
                        Log->Write ("+CPS: %lu (%lu bytes)  Efficiency: %d%%", Cps, TxQueue.Size, (Cps * 100L) / (Speed / 10));
                        Log->Write ("+Sent-%s %s", Protocol->Key, strupr (TxQueue.Complete));
                        TxQueue.Sent = TRUE;
                        TxQueue.Update ();
                     }
                  }
               }
            }
            fclose (fp);
         }

         unlink (Command);
      }
   }
}

// ----------------------------------------------------------------------

TProgress::TProgress (void)
{
   Type = 0;
   RxBlockSize = TxBlockSize = 0;
   RxFileName[0] = TxFileName[0] = '\0';
   RxSize = RxPosition = 0L;
   TxSize = TxPosition = 0L;
}

TProgress::~TProgress (void)
{
}

VOID TProgress::Begin (VOID)
{
}

VOID TProgress::End (VOID)
{
}

VOID TProgress::Update (VOID)
{
}


