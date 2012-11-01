
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.19
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

//#if !defined(__OS2__) && !defined(__NT__)
//#error This module is for use with OS/2 and Windows NT only!
//#endif

#include "_ldefs.h"
#include "combase.h"

#if defined(__OS2__)
#include <types.h>
#include <netinet\in.h>
#include <sys\socket.h>
#include <netdb.h>
#include <sys\ioctl.h>
#elif defined(__DOS__)
extern "C" {
#include <sys\types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <pctcp/types.h>
#include <pctcp/rwconf.h>
#include <pctcp/pctcp.h>
#include <pctcp/syscalls.h>
};
#else
#error This module is for use with OS/2 and Windows NT only!
#endif

TTcpip::TTcpip (void)
{
   EndRun = FALSE;
   fCarrierDown = FALSE;
   TxBytes = RxBytes = RxPosition = 0;
   LSock = Sock = 0;
}

TTcpip::~TTcpip (void)
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
      } while (len > 0 && EndRun == FALSE);
}

SHORT TTcpip::BytesReady (VOID)
{
   int i;
   SHORT RetVal = FALSE;

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

   return (RetVal);
}

SHORT TTcpip::Carrier (VOID)
{
   SHORT RetVal = FALSE;
   CHAR Data[1];

   if (Sock != 0 && fCarrierDown == FALSE && EndRun == FALSE) {
      if (recv (Sock, (char *)Data, sizeof (Data), MSG_PEEK) > 0)
         RetVal = TRUE;
#if defined(__OS2__)
      else if (sock_errno () == SOCEWOULDBLOCK)
#elif defined(__NT__)
      else if (WSAGetLastError () != WSAEWOULDBLOCK)
#endif
         RetVal = TRUE;
   }

   return (RetVal);
}

VOID TTcpip::ClearInbound (VOID)
{
   RxBytes = 0;
}

VOID TTcpip::ClearOutbound (VOID)
{
}

VOID TTcpip::ClosePort (VOID)
{
   if (Sock != 0) {
#if defined(__NT__)
      closesocket (Sock);
#elif defined(__OS2__)
      soclose (Sock);
#elif defined(__DOS__)
      close (Sock);
#endif
      Sock = 0;
   }
   if (LSock != 0) {
#if defined(__NT__)
      closesocket (LSock);
#elif defined(__OS2__)
      soclose (LSock);
#elif defined(__DOS__)
      close (LSock);
#endif
      LSock = 0;
   }
}

SHORT TTcpip::ConnectServer (PSZ pszServer, USHORT usPort)
{
   INT i;
   SHORT RetVal = FALSE;
   USHORT Port;
   struct hostent *hostnm;
   struct sockaddr_in server;

#if defined(__OS2__)
   if (sock_init () == 0) {
      HostID = gethostid ();
      sprintf (HostIP, "%ld.%ld.%ld.%ld", (HostID & 0xFF000000L) >> 24, (HostID & 0xFF0000L) >> 16, (HostID & 0xFF00L) >> 8, (HostID & 0xFFL));
#elif defined(__DOS__)
   if (vec_search () != 0) {
#endif

      Port = usPort;
      server.sin_family = AF_INET;
      server.sin_port = htons (Port);
#if defined(__OS2__) || defined(__NT__)
      if (isdigit (pszServer[0]))
         server.sin_addr.s_addr = inet_addr (pszServer);
      else {
         if ((hostnm = gethostbyname (pszServer)) == NULL)
            return (FALSE);
         server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);
      }
#elif defined(__DOS__)
      hostnm = gethostbyname (pszServer);
      bcopy(hostnm->h_addr, &server.sin_addr, hostnm->h_length);
#endif

      if ((Sock = socket (AF_INET, SOCK_STREAM, 0)) >= 0) {
         if (connect (Sock, (struct sockaddr *)&server, sizeof (server)) >= 0) {
            i = 1;
#if defined(__OS2__) || defined(__DOS__)
            ioctl (Sock, FIONBIO, (char *)&i, sizeof (int));
#elif defined(__NT__)
            ioctlsocket (Sock, FIONBIO, (unsigned long *)&i);
#endif
            RetVal = TRUE;
         }
      }
#if defined(__OS2__) || defined(__DOS__)
   }
#endif

   return (RetVal);
}

