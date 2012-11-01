
// --------------------------------------------------------------------
// LoraBBS Professional Edition
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// BBS-spcific class definitions. Used internally only.
//
// History list:
//    23/04/95 - Initial coding
// --------------------------------------------------------------------

#include "_ldefs.h"
#include "menu.h"

TForum::TForum (class TBbs *bbs) : TMenu (bbs)
{
   Cfg = bbs->Cfg;
   Lang = bbs->Lang;
   Log = bbs->Log;
   User = bbs->User;

   Bbs = bbs;
}

TForum::~TForum (void)
{
}

USHORT TForum::ExecuteOption (ITEM *pItem)
{
   switch (pItem->usCommand) {
      case MNU_RETURN:
         if (Return () == FALSE)
            return (MENU_RETURN);
         break;

      case MNU_SELECTAREA:
         SelectArea ();
         break;

      default:
         TMenu::ExecuteOption (pItem);
         break;
   }

   return (MENU_OK);
}

USHORT TForum::Run (PSZ pszName)
{
   if (Current.MenuName[0])
      pszName = Current.MenuName;
   return ((TMenu::Run (pszName) != MENU_LOGOFF) ? MENU_OK : MENU_LOGOFF);
}

USHORT TForum::SelectArea (VOID)
{
   USHORT fRet = FALSE;
   CHAR szCommand[16];
   class ForumData *Forum;

   if ((Forum = new ForumData) != NULL) {
      do {
         Bbs->Printf (Lang->MessageSelect);
         Bbs->GetString (szCommand, (USHORT)(sizeof (szCommand) - 1), INP_FIELD);

         if (!stricmp (szCommand, Lang->ListKey)) {
            if (Forum->First () == TRUE) {
               Bbs->Printf (Lang->ForumListHeader);
               do {
                  if (User->Level >= Forum->Level) {
                     if ((Forum->AccessFlags & User->AccessFlags) == Forum->AccessFlags)
                        Bbs->Printf (Lang->ForumList, Forum->Key, Forum->ActiveMsgs, Forum->ActiveFiles, Forum->Display);
                  }
               } while (Bbs->AbortSession () == FALSE && Forum->Next () == TRUE);
            }
         }
         else if (szCommand[0] != '\0') {
            if (Forum->Read (szCommand) == TRUE) {
               if (User->Level >= Forum->Level) {
                  if ((Forum->AccessFlags & User->AccessFlags) == Forum->AccessFlags) {
                     Current.Read (szCommand);
                     if (Current.MenuName[0])
                        Goto (Current.MenuName);
                     fRet = TRUE;
                  }
               }
            }

            if (fRet == FALSE)
               Bbs->Printf (Lang->ForumNotAvailable);
         }
      } while (szCommand[0] != '\0' && fRet == FALSE && Bbs->AbortSession () == FALSE);

      delete Forum;
   }

   return (fRet);
}

USHORT TForum::SelectLibrary (VOID)
{
   USHORT fRet = FALSE;
   CHAR Command[16], *Area;
   class FileData *Data;

   if ((Data = new FileData) != NULL) {
      do {
         Bbs->Printf ("\n\026\001\013Enter the name of new Library, or ? for a list: \026\001\x1E");
         Bbs->GetString (Command, (USHORT)(sizeof (Command) - 1), INP_FIELD);

         if (!stricmp (Command, Lang->ListKey)) {
            if ((Area = (PSZ)Current.File.First ()) != NULL) {
               Bbs->Printf ("\n\026\001\012Library          Files  Description\n---------------  -----  -------------------------------------------------------\n");
               do {
                  if (Data->Read (Area) == TRUE) {
                     if (User->Level >= Data->Level) {
                        if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags)
                           Bbs->Printf ("\026\001\013%-15.15s  \026\001\016%5ld  %s\n", Data->Key, Data->ActiveFiles, Data->Display);
                     }
                  }
               } while (Bbs->AbortSession () == FALSE && (Area = (PSZ)Current.File.Next ()) != NULL);
            }
         }
         else if (Command[0] != '\0') {
            if (Data->Read (Command) == TRUE) {
               if (User->Level >= Data->Level) {
                  if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                     File.Read (Command);
                     fRet = TRUE;
                  }
               }
            }

            if (fRet == FALSE)
               Bbs->Printf (Lang->ForumNotAvailable);
         }
      } while (szCommand[0] != '\0' && fRet == FALSE && Bbs->AbortSession () == FALSE);

      delete Data;
   }

   return (fRet);
}


