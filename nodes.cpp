
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.7
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    08/02/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora_api.h"

TNodes::TNodes (void)
{
#if defined(__LINUX__)
   strcpy (DataFile, "./nodes");
#else
   strcpy (DataFile, ".\\nodes");
#endif
   fd = -1;
}

TNodes::TNodes (PSZ pszDataPath)
{
   strcpy (DataFile, pszDataPath);
   if (DataFile[0] != '\0') {
#if defined(__LINUX__)
      if (DataFile[strlen (DataFile) - 1] != '\\')
         strcat (DataFile, "/");
#else
      if (DataFile[strlen (DataFile) - 1] != '\\')
         strcat (DataFile, "\\");
#endif
   }
   strcat (DataFile, "nodes");
   fd = -1;
}

TNodes::~TNodes (void)
{
   if (fd != -1)
      close (fd);
}

VOID TNodes::Class2Struct (NODES &Nodes)
{
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
   Nodes.CreateNewAreas = CreateNewAreas;
   strcpy (Nodes.NewAreasFilter, NewAreasFilter);
   strcpy (Nodes.TicPwd, TicPwd);
   strcpy (Nodes.Packer, Packer);
   Nodes.ImportPOP3Mail = ImportPOP3Mail;
   Nodes.UseInetAddress = UseInetAddress;
   strcpy (Nodes.InetAddress, InetAddress);
   strcpy (Nodes.Pop3Pwd, Pop3Pwd);
}

VOID TNodes::Struct2Class (NODES &Nodes)
{
   class TAddress Addr;

   strcpy (Address, Nodes.Address);
   Addr.Parse (Address);
   Zone = Addr.Zone;
   Net = Addr.Net;
   Node = Addr.Node;
   Point = Addr.Point;
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
   CreateNewAreas = Nodes.CreateNewAreas;
   strcpy (NewAreasFilter, Nodes.NewAreasFilter);
   strcpy (TicPwd, Nodes.TicPwd);
   strcpy (Packer, Nodes.Packer);
   ImportPOP3Mail = Nodes.ImportPOP3Mail;
   UseInetAddress = Nodes.UseInetAddress;
   strcpy (InetAddress, Nodes.InetAddress);
   strcpy (Pop3Pwd, Nodes.Pop3Pwd);
}

