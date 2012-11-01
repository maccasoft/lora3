
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.06
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/21/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "tools.h"

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
   ADDR Addr;

   memset (&Addr, 0, sizeof (Addr));
   Addr.Zone = usZone;
   Addr.Net= usNet;
   Addr.Node = usNode;
   Addr.Point = usPoint;
   if (pszDomain != NULL)
      strcpy (Addr.Domain, pszDomain);

   return (List.Add (&Addr, sizeof (Addr)));
}

VOID TAddress::Clear (VOID)
{
   List.Clear ();
}

SHORT TAddress::First (VOID)
{
   SHORT RetVal = FALSE;
   ADDR *Addr;

   Zone = Net = Node = Point = 0;
   Domain[0] = String[0] = '\0';

   if ((Addr = (ADDR *)List.First ()) != NULL) {
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
      RetVal = TRUE;
   }

   return (RetVal);
}

SHORT TAddress::Next (VOID)
{
   SHORT RetVal = FALSE;
   ADDR *Addr;

   if ((Addr = (ADDR *)List.Next ()) != NULL) {
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
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TAddress::Parse (PSZ pszAddress)
{
   PSZ p;

   Zone = Net = Node = Point = 0;
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


