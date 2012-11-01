
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.5
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/13/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"

HUDSON::HUDSON (void)
{
   fdTxt = fdHdr = -1;
   Current = Id = 0L;
   msgIdx = NULL;
   Locked = FALSE;
}

HUDSON::HUDSON (PSZ pszName, UCHAR board)
{
   fdTxt = fdHdr = -1;
   Id = 0L;
   msgIdx = NULL;
   Locked = FALSE;

   Open (pszName, board);
}

HUDSON::~HUDSON (void)
{
   Close ();
}

VOID HUDSON::Pascal2C (PSZ strp, PSZ strc)
{
   memcpy (strc, &strp[1], strp[0]);
   strc[strp[0]] = '\0';
}

VOID HUDSON::C2Pascal (PSZ strp, PSZ strc)
{
   memcpy (&strp[1], strc, strlen (strc));
   strp[0] = (CHAR)strlen (strc);
}

USHORT HUDSON::Add (VOID)
{
   return (Add (Text));
}

USHORT HUDSON::Add (class TMsgBase *MsgBase)
{
   New ();

   strcpy (From, MsgBase->From);
   strcpy (To, MsgBase->To);
   strcpy (Subject, MsgBase->Subject);

   strcpy (FromAddress, MsgBase->FromAddress);
   strcpy (ToAddress, MsgBase->ToAddress);

   Written.Day = MsgBase->Written.Day;
   Written.Month = MsgBase->Written.Month;
   Written.Year = MsgBase->Written.Year;
   Written.Hour = MsgBase->Written.Hour;
   Written.Minute = MsgBase->Written.Minute;
   Written.Second = MsgBase->Written.Second;

   Arrived.Day = MsgBase->Arrived.Day;
   Arrived.Month = MsgBase->Arrived.Month;
   Arrived.Year = MsgBase->Arrived.Year;
   Arrived.Hour = MsgBase->Arrived.Hour;
   Arrived.Minute = MsgBase->Arrived.Minute;
   Arrived.Second = MsgBase->Arrived.Second;

   Crash = MsgBase->Crash;
   Direct = MsgBase->Direct;
   FileAttach = MsgBase->FileAttach;
   FileRequest = MsgBase->FileRequest;
   Hold = MsgBase->Hold;
   Immediate = MsgBase->Immediate;
   Intransit = MsgBase->Intransit;
   KillSent = MsgBase->KillSent;
   Local = MsgBase->Local;
   Private = MsgBase->Private;
   ReceiptRequest = MsgBase->ReceiptRequest;
   Received = MsgBase->Received;
   Sent = MsgBase->Sent;

   return (Add (MsgBase->Text));
}