VOID TNodes::Add (VOID)
{
   int fdn;
   USHORT DoClose = FALSE, Saved = FALSE;
   CHAR Temp[64];
   NODES Nodes;

   if (fd == -1) {
      sprintf (Temp, "%s.dat", DataFile);
      if ((fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
         DoClose = TRUE;
   }

   sprintf (Temp, "%s.new", DataFile);
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
               Class2Struct (Nodes);
               write (fdn, &Nodes, sizeof (Nodes));

               lseek (fd, tell (fd) - sizeof (Nodes), SEEK_SET);
               read (fd, &Nodes, sizeof (Nodes));
            }
         }

         write (fdn, &Nodes, sizeof (Nodes));
      }

      if (Saved == FALSE) {
         Class2Struct (Nodes);
         write (fdn, &Nodes, sizeof (Nodes));
      }

      lseek (fd, 0L, SEEK_SET);
      lseek (fdn, 0L, SEEK_SET);
      while (read (fdn, &Nodes, sizeof (Nodes)) == sizeof (Nodes))
         write (fd, &Nodes, sizeof (Nodes));
   }

   if (fdn != -1) {
      close (fdn);
      sprintf (Temp, "%s.new", DataFile);
      unlink (Temp);
   }
   if (fd != -1 && DoClose == TRUE) {
      close (fd);
      fd = -1;
   }

   if (Read (Addr1, FALSE) == FALSE) {
      if (First () == FALSE)
         New ();
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
      sprintf (Temp, "%s.dat", DataFile);
      if ((fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
         DoClose = TRUE;
   }
   sprintf (NewName, "%s.new", DataFile);
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
         Struct2Class (Nodes);
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
      sprintf (Temp, "%s.dat", DataFile);
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
   memset (TicPwd, 0, sizeof (TicPwd));
   memset (NewAreasFilter, 0, sizeof (NewAreasFilter));
   UsePkt22 = FALSE;
   CreateNewAreas = FALSE;
   ImportPOP3Mail = UseInetAddress = FALSE;
   memset (InetAddress, 0, sizeof (InetAddress));
   memset (Pop3Pwd, 0, sizeof (Pop3Pwd));
   memset (Packer, 0, sizeof (Packer));
}

USHORT TNodes::Next (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[64];
   NODES Nodes;

   if (fd == -1) {
      sprintf (Temp, "%s.dat", DataFile);
      fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fd != -1) {
      if (read (fd, &Nodes, sizeof (Nodes)) == sizeof (Nodes)) {
         Struct2Class (Nodes);
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
      sprintf (Temp, "%s.dat", DataFile);
      fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fd != -1) {
      if (tell (fd) >= sizeof (Nodes) * 2) {
         lseek (fd, tell (fd) - sizeof (Nodes) * 2, SEEK_SET);
         read (fd, &Nodes, sizeof (Nodes));
         Struct2Class (Nodes);
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

   New ();

   if (fd == -1) {
      sprintf (Temp, "%s.dat", DataFile);
      if ((fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
         DoClose = TRUE;
   }

   if (fd != -1) {
      Addr1.Clear ();
      Addr1.Add (usZone, usNet, usNode, usPoint, pszDomain);
      Addr1.First ();

      lseek (fd, 0L, SEEK_SET);
      while (read (fd, &Nodes, sizeof (Nodes)) == sizeof (Nodes)) {
         if (!stricmp (Nodes.Address, Addr1.String)) {
            RetVal = TRUE;
            break;
         }
      }

      Addr1.Clear ();

      if (RetVal == TRUE)
         Struct2Class (Nodes);
   }

   if (fd != -1 && DoClose == TRUE) {
      close (fd);
      fd = -1;
   }

   return (RetVal);
}

USHORT TNodes::Read (class TAddress *lpAddress, USHORT flAddNodelist)
{
   USHORT RetVal = FALSE, DoClose = FALSE;
   CHAR Temp[64];
   NODES Nodes;

   New ();

   if (fd == -1) {
      sprintf (Temp, "%s.Dat", DataFile);
      if ((fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
         DoClose = TRUE;
   }

   if (fd != -1 && lpAddress != NULL) {
      lseek (fd, 0L, SEEK_SET);
      while (read (fd, &Nodes, sizeof (Nodes)) == sizeof (Nodes)) {
         if (!stricmp (Nodes.Address, lpAddress->String)) {
            RetVal = TRUE;
            break;
         }
      }

      if (RetVal == TRUE)
         Struct2Class (Nodes);
   }

   if (fd != -1 && DoClose == TRUE) {
      close (fd);
      fd = -1;
   }

   if (flAddNodelist == TRUE) {
      if (ReadNodelist (lpAddress) == TRUE) {
         strcpy (Address, lpAddress->String);
         RetVal = TRUE;
      }
   }

   return (RetVal);
}

USHORT TNodes::Read (class TAddress &lpAddress, USHORT flAddNodelist)
{
   USHORT RetVal = FALSE, DoClose = FALSE;
   CHAR Temp[64];
   NODES Nodes;

   New ();

   if (fd == -1) {
      sprintf (Temp, "%s.dat", DataFile);
      if ((fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
         DoClose = TRUE;
   }

   if (fd != -1) {
      lseek (fd, 0L, SEEK_SET);
      while (read (fd, &Nodes, sizeof (Nodes)) == sizeof (Nodes)) {
         if (!stricmp (Nodes.Address, lpAddress.String)) {
            RetVal = TRUE;
            break;
         }
      }

      if (RetVal == TRUE)
         Struct2Class (Nodes);
   }

   if (fd != -1 && DoClose == TRUE) {
      close (fd);
      fd = -1;
   }

   if (flAddNodelist == TRUE) {
      if (ReadNodelist (lpAddress) == TRUE) {
         strcpy (Address, lpAddress.String);
         RetVal = TRUE;
      }
   }

   return (RetVal);
}

USHORT TNodes::Read (PSZ pszAddress, USHORT flAddNodelist)
{
   class TAddress Addr;

   Addr.Parse (pszAddress);

   return (Read (Addr, flAddNodelist));
}

USHORT TNodes::ReadNodelist (class TAddress &Address)
{
   return (ReadNodelist (&Address));
}

USHORT TNodes::ReadNodelist (class TAddress *Address)
{
   FILE *fp, *fpText;
   USHORT RetVal = FALSE, First;
   ULONG Num;
   CHAR Temp[128], *p;
   IDXHEADER idxHead;
   NODEIDX nodeIdx;

   sprintf (Temp, "%s.idx", DataFile);
   if ((fp = _fsopen (Temp, "rb", SH_DENYNO)) != NULL) {
      while (fread (&idxHead, sizeof (IDXHEADER), 1, fp) == 1 && RetVal == FALSE) {
         for (Num = 0L; Num < idxHead.Entry && RetVal == FALSE; Num++) {
            if (fread (&nodeIdx, sizeof (NODEIDX), 1, fp) != 1)
               break;
            if (nodeIdx.Zone == Address->Zone && nodeIdx.Net == Address->Net && (nodeIdx.Node == 0 || nodeIdx.Node == Address->Node)) {
               strcpy (Temp, DataFile);
               if ((p = strchr (Temp, '\0')) != NULL) {
                  while (p > Temp && *p != '\\')
                     *p-- = '\0';
               }
               strcat (Temp, idxHead.Name);
               if ((fpText = _fsopen (Temp, "rt", SH_DENYNO)) != NULL) {
                  fseek (fpText, nodeIdx.Position, SEEK_SET);
                  First = FALSE;
                  while (fgets (Temp, sizeof (Temp) - 1, fpText) != NULL && RetVal == FALSE) {
                     if (Temp[0] != ';') {
                        if (Temp[0] != ',') {
                           p = strtok (Temp, ",");
                           if (!stricmp (p, "Zone") || !stricmp (p, "Region") || !stricmp (p, "Host")) {
                              if (First == TRUE)
                                 break;
                              First = TRUE;
                              p = strtok (NULL, ",");
                              *p = '\0';
                           }
                           else
                              p = strtok (NULL, ",");
                        }
                        else
                           p = strtok (Temp, ",");

                        if (p != NULL) {
                           if (Address->Node == (USHORT)atoi (p)) {
                              RetVal = TRUE;
                              if ((p = strtok (NULL, ",")) != NULL) {
                                 if (strlen (p) > sizeof (SystemName))
                                    p[sizeof (SystemName)] = '\0';
                                 if (SystemName[0] == '\0') {
                                    strcpy (SystemName, p);
                                    while ((p = strchr (SystemName, '_')) != NULL)
                                       *p = ' ';
                                 }
                              }
                              if ((p = strtok (NULL, ",")) != NULL) {
                                 if (strlen (p) > sizeof (Location))
                                    p[sizeof (Location)] = '\0';
                                 if (Location[0] == '\0') {
                                    strcpy (Location, p);
                                    while ((p = strchr (Location, '_')) != NULL)
                                       *p = ' ';
                                 }
                              }
                              if ((p = strtok (NULL, ",")) != NULL) {
                                 if (strlen (p) > sizeof (SysopName))
                                    p[sizeof (SysopName)] = '\0';
                                 if (SysopName[0] == '\0') {
                                    strcpy (SysopName, p);
                                    while ((p = strchr (SysopName, '_')) != NULL)
                                       *p = ' ';
                                 }
                              }
                              if ((p = strtok (NULL, ",")) != NULL) {
                                 if (strlen (p) > sizeof (Phone))
                                    p[sizeof (Phone)] = '\0';
                                 if (Phone[0] == '\0') {
                                    strcpy (Phone, p);
                                    while ((p = strchr (Phone, '_')) != NULL)
                                       *p = ' ';
                                 }
                              }
                              if ((p = strtok (NULL, ",")) != NULL) {
                                 if (Speed == 0L)
                                    Speed = atol (p);
                              }
                              if ((p = strtok (NULL, ",")) != NULL) {
                                 if (strlen (p) > sizeof (Flags))
                                    p[sizeof (Flags)] = '\0';
                                 if (Flags[0] == '\0')
                                    strcpy (Flags, p);
                              }
                           }
                        }
                     }
                  }
                  fclose (fpText);
               }
            }
         }
      }
      fclose (fp);
   }

   return (RetVal);
}

VOID TNodes::Update (VOID)
{
   USHORT DoClose = FALSE;
   CHAR Temp[64];
   NODES Nodes;

   if (fd == -1) {
      sprintf (Temp, "%s.dat", DataFile);
      if ((fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
         DoClose = TRUE;
   }

   if (fd != -1) {
      lseek (fd, 0L, SEEK_SET);
      Addr1.Parse (Address);
      strcpy (Address, Addr1.String);
      while (read (fd, &Nodes, sizeof (Nodes)) == sizeof (Nodes)) {
         if (!strcmp (Address, Nodes.Address)) {
            Class2Struct (Nodes);
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

USHORT TNodes::FirstNodelist (VOID)
{
   USHORT RetVal = FALSE;
   NODELIST *data;

   if ((data = (NODELIST *)ListData.First ()) != NULL) {
      DefaultZone = data->Zone;
      strcpy (Nodelist, data->Name);
      strcpy (Nodediff, data->Diff);
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TNodes::NextNodelist (VOID)
{
   USHORT RetVal = FALSE;
   NODELIST *data;

   if ((data = (NODELIST *)ListData.Next ()) != NULL) {
      DefaultZone = data->Zone;
      strcpy (Nodelist, data->Name);
      strcpy (Nodediff, data->Diff);
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TNodes::AddNodelist (PSZ name, PSZ diff, USHORT zone)
{
   NODELIST data;

   memset (&data, 0, sizeof (NODELIST));
   data.Size = sizeof (NODELIST);
   data.Zone = zone;
   strcpy (data.Name, name);
   strcpy (data.Diff, diff);
   ListData.Add (&data, sizeof (NODELIST));
}

VOID TNodes::LoadNodelist (VOID)
{
   int fd;
   CHAR Temp[64];
   NODELIST data;

   ListData.Clear ();

   strcpy (Temp, DataFile);
   strcpy (&Temp[strlen (Temp) - 5], "nodelist.dat");
   if ((fd = open (Temp, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
      while (read (fd, &data, sizeof (data)) == sizeof (data))
         ListData.Add (&data, sizeof (data));
      close (fd);
   }
}

VOID TNodes::SaveNodelist (VOID)
{
   int fd;
   CHAR Temp[64];
   NODELIST *data;

   strcpy (Temp, DataFile);
   strcpy (&Temp[strlen (Temp) - 5], "nodelist.dat");
   if ((fd = open (Temp, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) != -1) {
      if ((data = (NODELIST *)ListData.First ()) != NULL)
         do {
            data->Size = sizeof (NODELIST);
            write (fd, data, sizeof (NODELIST));
         } while ((data = (NODELIST *)ListData.Next ()) != NULL);
      close (fd);
   }
}

VOID TNodes::DeleteNodelist (VOID)
{
   ListData.Remove ();
}

