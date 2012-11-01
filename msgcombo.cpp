
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    04/22/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"

typedef struct {
   ULONG  Number;
   ULONG  RealNumber;
   USHORT Storage;
   CHAR   Path[64];
} COMBOINDEX;

COMBO::COMBO (void)
{
   Id = 0L;
   TotalMsgs = 0L;
   NextNumber = 0L;
   Msg = NULL;
   Index.Clear ();
}

COMBO::~COMBO (void)
{
   Index.Clear ();
}

USHORT COMBO::Add (VOID)
{
   return (FALSE);
}

USHORT COMBO::Add (class TMsgBase *MsgBase)
{
   return (FALSE);
}

USHORT COMBO::Add (class TCollection &MsgText)
{
   return (FALSE);
}

USHORT COMBO::Add (PSZ pszPath, USHORT usStorage, ULONG ulNumber)
{
   USHORT RetVal;
   COMBOINDEX msgIndex;

   memset (&msgIndex, 0, sizeof (msgIndex));
   msgIndex.Number = NextNumber;
   msgIndex.RealNumber = ulNumber;
   msgIndex.Storage = usStorage;
   strcpy (msgIndex.Path, pszPath);
   if ((RetVal = Index.Add (&msgIndex, (USHORT)sizeof (COMBOINDEX))) == TRUE)
      TotalMsgs++;

   return (RetVal);
}

VOID COMBO::Close (VOID)
{
}

USHORT COMBO::Delete (ULONG ulMsg)
{
   USHORT RetVal = FALSE;
   COMBOINDEX *msgIndex;

   if ((msgIndex = (COMBOINDEX *)Index.First ()) != NULL)
      do {
         if (msgIndex->Number == ulMsg) {
            Index.Remove ();
            TotalMsgs--;
            RetVal = TRUE;
         }
      } while ((msgIndex = (COMBOINDEX *)Index.Next ()) != NULL);

   return (RetVal);
}

USHORT COMBO::GetHWM (ULONG &ulMsg)
{
   ulMsg = 0L;

   return (FALSE);
}

ULONG COMBO::Highest (VOID)
{
   ULONG RetVal = 0L;
   COMBOINDEX *msgIndex;

   if ((msgIndex = (COMBOINDEX *)Index.Last ()) != NULL)
      RetVal = msgIndex->Number;

   return (RetVal);
}

USHORT COMBO::Lock (ULONG ulTimeout)
{
   ulTimeout = ulTimeout;
   return (TRUE);
}

ULONG COMBO::Lowest (VOID)
{
   ULONG RetVal = 0L;
   COMBOINDEX *msgIndex;

   if ((msgIndex = (COMBOINDEX *)Index.First ()) != NULL)
      RetVal = msgIndex->Number;

   return (RetVal);
}

VOID COMBO::New (VOID)
{
   From[0] = To[0] = Subject[0] = '\0';
   Crash = Direct = FileAttach = FileRequest = Hold = Immediate = FALSE;
   Intransit = KillSent = Local = Private = ReceiptRequest = Received = FALSE;
   Sent = 0;
   memset (&Written, 0, sizeof (Written));
   Written.Month = 1;
   memset (&Arrived, 0, sizeof (Arrived));
   Arrived.Month = 1;
   Text.Clear ();
}

USHORT COMBO::Next (ULONG &ulMsg)
{
   USHORT RetVal = FALSE;
   COMBOINDEX *msgIndex;

   if ((msgIndex = (COMBOINDEX *)Index.Next ()) != NULL) {
      ulMsg = msgIndex->Number;
      RetVal = TRUE;
   }

   return (RetVal);
}

ULONG COMBO::Number (VOID)
{
   return (TotalMsgs);
}

VOID COMBO::Pack (VOID)
{
}