USHORT HUDSON::Add (class TCollection &MsgText)
{
   int fd;
   USHORT RetVal = FALSE, inblock, len;
   CHAR Temp[32], File[128], *pszAddress, *pszText;
   HMSGTOIDX msgToIdx;
   HMSGIDX hmsgIdx;

   //////////////////////////////////////////////////////////////////////////////
   // Se la base non e' lockata, rilegge il msginfo.bbs                        //
   //////////////////////////////////////////////////////////////////////////////
   if (Locked == FALSE) {
      memset (&msgInfo, 0, sizeof (HMSGINFO));
      sprintf (File, "%smsginfo.bbs", BaseName);
      if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
         read (fd, &msgInfo, sizeof (HMSGINFO));
         close (fd);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   // Costruisce la struttura MSGHDR                                           //
   //////////////////////////////////////////////////////////////////////////////
   memset (&msgHdr, 0, sizeof (HMSGHDR));
   msgHdr.MsgNum = ++msgInfo.HighMsg;
   msgHdr.PrevReply = (USHORT)Original;
   msgHdr.NextReply = (USHORT)Reply;

   if (msgInfo.LowMsg == 0)
      msgInfo.LowMsg = msgInfo.HighMsg;
   msgInfo.TotalMsgs++;
   msgInfo.TotalOnBoard[BoardNum - 1]++;
   TotalMsgs = msgInfo.TotalOnBoard[BoardNum - 1];

   pszAddress = FromAddress;
   if (strchr (pszAddress, ':') != NULL) {
      msgHdr.OrigZone = (UCHAR)atoi (pszAddress);
      pszAddress = strchr (pszAddress, ':') + 1;
   }
   if (strchr (pszAddress, '/') != NULL) {
      msgHdr.OrigNet = (USHORT)atoi (pszAddress);
      pszAddress = strchr (pszAddress, '/') + 1;
   }
   msgHdr.OrigNode = (USHORT)atoi (pszAddress);

   pszAddress = ToAddress;
   if (strchr (pszAddress, ':') != NULL) {
      msgHdr.DestZone = (UCHAR)atoi (pszAddress);
      pszAddress = strchr (pszAddress, ':') + 1;
   }
   if (strchr (pszAddress, '/') != NULL) {
      msgHdr.DestNet = (USHORT)atoi (pszAddress);
      pszAddress = strchr (pszAddress, '/') + 1;
   }
   msgHdr.DestNode = (USHORT)atoi (pszAddress);

   msgHdr.Board = BoardNum;
   sprintf (Temp, "%02d:%02d", Written.Hour, Written.Minute);
   C2Pascal (msgHdr.Time, Temp);
   sprintf (Temp, "%02d-%02d-%02d", Written.Month, Written.Day, Written.Year % 100);
   C2Pascal (msgHdr.Date, Temp);
   C2Pascal (msgHdr.WhoFrom, From);
   C2Pascal (msgHdr.WhoTo, To);
   C2Pascal (msgHdr.Subject, Subject);

   if (Local == TRUE)
      msgHdr.MsgAttr |= HUD_LOCAL;
   if (Private == TRUE)
      msgHdr.MsgAttr |= HUD_PRIVATE;
   if (Received == TRUE)
      msgHdr.MsgAttr |= HUD_RECEIVED;
   if (Crash == TRUE)
      msgHdr.NetAttr |= HUD_CRASH;
   if (KillSent == TRUE)
      msgHdr.NetAttr |= HUD_KILL;
   if (Sent == TRUE)
      msgHdr.NetAttr |= HUD_SENT;
   if (FileAttach == TRUE)
      msgHdr.NetAttr |= HUD_FILE;
   if (FileRequest == TRUE)
      msgHdr.NetAttr |= HUD_FRQ;

   //////////////////////////////////////////////////////////////////////////////
   // Inserisce il destinatario del file msgtoidx.bbs                          //
   //////////////////////////////////////////////////////////////////////////////
   memset (&msgToIdx, 0, sizeof (HMSGTOIDX));
   C2Pascal (msgToIdx.String, To);
   if (Locked == FALSE) {
      sprintf (File, "%smsgtoidx.bbs", BaseName);
      fdToIdx = open (File, O_RDWR|O_BINARY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE);
   }
   if (fdToIdx != -1) {
      lseek (fdToIdx, 0L, SEEK_END);
      write (fdToIdx, &msgToIdx, sizeof (HMSGTOIDX));
   }
   if (Locked == FALSE)
      close (fdToIdx);

   //////////////////////////////////////////////////////////////////////////////
   // Inserisce l'entry nuova nel msgidx.bbs                                   //
   //////////////////////////////////////////////////////////////////////////////
   memset (&hmsgIdx, 0, sizeof (HMSGIDX));
   hmsgIdx.MsgNum = msgHdr.MsgNum;
   hmsgIdx.Board = msgHdr.Board;
   if (Locked == FALSE) {
      sprintf (File, "%smsgidx.bbs", BaseName);
      if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE)) != -1) {
         lseek (fd, 0L, SEEK_END);
         write (fd, &hmsgIdx, sizeof (HMSGIDX));
         close (fd);
      }
   }
   else {
      msgIdx[msgInfo.TotalMsgs - 1].MsgNum = hmsgIdx.MsgNum;
      msgIdx[msgInfo.TotalMsgs - 1].Board = hmsgIdx.Board;
   }

   //////////////////////////////////////////////////////////////////////////////
   // Inserisce il testo del messaggio nel file msgtxt.bbs                     //
   //////////////////////////////////////////////////////////////////////////////
   if (Locked == FALSE) {
      sprintf (File, "%smsgtxt.bbs", BaseName);
      fdTxt = open (File, O_RDWR|O_BINARY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE);
   }
   if (fdTxt != -1) {
      lseek (fdTxt, 0L, SEEK_END);
      msgHdr.StartBlock = (USHORT)(tell (fd) / 256L);
      inblock = 0;
      if ((pszText = (PSZ)MsgText.First ()) != NULL)
         do {
            len = (USHORT)strlen (pszText);
            while (len > 0) {
               if (inblock + len <= 255) {
                  memcpy (&szBuff[1 + inblock], pszText, len);
                  inblock += len;
                  len = 0;
               }
               else {
                  memcpy (&szBuff[1 + inblock], pszText, 255 - inblock);
                  szBuff[0] = 255;
                  write (fdTxt, szBuff, 256);
                  msgHdr.NumBlocks++;
                  len -= (255 - inblock);
                  inblock = 0;
               }
            }

            if (inblock >= 255) {
               szBuff[0] = 255;
               write (fdTxt, szBuff, 256);
               msgHdr.NumBlocks++;
               inblock = 0;
            }
            szBuff[1 + inblock] = '\r';
            inblock++;

            if (inblock >= 255) {
               szBuff[0] = 255;
               write (fdTxt, szBuff, 256);
               msgHdr.NumBlocks++;
               inblock = 0;
            }
            szBuff[1 + inblock] = '\n';
            inblock++;
         } while ((pszText = (PSZ)MsgText.Next ()) != NULL);

      if (inblock > 0) {
         szBuff[0] = (CHAR)inblock;
         write (fdTxt, szBuff, 256);
         msgHdr.NumBlocks++;
      }
   }
   if (Locked == FALSE)
      close (fdTxt);

   //////////////////////////////////////////////////////////////////////////////
   // Aggiunge l'header del messaggio al file msghdr.bbs                       //
   //////////////////////////////////////////////////////////////////////////////
   if (Locked == FALSE) {
      sprintf (File, "%smsghdr.bbs", BaseName);
      fdHdr = open (File, O_RDWR|O_BINARY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE);
   }
   if (fdHdr != -1) {
      lseek (fdHdr, 0L, SEEK_END);
      write (fdHdr, &msgHdr, sizeof (HMSGHDR));
   }
   if (Locked == FALSE)
      close (fdHdr);

   //////////////////////////////////////////////////////////////////////////////
   // Se la base non e' lockata aggiorna gli indici e il msginfo.bbs           //
   //////////////////////////////////////////////////////////////////////////////
   if (Locked == FALSE) {
      if (msgIdx != NULL) {
         free (msgIdx);
         msgIdx = NULL;
      }

      sprintf (File, "%smsgidx.bbs", BaseName);
      if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
         if ((msgIdx = (HMSGIDX *)malloc (filelength (fd))) != NULL)
            read (fd, msgIdx, filelength (fd));
         close (fd);
      }

      sprintf (File, "%smsginfo.bbs", BaseName);
      if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
         write (fd, &msgInfo, sizeof (HMSGINFO));
         close (fd);
      }
   }

   return (TRUE);
}

