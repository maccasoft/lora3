
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

#if defined(__OS2__) && defined(__BORLANDC__)
#define _System      _pascal
#endif

#if defined(__LINUX__)
#include <errno.h>
#endif
#include "_ldefs.h"
#include "combase.h"

#if defined(__OS2__)
VOID WaitThread (PVOID Args)
{
   FILE *fp;
#if defined(__LINUX__)
   int i, s;
   socklen_t namelen;
#elif
   int i, s, namelen;
#endif

   struct sockaddr_in client, sock;
   struct hostent *host;
   class TTcpip *Tcp = (class TTcpip *)Args;

   Tcp->Accepted = 0;

   namelen = sizeof (client);
   if ((s = accept (Tcp->LSock, (struct sockaddr *)&client, &namelen)) > 0) {
      sprintf (Tcp->ClientIP, "%ld.%ld.%ld.%ld", (client.sin_addr.s_addr & 0xFFL), (client.sin_addr.s_addr & 0xFF00L) >> 8, (client.sin_addr.s_addr & 0xFF0000L) >> 16, (client.sin_addr.s_addr & 0xFF000000L) >> 24);
#if defined(__OS2__) || defined(__DOS__)
      if ((host = gethostbyaddr ((char *)&client.sin_addr.s_addr, sizeof (client.sin_addr.s_addr), AF_INET)) != NULL)
         strcpy (Tcp->ClientName, host->h_name);
      else
#endif
         strcpy (Tcp->ClientName, Tcp->ClientIP);
      Tcp->Accepted = (USHORT)s;
      i = 1;
#if defined(__OS2__) || defined(__DOS__) || defined(__LINUX__)
      ioctl (Tcp->Accepted, FIONBIO, (char *)&i, sizeof (int));
#elif defined(__NT__)
      ioctlsocket (Tcp->Accepted, FIONBIO, (unsigned long *)&i);
#endif

      namelen = sizeof (struct sockaddr_in);
      getsockname (Tcp->Accepted, (struct sockaddr *)&sock, &namelen);
      Tcp->HostID = (sock.sin_addr.s_addr & 0xFF000000L) >> 24;
      Tcp->HostID |= (sock.sin_addr.s_addr & 0x00FF0000L) >> 8;
      Tcp->HostID |= (sock.sin_addr.s_addr & 0x0000FF00L) << 8;
      Tcp->HostID |= (sock.sin_addr.s_addr & 0x000000FFL) << 24;
      if ((fp = fopen ("sock.log", "wt")) != NULL) {
         fprintf (fp, "HOSTID %ld,%ld,%ld,%ld", (Tcp->HostID & 0xFF000000L) >> 24, (Tcp->HostID & 0xFF0000L) >> 16, (Tcp->HostID & 0xFF00L) >> 8, (Tcp->HostID & 0xFFL));
         fclose (fp);
      }
   }

   _endthread ();
}
#endif

TTcpip::TTcpip ()
{
   EndRun = FALSE;
   fCarrierDown = FALSE;
   TxBytes = RxBytes = RxPosition = 0;
   LSock = Sock = 0;
}

TTcpip::~TTcpip ()
{
   ClosePort ();
}

VOID TTcpip::BufferByte (UCHAR byte)
{
   TxBuffer[TxBytes++] = byte;
   if (TxBytes >= TSIZE)
      UnbufferBytes ();
}

VOID TTcpip::BufferBytes (UCHAR *bytes, USHORT len)
{
   USHORT ToCopy;

   if (len > 0 && EndRun == FALSE)
      do {
         ToCopy = len;
         if (ToCopy > TSIZE - TxBytes)
            ToCopy = (USHORT)(TSIZE - TxBytes);
         memcpy (&TxBuffer[TxBytes], bytes, ToCopy);
         bytes += ToCopy;
         TxBytes += ToCopy;
         len -= ToCopy;
         if (TxBytes >= TSIZE)
            UnbufferBytes ();
      } while (len > 0 && EndRun == FALSE && Carrier () == TRUE);
}

