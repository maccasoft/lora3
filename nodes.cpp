
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.20
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    08/02/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "nodes.h"

TNodes::TNodes (void)
{
   strcpy (DataPath, ".\\");
   fd = -1;
}

TNodes::TNodes (PSZ pszDataPath)
{
   strcpy (DataPath, pszDataPath);
   if (DataPath[0] != '\0' && DataPath[strlen (DataPath) - 1] != '\\')
      strcat (DataPath, "\\");
   fd = -1;
}

TNodes::~TNodes (void)
{
   if (fd != -1)
      close (fd);
}

VOID TNodes::Add (VOID)
{
   int fdn;
   USHORT DoClose = FALSE, Saved = FALSE;
   CHAR Temp[64];
   NODES Nodes;

   if (fd == -1) {
      sprintf (Temp, "%sNodes.Dat", DataPath);
      if ((fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
         DoClose = TRUE;
   }

   sprintf (Temp, "%sNodes.New", DataPath);
   while ((fdn = sopen (Temp, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYRW, S_IREAD|S_IWRITE)) == -1)
      ;

   if (fd != -1 && fdn != -1) {
      lseek (fd, 0L, SEEK_SET);
      Addr1.Parse (Address);
      strcpy (Address, Addr1.String);
      while (read (fd, &Nodes, sizeof (Nodes)) == sizeof (Nodes)) {
         if (Saved == FALSE) {
            Addr2.Parse (Nodes.Address);
            if (Addr2.Zone != Addr1.Zone) {
               if (Addr2.Zone > Addr1.Zone)
                  Saved = TRUE;
            }
            else if (Addr2.Net != Addr1.Net) {
               if (Addr2.Net > Addr1.Net)
                  Saved = TRUE;
            }
            else if (Addr2.Node != Addr1.Node) {
               if (Addr2.Node > Addr1.Node)
                  Saved = TRUE;
            }
            else if (Addr2.Point != Addr1.Point) {
               if (Addr2.Point > Addr1.Point)
                  Saved = TRUE;
            }
            else if (stricmp (Addr2.Domain, Addr1.Domain) > 0)
               Saved = TRUE;

            if (Saved == TRUE) {
               memset (&Nodes, 0, sizeof (Nodes));
               strcpy (Nodes.Address, Address);
               strcpy (Nodes.SystemName, SystemName);
               strcpy (Nodes.SysopName, SysopName);
               strcpy (Nodes.Location, Location);
               Nodes.Speed = Speed;
               Nodes.MinSpeed = MinSpeed;
               strcpy (Nodes.Phone, Phone);
               strcpy (Nodes.Flags, Flags);
               strcpy (Nodes.DialCmd, DialCmd);
               Nodes.RemapMail = RemapMail;
               strcpy (Nodes.SessionPwd, SessionPwd);
               strcpy (Nodes.AreaMgrPwd, AreaMgrPwd);
               strcpy (Nodes.OutPktPwd, OutPktPwd);
               strcpy (Nodes.InPktPwd, InPktPwd);
               Nodes.UsePkt22 = UsePkt22;
               write (fdn, &Nodes, sizeof (Nodes));

               lseek (fd, tell (fd) - sizeof (Nodes), SEEK_SET);
               read (fd, &Nodes, sizeof (Nodes));
            }
         }

         write (fdn, &Nodes, sizeof (Nodes));
      }

      if (Saved == FALSE) {
         memset (&Nodes, 0, sizeof (Nodes));
         strcpy (Nodes.Address, Address);
         strcpy (Nodes.SystemName, SystemName);
         strcpy (Nodes.SysopName, SysopName);
         strcpy (Nodes.Location, Location);
         Nodes.Speed = Speed;
         Nodes.MinSpeed = MinSpeed;
         strcpy (Nodes.Phone, Phone);
         strcpy (Nodes.Flags, Flags);
         strcpy (Nodes.DialCmd, DialCmd);
         Nodes.RemapMail = RemapMail;
         strcpy (Nodes.SessionPwd, SessionPwd);
         strcpy (Nodes.AreaMgrPwd, AreaMgrPwd);
         strcpy (Nodes.OutPktPwd, OutPktPwd);
         strcpy (Nodes.InPktPwd, InPktPwd);
         Nodes.UsePkt22 = UsePkt22;
         write (fdn, &Nodes, sizeof (Nodes));
      }

      lseek (fd, 0L, SEEK_SET);
      lseek (fdn, 0L, SEEK_SET);
      while (read (fdn, &Nodes, sizeof (Nodes)) == sizeof (Nodes))
         write (fd, &Nodes, sizeof (Nodes));
   }

   if (fdn != -1) {
      close (fdn);
      sprintf (Temp, "%sNodes.New", DataPath);
      unlink (Temp);
   }
   if (fd != -1 && DoClose == TRUE) {
      close (fd);
      fd = -1;
   }
}

VOID TNodes::Delete (VOID)
{
   int fdn;
   USHORT DoClose = FALSE;
   CHAR Temp[64], NewName[64];
   ULONG Position;
   NODES Nodes;

   if (fd == -1) {
      sprintf (Temp, "%sNodes.Dat", DataPath);
      if ((fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
         DoClose = TRUE;
   }
   sprintf (NewName, "%sNodes.New", DataPath);
   fdn = sopen (NewName, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fd != -1 && fdn != -1) {
      Position = 0L;
      lseek (fd, 0L, SEEK_SET);
      while (read (fd, &Nodes, sizeof (Nodes)) == sizeof (Nodes)) {
         if (stricmp (Nodes.Address, Address))
            write (fdn, &Nodes, sizeof (Nodes));
         else
            Position = tell (fd);
      }

      lseek (fd, 0L, SEEK_SET);
      lseek (fdn, 0L, SEEK_SET);

      while (read (fdn, &Nodes, sizeof (Nodes)) == sizeof (Nodes))
         write (fd, &Nodes, sizeof (Nodes));
      chsize (fd, tell (fd));

      if (Position > tell (fd))
         Position = tell (fd);

      if (Position >= sizeof (Temp)) {
         lseek (fd, Position - sizeof (Nodes), SEEK_SET);
         read (fd, &Nodes, sizeof (Nodes));
         strcpy (Address, Nodes.Address);
         strcpy (SystemName, Nodes.SystemName);
         strcpy (SysopName, Nodes.SysopName);
         strcpy (Location, Nodes.Location);
         Speed = Nodes.Speed;
         MinSpeed = Nodes.MinSpeed;
         strcpy (Phone, Nodes.Phone);
         strcpy (Flags, Nodes.Flags);
         strcpy (DialCmd, Nodes.DialCmd);
         RemapMail = Nodes.RemapMail;
         strcpy (SessionPwd, Nodes.SessionPwd);
         strcpy (AreaMgrPwd, Nodes.AreaMgrPwd);
         strcpy (OutPktPwd, Nodes.OutPktPwd);
         strcpy (InPktPwd, Nodes.InPktPwd);
         UsePkt22 = Nodes.UsePkt22;
      }
      else
         New ();
   }

   if (DoClose == TRUE) {
      close (fd);
      fd = -1;
   }

   if (fdn != -1)
      close (fdn);
   unlink (NewName);
}

USHORT TNodes::First (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[64];

   if (fd == -1) {
      sprintf (Temp, "%sNodes.Dat", DataPath);
      fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fd != -1) {
      lseek (fd, 0L, SEEK_SET);
      RetVal = Next ();
   }

   return (RetVal);
}

VOID TNodes::New (VOID)
{
   memset (Address, 0, sizeof (Address));
   memset (SystemName, 0, sizeof (SystemName));
   memset (SysopName, 0, sizeof (SysopName));
   memset (Location, 0, sizeof (Location));
   Speed = 0L;
   MinSpeed = 0L;
   memset (Phone, 0, sizeof (Phone));
   memset (Flags, 0, sizeof (Flags));
   memset (DialCmd, 0, sizeof (DialCmd));
   RemapMail = FALSE;
   memset (SessionPwd, 0, sizeof (SessionPwd));
   memset (AreaMgrPwd, 0, sizeof (AreaMgrPwd));
   memset (OutPktPwd, 0, sizeof (OutPktPwd));
   memset (InPktPwd, 0, sizeof (InPktPwd));
   UsePkt22 = FALSE;
}

USHORT TNodes::Next (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[64];
   NODES Nodes;

   if (fd == -1) {
      sprintf (Temp, "%sNodes.Dat", DataPath);
      fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fd != -1) {
      if (read (fd, &Nodes, sizeof (Nodes)) == sizeof (Nodes)) {
         strcpy (Address, Nodes.Address);
         strcpy (SystemName, Nodes.SystemName);
         strcpy (SysopName, Nodes.SysopName);
         strcpy (Location, Nodes.Location);
         Speed = Nodes.Speed;
         MinSpeed = Nodes.MinSpeed;
         strcpy (Phone, Nodes.Phone);
         strcpy (Flags, Nodes.Flags);
         strcpy (DialCmd, Nodes.DialCmd);
         RemapMail = Nodes.RemapMail;
         strcpy (SessionPwd, Nodes.SessionPwd);
         strcpy (AreaMgrPwd, Nodes.AreaMgrPwd);
         strcpy (OutPktPwd, Nodes.OutPktPwd);
         strcpy (InPktPwd, Nodes.InPktPwd);
         UsePkt22 = Nodes.UsePkt22;
         RetVal = TRUE;
      }
   }

   return (RetVal);
}

USHORT TNodes::Previous (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[64];
   NODES Nodes;

   if (fd == -1) {
      sprintf (Temp, "%sNodes.Dat", DataPath);
      fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fd != -1) {
      if (tell (fd) >= sizeof (Nodes) * 2) {
         lseek (fd, tell (fd) - sizeof (Nodes) * 2, SEEK_SET);
         read (fd, &Nodes, sizeof (Nodes));
         strcpy (Address, Nodes.Address);
         strcpy (SystemName, Nodes.SystemName);
         strcpy (SysopName, Nodes.SysopName);
         strcpy (Location, Nodes.Location);
         Speed = Nodes.Speed;
         MinSpeed = Nodes.MinSpeed;
         strcpy (Phone, Nodes.Phone);
         strcpy (Flags, Nodes.Flags);
         strcpy (DialCmd, Nodes.DialCmd);
         RemapMail = Nodes.RemapMail;
         strcpy (SessionPwd, Nodes.SessionPwd);
         strcpy (AreaMgrPwd, Nodes.AreaMgrPwd);
         strcpy (OutPktPwd, Nodes.OutPktPwd);
         strcpy (InPktPwd, Nodes.InPktPwd);
         UsePkt22 = Nodes.UsePkt22;
         RetVal = TRUE;
      }
   }

   return (RetVal);
}

USHORT TNodes::Read (USHORT usZone, USHORT usNet, USHORT usNode, USHORT usPoint, PSZ pszDomain)
{
   USHORT RetVal = FALSE, DoClose = FALSE;
   CHAR Temp[64];
   NODES Nodes;

   if (fd == -1) {
      sprintf (Temp, "%sNodes.Dat", DataPath);
      if ((fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
         DoClose = TRUE;
   }

   if (fd != -1) {
      Addr1.Clear ();
      Addr1.Add (usZone, usNet, usNode, usPoint, pszDomain);
      Addr1.First ();

      while (read (fd, &Nodes, sizeof (Nodes)) == sizeof (Nodes)) {
         if (!stricmp (Nodes.Address, Addr1.String)) {
            RetVal = TRUE;
            break;
         }
      }
      close (fd);
      Addr1.Clear ();

      if (RetVal == TRUE) {
         strcpy (Address, Nodes.Address);
         strcpy (SystemName, Nodes.SystemName);
         strcpy (SysopName, Nodes.SysopName);
         strcpy (Location, Nodes.Location);
         Speed = Nodes.Speed;
         MinSpeed = Nodes.MinSpeed;
         strcpy (Phone, Nodes.Phone);
         strcpy (Flags, Nodes.Flags);
         strcpy (DialCmd, Nodes.DialCmd);
         RemapMail = Nodes.RemapMail;
         strcpy (SessionPwd, Nodes.SessionPwd);
         strcpy (AreaMgrPwd, Nodes.AreaMgrPwd);
         strcpy (OutPktPwd, Nodes.OutPktPwd);
         strcpy (InPktPwd, Nodes.InPktPwd);
         UsePkt22 = Nodes.UsePkt22;
      }
   }

   if (fd != -1 && DoClose == TRUE) {
      close (fd);
      fd = -1;
   }

   return (RetVal);
}

USHORT TNodes::Read (class TAddress *lpAddress)
{
   USHORT RetVal = FALSE, DoClose = FALSE;
   CHAR Temp[64];
   NODES Nodes;

   if (fd == -1) {
      sprintf (Temp, "%sNodes.Dat", DataPath);
      if ((fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
         DoClose = TRUE;
   }

   if (fd != -1 && lpAddress != NULL) {
      while (read (fd, &Nodes, sizeof (Nodes)) == sizeof (Nodes)) {
         if (!stricmp (Nodes.Address, lpAddress->String)) {
            RetVal = TRUE;
            break;
         }
      }
      close (fd);

      if (RetVal == TRUE) {
         strcpy (Address, Nodes.Address);
         strcpy (SystemName, Nodes.SystemName);
         strcpy (SysopName, Nodes.SysopName);
         strcpy (Location, Nodes.Location);
         Speed = Nodes.Speed;
         MinSpeed = Nodes.MinSpeed;
         strcpy (Phone, Nodes.Phone);
         strcpy (Flags, Nodes.Flags);
         strcpy (DialCmd, Nodes.DialCmd);
         RemapMail = Nodes.RemapMail;
         strcpy (SessionPwd, Nodes.SessionPwd);
         strcpy (AreaMgrPwd, Nodes.AreaMgrPwd);
         strcpy (OutPktPwd, Nodes.OutPktPwd);
         strcpy (InPktPwd, Nodes.InPktPwd);
         UsePkt22 = Nodes.UsePkt22;
      }
   }

   if (fd != -1 && DoClose == TRUE) {
      close (fd);
      fd = -1;
   }

   return (RetVal);
}

USHORT TNodes::Read (class TAddress &lpAddress)
{
   USHORT RetVal = FALSE, DoClose = FALSE;
   CHAR Temp[64];
   NODES Nodes;

   if (fd == -1) {
      sprintf (Temp, "%sNodes.Dat", DataPath);
      if ((fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
         DoClose = TRUE;
   }

   if (fd != -1) {
      while (read (fd, &Nodes, sizeof (Nodes)) == sizeof (Nodes)) {
         if (!stricmp (Nodes.Address, lpAddress.String)) {
            RetVal = TRUE;
            break;
         }
      }
      close (fd);

      if (RetVal == TRUE) {
         strcpy (Address, Nodes.Address);
         strcpy (SystemName, Nodes.SystemName);
         strcpy (SysopName, Nodes.SysopName);
         strcpy (Location, Nodes.Location);
         Speed = Nodes.Speed;
         MinSpeed = Nodes.MinSpeed;
         strcpy (Phone, Nodes.Phone);
         strcpy (Flags, Nodes.Flags);
         strcpy (DialCmd, Nodes.DialCmd);
         RemapMail = Nodes.RemapMail;
         strcpy (SessionPwd, Nodes.SessionPwd);
         strcpy (AreaMgrPwd, Nodes.AreaMgrPwd);
         strcpy (OutPktPwd, Nodes.OutPktPwd);
         strcpy (InPktPwd, Nodes.InPktPwd);
         UsePkt22 = Nodes.UsePkt22;
      }
   }

   if (fd != -1 && DoClose == TRUE) {
      close (fd);
      fd = -1;
   }

   return (RetVal);
}

VOID TNodes::Update (VOID)
{
   USHORT DoClose = FALSE;
   CHAR Temp[64];
   NODES Nodes;

   if (fd == -1) {
      sprintf (Temp, "%sNodes.Dat", DataPath);
      if ((fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
         DoClose = TRUE;
   }

   if (fd != -1) {
      lseek (fd, 0L, SEEK_SET);
      Addr1.Parse (Address);
      strcpy (Address, Addr1.String);
      while (read (fd, &Nodes, sizeof (Nodes)) == sizeof (Nodes)) {
         if (!strcmp (Address, Nodes.Address)) {
            memset (&Nodes, 0, sizeof (Nodes));
            strcpy (Nodes.Address, Address);
            strcpy (Nodes.SystemName, SystemName);
            strcpy (Nodes.SysopName, SysopName);
            strcpy (Nodes.Location, Location);
            Nodes.Speed = Speed;
            Nodes.MinSpeed = MinSpeed;
            strcpy (Nodes.Phone, Phone);
            strcpy (Nodes.Flags, Flags);
            strcpy (Nodes.DialCmd, DialCmd);
            Nodes.RemapMail = RemapMail;
            strcpy (Nodes.SessionPwd, SessionPwd);
            strcpy (Nodes.AreaMgrPwd, AreaMgrPwd);
            strcpy (Nodes.OutPktPwd, OutPktPwd);
            strcpy (Nodes.InPktPwd, InPktPwd);
            Nodes.UsePkt22 = UsePkt22;

            lseek (fd, tell (fd) - sizeof (Nodes), SEEK_SET);
            write (fd, &Nodes, sizeof (Nodes));
            break;
         }
      }
   }

   if (fd != -1 && DoClose == TRUE) {
      close (fd);
      fd = -1;
   }
}




