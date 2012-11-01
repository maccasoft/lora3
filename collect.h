
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _TOOLS_H
#define _TOOLS_H

typedef struct _lData {
   struct _lData  *Previous;
   struct _lData  *Next;
   PVOID           Value;
   CHAR            Data[1];
} LDATA;

class DLL_EXPORT TCollection
{
public:
   TCollection (void);
   ~TCollection (void);

   USHORT Elements;

   USHORT Add (PVOID lpData);
   USHORT Add (PSZ lpData);
   USHORT Add (PVOID lpData, USHORT usSize);
   VOID   Clear (VOID);
   PVOID  First (VOID);
   USHORT Insert (PVOID lpData);
   USHORT Insert (PVOID lpData, USHORT usSize);
   PVOID  Last (VOID);
   PVOID  Next (VOID);
   PVOID  Previous (VOID);
   VOID   Remove (VOID);
   USHORT Replace (PVOID lpData);
   USHORT Replace (PVOID lpData, USHORT usSize);
   PVOID  Value (VOID);

private:
   LDATA *List;
};

#endif