USHORT TTcpip::BytesReady ()
{
   int i;
   USHORT RetVal = FALSE;

   if (Sock != 0 && fCarrierDown == FALSE && EndRun == FALSE) {
      if (RxBytes != 0)
         RetVal = TRUE;
      else {
         if ((i = recv (Sock, (char *)RxBuffer, sizeof (RxBuffer), 0)) == 0)
            fCarrierDown = TRUE;
         else if (i == -1) {
            RxBytes = 0;
#if defined(__OS2__)
            if (sock_errno () != SOCEWOULDBLOCK)
#elif defined(__NT__)
            if (WSAGetLastError () != WSAEWOULDBLOCK)
#elif defined(__LINUX__)
            if (errno != EWOULDBLOCK)
#endif
               fCarrierDown = TRUE;
         }
         else {
            RxBytes = (USHORT)i;
            RxPosition = 0;
            RetVal = TRUE;
         }
      }
   }

#if defined(__OS2__)
   if (RetVal == FALSE)
      DosSleep (1L);
#elif defined(__NT__)
   if (RetVal == FALSE)
      Sleep (1L);
#endif

   return (RetVal);
}

USHORT TTcpip::Carrier ()
{
   return ((fCarrierDown == TRUE) ? FALSE : TRUE);
}

VOID TTcpip::ClearInbound ()
{
   RxBytes = 0;
}

VOID TTcpip::ClearOutbound ()
{
}

VOID TTcpip::ClosePort ()
{
   if (Sock != 0) {
#if defined(__NT__)
      closesocket (Sock);
#elif defined(__OS2__)
      soclose (Sock);
#elif defined(__DOS__) || defined(__LINUX__)
      close (Sock);
#endif
      Sock = 0;
   }
   if (LSock != 0) {
#if defined(__NT__)
      closesocket (LSock);
#elif defined(__OS2__)
      soclose (LSock);
#elif defined(__DOS__) || defined(__LINUX__)
      close (LSock);
#endif
      LSock = 0;
   }
}

