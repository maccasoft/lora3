
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
#include "menu.h"

TMenu::TMenu (void)
{
   strcpy (Path, "");
   strcpy (AltPath, "");
   Items.Clear ();
}

TMenu::~TMenu (void)
{
   Items.Clear ();
}

VOID TMenu::Add (VOID)
{
   memset (&Item, 0, sizeof (Item));

   strcpy (Item.Display, Display);
   Item.Color = Color;
   Item.Hilight = Hilight;
   strcpy (Item.Key, Key);
   Item.Command = Command;
   strcpy (Item.Argument, Argument);
   Item.Level = Level;
   Item.AccessFlags = AccessFlags;
   Item.DenyFlags = DenyFlags;
   Item.Automatic = Automatic;
   Item.FirstTime = FirstTime;

   Items.Add (&Item, sizeof (Item));
}

USHORT TMenu::Check (PSZ pszKey)
{
   USHORT RetVal = FALSE;
   ITEM *Current;

   if ((Current = (ITEM *)Items.First ()) != NULL)
      do {
         if (!stricmp (Current->Key, pszKey)) {
            strcpy (Display, Current->Display);
            Color = Current->Color;
            Hilight = Current->Hilight;
            strcpy (Key, Current->Key);
            Command = Current->Command;
            strcpy (Argument, Current->Argument);
            Level = Current->Level;
            AccessFlags = Current->AccessFlags;
            DenyFlags = Current->DenyFlags;
            Automatic = Current->Automatic;
            FirstTime = Current->FirstTime;
            RetVal = TRUE;
         }
      } while (RetVal == FALSE && (Current = (ITEM *)Items.Next ()) != NULL);

   return (RetVal);
}

VOID TMenu::Delete (VOID)
{
   ITEM *Current;

   Items.Remove ();
   if (Items.Value () == NULL)
      New ();

   if ((Current = (ITEM *)Items.Value ()) != NULL) {
      strcpy (Display, Current->Display);
      Color = Current->Color;
      Hilight = Current->Hilight;
      strcpy (Key, Current->Key);
      Command = Current->Command;
      strcpy (Argument, Current->Argument);
      Level = Current->Level;
      AccessFlags = Current->AccessFlags;
      DenyFlags = Current->DenyFlags;
      Automatic = Current->Automatic;
      FirstTime = Current->FirstTime;
   }
}