VOID HUDSON::Close (VOID)
{
   UnLock ();

   if (msgIdx != NULL)
      free (msgIdx);

   fdTxt = fdHdr = -1;
   Id = 0L;
   msgIdx = NULL;
}

USHORT HUDSON::Delete (ULONG ulMsg)
{
   int fd, i;
   USHORT RetVal = FALSE;
   CHAR File[128];

   if (Locked == FALSE) {
      sprintf (File, "%smsginfo.bbs", BaseName);
      if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
         read (fd, &msgInfo, sizeof (HMSGINFO));
         close (fd);
      }
      TotalMsgs = msgInfo.TotalOnBoard[BoardNum - 1];
   }

   if (Locked == FALSE) {
      if (TotalMsgs != 0) {
         sprintf (File, "%smsgidx.bbs", BaseName);
         if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
            if ((msgIdx = (HMSGIDX *)malloc (filelength (fd))) != NULL) {
               read (fd, msgIdx, filelength (fd));
               for (i = 0; i < msgInfo.TotalMsgs; i++) {
                  if (msgIdx[i].Board == BoardNum && msgIdx[i].MsgNum == ulMsg) {
                     msgIdx[i].MsgNum = 0xFFFFU;
                     lseek (fd, 0L, SEEK_SET);
                     write (fd, msgIdx, filelength (fd));
                     RetVal = TRUE;
                     break;
                  }
               }
            }
            close (fd);
         }
      }
   }
   else {
      for (i = 0; i < msgInfo.TotalMsgs; i++) {
         if (msgIdx[i].Board == BoardNum && msgIdx[i].MsgNum == ulMsg) {
            msgIdx[i].MsgNum = 0xFFFFU;
            RetVal = TRUE;
            break;
         }
      }
   }

   if (RetVal == TRUE) {
      msgInfo.TotalMsgs--;
      msgInfo.TotalOnBoard[BoardNum - 1]--;
      TotalMsgs = msgInfo.TotalOnBoard[BoardNum - 1];

      if (Locked == FALSE) {
         sprintf (File, "%smsginfo.bbs", BaseName);
         if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
            write (fd, &msgInfo, sizeof (HMSGINFO));
            close (fd);
         }
      }
   }

   return (RetVal);
}