USHORT COMBO::Previous (ULONG &ulMsg)
{
   USHORT RetVal = FALSE;
   COMBOINDEX *msgIndex;

   if ((msgIndex = (COMBOINDEX *)Index.Previous ()) != NULL) {
      ulMsg = msgIndex->Number;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT COMBO::ReadHeader (ULONG ulMsg)
{
   USHORT RetVal = FALSE;
   COMBOINDEX *msgIndex;

   New ();

   if ((msgIndex = (COMBOINDEX *)Index.First ()) != NULL)
      do {
         if (msgIndex->Number == ulMsg) {
            if (Msg != NULL && stricmp (LastPath, msgIndex->Path)) {
               delete Msg;
               Msg = NULL;
            }
            if (Msg == NULL) {
               if (msgIndex->Storage == ST_JAM)
                  Msg = new JAM (msgIndex->Path);
               else if (msgIndex->Storage == ST_SQUISH)
                  Msg = new SQUISH (msgIndex->Path);
               else if (msgIndex->Storage == ST_FIDO)
                  Msg = new FIDOSDM (msgIndex->Path);
               else if (msgIndex->Storage == ST_ADEPT)
                  Msg = new ADEPT (msgIndex->Path);
            }
            if (Msg != NULL) {
               strcpy (LastPath, msgIndex->Path);
               Current = msgIndex->Number;
               if (Msg->ReadHeader (msgIndex->RealNumber) == TRUE) {
                  New ();
                  RetVal = TRUE;

                  strcpy (From, Msg->From);
                  strcpy (To, Msg->To);
                  strcpy (Subject, Msg->Subject);

                  strcpy (FromAddress, Msg->FromAddress);
                  strcpy (ToAddress, Msg->ToAddress);

                  Written.Day = Msg->Written.Day;
                  Written.Month = Msg->Written.Month;
                  Written.Year = Msg->Written.Year;
                  Written.Hour = Msg->Written.Hour;
                  Written.Minute = Msg->Written.Minute;
                  Written.Second = Msg->Written.Second;

                  Arrived.Day = Msg->Arrived.Day;
                  Arrived.Month = Msg->Arrived.Month;
                  Arrived.Year = Msg->Arrived.Year;
                  Arrived.Hour = Msg->Arrived.Hour;
                  Arrived.Minute = Msg->Arrived.Minute;
                  Arrived.Second = Msg->Arrived.Second;

                  Crash = Msg->Crash;
                  Direct = Msg->Direct;
                  FileAttach = Msg->FileAttach;
                  FileRequest = Msg->FileRequest;
                  Hold = Msg->Hold;
                  Immediate = Msg->Immediate;
                  Intransit = Msg->Intransit;
                  KillSent = Msg->KillSent;
                  Local = Msg->Local;
                  Private = Msg->Private;
                  ReceiptRequest = Msg->ReceiptRequest;
                  Received = Msg->Received;
                  Sent = Msg->Sent;
               }
            }
            break;
         }
      } while ((msgIndex = (COMBOINDEX *)Index.Next ()) != NULL);

   return (RetVal);
}

USHORT COMBO::Read (ULONG ulMsg, SHORT nWidth)
{
   return (Read (ulMsg, Text, nWidth));
}

USHORT COMBO::Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth)
{
   USHORT RetVal = FALSE;
   CHAR *p;
   COMBOINDEX *msgIndex;

   New ();

   if ((msgIndex = (COMBOINDEX *)Index.First ()) != NULL)
      do {
         if (msgIndex->Number == ulMsg) {
            if (Msg != NULL && stricmp (LastPath, msgIndex->Path)) {
               delete Msg;
               Msg = NULL;
            }
            if (Msg == NULL) {
               if (msgIndex->Storage == ST_JAM)
                  Msg = new JAM (msgIndex->Path);
               else if (msgIndex->Storage == ST_SQUISH)
                  Msg = new SQUISH (msgIndex->Path);
               else if (msgIndex->Storage == ST_FIDO)
                  Msg = new FIDOSDM (msgIndex->Path);
               else if (msgIndex->Storage == ST_ADEPT)
                  Msg = new ADEPT (msgIndex->Path);
            }
            if (Msg != NULL) {
               strcpy (LastPath, msgIndex->Path);
               Current = msgIndex->Number;
               if (Msg->Read (msgIndex->RealNumber, nWidth) == TRUE) {
                  New ();
                  RetVal = TRUE;

                  strcpy (From, Msg->From);
                  strcpy (To, Msg->To);
                  strcpy (Subject, Msg->Subject);

                  strcpy (FromAddress, Msg->FromAddress);
                  strcpy (ToAddress, Msg->ToAddress);

                  Written.Day = Msg->Written.Day;
                  Written.Month = Msg->Written.Month;
                  Written.Year = Msg->Written.Year;
                  Written.Hour = Msg->Written.Hour;
                  Written.Minute = Msg->Written.Minute;
                  Written.Second = Msg->Written.Second;

                  Arrived.Day = Msg->Arrived.Day;
                  Arrived.Month = Msg->Arrived.Month;
                  Arrived.Year = Msg->Arrived.Year;
                  Arrived.Hour = Msg->Arrived.Hour;
                  Arrived.Minute = Msg->Arrived.Minute;
                  Arrived.Second = Msg->Arrived.Second;

                  Crash = Msg->Crash;
                  Direct = Msg->Direct;
                  FileAttach = Msg->FileAttach;
                  FileRequest = Msg->FileRequest;
                  Hold = Msg->Hold;
                  Immediate = Msg->Immediate;
                  Intransit = Msg->Intransit;
                  KillSent = Msg->KillSent;
                  Local = Msg->Local;
                  Private = Msg->Private;
                  ReceiptRequest = Msg->ReceiptRequest;
                  Received = Msg->Received;
                  Sent = Msg->Sent;

                  while ((p = (CHAR *)Msg->Text.First ()) != NULL) {
                     MsgText.Add (p, (USHORT)(strlen (p) + 1));
                     Msg->Text.Remove ();
                  }
               }
            }
            break;
         }
      } while ((msgIndex = (COMBOINDEX *)Index.Next ()) != NULL);

   return (RetVal);
}

VOID COMBO::SetHWM (ULONG ulMsg)
{
   ulMsg = ulMsg;
}

VOID COMBO::UnLock (VOID)
{
}

USHORT COMBO::WriteHeader (ULONG ulMsg)
{
   return (FALSE);
}

USHORT COMBO::Write (ULONG ulMsg)
{
   return (Write (ulMsg, Text));
}

USHORT COMBO::Write (ULONG ulMsg, class TCollection &MsgText)
{
   return (FALSE);
}