USHORT TMenu::First (VOID)
{
   USHORT RetVal = FALSE;
   ITEM *Current;

   if ((Current = (ITEM *)Items.First ()) != NULL) {
      strcpy (Display, Current->Display);
      Color = Current->Color;
      Hilight = Current->Hilight;
      strcpy (Key, Current->Key);
      Command = Current->Command;
      strcpy (Argument, Current->Argument);
      Level = Current->Level;
      AccessFlags = Current->AccessFlags;
      DenyFlags = Current->DenyFlags;
      Automatic = Current->Automatic;
      FirstTime = Current->FirstTime;
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TMenu::Insert (VOID)
{
   memset (&Item, 0, sizeof (Item));

   strcpy (Item.Display, Display);
   Item.Color = Color;
   Item.Hilight = Hilight;
   strcpy (Item.Key, Key);
   Item.Command = Command;
   strcpy (Item.Argument, Argument);
   Item.Level = Level;
   Item.AccessFlags = AccessFlags;
   Item.DenyFlags = DenyFlags;
   Item.Automatic = Automatic;
   Item.FirstTime = FirstTime;

//   if (Items.Previous () != NULL)
      Items.Insert (&Item, sizeof (Item));
//   else {
//      Current = (ITEM *)Items.Value ();
//      Items.Insert (&Item, sizeof (Item));
//      if (Current != NULL) {
//         Items.Insert (Current, sizeof (Item));
//         Items.First ();
//         Items.Remove ();
//      }
//   }
}

USHORT TMenu::Load (PSZ pszName, USHORT fAppend)
{
   int fd;
   USHORT RetVal = FALSE;
   CHAR Temp[128];
   ITEM *Current = NULL;

   if (fAppend == TRUE)
      Current = (ITEM *)Items.Value ();

   if (AltPath[0] != '\0') {
      sprintf (Temp, "%s%s", AltPath, pszName);
      if (strstr (strupr (Temp), ".MNU") == NULL)
         strcat (Temp, ".MNU");

      AdjustPath (Temp);
      if ((fd = sopen (strlwr (Temp), O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         if (read (fd, &Hdr, sizeof (Hdr)) == sizeof (Hdr)) {
            if (fAppend == FALSE) {
               Items.Clear ();
               New ();
               strcpy (Name, Hdr.MenuName);
               strcpy (Prompt, Hdr.Prompt);
               PromptColor = Hdr.Color;
               PromptHilight = Hdr.Hilight;
            }
            while (read (fd, &Item, sizeof (Item)) == sizeof (Item)) {
               if (fAppend == TRUE)
                  Items.Insert (&Item, sizeof (Item));
               else
                  Items.Add (&Item, sizeof (Item));
            }
            RetVal = TRUE;
         }
         close (fd);
      }
   }

   if (RetVal == FALSE) {
      sprintf (Temp, "%s%s", Path, pszName);
      if (strstr (strupr (Temp), ".MNU") == NULL)
         strcat (Temp, ".MNU");

      AdjustPath (Temp);
      if ((fd = sopen (strlwr (Temp), O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         if (read (fd, &Hdr, sizeof (Hdr)) == sizeof (Hdr)) {
            if (fAppend == FALSE) {
               Items.Clear ();
               New ();
               strcpy (Name, Hdr.MenuName);
               strcpy (Prompt, Hdr.Prompt);
               PromptColor = Hdr.Color;
               PromptHilight = Hdr.Hilight;
            }
            while (read (fd, &Item, sizeof (Item)) == sizeof (Item)) {
               if (fAppend == TRUE)
                  Items.Insert (&Item, sizeof (Item));
               else
                  Items.Add (&Item, sizeof (Item));
            }
            RetVal = TRUE;
         }
         close (fd);
      }
   }

   if (RetVal == TRUE) {
      if (fAppend == TRUE) {
         if (Items.First () != NULL)
            do {
               if (Items.Value () == Current)
                  break;
            } while (Items.Next () != NULL);
      }
      else
         Current = (ITEM *)Items.First ();

      if (Current != NULL) {
         strcpy (Display, Current->Display);
         Color = Current->Color;
         Hilight = Current->Hilight;
         strcpy (Key, Current->Key);
         Command = Current->Command;
         strcpy (Argument, Current->Argument);
         Level = Current->Level;
         AccessFlags = Current->AccessFlags;
         DenyFlags = Current->DenyFlags;
         Automatic = Current->Automatic;
         FirstTime = Current->FirstTime;
      }
   }

   return (RetVal);
}

VOID TMenu::New (USHORT usNewPrompt)
{
   memset (Display, 0, sizeof (Display));
   Color = 7;
   Hilight = 14;
   memset (Key, 0, sizeof (Key));
   Command = 0;
   memset (Argument, 0, sizeof (Argument));
   Level = 0;
   AccessFlags = 0L;
   DenyFlags = 0L;
   FirstTime = Automatic = FALSE;

   if (usNewPrompt == TRUE) {
      memset (Name, 0, sizeof (Name));
      memset (Prompt, 0, sizeof (Prompt));
      PromptColor = 14;
      PromptHilight = 14;
   }
}

USHORT TMenu::Next (VOID)
{
   USHORT RetVal = FALSE;
   ITEM *Current;

   if ((Current = (ITEM *)Items.Next ()) != NULL) {
      strcpy (Display, Current->Display);
      Color = Current->Color;
      Hilight = Current->Hilight;
      strcpy (Key, Current->Key);
      Command = Current->Command;
      strcpy (Argument, Current->Argument);
      Level = Current->Level;
      AccessFlags = Current->AccessFlags;
      DenyFlags = Current->DenyFlags;
      Automatic = Current->Automatic;
      FirstTime = Current->FirstTime;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TMenu::Previous (VOID)
{
   USHORT RetVal = FALSE;
   ITEM *Current;

   if ((Current = (ITEM *)Items.Previous ()) != NULL) {
      strcpy (Display, Current->Display);
      Color = Current->Color;
      Hilight = Current->Hilight;
      strcpy (Key, Current->Key);
      Command = Current->Command;
      strcpy (Argument, Current->Argument);
      Level = Current->Level;
      AccessFlags = Current->AccessFlags;
      DenyFlags = Current->DenyFlags;
      Automatic = Current->Automatic;
      FirstTime = Current->FirstTime;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TMenu::Save (PSZ pszName)
{
   int fd;
   USHORT RetVal = FALSE;
   CHAR Temp[128];
   ITEM *Current, *Saved;

   if (AltPath[0] != '\0')
      sprintf (Temp, "%s%s", AltPath, pszName);
   else
      sprintf (Temp, "%s%s", Path, pszName);
   if (strstr (strupr (Temp), ".MNU") == NULL)
      strcat (Temp, ".MNU");

   AdjustPath (Temp);
   if ((fd = sopen (strlwr (Temp), O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      memset (&Hdr, 0, sizeof (Hdr));
      strcpy (Hdr.MenuName, Name);
      strcpy (Hdr.Prompt, Prompt);
      Hdr.Color = PromptColor;
      Hdr.Hilight = PromptHilight;
      write (fd, &Hdr, sizeof (Hdr));

      Saved = (ITEM *)Items.Value ();

      if ((Current = (ITEM *)Items.First ()) != NULL)
         do {
            write (fd, Current, sizeof (ITEM));
         } while ((Current = (ITEM *)Items.Next ()) != NULL);

      close (fd);

      if (Items.First () != NULL)
         do {
            if ((ITEM *)Items.Value () == Saved)
               break;
         } while (Items.Next () != NULL);

      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TMenu::Update (VOID)
{
   USHORT RetVal = FALSE;
   ITEM *Current;

   if ((Current = (ITEM *)Items.Value ()) != NULL) {
      strcpy (Current->Display, Display);
      Current->Color = Color;
      Current->Hilight = Hilight;
      strcpy (Current->Key, Key);
      Current->Command = Command;
      strcpy (Current->Argument, Argument);
      Current->Level = Level;
      Current->AccessFlags = AccessFlags;
      Current->DenyFlags = DenyFlags;
      Current->Automatic = Automatic;
      Current->FirstTime = FirstTime;
   }
}