USHORT HUDSON::GetHWM (ULONG &ulMsg)
{
   ulMsg = 0L;

   return (FALSE);
}

ULONG HUDSON::Highest (VOID)
{
   int i;
   ULONG RetVal = 0L;

   if (msgIdx != NULL && TotalMsgs != 0L) {
      for (i = msgInfo.TotalMsgs - 1; i >= 0; i--) {
         if (msgIdx[i].Board == BoardNum && msgIdx[i].MsgNum != 0xFFFFU) {
            RetVal = msgIdx[i].MsgNum;
            break;
         }
      }
   }

   return (RetVal);
}

USHORT HUDSON::Lock (ULONG ulTimeout)
{
   int fd;
   CHAR File[128];

   ulTimeout = ulTimeout;

   if (Locked == FALSE) {
      sprintf (File, "%smsginfo.bbs", BaseName);
      if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
         read (fd, &msgInfo, sizeof (HMSGINFO));
         close (fd);

         TotalMsgs = msgInfo.TotalOnBoard[BoardNum - 1];

         sprintf (File, "%smsgidx.bbs", BaseName);
         if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
            if ((msgIdx = (HMSGIDX *)malloc (filelength (fd) + 5000 * sizeof (HMSGIDX))) != NULL)
               read (fd, msgIdx, filelength (fd));
            close (fd);
         }

         sprintf (File, "%smsgtxt.bbs", BaseName);
         fdHdr = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
         sprintf (File, "%smsghdr.bbs", BaseName);
         fdTxt = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
         sprintf (File, "%smsgtoidx.bbs", BaseName);
         fdToIdx = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
      }
   }

   return (TRUE);
}

ULONG HUDSON::Lowest (VOID)
{
   int i;
   ULONG RetVal = 0L;

   if (msgIdx != NULL && TotalMsgs != 0L) {
      for (i = 0; i < msgInfo.TotalMsgs; i++) {
         if (msgIdx[i].Board == BoardNum && msgIdx[i].MsgNum != 0xFFFFU) {
            RetVal = msgIdx[i].MsgNum;
            break;
         }
      }
   }

   return (RetVal);
}

ULONG HUDSON::MsgnToUid (ULONG ulMsg)
{
   int i;
   ULONG Num = 1L;

   if (msgIdx != NULL && TotalMsgs != 0L) {
      for (i = 0; i < msgInfo.TotalMsgs; i++) {
         if (msgIdx[i].Board == BoardNum && msgIdx[i].MsgNum != 0xFFFFU) {
            if (Num == ulMsg) {
               ulMsg = msgIdx[i].MsgNum;
               break;
            }
            Num++;
         }
      }
   }

   return (ulMsg);
}

VOID HUDSON::New (VOID)
{
   From[0] = To[0] = Subject[0] = '\0';
   Crash = Direct = FileAttach = FileRequest = Hold = Immediate = FALSE;
   Intransit = KillSent = Local = Private = ReceiptRequest = Received = FALSE;
   Sent = 0;
   memset (&Written, 0, sizeof (Written));
   memset (&Arrived, 0, sizeof (Arrived));
   FromAddress[0] = ToAddress[0] = '\0';
   Original = Reply = 0L;
   Text.Clear ();
}

USHORT HUDSON::Next (ULONG &ulMsg)
{
   int i;
   USHORT RetVal = FALSE;

   if (msgIdx != NULL && TotalMsgs != 0L) {
      for (i = 0; i < msgInfo.TotalMsgs; i++) {
         if (msgIdx[i].Board == BoardNum && msgIdx[i].MsgNum > ulMsg && msgIdx[i].MsgNum != 0xFFFFU) {
            ulMsg = msgIdx[i].MsgNum;
            RetVal = TRUE;
            break;
         }
      }
   }

   return (RetVal);
}

ULONG HUDSON::Number (VOID)
{
   return (TotalMsgs);
}

USHORT HUDSON::Open (PSZ pszName, UCHAR board)
{
   int fd;
   USHORT RetVal = FALSE;
   CHAR File[128];

   Close ();

   strcpy (BaseName, pszName);
   BoardNum = board;

   //////////////////////////////////////////////////////////////////////////////
   // Legge il msginfo.bbs per sapere quanti messaggi ci sono nella board      //
   //////////////////////////////////////////////////////////////////////////////
   sprintf (File, "%smsginfo.bbs", BaseName);
   if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
      read (fd, &msgInfo, sizeof (HMSGINFO));
      close (fd);

      TotalMsgs = msgInfo.TotalOnBoard[BoardNum - 1];

      //////////////////////////////////////////////////////////////////////////////
      // Legge tutto l'indice in memoria                                          //
      //////////////////////////////////////////////////////////////////////////////
      sprintf (File, "%smsgidx.bbs", BaseName);
      if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
         if ((msgIdx = (HMSGIDX *)malloc (filelength (fd))) != NULL)
            read (fd, msgIdx, filelength (fd));
         close (fd);
      }

      RetVal = TRUE;
   }

   return (RetVal);
}

