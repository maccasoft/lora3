
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
#include "lora_api.h"

TAddress::TAddress (void)
{
   List.Clear ();
}

TAddress::~TAddress (void)
{
   List.Clear ();
}

SHORT TAddress::Add (VOID)
{
   return (Add (Zone, Net, Node, Point, Domain));
}

SHORT TAddress::Add (PSZ pszAddress)
{
   SHORT RetVal = FALSE;
   PSZ p;

   Zone = Net = Node = Point = 0;
   FakeNet = 0;
   Domain[0] = '\0';

   if (pszAddress != NULL) {
      if (strchr (pszAddress, ':') != NULL) {
         Zone = (USHORT)atoi (pszAddress);
         pszAddress = strchr (pszAddress, ':') + 1;
      }
      if (strchr (pszAddress, '/') != NULL) {
         Net = (USHORT)atoi (pszAddress);
         pszAddress = strchr (pszAddress, '/') + 1;
      }
      Node = (USHORT)atoi (pszAddress);
      if ((p = strchr (pszAddress, '@')) != NULL) {
         *p++ = '\0';
         strcpy (Domain, p);
      }
      if (strchr (pszAddress, '.') != NULL) {
         pszAddress = strchr (pszAddress, '.') + 1;
         Point = (USHORT)atoi (pszAddress);
      }

      RetVal = Add (Zone, Net, Node, Point, Domain);
   }

   return (RetVal);
}

SHORT TAddress::Add (USHORT usZone, USHORT usNet, USHORT usNode, USHORT usPoint, PSZ pszDomain)
{
   SHORT RetVal = TRUE;
   MAILADDRESS Addr, *Check;

   if ((Check = (MAILADDRESS *)List.First ()) != NULL)
      do {
         if (Check->Zone == usZone && Check->Node == usNode && Check->Net == usNet && Check->Point == usPoint)
            RetVal = FALSE;
      } while ((Check = (MAILADDRESS *)List.Next ()) != NULL);

   if (RetVal == TRUE) {
      memset (&Addr, 0, sizeof (Addr));
      Addr.Zone = usZone;
      Addr.Net= usNet;
      Addr.Node = usNode;
      Addr.Point = usPoint;
      if (pszDomain != NULL)
         strcpy (Addr.Domain, pszDomain);
      RetVal = List.Add (&Addr, sizeof (Addr));
   }

   return (RetVal);
}

VOID TAddress::Clear (VOID)
{
   List.Clear ();
   Zone = Net = Node = Point = 0;
   FakeNet = 0;
   Domain[0] = String[0] = '\0';
}

VOID TAddress::Delete (VOID)
{
   List.Remove ();
}

