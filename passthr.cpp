
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
#include "msgbase.h"

PASSTHR::PASSTHR ()
{
   Id = 0L;
}

PASSTHR::~PASSTHR ()
{
}

USHORT PASSTHR::Add ()
{
   return (Add (Text));
}

USHORT PASSTHR::Add (class TMsgBase *MsgBase)
{
   New ();

   strcpy (From, MsgBase->From);
   strcpy (To, MsgBase->To);
   strcpy (Subject, MsgBase->Subject);

   if (MsgBase->FromAddress[0] != '\0')
      strcpy (FromAddress, MsgBase->FromAddress);
   if (MsgBase->ToAddress[0] != '\0')
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

USHORT PASSTHR::Add (class TCollection &MsgText)
{
   PSZ p;

   Text.Clear ();
   if ((p = (PSZ)MsgText.First ()) != NULL)
      do {
         Text.Add (p);
      } while ((p = (PSZ)MsgText.Next ()) != NULL);

   return (TRUE);
}

VOID PASSTHR::Close ()
{
}

USHORT PASSTHR::Delete (ULONG ulMsg)
{
   ulMsg = ulMsg;
   return (FALSE);
}

USHORT PASSTHR::GetHWM (ULONG &ulMsg)
{
   ulMsg = 0L;
   return (TRUE);
}

ULONG PASSTHR::Highest ()
{
   return (0L);
}

USHORT PASSTHR::Lock (ULONG ulTimeout)
{
   ulTimeout = ulTimeout;
   return (TRUE);
}

ULONG PASSTHR::Lowest ()
{
   return (0L);
}

ULONG PASSTHR::MsgnToUid (ULONG ulMsg)
{
   return (ulMsg);
}

VOID PASSTHR::New ()
{
   From[0] = To[0] = Subject[0] = '\0';
   Crash = Direct = FileAttach = FileRequest = Hold = Immediate = FALSE;
   Intransit = KillSent = Local = Private = ReceiptRequest = Received = FALSE;
   Sent = 0;
   memset (&Written, 0, sizeof (Written));
   Written.Month = 1;
   memset (&Arrived, 0, sizeof (Arrived));
   Arrived.Month = 1;
   Original = Reply = 0L;
   Text.Clear ();
}

USHORT PASSTHR::Next (ULONG &ulMsg)
{
   ulMsg = ulMsg;
   return (FALSE);
}

ULONG PASSTHR::Number ()
{
   return (0L);
}

VOID PASSTHR::Pack ()
{
}

USHORT PASSTHR::Previous (ULONG &ulMsg)
{
   ulMsg = ulMsg;
   return (FALSE);
}

USHORT PASSTHR::ReadHeader (ULONG ulMsg)
{
   ulMsg = ulMsg;
   return (TRUE);
}

USHORT PASSTHR::Read (ULONG ulMsg, SHORT nWidth)
{
   return (Read (ulMsg, Text, nWidth));
}

USHORT PASSTHR::Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth)
{
   ulMsg = ulMsg;
   MsgText.First ();
   nWidth = nWidth;

   return (TRUE);
}

VOID PASSTHR::SetHWM (ULONG ulMsg)
{
   ulMsg = ulMsg;
}

ULONG PASSTHR::UidToMsgn (ULONG ulMsg)
{
   return (ulMsg);
}

VOID PASSTHR::UnLock ()
{
}

USHORT PASSTHR::WriteHeader (ULONG ulMsg)
{
   ulMsg = ulMsg;
   return (FALSE);
}