USHORT TTcpip::Initialize (USHORT usPort, USHORT usSocket)
{
   int i;
   USHORT RetVal = FALSE;
   struct sockaddr_in server;

#if defined(__OS2__)
   if (sock_init () == 0) {
      HostID = gethostid ();
      sprintf (HostIP, "%ld.%ld.%ld.%ld", (HostID & 0xFF000000L) >> 24, (HostID & 0xFF0000L) >> 16, (HostID & 0xFF00L) >> 8, (HostID & 0xFFL));
#elif defined(__DOS__)
   if (vec_search () != 0) {
#endif
      if (usSocket == 0) {
         if ((LSock = socket (AF_INET, SOCK_STREAM, 0)) >= 0) {
            server.sin_family = AF_INET;
            server.sin_port = htons (usPort);
            server.sin_addr.s_addr = INADDR_ANY;
            if (bind (LSock, (struct sockaddr *)&server, sizeof (server)) >= 0) {
#if defined(__NT__)
               sprintf (ClientIP, "%ld.%ld.%ld.%ld", (server.sin_addr.s_addr & 0xFFL), (server.sin_addr.s_addr & 0xFF00L) >> 8, (server.sin_addr.s_addr & 0xFF0000L) >> 16, (server.sin_addr.s_addr & 0xFF000000L) >> 24);
#endif
               i = 1;
#if defined(__OS2__) || defined(__DOS__)
               ioctl (LSock, FIONBIO, (char *)&i, sizeof (int));
#elif defined(__NT__)
               ioctlsocket (LSock, FIONBIO, (unsigned long *)&i);
#endif
               if (listen (LSock, 1) >= 0)
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
#if defined(__OS2__) || defined(__DOS__)
   }
#endif

   return (RetVal);
}

UCHAR TTcpip::ReadByte (VOID)
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

USHORT TTcpip::WaitClient (VOID)
{
   int i, s, namelen;
   struct sockaddr_in client;
   struct hostent *host;

   Sock = 0;

   namelen = sizeof (client);
   if ((s = accept (LSock, (struct sockaddr *)&client, &namelen)) > 0) {
      Sock = (USHORT)s;
      sprintf (ClientIP, "%ld.%ld.%ld.%ld", (client.sin_addr.s_addr & 0xFFL), (client.sin_addr.s_addr & 0xFF00L) >> 8, (client.sin_addr.s_addr & 0xFF0000L) >> 16, (client.sin_addr.s_addr & 0xFF000000L) >> 24);
      if ((host = gethostbyaddr ((char *)&client.sin_addr.s_addr, sizeof (client.sin_addr.s_addr), AF_INET)) != NULL)
         strcpy (ClientName, host->h_name);
      else
         strcpy (ClientName, ClientIP);
      i = 1;
#if defined(__OS2__) || defined(__DOS__)
      ioctl (Sock, FIONBIO, (char *)&i, sizeof (int));
#elif defined(__NT__)
      ioctlsocket (Sock, FIONBIO, (unsigned long *)&i);
#endif
   }

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
#if defined(__OS2__)
            if (i < TxBytes)
               DosSleep (10L);
#endif
            len -= (USHORT)i;
            bytes += i;
         }
      } while (i >= 0 && len > 0 && EndRun == FALSE);
   }
}

VOID TTcpip::UnbufferBytes (VOID)
{
   int i;
   UCHAR *p;

   while (Sock != 0 && fCarrierDown == FALSE && EndRun == FALSE && TxBytes > 0) {
      p = TxBuffer;
      do {
         if ((i = send (Sock, (char *)p, TxBytes, 0)) > 0) {
#if defined(__OS2__)
            if (i < TxBytes)
               DosSleep (10L);
#endif
            p += i;
            TxBytes -= (USHORT)i;
         }
      } while (TxBytes > 0 && i > 0 && EndRun == FALSE);
   }
}