VOID HUDSON::Pack (VOID)
{
   int i, fd, fdo[4], fdn[4];
   USHORT Number, DoRename = FALSE;
   CHAR File[128], NewName[128];
   HMSGIDX hmsgIdx;

   UnLock ();

   sprintf (File, "%smsgidx.bbs", BaseName);
   fdo[0] = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   sprintf (File, "%smsgidx.new", BaseName);
   fdn[0] = open (File, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   sprintf (File, "%smsghdr.bbs", BaseName);
   fdo[1] = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   sprintf (File, "%smsghdr.new", BaseName);
   fdn[1] = open (File, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   sprintf (File, "%smsgtoidx.bbs", BaseName);
   fdo[2] = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   sprintf (File, "%smsgtoidx.new", BaseName);
   fdn[2] = open (File, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   sprintf (File, "%smsgtxt.bbs", BaseName);
   fdo[3] = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   sprintf (File, "%smsgtxt.new", BaseName);
   fdn[3] = open (File, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   if (fdo[0] != -1 && fdo[1] != -1 && fdo[2] != -1 && fdo[3] != -1 && fdn[0] != -1 && fdn[1] != -1 && fdn[2] != -1 && fdn[3] != -1) {
      Number = 1;
      memset (&msgInfo, 0, sizeof (HMSGINFO));
      while (read (fdo[0], &hmsgIdx, sizeof (HMSGIDX)) == sizeof (HMSGIDX)) {
         if (hmsgIdx.MsgNum != 0xFFFFU) {
            hmsgIdx.MsgNum = Number;
            write (fdn[0], &hmsgIdx, sizeof (HMSGIDX));

            lseek (fdo[3], (long)msgHdr.StartBlock * 256L, SEEK_SET);
            msgHdr.StartBlock = (USHORT)(tell (fdn[3]) / 256L);
            for (i = 0; i < msgHdr.NumBlocks; i++) {
               read (fdo[3], szBuff, 256);
               write (fdn[3], szBuff, 256);
            }

            read (fdo[1], &msgHdr, sizeof (HMSGHDR));
            msgHdr.MsgNum = Number;
            write (fdn[1], &msgHdr, sizeof (HMSGHDR));

            read (fdo[2], szBuff, sizeof (HMSGTOIDX));
            write (fdn[2], szBuff, sizeof (HMSGTOIDX));

            msgInfo.TotalMsgs++;
            msgInfo.TotalOnBoard[hmsgIdx.Board - 1]++;
            if (msgInfo.LowMsg == 0)
               msgInfo.LowMsg = Number;
            Number++;
         }
      }

      sprintf (File, "%smsginfo.bbs", BaseName);
      if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) != -1) {
         write (fd, &msgInfo, sizeof (HMSGINFO));
         close (fd);
      }

      DoRename = TRUE;
   }

   if (fdo[0] != -1)
      close (fdo[0]);
   if (fdo[1] != -1)
      close (fdo[1]);
   if (fdo[2] != -1)
      close (fdo[2]);
   if (fdo[3] != -1)
      close (fdo[3]);

   if (fdn[0] != -1)
      close (fdn[0]);
   if (fdn[1] != -1)
      close (fdn[1]);
   if (fdn[2] != -1)
      close (fdn[2]);
   if (fdn[3] != -1)
      close (fdn[3]);

   sprintf (NewName, "%smsgidx.bbs", BaseName);
   unlink (NewName);
   sprintf (File, "%smsgidx.new", BaseName);
   rename (File, NewName);

   sprintf (NewName, "%smsghdr.bbs", BaseName);
   unlink (NewName);
   sprintf (File, "%smsghdr.new", BaseName);
   rename (File, NewName);

   sprintf (NewName, "%smsgtoidx.bbs", BaseName);
   unlink (NewName);
   sprintf (File, "%smsgtoidx.new", BaseName);
   rename (File, NewName);

   sprintf (NewName, "%smsgtxt.bbs", BaseName);
   unlink (NewName);
   sprintf (File, "%smsgtxt.new", BaseName);
   rename (File, NewName);
}

USHORT HUDSON::Previous (ULONG &ulMsg)
{
   int i;
   USHORT RetVal = FALSE;

   if (msgIdx != NULL && TotalMsgs != 0L) {
      for (i = msgInfo.TotalMsgs - 1; i >= 0; i--) {
         if (msgIdx[i].Board == BoardNum && msgIdx[i].MsgNum < ulMsg && msgIdx[i].MsgNum != 0xFFFFU) {
            ulMsg = msgIdx[i].MsgNum;
            RetVal = TRUE;
            break;
         }
      }
   }

   return (RetVal);
}

USHORT HUDSON::ReadHeader (ULONG ulMsg)
{
   int fz, fn, fo, tz, tn, to;
   USHORT RetVal = FALSE, FromPoint, ToPoint, FromZone, ToZone;
   SHORT i, nReaded, nCol;
   CHAR File[128];

   ToPoint = ToZone = FromPoint = FromZone = 0;

   if (msgIdx != NULL && TotalMsgs != 0L) {
      for (i = 0; i < msgInfo.TotalMsgs; i++) {
         if (msgIdx[i].Board == BoardNum && msgIdx[i].MsgNum == ulMsg && msgIdx[i].MsgNum != 0xFFFFU) {
            RetVal = TRUE;
            break;
         }
      }
   }

   if (RetVal == TRUE) {
      Current = Id = ulMsg;

      if (Locked == FALSE) {
         sprintf (File, "%smsghdr.bbs", BaseName);
         fdHdr = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
      }
      if (fdHdr != -1) {
         lseek (fdHdr, (long)i * (long)sizeof (HMSGHDR), SEEK_SET);
         read (fdHdr, &msgHdr, sizeof (HMSGHDR));
         if (Locked == FALSE)
            close (fdHdr);

         Pascal2C (msgHdr.WhoFrom, From);
         Pascal2C (msgHdr.WhoTo, To);
         Pascal2C (msgHdr.Subject, Subject);

         sprintf (FromAddress, "%d:%d/%d", msgHdr.OrigZone, msgHdr.OrigNet, msgHdr.OrigNode);
         sprintf (ToAddress, "%d:%d/%d", msgHdr.DestZone, msgHdr.DestNet, msgHdr.DestNode);

         Original = msgHdr.PrevReply;
         Reply = msgHdr.NextReply;

         Arrived.Day = Written.Day = (UCHAR)((msgHdr.Date[4] - '0') * 10 + (msgHdr.Date[5] - '0'));
         Arrived.Month = Written.Month = (UCHAR)((msgHdr.Date[1] - '0') * 10 + (msgHdr.Date[2] - '0'));
         Written.Year = (USHORT)((msgHdr.Date[7] - '0') * 10 + (msgHdr.Date[8] - '0'));
         if (Written.Year < 90)
            Written.Year += 2000;
         else
            Written.Year += 1900;
         Arrived.Year = Written.Year;
         Arrived.Hour = Written.Hour = (UCHAR)((msgHdr.Time[1] - '0') * 10 + (msgHdr.Time[2] - '0'));
         Arrived.Minute = Written.Minute = (UCHAR)((msgHdr.Time[4] - '0') * 10 + (msgHdr.Time[5] - '0'));
         Arrived.Second = Written.Second = 0;

         Local = ((msgHdr.MsgAttr & HUD_LOCAL) == HUD_LOCAL) ? TRUE : FALSE;
         Private = (msgHdr.MsgAttr & HUD_PRIVATE) ? TRUE : FALSE;
         Received = (msgHdr.MsgAttr & HUD_RECEIVED) ? TRUE : FALSE;
         Crash = (msgHdr.NetAttr & HUD_CRASH) ? TRUE : FALSE;
         KillSent = (msgHdr.NetAttr & HUD_KILL) ? TRUE : FALSE;
         Sent = (msgHdr.NetAttr & HUD_SENT) ? TRUE : FALSE;
         FileAttach = (msgHdr.NetAttr & HUD_FILE) ? TRUE : FALSE;
         FileRequest = (msgHdr.NetAttr & HUD_FRQ) ? TRUE : FALSE;

         if (Locked == FALSE) {
            sprintf (File, "%smsgtxt.bbs", BaseName);
            fdTxt = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
         }
         if (fdTxt != -1) {
            lseek (fdTxt, (long)msgHdr.StartBlock * 256L, SEEK_SET);
            read (fdTxt, szBuff, 256);
            nReaded = szBuff[0];

            pLine = szLine;
            nCol = 0;
            FromZone = msgHdr.OrigZone;
            ToZone = msgHdr.DestZone;

            for (i = 0, pBuff = &szBuff[1]; i < nReaded; i++, pBuff++) {
               if (*pBuff == '\r') {
                  *pLine = '\0';
                  if (!strncmp (szLine, "\001FMPT ", 6))
                     FromPoint = (USHORT)atoi (&szLine[6]);
                  else if (!strncmp (szLine, "\001TOPT ", 6))
                     ToPoint = (USHORT)atoi (&szLine[6]);
                  else if (!strncmp (szLine, "\001INTL ", 6)) {
                     sscanf (&szLine[6], "%d:%d/%d %d:%d/%d", &tz, &tn, &to, &fz, &fn, &fo);
                     if (tn == msgHdr.DestNet && to == msgHdr.DestNode)
                        ToZone = (USHORT)tz;
                     if (fn == msgHdr.OrigNet && fo == msgHdr.OrigNode)
                        FromZone = (USHORT)fz;
                  }
                  else if (!strncmp (szLine, "\001FLAGS ", 7)) {
                     if (strstr (szLine, "DIR"))
                        Direct = TRUE;
                  }
                  pLine = szLine;
                  nCol = 0;
               }
               else if (*pBuff != '\n') {
                  *pLine++ = *pBuff;
                  nCol++;
                  if (nCol >= 80) {
                     *pLine = '\0';
                     pLine = szLine;
                     nCol = 0;
                  }
               }
            }

            sprintf (FromAddress, "%u:%u/%u.%u", FromZone, msgHdr.OrigNet, msgHdr.OrigNode, FromPoint);
            sprintf (ToAddress, "%u:%u/%u.%u", ToZone, msgHdr.DestNet, msgHdr.DestNode, ToPoint);

            if (Locked == FALSE)
               close (fdTxt);
         }
      }
   }

   return (RetVal);
}

USHORT HUDSON::Read (ULONG ulMsg, SHORT nWidth)
{
   return (Read (ulMsg, Text, nWidth));
}

USHORT HUDSON::Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth)
{
   int fz, fn, fo, tz, tn, to;
   USHORT RetVal = FALSE, SkipNext = FALSE, FromPoint, ToPoint, FromZone, ToZone;
   SHORT i, m, nReaded, nCol;
   CHAR File[128];

   MsgText.Clear ();
   ToPoint = ToZone = FromPoint = FromZone = 0;

   if ((RetVal = ReadHeader (ulMsg)) == TRUE) {
      if (Locked == FALSE) {
         sprintf (File, "%smsgtxt.bbs", BaseName);
         fdTxt = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
      }
      if (fdTxt != -1) {
         lseek (fdTxt, (long)msgHdr.StartBlock * 256L, SEEK_SET);

         pLine = szLine;
         nCol = 0;

         for (m = 0; m < msgHdr.NumBlocks; m++) {
            read (fdTxt, szBuff, 256);
            nReaded = szBuff[0];

            for (i = 0, pBuff = &szBuff[1]; i < nReaded; i++, pBuff++) {
               if (*pBuff == '\r') {
                  *pLine = '\0';
                  if (!strncmp (szLine, "\001FMPT ", 6))
                     FromPoint = (USHORT)atoi (&szLine[6]);
                  else if (!strncmp (szLine, "\001TOPT ", 6))
                     ToPoint = (USHORT)atoi (&szLine[6]);
                  else if (!strncmp (szLine, "\001INTL ", 6)) {
                     sscanf (&szLine[6], "%d:%d/%d %d:%d/%d", &tz, &tn, &to, &fz, &fn, &fo);
                     if (tn == msgHdr.DestNet && to == msgHdr.DestNode)
                        ToZone = (USHORT)tz;
                     if (fn == msgHdr.OrigNet && fo == msgHdr.OrigNode)
                        FromZone = (USHORT)fz;
                  }
                  else if (!strncmp (szLine, "\001FLAGS ", 7)) {
                     if (strstr (szLine, "DIR"))
                        Direct = TRUE;
                  }
                  if (pLine > szLine && SkipNext == TRUE) {
                     pLine--;
                     while (pLine > szLine && *pLine == ' ')
                        *pLine-- = '\0';
                     if (pLine > szLine)
                        MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
                  }
                  else if (SkipNext == FALSE)
                     MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
                  SkipNext = FALSE;
                  pLine = szLine;
                  nCol = 0;
               }
               else if (*pBuff != '\n') {
                  *pLine++ = *pBuff;
                  nCol++;
                  if (nCol >= nWidth) {
                     *pLine = '\0';
                     if (strchr (szLine, ' ') != NULL) {
                        while (nCol > 1 && *pLine != ' ') {
                           nCol--;
                           pLine--;
                        }
                        if (nCol > 0) {
                           while (*pLine == ' ')
                              pLine++;
                           strcpy (szWrp, pLine);
                        }
                        *pLine = '\0';
                     }
                     else
                        szWrp[0] = '\0';
                     MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
                     strcpy (szLine, szWrp);
                     pLine = strchr (szLine, '\0');
                     nCol = (SHORT)strlen (szLine);
                     SkipNext = TRUE;
                  }
               }
            }
         }

         sprintf (FromAddress, "%u:%u/%u.%u", FromZone, msgHdr.OrigNet, msgHdr.OrigNode, FromPoint);
         sprintf (ToAddress, "%u:%u/%u.%u", ToZone, msgHdr.DestNet, msgHdr.DestNode, ToPoint);

         if (Locked == FALSE)
            close (fdTxt);
      }
   }

   return (RetVal);
}

VOID HUDSON::SetHWM (ULONG ulMsg)
{
   ulMsg = ulMsg;
}

ULONG HUDSON::UidToMsgn (ULONG ulMsg)
{
   int i;
   ULONG Num = 1L;

   if (msgIdx != NULL && TotalMsgs != 0L) {
      for (i = 0; i < msgInfo.TotalMsgs; i++) {
         if (msgIdx[i].Board == BoardNum && msgIdx[i].MsgNum != 0xFFFFU) {
            if (msgIdx[i].MsgNum == ulMsg) {
               ulMsg = Num;
               break;
            }
            Num++;
         }
      }
   }

   return (ulMsg);
}

VOID HUDSON::UnLock (VOID)
{
   int fd;
   CHAR File[128];

   if (Locked == TRUE) {
      sprintf (File, "%smsginfo.bbs", BaseName);
      if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
         write (fd, &msgInfo, sizeof (HMSGINFO));
         close (fd);

         sprintf (File, "%smsgidx.bbs", BaseName);
         if ((fd = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
            write (fd, msgIdx, msgInfo.TotalMsgs * sizeof (HMSGIDX));
            close (fd);
         }

         if (fdHdr != -1)
            close (fdHdr);
         if (fdTxt != -1)
            close (fdTxt);
         if (fdToIdx != -1)
            close (fdToIdx);
      }
   }
}

USHORT HUDSON::WriteHeader (ULONG ulMsg)
{
   int i;
   USHORT RetVal = FALSE;
   CHAR File[128];

   if (msgIdx != NULL && TotalMsgs != 0L) {
      for (i = 0; i < msgInfo.TotalMsgs; i++) {
         if (msgIdx[i].Board == BoardNum && msgIdx[i].MsgNum == ulMsg && msgIdx[i].MsgNum != 0xFFFFU) {
            RetVal = TRUE;
            break;
         }
      }
   }

   if (RetVal == TRUE) {
      if (Locked == FALSE) {
         sprintf (File, "%smsghdr.bbs", BaseName);
         fdHdr = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
      }
      if (fdHdr != -1) {
         lseek (fdHdr, (long)i * (long)sizeof (HMSGHDR), SEEK_SET);
         read (fdHdr, &msgHdr, sizeof (HMSGHDR));

         msgHdr.MsgAttr = msgHdr.NetAttr = 0;
         if (Local == TRUE)
            msgHdr.MsgAttr |= HUD_LOCAL;
         if (Private == TRUE)
            msgHdr.MsgAttr |= HUD_PRIVATE;
         if (Received == TRUE)
            msgHdr.MsgAttr |= HUD_RECEIVED;
         if (Crash == TRUE)
            msgHdr.NetAttr |= HUD_CRASH;
         if (KillSent == TRUE)
            msgHdr.NetAttr |= HUD_KILL;
         if (Sent == TRUE)
            msgHdr.NetAttr |= HUD_SENT;
         if (FileAttach == TRUE)
            msgHdr.NetAttr |= HUD_FILE;
         if (FileRequest == TRUE)
            msgHdr.NetAttr |= HUD_FRQ;

         lseek (fdHdr, (long)i * (long)sizeof (HMSGHDR), SEEK_SET);
         write (fdHdr, &msgHdr, sizeof (HMSGHDR));

         if (Locked == FALSE)
            close (fdHdr);
      }
   }

   return (RetVal);
}


