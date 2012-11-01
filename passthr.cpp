
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.5
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/07/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"

PASSTHR::PASSTHR (void)
{
   Id = 0L;
}

PASSTHR::~PASSTHR (void)
{
}

USHORT PASSTHR::Add (VOID)
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

VOID PASSTHR::Close (VOID)
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

ULONG PASSTHR::Highest (VOID)
{
   return (0L);
}

USHORT PASSTHR::Lock (ULONG ulTimeout)
{
   ulTimeout = ulTimeout;
   return (TRUE);
}

ULONG PASSTHR::Lowest (VOID)
{
   return (0L);
}

ULONG PASSTHR::MsgnToUid (ULONG ulMsg)
{
   return (ulMsg);
}

VOID PASSTHR::New (VOID)
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

ULONG PASSTHR::Number (VOID)
{
   return (0L);
}

VOID PASSTHR::Pack (VOID)
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

VOID PASSTHR::UnLock (VOID)
{
}

USHORT PASSTHR::WriteHeader (ULONG ulMsg)
{
   ulMsg = ulMsg;
   return (FALSE);
}