USHORT TTcpip::ConnectServer (PSZ pszServer, USHORT usPort)
{
#if !defined(__LINUX__)
   int i, namelen;
#else
   int i;
   socklen_t namelen;
#endif
   USHORT RetVal = FALSE, Port;
   struct hostent *hostnm;
   struct sockaddr_in server, sock;
#if defined(__NT__)
   WSADATA wsaData;
#endif

#if defined(__OS2__)
   if (sock_init () == 0) {
#elif defined(__DOS__)
   if (vec_search () != 0) {
#elif defined(__NT__)
   if (WSAStartup (MAKEWORD(1, 1), &wsaData) == 0) {
#endif

      Port = usPort;
      server.sin_family = AF_INET;
      server.sin_port = htons (Port);
#if defined(__OS2__) || defined(__NT__) || defined(__LINUX__)
#if !defined(__LINUX__)
      if (isdigit (pszServer[0]))
         server.sin_addr.s_addr = inet_addr (pszServer);
      else {
#endif
         if ((hostnm = gethostbyname (pszServer)) == NULL) {
            fCarrierDown = TRUE;
            return (FALSE);
         }
         server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);
#if !defined(__LINUX__)
      }
#endif
#elif defined(__DOS__)
      hostnm = gethostbyname (pszServer);
      bcopy(hostnm->h_addr, &server.sin_addr, hostnm->h_length);
#endif

      if ((Sock = socket (AF_INET, SOCK_STREAM, 0)) >= 0) {
         if (connect (Sock, (struct sockaddr *)&server, sizeof (server)) >= 0) {
            i = 1;
#if defined(__OS2__) || defined(__DOS__) || defined(__LINUX__)
            ioctl (Sock, FIONBIO, (char *)&i, sizeof (int));
#elif defined(__NT__)
            ioctlsocket (Sock, FIONBIO, (unsigned long *)&i);
#endif

            namelen = sizeof (struct sockaddr_in);
            getsockname (Sock, (struct sockaddr *)&sock, &namelen);
            HostID = (sock.sin_addr.s_addr & 0xFF000000L) >> 24;
            HostID |= (sock.sin_addr.s_addr & 0x00FF0000L) >> 8;
            HostID |= (sock.sin_addr.s_addr & 0x0000FF00L) << 8;
            HostID |= (sock.sin_addr.s_addr & 0x000000FFL) << 24;

            RetVal = TRUE;
         }
     }
#if !defined(__LINUX__)
   }
#endif

   if (RetVal == FALSE)
      fCarrierDown = TRUE;

   return (RetVal);
}

USHORT TTcpip::Initialize (USHORT usPort, USHORT usSocket, USHORT usProtocol)
{
   int i, socktype = SOCK_STREAM;
   USHORT RetVal = FALSE;
   struct sockaddr_in server;
#if defined(__NT__)
   int addrSize;
   char sHostName[64];
   struct hostent *pHostEnt;
   WSADATA wsaData;
#endif

#if defined(__OS2__)
   if (sock_init () == 0) {
#elif defined(__DOS__)
   if (vec_search () != 0) {
#elif defined(__NT__)
   if (WSAStartup (MAKEWORD(1, 1), &wsaData) == 0) {
#endif

#if defined(__OS2__)
      HostID = gethostid ();
#elif defined(__LINUX__)
      HostID = (gethostid () & 0x00FF0000L) << 8;
      HostID |= (gethostid () & 0xFF000000L) >> 8;
      HostID |= (gethostid () & 0x000000FFL) << 8;
      HostID |= (gethostid () & 0x0000FF00L) >> 8;
#elif defined(__NT__)
      gethostname (sHostName, sizeof (sHostName));
      pHostEnt = gethostbyname (sHostName);
      HostID = *(unsigned long *)pHostEnt->h_addr;
#else
      HostID = 0x7F000001L;
#endif
      sprintf (HostIP, "%ld.%ld.%ld.%ld", (HostID & 0xFF000000L) >> 24, (HostID & 0xFF0000L) >> 16, (HostID & 0xFF00L) >> 8, (HostID & 0xFFL));

      if (usSocket == 0) {
         if (usProtocol == IPPROTO_UDP)
            socktype = SOCK_DGRAM;
         if ((LSock = socket (AF_INET, socktype, usProtocol)) >= 0) {
            server.sin_family = AF_INET;
            server.sin_port = htons (usPort);
            server.sin_addr.s_addr = INADDR_ANY;
            if (bind (LSock, (struct sockaddr *)&server, sizeof (server)) >= 0) {
#if defined(__NT__)
               sprintf (ClientIP, "%ld.%ld.%ld.%ld", (server.sin_addr.s_addr & 0xFFL), (server.sin_addr.s_addr & 0xFF00L) >> 8, (server.sin_addr.s_addr & 0xFF0000L) >> 16, (server.sin_addr.s_addr & 0xFF000000L) >> 24);
               addrSize = sizeof (struct sockaddr);
               getsockname (LSock, (struct sockaddr *)&server, &addrSize);
#endif
               i = 1;
#if defined(__DOS__)
               ioctl (LSock, FIONBIO, (char *)&i, sizeof (int));
#elif defined(__NT__)
               ioctlsocket (LSock, FIONBIO, (unsigned long *)&i);
#elif defined(__OS2__)
               if (usProtocol == IPPROTO_UDP)
                  ioctl (LSock, FIONBIO, (char *)&i, sizeof (int));
#endif
               if (usProtocol == IPPROTO_TCP) {
                  if (listen (LSock, 1) >= 0) {
                     Sock = 0;
#if defined(__OS2__)
                     Accepted = 0;
                     _beginthread (WaitThread, NULL, 32767, (PVOID)this);
#endif
                     RetVal = TRUE;
                  }
               }
               else
                  RetVal = TRUE;
            }
         }
      }
      else {
         Sock = usSocket;
         i = 1;
#if defined(__OS2__) || defined(__DOS__)
         ioctl (Sock, FIONBIO, (char *)&i, sizeof (int));
#elif defined(__NT__)
         ioctlsocket (Sock, FIONBIO, (unsigned long *)&i);
#endif
         RetVal = TRUE;
      }
#if !defined(__LINUX__)
   }
#endif

   return (RetVal);
}

UCHAR TTcpip::ReadByte ()
{
   int i;
   short c = 0;

   if (Sock != 0) {
      while (RxBytes == 0 && EndRun == FALSE && fCarrierDown == FALSE) {
         if ((i = recv (Sock, (char *)RxBuffer, sizeof (RxBuffer), 0)) == 0)
            fCarrierDown = TRUE;
         else if (i == -1) {
            RxBytes = 0;
#if defined(__OS2__)
            if (sock_errno () != SOCEWOULDBLOCK)
#elif defined(__NT__)
            if (WSAGetLastError () != WSAEWOULDBLOCK)
#elif defined(__LINUX__)
            if (errno != EWOULDBLOCK)
#endif
               fCarrierDown = TRUE;
         }
         else {
            RxBytes = (USHORT)i;
            RxPosition = 0;
         }
      }

      if (EndRun == FALSE && fCarrierDown == FALSE) {
         c = RxBuffer[RxPosition++];
         RxBytes--;
      }
   }

   return ((UCHAR)c);
}

USHORT TTcpip::ReadBytes (UCHAR *bytes, USHORT len)
{
   int i;
   USHORT Max;

   if (Sock != 0) {
      while (RxBytes == 0 && EndRun == FALSE && fCarrierDown == FALSE) {
         if ((i = recv (Sock, (char *)RxBuffer, sizeof (RxBuffer), 0)) == 0)
            fCarrierDown = TRUE;
         else if (i == -1) {
            RxBytes = 0;
#if defined(__OS2__)
            if (sock_errno () != SOCEWOULDBLOCK)
#elif defined(__NT__)
            if (WSAGetLastError () != WSAEWOULDBLOCK)
#elif defined(__LINUX__)
            if (errno != EWOULDBLOCK)
#endif
               fCarrierDown = TRUE;
         }
         else {
            RxBytes = (USHORT)i;
            RxPosition = 0;
         }
      }

      if (EndRun == FALSE && fCarrierDown == FALSE) {
         if ((Max = len) > RxBytes)
            Max = RxBytes;
         memcpy (bytes, &RxBuffer[RxPosition], Max);
         RxBytes -= Max;
         RxPosition += Max;
      }
   }

   return (Max);
}

USHORT TTcpip::PeekPacket (PVOID lpBuffer, USHORT usSize)
{
#if !defined(__LINUX__)
   int namelen = sizeof (udp_client);
#else
   socklen_t namelen = sizeof (udp_client);
#endif
   if (recvfrom (LSock, (char *)lpBuffer, usSize, MSG_PEEK, (struct sockaddr *)&udp_client, &namelen) > 0)
      return (TRUE);

   return (FALSE);
}

USHORT TTcpip::GetPacket (PVOID lpBuffer, USHORT usSize)
{
#if !defined(__LINUX__)
   int namelen = sizeof (udp_client);
#else
   socklen_t namelen = sizeof (udp_client);
#endif
   return ((USHORT)recvfrom (LSock, (char *)lpBuffer, usSize, 0, (struct sockaddr *)&udp_client, &namelen));
}

USHORT TTcpip::SendPacket (PVOID lpBuffer, USHORT usSize)
{
   return ((USHORT)sendto (LSock, (char *)lpBuffer, usSize, 0, (struct sockaddr *)&udp_client, sizeof (udp_client)));
}

USHORT TTcpip::WaitClient ()
{
#if defined(__OS2__)
   if (Accepted != 0 && Sock == 0)
      Sock = Accepted;
   else if (Sock != 0 && Accepted != 0) {
      Sock = Accepted = 0;
      _beginthread (WaitThread, NULL, 32767, (PVOID)this);
   }
#else
#if !defined(__LINUX__)
   int i, s, namelen;
#else
   int i, s;
   socklen_t namelen;
#endif
   struct sockaddr_in client;
   struct hostent *host;

   Sock = 0;

   namelen = sizeof (client);
   if ((s = accept (LSock, (struct sockaddr *)&client, &namelen)) > 0) {
      Sock = (USHORT)s;
      sprintf (ClientIP, "%ld.%ld.%ld.%ld", (client.sin_addr.s_addr & 0xFFL), (client.sin_addr.s_addr & 0xFF00L) >> 8, (client.sin_addr.s_addr & 0xFF0000L) >> 16, (client.sin_addr.s_addr & 0xFF000000L) >> 24);
#if defined(__OS2__) || defined(__DOS__)
      if ((host = gethostbyaddr ((char *)&client.sin_addr.s_addr, sizeof (client.sin_addr.s_addr), AF_INET)) != NULL)
         strcpy (ClientName, host->h_name);
      else
#endif
         strcpy (ClientName, ClientIP);
      i = 1;
#if defined(__OS2__) || defined(__DOS__) || defined(__LINUX__)
      ioctl (Sock, FIONBIO, (char *)&i, sizeof (int));
#elif defined(__NT__)
      ioctlsocket (Sock, FIONBIO, (unsigned long *)&i);
#endif
   }
#endif

#if defined(__OS2__)
   DosSleep (1L);
#endif

   return ((USHORT)Sock);
}

VOID TTcpip::SendByte (UCHAR byte)
{
   if (Sock != 0 && fCarrierDown == FALSE && EndRun == FALSE)
      send (Sock, (char *)&byte, 1, 0);
}

VOID TTcpip::SendBytes (UCHAR *bytes, USHORT len)
{
   int i;

   if (Sock != 0 && fCarrierDown == FALSE && EndRun == FALSE) {
      do {
         if ((i = send (Sock, (char *)bytes, len, 0)) > 0) {
            len -= (USHORT)i;
            bytes += i;
         }
         else if (i < 0) {
#if defined(__OS2__)
            if (sock_errno () != SOCEWOULDBLOCK && sock_errno () != SOCENOBUFS)
#elif defined(__NT__)
            if (WSAGetLastError () != WSAEWOULDBLOCK && WSAGetLastError () != WSAENOBUFS)
#elif defined(__LINUX__)
            if (errno != EWOULDBLOCK && errno != ENOBUFS)
#endif
               fCarrierDown = TRUE;
         }
      } while (len > 0 && EndRun == FALSE && Carrier () == TRUE);
   }
}

VOID TTcpip::UnbufferBytes ()
{
   int i, Written;
   UCHAR *p;

   while (Sock != 0 && fCarrierDown == FALSE && EndRun == FALSE && TxBytes > 0) {
      i = 0;
#if defined(__OS2__) || defined(__DOS__) || defined(__LINUX__)
      ioctl (Sock, FIONBIO, (char *)&i, sizeof (int));
#elif defined(__NT__)
      ioctlsocket (Sock, FIONBIO, (unsigned long *)&i);
#endif

      p = TxBuffer;
      do {
         if ((Written = send (Sock, (char *)p, TxBytes, 0)) > 0) {
            p += Written;
            TxBytes -= (USHORT)Written;
         }
         else if (Written < 0) {
#if defined(__OS2__)
            if (sock_errno () != SOCEWOULDBLOCK && sock_errno () != SOCENOBUFS)
#elif defined(__NT__)
            if (WSAGetLastError () != WSAEWOULDBLOCK && WSAGetLastError () != WSAENOBUFS)
#elif defined(__LINUX__)
            if (errno != EWOULDBLOCK && errno != ENOBUFS)
#endif
               fCarrierDown = TRUE;
         }
      } while (TxBytes > 0 && EndRun == FALSE && Carrier () == TRUE);

      i = 1;
#if defined(__OS2__) || defined(__DOS__) || defined(__LINUX__)
      ioctl (Sock, FIONBIO, (char *)&i, sizeof (int));
#elif defined(__NT__)
      ioctlsocket (Sock, FIONBIO, (unsigned long *)&i);
#endif
   }
}

VOID TTcpip::SetName (PSZ name)
{
   name = name;
}

VOID TTcpip::SetCity (PSZ name)
{
   name = name;
}

VOID TTcpip::SetLevel (PSZ level)
{
   level = level;
}

VOID TTcpip::SetTimeLeft (ULONG seconds)
{
   seconds = seconds;
}

VOID TTcpip::SetTime (ULONG seconds)
{
   seconds = seconds;
}


