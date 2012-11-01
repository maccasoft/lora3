
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

#if !defined(__OS2__)
#error This module is for use with OS/2 only!
#endif

#include "_ldefs.h"
#define INCL_REXXSAA
#include "rexxsaa.h"
#include "lora.h"

typedef struct {
   PSZ Name;
   PFN Function;
} REXXREGISTER;

class TBbs *Bbs = NULL;
class TEmbedded *Embedded = NULL;
class TUser *User = NULL;
class TLog *Log = NULL;
class TCom *Com = NULL, *Snoop = NULL;

ULONG _cdecl LoraBytesReady (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   USHORT Ready = FALSE;
   ULONG rc = 40L;

   if (Argc == 0) {
      if (Com != NULL)
         Ready = Com->BytesReady ();
      if (Ready == FALSE && Snoop != NULL)
         Ready = Snoop->BytesReady ();
      strcpy (Retstr->strptr, (Ready == TRUE) ? "1" : "0");
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;
   Argv = Argv;

   return (rc);
}

ULONG _cdecl LoraGetLastCaller (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   ULONG rc = 40L;
   class TStatistics *Stats;

   if (Argc == 0) {
      strcpy (Retstr->strptr, "None");
      if ((Stats = new TStatistics) != NULL) {
         Stats->Read (Embedded->Task);
         strcpy (Retstr->strptr, Stats->LastCaller);
         delete Stats;
      }
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }
   else if (Argc == 1) {
      strcpy (Retstr->strptr, "None");
      if ((Stats = new TStatistics) != NULL) {
         Stats->Read ((USHORT)atoi (Argv[0].strptr));
         strcpy (Retstr->strptr, Stats->LineLastCaller);
         delete Stats;
      }
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;

   return (rc);
}

ULONG _cdecl LoraGetUserVar (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   ULONG rc = 40L;

   if (Argc == 1) {
      if (!stricmp (Argv[0].strptr, "Name"))
         strcpy (Retstr->strptr, User->Name);
      else if (!stricmp (Argv[0].strptr, "City"))
         strcpy (Retstr->strptr, User->City);
      else if (!stricmp (Argv[0].strptr, "Ansi"))
         strcpy (Retstr->strptr, (User->Ansi == TRUE) ? "1" : "0");
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;

   return (rc);
}

ULONG _cdecl LoraInput (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   USHORT Flags = 0;
   ULONG rc = 40L;

   if (Argc >= 1 && Argc <= 2) {
      if (Argc == 2)
         Flags = (USHORT)atoi (Argv[1].strptr);
      if (Embedded != NULL)
         Embedded->Input (Retstr->strptr, (USHORT)atoi (Argv[0].strptr), Flags);
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;

   return (rc);
}

ULONG _cdecl LoraIsAnsi (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   ULONG rc = 40L;

   if (Argc == 0) {
      strcpy (Retstr->strptr, (Embedded->Ansi == TRUE) ? "1" : "0");
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;
   Argv = Argv;

   return (rc);
}

ULONG _cdecl LoraIsAvatar (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   ULONG rc = 40L;

   if (Argc == 0) {
      strcpy (Retstr->strptr, (Embedded->Avatar == TRUE) ? "1" : "0");
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;
   Argv = Argv;

   return (rc);
}

ULONG _cdecl LoraIsCarrier (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   ULONG rc = 40L;

   if (Argc == 0) {
      strcpy (Retstr->strptr, (Embedded->AbortSession () == TRUE) ? "0" : "1");
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;
   Argv = Argv;

   return (rc);
}

ULONG _cdecl LoraPrint (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   ULONG rc = 40L;

   if (Argc == 1) {
      if (Embedded != NULL) {
         if (Argv[0].strptr[Argv[0].strlength - 1] == ';') {
            Argv[0].strptr[Argv[0].strlength - 1] = '\0';
            Embedded->Printf ("%s", RXSTRPTR (Argv[0]));
         }
         else
            Embedded->Printf ("%s\n", RXSTRPTR (Argv[0]));
      }
      Retstr->strptr[0] = '\0';
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;

   return (rc);
}

ULONG _cdecl LoraReadByte (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   ULONG rc = 40L;

   if (Argc == 0) {
      for (;;) {
         if (Com != NULL && Com->BytesReady ()) {
            sprintf (Retstr->strptr, "%c", Com->ReadByte ());
            break;
         }
         if (Snoop != NULL && Snoop->BytesReady ()) {
            sprintf (Retstr->strptr, "%c", Com->ReadByte ());
            break;
         }
         DosSleep (1L);
      }
      Retstr->strlength = 1;
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;
   Argv = Argv;

   return (rc);
}

ULONG _cdecl LoraReadBytes (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   USHORT Max, Readed;
   ULONG rc = 40L;

   if (Argc == 1) {
      Max = (USHORT)atoi (Argv[0].strptr);
      for (;;) {
         if (Com != NULL && Com->BytesReady ()) {
            Readed = Com->ReadBytes ((UCHAR *)Retstr->strptr, Max);
            Retstr->strlength = Readed;
            break;
         }
         if (Snoop != NULL && Snoop->BytesReady ()) {
            Readed = Snoop->ReadBytes ((UCHAR *)Retstr->strptr, Max);
            Retstr->strlength = Readed;
            break;
         }
         DosSleep (1L);
      }
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;

   return (rc);
}

ULONG _cdecl LoraSetUserVar (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   ULONG rc = 40L;

   if (Argc == 2) {
      if (!stricmp (Argv[0].strptr, "Name"))
         strcpy (User->Name, Argv[1].strptr);
      else if (!stricmp (Argv[0].strptr, "City"))
         strcpy (User->City, Argv[1].strptr);
      else if (!stricmp (Argv[0].strptr, "Ansi"))
         User->Ansi = (strcmp (Argv[1].strptr, "1") == 0) ? 1 : 0;
      Retstr->strptr[0] = '\0';
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;

   return (rc);
}

ULONG _cdecl LoraSendBytes (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   ULONG rc = 40L;

   if (Argc == 1) {
      if (Com != NULL)
         Com->SendBytes ((UCHAR *)Argv[0].strptr, (USHORT)Argv[0].strlength);
      Retstr->strptr[0] = '\0';
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;

   return (rc);
}

ULONG _cdecl LoraSendBytesEcho (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   ULONG rc = 40L;

   if (Argc == 1) {
      if (Com != NULL)
         Com->SendBytes ((UCHAR *)Argv[0].strptr, (USHORT)Argv[0].strlength);
      if (Snoop != NULL)
         Snoop->SendBytes ((UCHAR *)Argv[0].strptr, (USHORT)Argv[0].strlength);
      Retstr->strptr[0] = '\0';
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;

   return (rc);
}

ULONG _cdecl LoraVersion (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   ULONG rc = 40L;

   if (Argc == 0) {
      strcpy (Retstr->strptr, "3.00");
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;
   Argv = Argv;

   return (rc);
}

ULONG _cdecl LoraWriteLog (PSZ Name, LONG Argc, RXSTRING Argv[], PSZ Queuename, PRXSTRING Retstr)
{
   ULONG rc = 40L;

   if (Argc == 1) {
      if (Log != NULL)
         Log->Write ("%s", Argv[0].strptr);
      Retstr->strptr[0] = '\0';
      Retstr->strlength = strlen (Retstr->strptr);
      rc = 0L;
   }

   Name = Name;
   Queuename = Queuename;

   return (rc);
}

REXXREGISTER RexxTable[] = {
   "LoraBytesReady",    (PFN)LoraBytesReady,
   "LoraGetLastCaller", (PFN)LoraGetLastCaller,
   "LoraGetUserVar",    (PFN)LoraGetUserVar,
   "LoraInput",         (PFN)LoraInput,
   "LoraIsAnsi",        (PFN)LoraIsAnsi,
   "LoraIsAvatar",      (PFN)LoraIsAvatar,
   "LoraIsCarrier",     (PFN)LoraIsCarrier,
   "LoraPrint",         (PFN)LoraPrint,
   "LoraReadByte",      (PFN)LoraReadByte,
   "LoraReadBytes",     (PFN)LoraReadBytes,
   "LoraSendBytes",     (PFN)LoraSendBytes,
   "LoraSendBytesEcho", (PFN)LoraSendBytesEcho,
   "LoraSetUserVar",    (PFN)LoraSetUserVar,
   "LoraVersion",       (PFN)LoraVersion,
   "LoraWriteLog",      (PFN)LoraWriteLog,
   NULL, NULL
};

VOID CallRexx (class TEmbedded *lpEmbed, PSZ Name)
{
   int i;
   LONG return_code;
   SHORT rc;
   CHAR macro_argument[16], return_buffer[250];
   RXSTRING argv[1];
   RXSTRING retstr;

   if ((Embedded = lpEmbed) != NULL) {
      Com = Embedded->Com;
      Snoop = Embedded->Snoop;
      Log = Embedded->Log;
      User = Embedded->User;

      Log->Write (":Executing Rexx Script '%s'", Name);

      for (i = 0; RexxTable[i].Name != NULL; i++)
         RexxRegisterFunctionExe (RexxTable[i].Name, RexxTable[i].Function);
//         RexxRegisterFunctionDll (RexxTable[i].Name, "BBS", (CHAR *)RexxTable[i].Function);

      sprintf (macro_argument, "%u", Embedded->Task);
      MAKERXSTRING (argv[0], macro_argument, strlen (macro_argument));
      MAKERXSTRING (retstr, return_buffer, sizeof (return_buffer));
      if ((return_code = RexxStart (1, argv, Name, NULL, "Editor", RXCOMMAND, NULL, &rc, &retstr)) != 0)
         Log->Write ("!Error %04ld executing script '%s'", abs (return_code), Name);
      if (RXSTRPTR (retstr) != return_buffer)
         DosFreeMem (RXSTRPTR (retstr));

      for (i = 0; RexxTable[i].Name != NULL; i++)
         RexxDeregisterFunction (RexxTable[i].Name);
   }

   Embedded = NULL;
}

VOID CallRexx (class TBbs *lpBbs, PSZ Name)
{
   if ((Bbs = lpBbs) != NULL)
      CallRexx (Bbs->Embedded, Name);

   Bbs = NULL;
}


