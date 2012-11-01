
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/29/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "user.h"

TFileTag::TFileTag (void)
{
   TotalNumber = 0;
   TotalSize = 0L;
   LastIndex = 1;
   Tags.Clear ();
}

TFileTag::~TFileTag (void)
{
   Tags.Clear ();
}

USHORT TFileTag::Add (VOID)
{
   USHORT RetVal;
   FTAGS ft;

   memset (&ft, 0, sizeof (ft));
   strcpy (ft.Name, Name);
   strcpy (ft.Library, Library);
   ft.Size = Size;
   strcpy (ft.Complete, Complete);
   Index = ft.Index = LastIndex;

   if ((RetVal = Tags.Add (&ft, sizeof (ft))) == TRUE) {
      TotalNumber++;
      TotalSize += Size;
      LastIndex++;
   }

   return (RetVal);
}

USHORT TFileTag::Check (PSZ pszName)
{
   USHORT RetVal = FALSE;
   FTAGS *ft;

   if ((ft = (FTAGS *)Tags.First ()) != NULL)
      do {
         if (!stricmp (ft->Name, pszName)) {
            Index = ft->Index;
            strcpy (Name, ft->Name);
            strcpy (Library, ft->Library);
            Size = ft->Size;
            strcpy (Complete, ft->Complete);
            RetVal = TRUE;
         }
      } while (RetVal == FALSE && (ft = (FTAGS *)Tags.Next ()) != NULL);

   return (RetVal);
}

VOID TFileTag::Clear (VOID)
{
   LastIndex = 1;
   TotalNumber = 0;
   TotalSize = 0L;
   Tags.Clear ();
}

USHORT TFileTag::First (VOID)
{
   USHORT RetVal = FALSE;
   FTAGS *ft;

   if ((ft = (FTAGS *)Tags.First ()) != NULL) {
      Index = ft->Index;
      strcpy (Name, ft->Name);
      strcpy (Library, ft->Library);
      Size = ft->Size;
      strcpy (Complete, ft->Complete);
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TFileTag::New (VOID)
{
   Index = LastIndex;
   DeleteAfter = FALSE;
   memset (Name, 0, sizeof (Name));
   memset (Library, 0, sizeof (Library));
   memset (Complete, 0, sizeof (Complete));
   Size = 0L;
}

USHORT TFileTag::Next (VOID)
{
   USHORT RetVal = FALSE;
   FTAGS *ft;

   if ((ft = (FTAGS *)Tags.Next ()) != NULL) {
      Index = ft->Index;
      strcpy (Name, ft->Name);
      strcpy (Library, ft->Library);
      Size = ft->Size;
      strcpy (Complete, ft->Complete);
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TFileTag::Reindex (VOID)
{
   FTAGS *ft;

   LastIndex = 1;
   if ((ft = (FTAGS *)Tags.First ()) != NULL)
      do {
         ft->Index = LastIndex++;
      } while ((ft = (FTAGS *)Tags.Next ()) != NULL);
}

VOID TFileTag::Remove (PSZ pszName)
{
   USHORT RetVal = FALSE;
   FTAGS *ft;

   if (pszName != NULL) {
      if ((ft = (FTAGS *)Tags.First ()) != NULL) {
         do {
            if (!stricmp (ft->Name, pszName))
               RetVal = TRUE;
         } while (RetVal == FALSE && (ft = (FTAGS *)Tags.Next ()) != NULL);
      }
      if (RetVal == TRUE) {
         if (ft->DeleteAfter == TRUE)
            unlink (ft->Complete);
         Tags.Remove ();
         TotalNumber--;
         TotalSize -= ft->Size;
      }
   }
   else {
      if (DeleteAfter == TRUE)
         unlink (Complete);
      Tags.Remove ();
      TotalNumber--;
      TotalSize -= Size;
   }
}

USHORT TFileTag::Select (USHORT usIndex)
{
   USHORT RetVal = FALSE;
   FTAGS *ft;

   if ((ft = (FTAGS *)Tags.First ()) != NULL)
      do {
         if (ft->Index == usIndex) {
            Index = ft->Index;
            strcpy (Name, ft->Name);
            strcpy (Library, ft->Library);
            Size = ft->Size;
            strcpy (Complete, ft->Complete);
            RetVal = TRUE;
         }
      } while (RetVal == FALSE && (ft = (FTAGS *)Tags.Next ()) != NULL);

   return (RetVal);
}

