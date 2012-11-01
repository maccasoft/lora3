
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.21
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/17/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "mailer.h"

TImport::TImport (void)
{
   InFile = NULL;
   strcpy (SourcePath, ".\\");
   LastRead = '\0';
   Msg = NULL;
   LastTag[0] = '\0';
   MsgTossed = Duplicate = Bad = 0L;
   Current = 0L;
}

TImport::TImport (PSZ pszSourcePath)
{
   InFile = NULL;
   strcpy (SourcePath, pszSourcePath);
   if (SourcePath[strlen (SourcePath) - 1] != '\\')
      strcat (SourcePath, "\\");
   LastRead = '\0';
   Msg = NULL;
   LastTag[0] = '\0';
   MsgTossed = Duplicate = Bad = 0L;
}

TImport::~TImport (void)
{
   CloseFile ();

   if (Msg != NULL) {
      delete Msg;
      Msg = NULL;
   }
}

VOID TImport::CloseFile (VOID)
{
   if (InFile != NULL) {
      fclose (InFile);
      InFile = NULL;
   }
}

VOID TImport::DeleteFile (VOID)
{
   CloseFile ();
   unlink (OpenFileName);
}

USHORT TImport::Forward (class TAddress &Addr)
{
   int fd;
   USHORT RetVal = FALSE;
   PSZ p;
   PKT2HDR PktHdr;
   MSGHDR MsgHdr;

   if (Addr.First () == TRUE)
      do {
         fd = -1;
         if (Addr.Point != 0) {
            sprintf (Line, "%04x%04x.pnt\\%08x.xpr", Addr.Net, Addr.Node, Addr.Point);
            if ((fd = open (Line, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) == -1) {
               sprintf (Line, "%04x%04x.pnt", Addr.Net, Addr.Node);
               mkdir (Line);
               sprintf (Line, "%04x%04x.pnt\\%08x.xpr", Addr.Net, Addr.Node, Addr.Point);
               fd = open (Line, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
            }
         }
         else {
            sprintf (Line, "%04x%04x.xpr", Addr.Net, Addr.Node);
            fd = open (Line, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
         }

         if (fd != -1) {
            if (filelength (fd) == 0L) {
               memset (&PktHdr, 0, sizeof (PktHdr));
               PktHdr.OrigNode = HostNode;
               PktHdr.DestNode = Addr.Node;
               PktHdr.Version = 2;
               PktHdr.OrigNet = HostNet;
               PktHdr.DestNet = Addr.Net;
               PktHdr.OrigZone2 = PktHdr.OrigZone = HostZone;
               PktHdr.DestZone = Addr.Zone;
               PktHdr.OrigPoint = HostPoint;
               PktHdr.DestPoint = Addr.Point;
               PktHdr.Capability = 1;
               PktHdr.CWValidation = 256;
               write (fd, &PktHdr, sizeof (PktHdr));
            }

            memset (&MsgHdr, 0, sizeof (MsgHdr));
            MsgHdr.Version = 2;
            MsgHdr.OrigNet = HostNet;
            MsgHdr.OrigNode = HostNode;
            MsgHdr.DestNet = Addr.Net;
            MsgHdr.DestNode = Addr.Node;
            if (Msg->Private == TRUE)
               MsgHdr.Attrib |= 1;
            write (fd, &MsgHdr, sizeof (MsgHdr));

            write (fd, "", 1);
            write (fd, To, strlen (To) + 1);
            write (fd, From, strlen (From) + 1);
            write (fd, Subject, strlen (Subject) + 1);

            sprintf (Line, "AREA: %s\r", LastTag);
            write (fd, Line, strlen (Line));

            if ((p = (PSZ)Text.First ()) != NULL)
               do {
                  write (fd, p, strlen (p));
                  write (fd, "\r", 1);
               } while ((p = (PSZ)Text.Next ()) != NULL);

            close (fd);
         }
      } while (Addr.Next () == TRUE);

   return (RetVal);
}

USHORT TImport::GetLine (VOID)
{
   int c;
   USHORT Readed = 0, MaybeEOM = FALSE;
   PSZ Ptr = Line;

   if (LastRead == 13) {
      MaybeEOM = TRUE;
      if ((c = fgetc (InFile)) >= ' ' || c == 0x01) {
        *Ptr++ = (CHAR)c;
        Readed++;
      }
      else if (c != 10)
         ungetc (c, InFile);
   }

   do {
      if (Readed < sizeof (Line) - 1) {
         if ((c = fgetc (InFile)) >= ' ' || c == 0x01) {
            *Ptr++ = (CHAR)c;
            Readed++;
         }
      }
   } while (Readed < sizeof (Line) - 1 && (c == 1 || c >= ' ') && c != EOF);

   *Ptr = '\0';
   LastRead = (CHAR)c;

   if (MaybeEOM == TRUE && c == 0)
      ungetc (c, InFile);

   return ((Readed == 0 && c == EOF) ? FALSE : TRUE);
}

USHORT TImport::OpenArea (PSZ pszEchoTag)
{
   USHORT Found = FALSE;
   class TMsgData *Data;

   Links.Clear ();
   if (Msg != NULL) {
      delete Msg;
      Msg = NULL;
   }

   if ((Data = new TMsgData (".\\")) != NULL) {
      if (Data->First () == TRUE)
         do {
            if (!stricmp (Data->EchoTag, pszEchoTag))
               Found = TRUE;
         } while (Found == FALSE && Data->Next () == TRUE);
      if (Found == TRUE) {
         if (Data->Storage == ST_JAM)
            Msg = new JAM (Data->Path);
         else if (Data->Storage == ST_SQUISH)
            Msg = new SQUISH (Data->Path);
      }
      if (Data->Feeder.First () == TRUE)
         Links.Add (Data->Feeder.String);
      if (Data->Forward.First () == TRUE)
         do {
            Links.Add (Data->Forward.String);
         } while (Data->Forward.Next () == TRUE);
      delete Data;
   }

   strcpy (LastTag, &Line[5]);
   if (Found == FALSE || Msg == NULL)
      Msg = new JAM ("BAD_MSGS");

   return (Found);
}

USHORT TImport::OpenNextFile (VOID)
{
   USHORT retVal = FALSE, PktDate, PktTime, MaxBad;
   CHAR Filename[128];
   PSZ p;
   struct find_t blk;

   sprintf (Filename, "%s*.PK?", SourcePath);
   if (!_dos_findfirst (Filename, 0, &blk)) {
      strcpy (PktName, blk.name);
      PktDate = (USHORT)blk.wr_date;
      PktTime = (USHORT)blk.wr_time;

      while (!_dos_findnext (&blk)) {
         if (PktDate > blk.wr_date || (PktDate == blk.wr_date && PktTime > blk.wr_time)) {
            strcpy (PktName, blk.name);
            PktDate = (USHORT)blk.wr_date;
            PktTime = (USHORT)blk.wr_time;
         }
      }

      retVal = TRUE;
   }

   if (retVal == TRUE) {
      MaxBad = 0;
      sprintf (Filename, "%sBAD_PKT.*", SourcePath);
      if (!_dos_findfirst (Filename, 0, &blk))
         do {
            if ((p = strchr (blk.name, '.')) != NULL) {
               p++;
               if (atoi (p) > MaxBad)
                  MaxBad = (USHORT)atoi (p);
            }
         } while (!_dos_findnext (&blk));

// We have found a valid packet. To avoid system lockups due to an invalid
// packet file we now rename the file to something that prevents the same
// file to be read two times.
      sprintf (Filename, "%s%s", SourcePath, PktName);
      sprintf (OpenFileName, "%sBAD_PKT.%03d", SourcePath, ++MaxBad);
      rename (Filename, OpenFileName);

      if ((InFile = fopen (OpenFileName, "rb")) == NULL)
         retVal = FALSE;
   }

   return (retVal);
}

USHORT TImport::ProcessFile (VOID)
{
   USHORT retVal = FALSE, i;
   PKT2HDR Hdr;
   PKT22HDR Hdr22;

   if (InFile != NULL) {
      fread (&Hdr, sizeof (Hdr), 1, InFile);
      if (Hdr.Version == 2) {
         if (Hdr.Rate == 2) {
            memcpy (&Hdr22, &Hdr, sizeof (Hdr22));
            retVal = ProcessType22Packet (Hdr22);
         }
         else {
            swab ((char *)&Hdr.CWValidation, (char *)&i, 2);
            Hdr.CWValidation = i;
            if (Hdr.Capability != Hdr.CWValidation || !(Hdr.Capability & 0x0001))
               retVal = ProcessType2Packet (Hdr);
            else
               retVal = ProcessType2plusPacket (Hdr);
         }
      }
   }

   return (retVal);
}

VOID TImport::ProcessPacket (VOID)
{
   int c;
   USHORT isBad = FALSE, FirstImport = TRUE;
   ULONG Position;
   MSGHDR Hdr;
   class TAddress Addr;

   Text.Clear ();
   cprintf ("\r\nPacket %s   Orig: %u:%u/%u.%u   Dest: %u:%u/%u.%u\r\n", OpenFileName, EchoZone, EchoNet, EchoNode, EchoPoint, DestZone, DestNet, DestNode, DestPoint);

   if (InFile != NULL) {
      do {
         memset (&Hdr, 0, sizeof (Hdr));
         fread (&Hdr, sizeof (Hdr), 1, InFile);
         if (Hdr.Version == 2) {
            GetLine ();                 // Date and time field
            GetLine ();                 // To field
            strcpy (To, Line);
            GetLine ();                 // From field
            strcpy (From, Line);
            GetLine ();                 // Subject field
            strcpy (Subject, Line);

            if ((c = fgetc (InFile)) != 0 && c != EOF) {
               ungetc (c, InFile);
               if (GetLine () == TRUE) {
// The first line of the message body determines if it is a netmail or
// an echomail message. Echomail messages begins with an AREA: line which
// indicates the tag of the echomail area.
                  if (!strnicmp (Line, "AREA:", 5)) {
// It is an echomail area. Now if the echotag is different than the previous
// message, we try to open the new area.
                     if (stricmp (LastTag, &Line[5])) {
                        if (FirstImport == FALSE)
                           cprintf ("%5lu> ... Total: <%5lu>\r\n", Current, Msg->Number ());
                        isBad = FALSE;
                        if (OpenArea (&Line[5]) == FALSE)
                           isBad = TRUE;
                        Current = 0L;
                        strcpy (Line, ".............................................");
                        memcpy (Line, LastTag, strlen (LastTag));
                        cprintf ("   %s <%5lu>\b\b\b\b\b\b", Line, Current);
                     }
                  }
                  else {
// We've got a netmail message. Opens a special message base that hold
// only the netmail messages.
                     NetMail++;
                     if (stricmp (LastTag, "Netmail")) {
                        if (FirstImport == FALSE)
                           cprintf ("%5lu> ... Total: <%5lu>\r\n", Current, Msg->Number ());
                        strcpy (LastTag, "Netmail");
                        if (Msg != NULL)
                           delete Msg;
                        Msg = new JAM ("Netmail");
                        Current = 0L;
                        strcpy (Line, ".............................................");
                        memcpy (Line, LastTag, strlen (LastTag));
                        cprintf ("   %s <%5lu>\b\b\b\b\b\b", Line, Current);
                     }
                     Text.Add (Line, (USHORT)(strlen (Line) + 1));
                  }
               }

               while ((c = fgetc (InFile)) != 0 && c != EOF) {
                  ungetc (c, InFile);
                  if (GetLine () == TRUE)
                     Text.Add (Line, (USHORT)(strlen (Line) + 1));
               }
            }

            if (Msg != NULL) {
               Msg->New ();
               strcpy (Msg->From, From);
               strcpy (Msg->To, To);
               strcpy (Msg->Subject, Subject);
               Addr.Clear ();
               Addr.Add (FromZone, Hdr.OrigNet, Hdr.OrigNode, FromPoint);
               if (Addr.First ()) {
                  strcpy (Msg->FromAddress, Addr.String);
                  Addr.Clear ();
               }
               if (!stricmp (LastTag, "Netmail")) {
                  Addr.Clear ();
                  Addr.Add (ToZone, Hdr.DestNet, Hdr.DestNode, ToPoint);
                  if (Addr.First ()) {
                     strcpy (Msg->ToAddress, Addr.String);
                     Addr.Clear ();
                  }
               }
               Msg->Write (Msg->Highest () + 1, Text);
               Current++;
               cprintf ("%5lu\b\b\b\b\b", Current);
               if (stricmp (LastTag, "Netmail"))
                  Forward (Links);
               FirstImport = FALSE;
            }

            MsgTossed++;
            if (isBad == TRUE)
               Bad++;

            Text.Clear ();
         }
         else if (Hdr.Version != 0) {
// It is possible that we are out of sync with the packet file, so we seek
// each null until we found a valid type2 message header.
            printf ("Packet %s out of sync position %lX\n", OpenFileName, ftell (InFile));
            while ((c = fgetc (InFile)) != EOF && c != 0)
               ;
            if (c == 0) {
               Position = ftell (InFile);
               memset (&Hdr, 0, sizeof (Hdr));
               fread (&Hdr, sizeof (Hdr), 1, InFile);
               fseek (InFile, Position, SEEK_SET);
            }
            else
               Hdr.Version = 0;
         }
      } while (!feof (InFile) && Hdr.Version != 0);
      if (FirstImport == FALSE)
         cprintf ("%5lu> ... Total: <%5lu>\r\n", Current, Msg->Number ());
   }
}

USHORT TImport::ProcessType2Packet (PKT2HDR &Hdr)
{
   USHORT retVal = TRUE;

   EchoZone = Hdr.OrigZone;
   EchoNet = Hdr.OrigNet;
   EchoNode = Hdr.OrigNode;
   EchoPoint = Hdr.OrigPoint;

   FromZone = Hdr.OrigZone;
   FromPoint = Hdr.OrigPoint;
   DestZone = ToZone = Hdr.DestZone;
   DestNet = Hdr.DestNet;
   DestNode = Hdr.DestNode;
   DestPoint = ToPoint = Hdr.DestPoint;

   ProcessPacket ();

   return (retVal);
}

USHORT TImport::ProcessType22Packet (PKT22HDR &Hdr)
{
   USHORT retVal = TRUE;

   EchoZone = Hdr.OrigZone;
   EchoNet = Hdr.OrigNet;
   EchoNode = Hdr.OrigNode;
   EchoPoint = Hdr.OrigPoint;

   FromZone = Hdr.OrigZone;
   FromPoint = Hdr.OrigPoint;
   DestZone = ToZone = Hdr.DestZone;
   DestNet = Hdr.DestNet;
   DestNode = Hdr.DestNode;
   DestPoint = ToPoint = Hdr.DestPoint;

   ProcessPacket ();

   return (retVal);
}

USHORT TImport::ProcessType2plusPacket (PKT2HDR &Hdr)
{
   USHORT retVal = TRUE;

   EchoZone = Hdr.OrigZone2;
   EchoNet = Hdr.OrigNet;
   EchoNode = Hdr.OrigNode;
   EchoPoint = Hdr.OrigPoint;

   FromZone = Hdr.OrigZone;
   FromPoint = Hdr.OrigPoint;
   DestZone = ToZone = Hdr.DestZone;
   DestNet = Hdr.DestNet;
   DestNode = Hdr.DestNode;
   DestPoint = ToPoint = Hdr.DestPoint;

   ProcessPacket ();

   return (retVal);
}

void main (void)
{
   long t;
   class TImport *Import;
   class TMsgData *Data;

   unlink ("msg.dat");
   unlink ("msg.idx");

   if ((Data = new TMsgData (".\\")) != NULL) {
      Data->New ();
      strcpy (Data->Key, "1");
      Data->Storage = ST_JAM;
      strcpy (Data->Path, "msg\\loraita");
      strcpy (Data->EchoTag, "LORA.ITA");
      Data->Add ();

      Data->New ();
      strcpy (Data->Key, "2");
      Data->Storage = ST_JAM;
      strcpy (Data->Path, "msg\\lora");
      strcpy (Data->EchoTag, "LORA");
      Data->Add ();

      Data->New ();
      strcpy (Data->Key, "3");
      Data->Storage = ST_JAM;
      strcpy (Data->Path, "msg\\local");
      strcpy (Data->EchoTag, "SYSOP_CHAT.402");
      Data->Add ();

      Data->New ();
      strcpy (Data->Key, "4");
      Data->Storage = ST_JAM;
      strcpy (Data->Path, "msg\\annunc");
      strcpy (Data->EchoTag, "ANNUNCI.400");
      Data->Add ();

      Data->New ();
      strcpy (Data->Key, "5");
      Data->Storage = ST_JAM;
      strcpy (Data->Path, "msg\\os2dev");
      strcpy (Data->EchoTag, "OS2_DEV.ITA");
      Data->Add ();

      Data->New ();
      strcpy (Data->Key, "6");
      Data->Storage = ST_JAM;
      strcpy (Data->Path, "msg\\os2appl");
      strcpy (Data->EchoTag, "OS2_APPL.ITA");
      Data->Add ();

      delete Data;
   }

   if ((Import = new TImport) != NULL) {
      t = time (NULL);
      while (Import->OpenNextFile () == TRUE) {
         Import->ProcessFile ();
         Import->DeleteFile ();
      }
      t = time (NULL) - t;
      printf ("%ld msgs. - exec. time %ld sec. (%ld msgs./sec)\n", Import->MsgTossed, t, Import->MsgTossed / t);
      delete Import;
   }
}