SHORT TAddress::First (VOID)
{
   SHORT RetVal = FALSE;
   MAILADDRESS *Addr;

   Zone = Net = Node = Point = 0;
   FakeNet = 0;
   Domain[0] = String[0] = '\0';

   if ((Addr = (MAILADDRESS *)List.First ()) != NULL) {
      Zone = Addr->Zone;
      Net = Addr->Net;
      Node = Addr->Node;
      Point = Addr->Point;
      strcpy (Domain, Addr->Domain);
      if (Point != 0)
         sprintf (String, "%u:%u/%u.%u", Zone, Net, Node, Point);
      else
         sprintf (String, "%u:%u/%u", Zone, Net, Node);
      if (Domain[0] != '\0') {
         strcat (String, "@");
         strcat (String, Domain);
      }
      FakeNet = Addr->FakeNet;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TAddress::Load (PSZ File)
{
   List.Clear ();
   return (Merge (File));
}

USHORT TAddress::Merge (PSZ File)
{
   int fd;
   USHORT RetVal = FALSE;
   CHAR Temp[128];
   MAILADDRESS Addr;

   strcpy (Temp, File);
   if (Temp[0] != '\0') {
#if defined(__LINUX__)
      if (Temp[strlen (Temp) - 1] != '/')
         strcat (Temp, "/");
#else
      if (Temp[strlen (Temp) - 1] != '\\')
         strcat (Temp, "\\");
#endif
   }
   strcat (Temp, "address.dat");
   if ((fd = sopen (Temp, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      RetVal = TRUE;
      while (read (fd, &Addr, sizeof (MAILADDRESS)) == sizeof (MAILADDRESS))
         List.Add (&Addr, sizeof (MAILADDRESS));
      close (fd);
   }

   return (RetVal);
}

SHORT TAddress::Next (VOID)
{
   SHORT RetVal = FALSE;
   MAILADDRESS *Addr;

   if ((Addr = (MAILADDRESS *)List.Next ()) != NULL) {
      Zone = Addr->Zone;
      Net = Addr->Net;
      Node = Addr->Node;
      Point = Addr->Point;
      strcpy (Domain, Addr->Domain);
      if (Point != 0)
         sprintf (String, "%u:%u/%u.%u", Zone, Net, Node, Point);
      else
         sprintf (String, "%u:%u/%u", Zone, Net, Node);
      if (Domain[0] != '\0') {
         strcat (String, "@");
         strcat (String, Domain);
      }
      FakeNet = Addr->FakeNet;
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TAddress::Parse (PSZ pszAddress)
{
   PSZ p;

   Zone = Net = Node = Point = 0;
   FakeNet = 0;
   Domain[0] = '\0';

   if (pszAddress != NULL) {
      if (strchr (pszAddress, ':') != NULL) {
         Zone = (USHORT)atoi (pszAddress);
         pszAddress = strchr (pszAddress, ':') + 1;
      }
      if (strchr (pszAddress, '/') != NULL) {
         if (!stricmp (pszAddress, "all") || !strcmp (pszAddress, "*"))
            Net = 65535U;
         else
            Net = (USHORT)atoi (pszAddress);
         pszAddress = strchr (pszAddress, '/') + 1;
      }
      if (!stricmp (pszAddress, "all") || !strcmp (pszAddress, "*")) {
         Node = 65535U;
         if (Net == 0)
            Net = Node;
      }
      else
         Node = (USHORT)atoi (pszAddress);
      if ((p = strchr (pszAddress, '@')) != NULL) {
         *p++ = '\0';
         strcpy (Domain, p);
      }
      if (strchr (pszAddress, '.') != NULL) {
         pszAddress = strchr (pszAddress, '.') + 1;
         if (!stricmp (pszAddress, "all") || !strcmp (pszAddress, "*"))
            Point = 65535U;
         else
            Point = (USHORT)atoi (pszAddress);
      }

      if (Point != 0)
         sprintf (String, "%u:%u/%u.%u", Zone, Net, Node, Point);
      else
         sprintf (String, "%u:%u/%u", Zone, Net, Node);
      if (Domain[0] != '\0') {
         strcat (String, "@");
         strcat (String, Domain);
      }
   }
}

VOID TAddress::Update (VOID)
{
   MAILADDRESS *Addr;

   if ((Addr = (MAILADDRESS *)List.Value ()) != NULL) {
      Addr->Zone = Zone;
      Addr->Net = Net;
      Addr->Node = Node;
      Addr->Point = Point;
      strcpy (Addr->Domain, Domain);
      Addr->FakeNet = FakeNet;
   }

   if (Point != 0)
      sprintf (String, "%u:%u/%u.%u", Zone, Net, Node, Point);
   else
      sprintf (String, "%u:%u/%u", Zone, Net, Node);
   if (Domain[0] != '\0') {
      strcat (String, "@");
      strcat (String, Domain);
   }
}

USHORT TAddress::Save (PSZ File)
{
   int fd;
   USHORT RetVal = FALSE;
   CHAR Temp[128];
   MAILADDRESS *Addr;

   strcpy (Temp, File);
   if (Temp[0] != '\0') {
#if defined(__LINUX__)
      if (Temp[strlen (Temp) - 1] != '/')
         strcat (Temp, "/");
#else
      if (Temp[strlen (Temp) - 1] != '\\')
         strcat (Temp, "\\");
#endif
   }
   strcat (Temp, "address.dat");
   if ((fd = sopen (Temp, O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      RetVal = TRUE;
      if ((Addr = (MAILADDRESS *)List.First ()) != NULL)
         do {
            write (fd, Addr, sizeof (MAILADDRESS));
         } while ((Addr = (MAILADDRESS *)List.Next ()) != NULL);
      close (fd);
   }

   return (RetVal);
}


