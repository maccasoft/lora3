
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 2.99.20
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"
#include "lorawin.h"

short cmd_sel = 0;

VOID AddShadow (VOID);
VOID DisplayButton (USHORT y, USHORT x, CHAR *Text, USHORT Shadow);
VOID DisplayTextField (USHORT y, USHORT x, CHAR *Text, USHORT FieldSize, USHORT Size);
VOID GetTextField (USHORT y, USHORT x, CHAR *Text, USHORT FieldSize, USHORT Size);
USHORT MessageBox (PSZ Caption, PSZ Text);
USHORT CFlagsDlg (PSZ title, ULONG *flags);
VOID DisplayFlags (USHORT y, USHORT x, ULONG Flags);
USHORT CSecurityDlg (PSZ title, USHORT *level, ULONG *flags, ULONG *deny_flags);

// ----------------------------------------------------------------------

typedef struct {
   USHORT Id;
   PSZ    Text;
} MENUCMD;

// Moving between menu
MENUCMD MenuCmd1[] = {
   MNU_GOTO,       "Goto menu          ",
   MNU_GOSUB,      "Gosub menu         ",
   MNU_RETURN,     "Return to previous ",
   MNU_CLEARSTACK, "Clear menu stack   ",
   MNU_CLEARGOSUB, "Clear gosub menu   ",
   MNU_CLEARGOTO,  "Clear goto menu    ",
   MNU_RETURNMAIN, "Return to MAIN menu",
   0, NULL
};

// Message areas
MENUCMD MenuCmd2[] = {
   MNU_MSGSELECT,       "Change message area      ",
   MNU_MSGDELETE,       "Kill message             ",
   MNU_MSGWRITE,        "Edit new message         ",
   MNU_MSGREPLY,        "Reply to message         ",
   MNU_MSGBRIEFLIST,    "Short message list       ",
   MNU_MSGFORWARD,      "Read next message        ",
   MNU_MSGBACKWARD,     "Read previous message    ",
   MNU_MSGREADNONSTOP,  "Read message non-stop    ",
   MNU_INQUIRETEXT,     "Inquire messages         ",
   MNU_MSGINDIVIDUAL,   "Read individual message  ",
   MNU_MSGTITLELIST,    "Verbose message list     ",
   MNU_MSGLISTNEWAREAS, "List areas w/new messages",
   MNU_MSGREAD,         "Read messages            ",
   MNU_INQUIREPERSONAL, "Inquire personal messages",
   MNU_MSGMODIFY,       "Change message           ",
   MNU_MSGUNRECEIVE,    "Unreceive message        ",
   MNU_MSGREADORIGINAL, "Read original message    ",
   MNU_MSGREADREPLY,    "Read reply message       ",
   MNU_MSGREADCURRENT,  "Read current message     ",
   MNU_TOGGLEKLUDGES,   "Toggle kludges lines     ",
   0, NULL
};

// File areas
MENUCMD MenuCmd3[] = {
   MNU_FILENAMELIST, "File list",
   MNU_FILEDOWNLOAD, "Download file",
   MNU_FILETEXTLIST, "Locate files",
   MNU_FILEUPLOAD, "Upload file",
   MNU_FILENEWLIST, "New files list",
   MNU_FILEDOWNLOADANY, "Download from any area",
   MNU_FILEDELETE, "Kill files",
   MNU_FILESELECT, "Change file area",
   MNU_SEARCHFILENAME, "Locate files by name",
   MNU_FILEKEYWORDLIST, "Locate files by keyword",
   MNU_FILEDATELIST, "File list by date",
   MNU_FILEDOWNLOADLIST, "Download list of files",
   MNU_FILEUPLOADUSER, "Upload file to user",
   MNU_FILEDISPLAY, "File Display",
   MNU_ADDTAGGED, "Tag files",
   MNU_DELETETAGGED, "Delete tagged files",
   MNU_LISTTAGGED, "List tagged files",
   MNU_DELETEALLTAGGED, "Delete all tagged files",
   0, NULL
};

// User configuration
MENUCMD MenuCmd4[] = {
   MNU_SETLANGUAGE, "Change language",
   MNU_SETPASSWORD, "Set password",
   MNU_TOGGLEANSI, "Toggle ANSI graphics",
   MNU_TOGGLEAVATAR, "Toggle AVATAR graphics",
   MNU_TOGGLECOLOR, "Toggle COLOR codes",
   MNU_TOGGLEHOTKEY, "Toggle hot-keyed menu",
   MNU_SETCOMPANY, "Set company name",
   MNU_SETADDRESS, "Set address",
   MNU_SETCITY, "Set city",
   MNU_SETPHONE, "Set phone number",
   MNU_SETGENDER, "Set gender",
   MNU_TOGGLEFULLSCREEN, "Toggle fullscreen lists",
   MNU_TOGGLEIBMCHARS, "Toggle IBM characters",
   MNU_TOGGLEMOREPROMPT, "Toggle More? prompt",
   MNU_TOGGLESCREENCLEAR, "Toggle screen clear",
   MNU_TOGGLEINUSERLIST, "Toggle in user list",
   MNU_SETARCHIVER, "Set default archiver",
   MNU_SETPROTOCOL, "Set default protocol",
   MNU_SETSIGNATURE, "Set personal signature",
   MNU_SETVIDEOMODE, "Set video mode",
   MNU_TOGGLEFULLED, "Toggle fullscreen editor",
   MNU_TOGGLEFULLREAD, "Toggle fullscreen reader",
   MNU_TOGGLENODISTURB, "Toggle do not disturb flag",
   MNU_TOGGLEMAILCHECK, "Toggle logon mail check",
   MNU_TOGGLEFILECHECK, "Toggle new files check",
   MNU_SETBIRTHDATE, "Set birthdate",
   MNU_SETSCREENLENGTH, "Set screen length",
   MNU_TOGGLERIP, "Toggle RIP graphics",
   0, NULL
};

// Personal mail
MENUCMD MenuCmd5[] = {
   MNU_MAILWRITELOCAL,    "Write local mail    ",
   MNU_MAILWRITEINTERNET, "Write Internet mail ",
   MNU_MAILREAD,          "Read mail           ",
   MNU_MAILDELETE,        "Kill mail           ",
   MNU_MAILLIST,          "List mail           ",
   MNU_MAILWRITEFIDONET,  "Write FidoNet mail  ",
   MNU_MAILCHECK,         "Check unread mail   ",
   MNU_MAILNEXT,          "Read next mail      ",
   MNU_MAILPREVIOUS,      "Read previous mail  ",
   MNU_MAILINDIVIDUAL,    "Read individual mail",
   MNU_MAILNONSTOP,       "Read mail non-stop  ",
   0, NULL
};

// Offline reader
MENUCMD MenuCmd6[] = {
   MNU_OLRDOWNLOADASCII, "ASCII download    ",
   MNU_OLRDOWNLOADBW,    "BlueWave download ",
   MNU_OLRDOWNLOADQWK,   "QWK download      ",
   MNU_OLRTAGAREA,       "Tag areas         ",
   MNU_OLRREMOVEAREA,    "Untag areas       ",
   MNU_OLRVIEWTAGGED,    "View tagged areas ",
   MNU_OLRUPLOAD,        "Upload replies    ",
   MNU_OLRDOWNLOADPNT,   "PointMail download",
   MNU_OLRRESTRICTDATE,  "Restrict date     ",
   0, NULL
};

   // Miscellaneous
MENUCMD MenuCmd7[] = {
   MNU_NULL,        "Display only           ",
   MNU_LOGOFF,      "Logoff                 ",
   MNU_ONLINEUSERS, "Online users           ",
   MNU_DISPLAY,     "Display file (anywhere)",
   MNU_RUNEXTERNAL, "Run external program   ",
   MNU_PRESSENTER,  "Press Enter to Continue",
   MNU_VERSION,     "Version information    ",
   MNU_TELNET,      "Telnet client          ",
   MNU_FINGER,      "Finger                 ",
   MNU_FTP,         "FTP client             ",
   MNU_IRC,         "IRC client             ",
   MNU_APPENDMENU,  "Append menu            ",
//   MNU_GOPHER, "Gopher Client",
   0, NULL
};

VOID AddCmdShadow (VOID)
{
   wtitle (" Menu Commands ", TCENTER, WHITE|_CYAN);
   wshadow (DGREY|_BLACK);
}

USHORT CSelectColorDlg (USHORT color)
{
   short x, y, i;

   wopen (7, 30, 16, 47, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Colors ", TCENTER, WHITE|_LGREY);

   for (y = 0; y < 8; y++) {
      for (x = 0; x < 16; x++)
         wprints (y, x, (short)(x | (y << 4)), "");
   }

   x = (short)(color & 0x0F);
   y = (short)((color & 0xF0) >> 4);

   wprints (y, x, LGREY|_BLACK, "");

   for (;;) {
      i = (short)getxch ();
      if ( (i & 0xFF) != 0 )
         i &= 0xFF;

      switch (i) {
         case 0x4800:
            if (y > 1) {
               wprints (y, x, (short)(x | (y << 4)), "");
               y--;
               wprints (y, x, LGREY|_BLACK, "");
            }
            break;

         case 0x5000:
            if (y < 8) {
               wprints (y, x, (short)(x | (y << 4)), "");
               y++;
               wprints (y, x, LGREY|_BLACK, "");
            }
            break;

         case 0x4B00:
            if (x > 1) {
               wprints (y, x, (short)(x | (y << 4)), "");
               x--;
               wprints (y, x, LGREY|_BLACK, "");
            }
            break;

         case 0x4D00:
            if (x < 16) {
               wprints (y, x, (short)(x | (y << 4)), "");
               x++;
               wprints (y, x, LGREY|_BLACK, "");
            }
            break;
      }

      if (i == 0x1B) {
         color = (short)0xFFFFU;
         break;
      }
      else if (i == 0x0D) {
         color = (short)(x | (y << 4));
         break;
      }
   }

   wclose ();
   return (color);
}

USHORT CMenuPromptDlg (class TMenu *Menu)
{
   short menu_sel = 1;
   USHORT r, RetVal = FALSE;
   class TMenu *Data;

   Data = new TMenu;
   strcpy (Data->Prompt, Menu->Prompt);
   Data->PromptColor = Menu->PromptColor;
   Data->PromptHilight = Menu->PromptHilight;

   wopen (8, 4, 15, 74, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Menu Prompt ", TCENTER, WHITE|_LGREY);

   DisplayButton (4,  2, "   Ok   ");
   DisplayButton (4, 12, " Cancel ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Display    ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Color      ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Hilight    ", 0, 3, 0, NULL, 0, 0);

      wmenuitem (4,  2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (4, 12, " Cancel ", 'C', 995, 0, NULL, 0, 0);

      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 14, Data->Prompt, sizeof (Data->Prompt), 54);
      wprints (1, 14, Data->PromptColor, "Sample color text");
      wprints (2, 14, Data->PromptHilight, "Sample color text");

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 14, Data->Prompt, sizeof (Data->Prompt), 54);
            break;
         case 2:
            if ((r = CSelectColorDlg (Data->PromptColor)) != 0xFFFFU)
               Data->Color = (UCHAR)r;
            break;
         case 3:
            if ((r = CSelectColorDlg (Data->PromptHilight)) != 0xFFFFU)
               Data->Hilight = (UCHAR)r;
            break;
         case 996:
            strcpy (Menu->Prompt, Data->Prompt);
            Menu->PromptColor = Data->PromptColor;
            Menu->PromptHilight = Data->PromptHilight;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

VOID CommandSelection (VOID)
{
   struct _item_t *item;

   item = wmenuicurr ();
   cmd_sel = item->tagid;
}

USHORT CMenuEditorDlg (PSZ pszFile)
{
   short i, menu_sel = 1, maxlen, start;
   CHAR Temp[48], *p;
   USHORT r, RetVal = FALSE;
   MENUCMD *MenuCmd;
   class TCollection List;
   class TMenu *Data;

   Data = new TMenu;
   Data->Load (pszFile);
   if (Data->First () == FALSE)
      Data->New ();

   wopen (5, 2, 19, 76, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Menu Editor ", TCENTER, WHITE|_LGREY);

   DisplayButton (9, 22, " Delete ");
   DisplayButton (9, 32, " Prompt ");
   DisplayButton (9, 52, "    Security     ");

   DisplayButton (11,  2, "  Save  ");
   DisplayButton (11, 12, "  Add   ");
   DisplayButton (11, 22, " Delete ");
   DisplayButton (11, 32, "  List  ");
   DisplayButton (11, 52, "  Prev. ");
   DisplayButton (11, 62, "  Next  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Key        ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Display    ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Color      ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Hilight    ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Command    ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Argument   ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " Automatic  ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (7, 1, " First Time ", 0, 8, 0, NULL, 0, 0);

      wmenuitem (9, 22, " Delete ", 'D', 121, 0, NULL, 0, 0);
      wmenuitem (9, 32, " Prompt ", 'r', 122, 0, NULL, 0, 0);
      wmenuitem (9, 52, "    Security      ", 'S', 123, 0, NULL, 0, 0);

      wmenuitem (11,  2, "  Save  ", 'v', 996, 0, NULL, 0, 0);
      wmenuitem (11, 12, "  Add   ", 'A', 115, 0, NULL, 0, 0);
      wmenuitem (11, 22, " Insert ", 'I', 116, 0, NULL, 0, 0);
      wmenuitem (11, 32, "  List  ", 'L', 117, 0, NULL, 0, 0);
      wmenuitem (11, 52, "  Prev. ", 'P', 118, 0, NULL, 0, 0);
      wmenuitem (11, 62, "  Next  ", 'N', 119, 0, NULL, 0, 0);

      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 14, Data->Key, sizeof (Data->Key), 15);
      DisplayTextField (1, 14, Data->Display, sizeof (Data->Display), 58);
      wprints (2, 14, Data->Color, "Sample color text");
      wprints (3, 14, Data->Hilight, "Sample color text");
      p = "???";
      for (i = 0; MenuCmd1[i].Text != NULL; i++) {
         if (MenuCmd1[i].Id == Data->Command) {
            sprintf (Temp, "%d - %s", MenuCmd1[i].Id, MenuCmd1[i].Text);
            p = Temp;
            break;
         }
      }
      for (i = 0; MenuCmd2[i].Text != NULL; i++) {
         if (MenuCmd2[i].Id == Data->Command) {
            sprintf (Temp, "%d - %s", MenuCmd2[i].Id, MenuCmd2[i].Text);
            p = Temp;
            break;
         }
      }
      for (i = 0; MenuCmd3[i].Text != NULL; i++) {
         if (MenuCmd3[i].Id == Data->Command) {
            sprintf (Temp, "%d - %s", MenuCmd3[i].Id, MenuCmd3[i].Text);
            p = Temp;
            break;
         }
      }
      for (i = 0; MenuCmd4[i].Text != NULL; i++) {
         if (MenuCmd4[i].Id == Data->Command) {
            sprintf (Temp, "%d - %s", MenuCmd4[i].Id, MenuCmd4[i].Text);
            p = Temp;
            break;
         }
      }
      for (i = 0; MenuCmd5[i].Text != NULL; i++) {
         if (MenuCmd5[i].Id == Data->Command) {
            sprintf (Temp, "%d - %s", MenuCmd5[i].Id, MenuCmd5[i].Text);
            p = Temp;
            break;
         }
      }
      for (i = 0; MenuCmd6[i].Text != NULL; i++) {
         if (MenuCmd6[i].Id == Data->Command) {
            sprintf (Temp, "%d - %s", MenuCmd6[i].Id, MenuCmd6[i].Text);
            p = Temp;
            break;
         }
      }
      for (i = 0; MenuCmd7[i].Text != NULL; i++) {
         if (MenuCmd7[i].Id == Data->Command) {
            sprintf (Temp, "%d - %s", MenuCmd7[i].Id, MenuCmd7[i].Text);
            p = Temp;
            break;
         }
      }
      DisplayTextField (4, 14, p, 35, 35);
      DisplayTextField (5, 14, Data->Argument, sizeof (Data->Argument), 58);
      wprints (6, 14, WHITE|_BLUE, (Data->Automatic == TRUE) ? "Yes" : "No ");
      wprints (7, 14, WHITE|_BLUE, (Data->FirstTime == TRUE) ? "Yes" : "No ");

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 14, Data->Key, sizeof (Data->Key), 15);
            break;
         case 2:
            GetTextField (1, 14, Data->Display, sizeof (Data->Display), 58);
            break;
         case 3:
            if ((r = CSelectColorDlg (Data->Color)) != 0xFFFFU)
               Data->Color = (UCHAR)r;
            break;
         case 4:
            if ((r = CSelectColorDlg (Data->Hilight)) != 0xFFFFU)
               Data->Hilight = (UCHAR)r;
            break;
         case 5:
            List.Clear ();
            start = maxlen = 0;

            wmenubeg (7, 25, 15, 48, 3, BLACK|_LGREY, BLACK|_LGREY, AddShadow);
            wmenuitem (0, 0, " Moving between menus ", 0, 901, 0, NULL, 0, 0);
               maxlen = 0;
               MenuCmd = MenuCmd1;
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  if (maxlen < strlen (Temp))
                     maxlen = (short)strlen (Temp);
               }
               wmenubeg (5, 40, (short)(5 + i + 1), (short)(40 + maxlen + 1), 3, RED|_LGREY, BLUE|_LGREY, AddShadow);
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  List.Add (Temp);
                  wmenuitem ( i, 0, (PSZ)List.Value (), 0, MenuCmd[i].Id, M_CLALL, CommandSelection, 0, 0);
               }
               wmenuend (MenuCmd[0].Id, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
            wmenuitem (1, 0, " Message areas        ", 0, 902, 0, NULL, 0, 0);
               maxlen = 0;
               MenuCmd = MenuCmd2;
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  if (maxlen < strlen (Temp))
                     maxlen = (short)strlen (Temp);
               }
               wmenubeg (2, 40, (short)(2 + i + 1), (short)(40 + maxlen + 1), 3, RED|_LGREY, BLUE|_LGREY, AddShadow);
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  List.Add (Temp);
                  wmenuitem ( i, 0, (PSZ)List.Value (), 0, MenuCmd[i].Id, M_CLALL, CommandSelection, 0, 0);
               }
               wmenuend (MenuCmd[0].Id, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
            wmenuitem (2, 0, " File areas           ", 0, 903, 0, NULL, 0, 0);
               maxlen = 0;
               MenuCmd = MenuCmd3;
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  if (maxlen < strlen (Temp))
                     maxlen = (short)strlen (Temp);
               }
               wmenubeg (3, 40, (short)(3 + i + 1), (short)(40 + maxlen + 1), 3, RED|_LGREY, BLUE|_LGREY, AddShadow);
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  List.Add (Temp);
                  wmenuitem ( i, 0, (PSZ)List.Value (), 0, MenuCmd[i].Id, M_CLALL, CommandSelection, 0, 0);
               }
               wmenuend (MenuCmd[0].Id, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
            wmenuitem (3, 0, " User configuration   ", 0, 904, 0, NULL, 0, 0);
               maxlen = 0;
               MenuCmd = MenuCmd4;
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  if (maxlen < strlen (Temp))
                     maxlen = (short)strlen (Temp);
               }
               wmenubeg (2, 40, (short)(2 + i + 1), (short)(40 + maxlen + 1), 3, RED|_LGREY, BLUE|_LGREY, AddShadow);
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  List.Add (Temp);
                  wmenuitem ( i, 0, (PSZ)List.Value (), 0, MenuCmd[i].Id, M_CLALL, CommandSelection, 0, 0);
               }
               wmenuend (MenuCmd[0].Id, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
            wmenuitem (4, 0, " Personal mail        ", 0, 905, 0, NULL, 0, 0);
               maxlen = 0;
               MenuCmd = MenuCmd5;
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  if (maxlen < strlen (Temp))
                     maxlen = (short)strlen (Temp);
               }
               wmenubeg (5, 40, (short)(5 + i + 1), (short)(40 + maxlen + 1), 3, RED|_LGREY, BLUE|_LGREY, AddShadow);
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  List.Add (Temp);
                  wmenuitem ( i, 0, (PSZ)List.Value (), 0, MenuCmd[i].Id, M_CLALL, CommandSelection, 0, 0);
               }
               wmenuend (MenuCmd[0].Id, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
            wmenuitem (5, 0, " Offline reader       ", 0, 906, 0, NULL, 0, 0);
               maxlen = 0;
               MenuCmd = MenuCmd6;
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  if (maxlen < strlen (Temp))
                     maxlen = (short)strlen (Temp);
               }
               wmenubeg (6, 40, (short)(6 + i + 1), (short)(40 + maxlen + 1), 3, RED|_LGREY, BLUE|_LGREY, AddShadow);
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  List.Add (Temp);
                  wmenuitem ( i, 0, (PSZ)List.Value (), 0, MenuCmd[i].Id, M_CLALL, CommandSelection, 0, 0);
               }
               wmenuend (MenuCmd[0].Id, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
            wmenuitem (6, 0, " Miscellaneous        ", 0, 907, 0, NULL, 0, 0);
               maxlen = 0;
               MenuCmd = MenuCmd7;
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  if (maxlen < strlen (Temp))
                     maxlen = (short)strlen (Temp);
               }
               wmenubeg (5, 40, (short)(5 + i + 1), (short)(40 + maxlen + 1), 3, RED|_LGREY, BLUE|_LGREY, AddShadow);
               for (i = 0; MenuCmd[i].Text != NULL; i++) {
                  sprintf (Temp, " %3d - %s ", MenuCmd[i].Id, MenuCmd[i].Text);
                  List.Add (Temp);
                  wmenuitem ( i, 0, (PSZ)List.Value (), 0, MenuCmd[i].Id, M_CLALL, CommandSelection, 0, 0);
               }
               wmenuend (MenuCmd[0].Id, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
            wmenuend (901, M_VERT, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);

            if (wmenuget () != -1) {
               for (i = 0; MenuCmd1[i].Text != NULL; i++) {
                  if (MenuCmd1[i].Id == cmd_sel) {
                     Data->Command = MenuCmd1[i].Id;
                     break;
                  }
               }
               for (i = 0; MenuCmd2[i].Text != NULL; i++) {
                  if (MenuCmd2[i].Id == cmd_sel) {
                     Data->Command = MenuCmd2[i].Id;
                     break;
                  }
               }
               for (i = 0; MenuCmd3[i].Text != NULL; i++) {
                  if (MenuCmd3[i].Id == cmd_sel) {
                     Data->Command = MenuCmd3[i].Id;
                     break;
                  }
               }
               for (i = 0; MenuCmd4[i].Text != NULL; i++) {
                  if (MenuCmd4[i].Id == cmd_sel) {
                     Data->Command = MenuCmd4[i].Id;
                     break;
                  }
               }
               for (i = 0; MenuCmd5[i].Text != NULL; i++) {
                  if (MenuCmd5[i].Id == cmd_sel) {
                     Data->Command = MenuCmd5[i].Id;
                     break;
                  }
               }
               for (i = 0; MenuCmd6[i].Text != NULL; i++) {
                  if (MenuCmd6[i].Id == cmd_sel) {
                     Data->Command = MenuCmd6[i].Id;
                     break;
                  }
               }
               for (i = 0; MenuCmd7[i].Text != NULL; i++) {
                  if (MenuCmd7[i].Id == cmd_sel) {
                     Data->Command = MenuCmd7[i].Id;
                     break;
                  }
               }
            }
            break;
         case 6:
            GetTextField (5, 14, Data->Argument, sizeof (Data->Argument), 58);
            break;
         case 7:
            Data->Automatic = (Data->Automatic == TRUE) ? FALSE : TRUE;
            break;
         case 8:
            Data->FirstTime = (Data->FirstTime == TRUE) ? FALSE : TRUE;
            break;
         case 115:
            Data->New ();
            Data->Add ();
            break;
         case 116:
            Data->New ();
            Data->Insert ();
            break;
         case 118:
            Data->Update ();
            Data->Previous ();
            break;
         case 119:
            Data->Update ();
            Data->Next ();
            break;
         case 121:
            if (MessageBox ("Delete Item", "Are you really sure ?") == TRUE)
               Data->Delete ();
            break;
         case 122:
            CMenuPromptDlg (Data);
            break;
         case 123:
            CSecurityDlg (" Item Security ", &Data->Level, &Data->AccessFlags, &Data->DenyFlags);
            break;
         case 996:
            Data->Update ();
            Data->Save (pszFile);
            break;
      }
   } while (menu_sel != -1);

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

USHORT CBBSGeneralDlg (VOID)
{
   short menu_sel = 996;
   USHORT RetVal = FALSE;
   CHAR Temp[16];

   wopen (5, 3, 20, 75, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" General BBS Options ", TCENTER, WHITE|_LGREY);

   DisplayButton (12, 2, "   Ok   ");
   DisplayButton (12, 12, " Cancel ");
   DisplayButton (12, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Users File        ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Menu Directory    ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " General Text Dir. ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Home Directory    ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " E-Mail Storage    ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (4, 38, " Board ", 0, 10, 0, NULL, 0, 0);
      wmenuitem (5, 1, " E-Mail Path       ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " Main Menu         ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (7, 1, " External editor   ", 0, 11, 0, NULL, 0, 0);
      wmenuitem (8, 1, " Editor command    ", 0, 12, 0, NULL, 0, 0);
      wmenuitem (9, 1, " ANSI at Logon     ", 0, 8, 0, NULL, 0, 0);
      wmenuitem (10, 1, " IEMSI Logins      ", 0, 9, 0, NULL, 0, 0);
      wmenuitem (12, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (12, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (12, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 21, Cfg->UserFile, sizeof (Cfg->UserFile), 49);
      DisplayTextField (1, 21, Cfg->MenuPath, sizeof (Cfg->MenuPath), 49);
      DisplayTextField (2, 21, Cfg->TextFiles, sizeof (Cfg->TextFiles), 49);
      DisplayTextField (3, 21, Cfg->UsersHomePath, sizeof (Cfg->UsersHomePath), 49);
      switch (Cfg->MailStorage) {
         case ST_JAM:
            wprints (4, 21, WHITE|_BLUE, "JAM             ");
            break;
         case ST_SQUISH:
            wprints (4, 21, WHITE|_BLUE, "Squish          ");
            break;
         case ST_FIDO:
            wprints (4, 21, WHITE|_BLUE, "Fido *.MSG      ");
            break;
         case ST_ADEPT:
            wprints (4, 21, WHITE|_BLUE, "AdeptXBBS       ");
            break;
         case ST_HUDSON:
            wprints (4, 21, WHITE|_BLUE, "Hudson (QBBS)   ");
            break;
      }
      sprintf (Temp, "%u", Cfg->MailBoard);
      DisplayTextField (4, 46, Temp, 5, 5);
      DisplayTextField (5, 21, Cfg->MailPath, sizeof (Cfg->MailPath), 49);
      DisplayTextField (6, 21, Cfg->MainMenu, sizeof (Cfg->MainMenu), 49);
      wprints (7, 21, WHITE|_BLUE, (Cfg->ExternalEditor == TRUE) ? "Yes" : "No ");
      DisplayTextField (8, 21, Cfg->EditorCmd, sizeof (Cfg->EditorCmd), 49);
      if (Cfg->Ansi == NO)
         wprints (9, 21, WHITE|_BLUE, "No  ");
      else if (Cfg->Ansi == YES)
         wprints (9, 21, WHITE|_BLUE, "Yes ");
      else if (Cfg->Ansi == AUTO)
         wprints (9, 21, WHITE|_BLUE, "Auto");
      wprints (10, 21, WHITE|_BLUE, (Cfg->IEMSI == TRUE) ? "Yes" : "No ");

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 21, Cfg->UserFile, sizeof (Cfg->UserFile), 49);
            break;
         case 2:
            GetTextField (1, 21, Cfg->MenuPath, sizeof (Cfg->MenuPath), 49);
            break;
         case 3:
            GetTextField (2, 21, Cfg->TextFiles, sizeof (Cfg->TextFiles), 49);
            break;
         case 4:
            GetTextField (3, 21, Cfg->UsersHomePath, sizeof (Cfg->UsersHomePath), 49);
            break;
         case 5:
            switch (Cfg->MailStorage) {
               case ST_JAM:
                  Cfg->MailStorage = ST_SQUISH;
                  break;
               case ST_SQUISH:
                  Cfg->MailStorage = ST_FIDO;
                  break;
               case ST_FIDO:
                  Cfg->MailStorage = ST_ADEPT;
                  break;
               case ST_ADEPT:
                  Cfg->MailStorage = ST_JAM;
                  break;
               case ST_HUDSON:
                  Cfg->MailStorage = ST_HUDSON;
                  break;
            }
            break;
         case 6:
            GetTextField (5, 21, Cfg->MailPath, sizeof (Cfg->MailPath), 49);
            break;
         case 7:
            GetTextField (6, 21, Cfg->MainMenu, sizeof (Cfg->MainMenu), 49);
            break;
         case 8:
            if (Cfg->Ansi == NO)
               Cfg->Ansi = YES;
            else if (Cfg->Ansi == YES)
               Cfg->Ansi = AUTO;
            else if (Cfg->Ansi == AUTO)
               Cfg->Ansi = NO;
            break;
         case 9:
            Cfg->IEMSI = (Cfg->IEMSI == TRUE) ? FALSE : TRUE;
            break;
         case 10:
            sprintf (Temp, "%u", Cfg->MailBoard);
            GetTextField (4, 46, Temp, 5, 5);
            Cfg->MailBoard = (USHORT)atoi (Temp);
            break;
         case 11:
            Cfg->ExternalEditor = (Cfg->ExternalEditor == TRUE) ? FALSE : TRUE;
            break;
         case 12:
            GetTextField (8, 21, Cfg->EditorCmd, sizeof (Cfg->EditorCmd), 49);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

// ----------------------------------------------------------------------

USHORT CAddFileDlg (PSZ Key, PSZ Description)
{
   short menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (9, 4, 15, 75, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Add File Area ", TCENTER, WHITE|_LGREY);

   DisplayButton (3, 2, "   Ok   ");
   DisplayButton (3, 12, " Cancel ");
   DisplayButton (3, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Key         ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Description ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (3, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (3, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (3, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 15, Key, 15, 15);
      DisplayTextField (1, 15, Description, 54, 54);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 15, Key, 15, 15);
            break;
         case 2:
            GetTextField (1, 15, Description, 54, 54);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CFileListDlg (PSZ key, PSZ search)
{
   short i, start = 0;
   CHAR Temp[128], *p, **Array;
   USHORT RetVal = FALSE, Found = TRUE;
   class TFileData *Data;
   class TCollection List;

   wopen (7, 2, 23, 77, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" File Areas List ", TCENTER, WHITE|_LGREY);

   sprintf (Temp, " %-15.15s %5s %5s %5s %-38.38s ", "Key", "Level", "Dnl.L", "Upl.L", "Description");
   wprints (0, 0, YELLOW|_LGREY, Temp);

   if (search != NULL)
      strupr (search);

   Data = new TFileData (Cfg->SystemPath);
   if (Data->First () == TRUE) {
      i = 0;
      List.Clear ();
      do {
         if (search != NULL) {
            Found = FALSE;
            strcpy (Temp, Data->Key);
            if (strstr (strupr (Temp), search) != NULL)
               Found = TRUE;
            else {
               strcpy (Temp, Data->Display);
               if (strstr (strupr (Temp), search) != NULL)
                  Found = TRUE;
               else {
                  strcpy (Temp, Data->EchoTag);
                  if (strstr (strupr (Temp), search) != NULL)
                     Found = TRUE;
               }
            }
         }
         if (Found == TRUE) {
            sprintf (Temp, " %-15.15s %5u %5u %5u %-38.38s ", Data->Key, Data->Level, Data->DownloadLevel, Data->UploadLevel, Data->Display);
            List.Add (Temp);
            if (!strcmp (Data->Key, key))
               start = i;
            i++;
         }
      } while (Data->Next () == TRUE);

      if (List.Elements > 0) {
         i = 0;
         Array = (CHAR **)malloc ((List.Elements + 1) * sizeof (CHAR *));
         if ((p = (CHAR *)List.First ()) != NULL)
            do {
               Array[i++] = p;
            } while ((p = (CHAR *)List.Next ()) != NULL);
         Array[i] = NULL;
         if ((i = wpickstr (9, 3, 22, 76, 5, WHITE|_LGREY, WHITE|_LGREY, WHITE|_BLUE, Array, start, NULL)) != -1) {
            p = Array[i] + 1;
            p[15] = '\0';
            strcpy (key, strtrim (p));
            RetVal = TRUE;
         }
         if (Array != NULL)
            free (Array);
      }
      else
         getxch ();
   }
   else
      getxch ();

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

USHORT CFileSecurityDlg (class TFileData *Data)
{
   short menu_sel = 1;
   CHAR Temp[48];
   USHORT RetVal = FALSE, Level, DownloadLevel, UploadLevel;
   ULONG AccessFlags, DenyFlags, DownloadFlags, DenyDownloadFlags;
   ULONG UploadFlags, DenyUploadFlags;

   Level = Data->Level;
   AccessFlags = Data->AccessFlags;
   DenyFlags = Data->DenyFlags;
   DownloadLevel = Data->DownloadLevel;
   DownloadFlags = Data->DownloadFlags;
   DenyDownloadFlags = Data->DownloadDenyFlags;
   UploadLevel = Data->UploadLevel;
   UploadFlags = Data->UploadFlags;
   DenyUploadFlags = Data->UploadDenyFlags;

   wopen (6, 14, 21, 65, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" File Security ", TCENTER, WHITE|_LGREY);

   DisplayButton (12,  2, "   Ok   ");
   DisplayButton (12, 12, " Cancel ");

   do {
      wmenubegc ();
      wprints (0, 1, WHITE|_LGREY, "Access Security");
      wmenuitem (0, 22, " Level      ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Flags      ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Deny Flags ", 0, 3, 0, NULL, 0, 0);

      wprints (4, 1, WHITE|_LGREY, "Download Security");
      wmenuitem (4, 22, " Level      ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Flags      ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (6, 1, " Deny Flags ", 0, 6, 0, NULL, 0, 0);

      wprints (8, 1, WHITE|_LGREY, "Upload Security");
      wmenuitem (8, 22, " Level      ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (9, 1, " Flags      ", 0, 8, 0, NULL, 0, 0);
      wmenuitem (10, 1, " Deny Flags ", 0, 9, 0, NULL, 0, 0);

      wmenuitem (12,  2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (12, 12, " Cancel ", 'C', 995, 0, NULL, 0, 0);

      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      sprintf (Temp, "%u", Level);
      DisplayTextField (0, 35, Temp, 5, 5);
      DisplayFlags (1, 14, AccessFlags);
      DisplayFlags (2, 14, DenyFlags);

      sprintf (Temp, "%u", DownloadLevel);
      DisplayTextField (4, 35, Temp, 5, 5);
      DisplayFlags (5, 14, DownloadFlags);
      DisplayFlags (6, 14, DenyDownloadFlags);

      sprintf (Temp, "%u", UploadLevel);
      DisplayTextField (8, 35, Temp, 5, 5);
      DisplayFlags (9, 14, UploadFlags);
      DisplayFlags (10, 14, DenyUploadFlags);

      switch (menu_sel = wmenuget ()) {
         case 1:
            sprintf (Temp, "%u", Level);
            GetTextField (0, 35, Temp, 5, 5);
            Level = (USHORT)atoi (Temp);
            break;
         case 2:
            CFlagsDlg (" Access Flags ", &AccessFlags);
            break;
         case 3:
            CFlagsDlg (" Deny Flags ", &DenyFlags);
            break;
         case 4:
            sprintf (Temp, "%u", DownloadLevel);
            GetTextField (4, 35, Temp, 5, 5);
            DownloadLevel = (USHORT)atoi (Temp);
            break;
         case 5:
            CFlagsDlg (" Access Flags ", &DownloadFlags);
            break;
         case 6:
            CFlagsDlg (" Deny Flags ", &DenyDownloadFlags);
            break;
         case 7:
            sprintf (Temp, "%u", UploadLevel);
            GetTextField (8, 35, Temp, 5, 5);
            UploadLevel = (USHORT)atoi (Temp);
            break;
         case 8:
            CFlagsDlg (" Access Flags ", &UploadFlags);
            break;
         case 9:
            CFlagsDlg (" Deny Flags ", &DenyUploadFlags);
            break;
         case 996:
            Data->Level = Level;
            Data->AccessFlags = AccessFlags;
            Data->DenyFlags = DenyFlags;
            Data->DownloadLevel = DownloadLevel;
            Data->DownloadFlags = DownloadFlags;
            Data->DownloadDenyFlags = DenyDownloadFlags;
            Data->UploadLevel = UploadLevel;
            Data->UploadFlags = UploadFlags;
            Data->UploadDenyFlags = DenyUploadFlags;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CFileDlg (VOID)
{
   short menu_sel = 1;
   USHORT RetVal = FALSE;
   class TFileData *Data;

   Data = new TFileData (Cfg->SystemPath);
   if (Data->First () == FALSE)
      Data->New ();

   wopen (5, 5, 19, 74, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" File Areas ", TCENTER, WHITE|_LGREY);

   DisplayButton (9, 2, "  Move  ");
   DisplayButton (9, 12, " Delete ");
   DisplayButton (9, 22, " Links  ");
   DisplayButton (9, 32, " Search ");
   DisplayButton (9, 47, "    Security     ");

   DisplayButton (11,  2, " Change ");
   DisplayButton (11, 12, "  Add   ");
   DisplayButton (11, 22, " Insert ");
   DisplayButton (11, 32, "  List  ");
   DisplayButton (11, 47, "  Prev. ");
   DisplayButton (11, 57, "  Next  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Key           ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Display       ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Download Path ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Upload Path   ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Show Global   ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (5, 1, " CD-ROM        ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " Free Download ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (7, 1, " FileEcho Tag  ", 0, 8, 0, NULL, 0, 0);

      wmenuitem (9, 2, "  Move  ", 'M', 124, 0, NULL, 0, 0);
      wmenuitem (9, 12, " Delete ", 'D', 120, 0, NULL, 0, 0);
      wmenuitem (9, 22, " Links  ", 'k', 121, 0, NULL, 0, 0);
      wmenuitem (9, 32, " Search ", 'r', 122, 0, NULL, 0, 0);
      wmenuitem (9, 47, "    Security      ", 'S', 123, 0, NULL, 0, 0);

      wmenuitem (11,  2, " Change ", 'C', 996, 0, NULL, 0, 0);
      wmenuitem (11, 12, "  Add   ", 'A', 115, 0, NULL, 0, 0);
      wmenuitem (11, 22, " Insert ", 'I', 116, 0, NULL, 0, 0);
      wmenuitem (11, 32, "  List  ", 'L', 117, 0, NULL, 0, 0);
      wmenuitem (11, 47, "  Prev. ", 'P', 118, 0, NULL, 0, 0);
      wmenuitem (11, 57, "  Next  ", 'N', 119, 0, NULL, 0, 0);

      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 17, Data->Key, sizeof (Data->Key), 16);
      DisplayTextField (1, 17, Data->Display, sizeof (Data->Display), 50);
      DisplayTextField (2, 17, Data->Download, sizeof (Data->Download), 50);
      DisplayTextField (3, 17, Data->Upload, sizeof (Data->Upload), 50);
      wprints (4, 17, WHITE|_BLUE, (Data->ShowGlobal == TRUE) ? "Yes" : "No ");
      wprints (5, 17, WHITE|_BLUE, (Data->CdRom == TRUE) ? "Yes" : "No ");
      wprints (6, 17, WHITE|_BLUE, (Data->FreeDownload == TRUE) ? "Yes" : "No ");
      DisplayTextField (7, 17, Data->EchoTag, sizeof (Data->EchoTag), 50);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 17, Data->Key, sizeof (Data->Key), 16);
            break;
         case 2:
            GetTextField (1, 17, Data->Display, sizeof (Data->Display), 50);
            break;
         case 3:
            GetTextField (2, 17, Data->Download, sizeof (Data->Download), 50);
            break;
         case 4:
            GetTextField (3, 17, Data->Upload, sizeof (Data->Upload), 50);
            break;
         case 5:
            Data->ShowGlobal = (Data->ShowGlobal == TRUE) ? FALSE : TRUE;
            break;
         case 6:
            Data->CdRom = (Data->CdRom == TRUE) ? FALSE : TRUE;
            break;
         case 7:
            Data->FreeDownload = (Data->FreeDownload == TRUE) ? FALSE : TRUE;
            break;
         case 8:
            GetTextField (7, 17, Data->EchoTag, sizeof (Data->EchoTag), 50);
            break;
         case 115:
         case 116: {
            CHAR Key[16], Description[64];

            Key[0] = Description[0] = '\0';
            if (CAddFileDlg (Key, Description) == TRUE) {
               Data->New ();
               strcpy (Data->Key, Key);
               strcpy (Data->Display, Description);
               if (menu_sel == 115)
                  Data->Add ();
               else if (menu_sel == 116)
                  Data->Insert ();
            }
            break;
         }
         case 117: {
            CHAR Key[16];

            strcpy (Key, Data->Key);
            if (CFileListDlg (Key, NULL) == TRUE)
               Data->Read (Key, FALSE);
            break;
         }
         case 118:
            Data->Previous ();
            break;
         case 119:
            Data->Next ();
            break;
         case 120:
            if (MessageBox ("Delete Area", "Are you really sure ?") == TRUE)
               Data->Delete ();
            break;
         case 122: {
            CHAR Key[16], Search[48];

            if (wopen (10, 19, 12, 60, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
               wshadow (DGREY|_BLACK);
               wtitle (" Search Area ", TCENTER, WHITE|_LGREY);

               wprints (0, 1, WHITE|_GREEN, " String ");
               Search[0] = '\0';
               winpbeg (WHITE|_BLUE, WHITE|_BLUE);
               winpdef (0, 10, Search, "???????????????????????????", 0, 2, NULL, 0);
               if (winpread () != W_ESCPRESS && Search[0] != '\0') {
                  hidecur ();
                  wclose ();

                  strtrim (Search);
                  strcpy (Key, Data->Key);
                  if (CFileListDlg (Key, Search) == TRUE)
                     Data->Read (Key, FALSE);
               }
               else {
                  hidecur ();
                  wclose ();
               }
            }
            break;
         }
         case 123:
            CFileSecurityDlg (Data);
            break;
         case 124: {
            CHAR Key[16];
            class TFileData *NewData;

            strcpy (Key, Data->Key);
            if (CFileListDlg (Key, NULL) == TRUE) {
               if ((NewData = new TFileData (Cfg->SystemPath)) != NULL) {
                  NewData->Read (Data->Key);
                  NewData->Delete ();
                  NewData->Read (Key);
                  NewData->Insert (Data);
                  delete NewData;
               }
            }
            if (Data->Read (Data->Key, FALSE) == FALSE)
               Data->Read (Key, FALSE);
            break;
         }
         case 996:
            Data->Update ();
            break;
      }
   } while (menu_sel != -1);

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

// ----------------------------------------------------------------------

USHORT CAddMessageDlg (PSZ Key, PSZ Description)
{
   short menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (9, 4, 15, 75, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Add Message Area ", TCENTER, WHITE|_LGREY);

   DisplayButton (3, 2, "   Ok   ");
   DisplayButton (3, 12, " Cancel ");
   DisplayButton (3, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Key         ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Description ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (3, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (3, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (3, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 15, Key, 15, 15);
      DisplayTextField (1, 15, Description, 54, 54);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 15, Key, 15, 15);
            break;
         case 2:
            GetTextField (1, 15, Description, 54, 54);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CMessageListDlg (PSZ key, PSZ search)
{
   short i, start = 0;
   CHAR Temp[128], *p, **Array;
   USHORT RetVal = FALSE, Found = TRUE;
   class TMsgData *Data;
   class TCollection List;

   wopen (7, 2, 23, 77, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Message Areas List ", TCENTER, WHITE|_LGREY);

   sprintf (Temp, " %-15.15s %5s %5s %-44.44s ", "Key", "Level", "Wrt.L", "Description");
   wprints (0, 0, YELLOW|_LGREY, Temp);

   if (search != NULL)
      strupr (search);

   Data = new TMsgData (Cfg->SystemPath);
   if (Data->First () == TRUE) {
      i = 0;
      List.Clear ();
      do {
         if (search != NULL) {
            Found = FALSE;
            strcpy (Temp, Data->Key);
            if (strstr (strupr (Temp), search) != NULL)
               Found = TRUE;
            else {
               strcpy (Temp, Data->Display);
               if (strstr (strupr (Temp), search) != NULL)
                  Found = TRUE;
               else {
                  strcpy (Temp, Data->EchoTag);
                  if (strstr (strupr (Temp), search) != NULL)
                     Found = TRUE;
                  else {
                     strcpy (Temp, Data->NewsGroup);
                     if (strstr (strupr (Temp), search) != NULL)
                        Found = TRUE;
                  }
               }
            }
         }
         if (Found == TRUE) {
            sprintf (Temp, " %-15.15s %5u %5u %-44.44s ", Data->Key, Data->Level, Data->WriteLevel, Data->Display);
            List.Add (Temp);
            if (!strcmp (Data->Key, key))
               start = i;
            i++;
         }
      } while (Data->Next () == TRUE);

      if (List.Elements > 0) {
         i = 0;
         Array = (CHAR **)malloc ((List.Elements + 1) * sizeof (CHAR *));
         if ((p = (CHAR *)List.First ()) != NULL)
            do {
               Array[i++] = p;
            } while ((p = (CHAR *)List.Next ()) != NULL);
         Array[i] = NULL;
         if ((i = wpickstr (9, 3, 22, 76, 5, WHITE|_LGREY, WHITE|_LGREY, WHITE|_BLUE, Array, start, NULL)) != -1) {
            p = Array[i] + 1;
            p[15] = '\0';
            strcpy (key, strtrim (p));
            RetVal = TRUE;
         }
         if (Array != NULL)
            free (Array);
      }
      else
         getxch ();
   }
   else
      getxch ();

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

USHORT CMessageLinksDlg (class TEchoLink *Link)
{
   short i, menu_sel = 996, start = 0;
   CHAR Temp[128], *p, **Array;
   USHORT RetVal = FALSE;
   class TCollection List;

   wopen (5, 22, 20, 56, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Links ", TCENTER, WHITE|_LGREY);

   DisplayButton (1, 22, "  Add   ");
   DisplayButton (3, 22, " Remove ");

   DisplayButton (12, 2, "   Ok   ");
   DisplayButton (12, 12, " Cancel ");
   DisplayButton (12, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (1, 22, "  Add   ", 'A', 102, 0, NULL, 0, 0);
      wmenuitem (3, 22, " Remove ", 'R', 103, 0, NULL, 0, 0);

      wmenuitem (12, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (12, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (12, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      for (i = 0; i <= 10; i++)
         wprints (i, 1, BLACK|_CYAN, "                   ");

      i = 0;
      if (Link->First () == TRUE)
         do {
            if (i <= 10) {
               sprintf (Temp, "%.17s", Link->Address);
               wprints (i++, 2, BLACK|_CYAN, Temp);
            }
         } while (Link->Next () == TRUE);

      switch (menu_sel = wmenuget ()) {
         case 102:
            if (wopen (10, 20, 12, 59, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
               wshadow (DGREY|_BLACK);
               wtitle (" Add Link ", TCENTER, WHITE|_LGREY);

               wprints (0, 1, WHITE|_GREEN, " Address ");
               Temp[0] = '\0';
               winpbeg (WHITE|_BLUE, WHITE|_BLUE);
               winpdef (0, 11, Temp, "??????????????????????????", 0, 2, NULL, 0);
               if (winpread () != W_ESCPRESS && Temp[0] != '\0')
                  Link->AddString (Temp);
               hidecur ();
               wclose ();
            }
            break;
         case 103:
            if (Link->First () == TRUE) {
               i = 0;
               List.Clear ();
               do {
                  sprintf (Temp, " %-17.17s ", Link->Address);
                  List.Add (Temp);
                  i++;
               } while (Link->Next () == TRUE);

               if (List.Elements > 0) {
                  i = 0;
                  Array = (CHAR **)malloc ((List.Elements + 1) * sizeof (CHAR *));
                  if ((p = (CHAR *)List.First ()) != NULL)
                     do {
                        Array[i++] = p;
                     } while ((p = (CHAR *)List.Next ()) != NULL);
                  Array[i] = NULL;
                  if ((i = wpickstr (0 + 5 + 1, 1 + 22 + 1, 9 + 5 + 1, 19 + 22 + 1, 5, BLACK|_CYAN, BLACK|_CYAN, YELLOW|_CYAN, Array, start, NULL)) != -1) {
                     if (Link->First () == TRUE)
                        do {
                           sprintf (Temp, " %-17.17s ", Link->Address);
                           if (!strcmp (Temp, Array[i])) {
                              Link->Delete ();
                              break;
                           }
                        } while (Link->Next () == TRUE);
                  }
                  if (Array != NULL)
                     free (Array);
               }
               else
                  getxch ();
            }
            else
               getxch ();
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CMessageSecurityDlg (class TMsgData *Data)
{
   short menu_sel = 1;
   CHAR Temp[48];
   USHORT RetVal = FALSE, Level, WriteLevel;
   ULONG AccessFlags, DenyFlags, WriteAccessFlags, DenyWriteFlags;

   Level = Data->Level;
   AccessFlags = Data->AccessFlags;
   DenyFlags = Data->DenyFlags;
   WriteLevel = Data->WriteLevel;
   WriteAccessFlags = Data->WriteFlags;
   DenyWriteFlags = Data->DenyWriteFlags;

   wopen (7, 14, 18, 65, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Message Security ", TCENTER, WHITE|_LGREY);

   DisplayButton (8,  2, "   Ok   ");
   DisplayButton (8, 12, " Cancel ");

   do {
      wmenubegc ();
      wprints (0, 1, WHITE|_LGREY, "Access Security");
      wmenuitem (0, 22, " Level      ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Flags      ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Deny Flags ", 0, 3, 0, NULL, 0, 0);

      wprints (4, 1, WHITE|_LGREY, "Write Security");
      wmenuitem (4, 22, " Level      ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Flags      ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (6, 1, " Deny Flags ", 0, 6, 0, NULL, 0, 0);

      wmenuitem (8,  2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (8, 12, " Cancel ", 'C', 995, 0, NULL, 0, 0);

      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      sprintf (Temp, "%u", Level);
      DisplayTextField (0, 36, Temp, 5, 5);
      DisplayFlags (1, 14, AccessFlags);
      DisplayFlags (2, 14, DenyFlags);

      sprintf (Temp, "%u", WriteLevel);
      DisplayTextField (4, 36, Temp, 5, 5);
      DisplayFlags (5, 14, WriteAccessFlags);
      DisplayFlags (6, 14, DenyWriteFlags);

      switch (menu_sel = wmenuget ()) {
         case 1:
            sprintf (Temp, "%u", Level);
            GetTextField (0, 36, Temp, 5, 5);
            Level = (USHORT)atoi (Temp);
            break;
         case 2:
            CFlagsDlg (" Access Flags ", &AccessFlags);
            break;
         case 3:
            CFlagsDlg (" Deny Flags ", &DenyFlags);
            break;
         case 4:
            sprintf (Temp, "%u", WriteLevel);
            GetTextField (4, 36, Temp, 5, 5);
            WriteLevel = (USHORT)atoi (Temp);
            break;
         case 5:
            CFlagsDlg (" Access Flags ", &WriteAccessFlags);
            break;
         case 6:
            CFlagsDlg (" Deny Flags ", &DenyWriteFlags);
            break;
         case 996:
            Data->Level = Level;
            Data->AccessFlags = AccessFlags;
            Data->DenyFlags = DenyFlags;
            Data->WriteLevel = WriteLevel;
            Data->WriteFlags = WriteAccessFlags;
            Data->DenyWriteFlags = DenyWriteFlags;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CMessageDlg (VOID)
{
   short menu_sel = 1;
   USHORT RetVal = FALSE;
   CHAR Temp[32];
   class TMsgData *Data;

   Data = new TMsgData (Cfg->SystemPath);
   if (Data->First () == FALSE)
      Data->New ();

   wopen (3, 5, 21, 74, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Message Areas ", TCENTER, WHITE|_LGREY);

   DisplayButton (13, 2, "  Move  ");
   DisplayButton (13, 12, " Delete ");
   DisplayButton (13, 22, " Links  ");
   DisplayButton (13, 32, " Search ");
   DisplayButton (13, 47, "    Security     ");

   DisplayButton (15,  2, " Change ");
   DisplayButton (15, 12, "  Add   ");
   DisplayButton (15, 22, " Insert ");
   DisplayButton (15, 32, "  List  ");
   DisplayButton (15, 47, "  Prev. ");
   DisplayButton (15, 57, "  Next  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Key       ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Display   ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Storage   ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (2, 30, " Hudson Board ", 0, 14, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Path      ", 0, 4, 0, NULL, 0, 0);

      wmenuitem (4, 13, " Show in Global Lists    ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (4, 45, " EchoMail Kludges ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (5, 13, " Use with Offline Reader ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (5, 45, " Update Newsgroup ", 0, 8, 0, NULL, 0, 0);

      wmenuitem (6, 1, " EchoTag   ", 0, 9, 0, NULL, 0, 0);
      wmenuitem (7, 1, " Newsgroup ", 0, 10, 0, NULL, 0, 0);
      wmenuitem (8, 1, " Last retr ", 0, 15, 0, NULL, 0, 0);
      wmenuitem (9, 1, " Address   ", 0, 11, 0, NULL, 0, 0);
      wmenuitem (10, 1, " Max.Msgs. ", 0, 12, 0, NULL, 0, 0);
      wmenuitem (11, 1, " Max Days  ", 0, 13, 0, NULL, 0, 0);

      wmenuitem (13, 2, "  Move  ", 'M', 124, 0, NULL, 0, 0);
      wmenuitem (13, 12, " Delete ", 'D', 120, 0, NULL, 0, 0);
      wmenuitem (13, 22, " Links  ", 'k', 121, 0, NULL, 0, 0);
      wmenuitem (13, 32, " Search ", 'r', 122, 0, NULL, 0, 0);
      wmenuitem (13, 47, "    Security      ", 'S', 123, 0, NULL, 0, 0);

      wmenuitem (15,  2, " Change ", 'C', 996, 0, NULL, 0, 0);
      wmenuitem (15, 12, "  Add   ", 'A', 115, 0, NULL, 0, 0);
      wmenuitem (15, 22, " Insert ", 'I', 116, 0, NULL, 0, 0);
      wmenuitem (15, 32, "  List  ", 'L', 117, 0, NULL, 0, 0);
      wmenuitem (15, 47, "  Prev. ", 'P', 118, 0, NULL, 0, 0);
      wmenuitem (15, 57, "  Next  ", 'N', 119, 0, NULL, 0, 0);

      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 13, Data->Key, sizeof (Data->Key), 16);
      DisplayTextField (1, 13, Data->Display, sizeof (Data->Display), 54);
      switch (Data->Storage) {
         case ST_JAM:
            wprints (2, 13, WHITE|_BLUE, "JAM             ");
            break;
         case ST_SQUISH:
            wprints (2, 13, WHITE|_BLUE, "Squish          ");
            break;
         case ST_FIDO:
            wprints (2, 13, WHITE|_BLUE, "Fido *.MSG      ");
            break;
         case ST_ADEPT:
            wprints (2, 13, WHITE|_BLUE, "AdeptXBBS       ");
            break;
         case ST_HUDSON:
            wprints (2, 13, WHITE|_BLUE, "Hudson (QBBS)   ");
            break;
         case ST_USENET:
            wprints (2, 13, WHITE|_BLUE, "Usenet Newsgroup");
            break;
         case ST_PASSTHROUGH:
            wprints (2, 13, WHITE|_BLUE, "Passthrough     ");
            break;
      }
      DisplayTextField (3, 13, Data->Path, sizeof (Data->Path), 54);
      wprints (4, 39, WHITE|_BLUE, (Data->ShowGlobal == TRUE) ? "Yes" : "No ");
      wprints (4, 64, WHITE|_BLUE, (Data->EchoMail == TRUE) ? "Yes" : "No ");
      wprints (5, 39, WHITE|_BLUE, (Data->Offline == TRUE) ? "Yes" : "No ");
      wprints (5, 64, WHITE|_BLUE, (Data->UpdateNews == TRUE) ? "Yes" : "No ");
      DisplayTextField (6, 13, Data->EchoTag, sizeof (Data->EchoTag), 54);
      DisplayTextField (7, 13, Data->NewsGroup, sizeof (Data->NewsGroup), 54);
      sprintf (Temp, "%lu", Data->Highest);
      DisplayTextField (8, 13, Temp, 7, 7);
      DisplayTextField (9, 13, Data->Address, sizeof (Data->Address), 32);
      sprintf (Temp, "%u", Data->MaxMessages);
      DisplayTextField (10, 13, Temp, 5, 5);
      sprintf (Temp, "%u", Data->DaysOld);
      DisplayTextField (11, 13, Temp, 5, 5);

      sprintf (Temp, "%u", Data->Board);
      DisplayTextField (2, 45, Temp, 5, 5);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 13, Data->Key, sizeof (Data->Key), 16);
            break;
         case 2:
            GetTextField (1, 13, Data->Display, sizeof (Data->Display), 54);
            break;
         case 3:
            switch (Data->Storage) {
               case ST_JAM:
                  Data->Storage = ST_SQUISH;
                  break;
               case ST_SQUISH:
                  Data->Storage = ST_FIDO;
                  break;
               case ST_FIDO:
                  Data->Storage = ST_ADEPT;
                  break;
               case ST_ADEPT:
                  Data->Storage = ST_HUDSON;
                  break;
               case ST_HUDSON:
                  Data->Storage = ST_USENET;
                  break;
               case ST_USENET:
                  Data->Storage = ST_PASSTHROUGH;
                  break;
               case ST_PASSTHROUGH:
                  Data->Storage = ST_JAM;
                  break;
            }
            break;
         case 4:
            GetTextField (3, 13, Data->Path, sizeof (Data->Path), 54);
            break;
         case 5:
            Data->ShowGlobal = (Data->ShowGlobal == TRUE) ? FALSE : TRUE;
            break;
         case 6:
            Data->EchoMail = (Data->EchoMail == TRUE) ? FALSE : TRUE;
            break;
         case 7:
            Data->Offline = (Data->Offline == TRUE) ? FALSE : TRUE;
            break;
         case 8:
            Data->UpdateNews = (Data->UpdateNews == TRUE) ? FALSE : TRUE;
            break;
         case 9:
            GetTextField (6, 13, Data->EchoTag, sizeof (Data->EchoTag), 54);
            break;
         case 10:
            GetTextField (7, 13, Data->NewsGroup, sizeof (Data->NewsGroup), 54);
            break;
         case 11:
            GetTextField (9, 13, Data->Address, sizeof (Data->Address), 32);
            break;
         case 12:
            sprintf (Temp, "%u", Data->MaxMessages);
            GetTextField (10, 13, Temp, 5, 5);
            Data->MaxMessages = (USHORT)atoi (Temp);
            break;
         case 13:
            sprintf (Temp, "%u", Data->DaysOld);
            GetTextField (11, 13, Temp, 5, 5);
            Data->DaysOld = (USHORT)atoi (Temp);
            break;
         case 14:
            sprintf (Temp, "%u", Data->Board);
            GetTextField (2, 45, Temp, 5, 5);
            Data->Board = (USHORT)atoi (Temp);
            break;
         case 15:
            sprintf (Temp, "%lu", Data->Highest);
            GetTextField (8, 13, Temp, 7, 7);
            Data->Highest = atol (Temp);
            break;
         case 115:
         case 116: {
            CHAR Key[16], Description[64];

            Key[0] = Description[0] = '\0';
            if (CAddMessageDlg (Key, Description) == TRUE) {
               Data->New ();
               strcpy (Data->Key, Key);
               strcpy (Data->Display, Description);
               Data->Storage = Cfg->NewAreasStorage;
               strcpy (Data->Path, Cfg->NewAreasPath);
               strcat (Data->Path, Data->Key);
               Data->Level = Cfg->NewAreasLevel;
               Data->AccessFlags = Cfg->NewAreasFlags;
               Data->DenyFlags = Cfg->NewAreasDenyFlags;
               Data->WriteLevel = Cfg->NewAreasWriteLevel;
               Data->WriteFlags = Cfg->NewAreasWriteFlags;
               Data->DenyWriteFlags = Cfg->NewAreasDenyWriteFlags;
               Data->MaxMessages = 200;
               Data->DaysOld = 14;
               if (menu_sel == 115)
                  Data->Add ();
               else if (menu_sel == 116)
                  Data->Insert ();
            }
            break;
         }
         case 117: {
            CHAR Key[16];

            strcpy (Key, Data->Key);
            if (CMessageListDlg (Key, NULL) == TRUE)
               Data->Read (Key, FALSE);
            break;
         }
         case 118:
            Data->Previous ();
            break;
         case 119:
            Data->Next ();
            break;
         case 120:
            if (MessageBox ("Delete Area", "Are you really sure ?") == TRUE)
               Data->Delete ();
            break;
         case 121: {
            class TEchoLink *Link;

            if ((Link = new TEchoLink (Cfg->SystemPath)) != NULL) {
               Link->Load (Data->EchoTag);
               if (CMessageLinksDlg (Link) == TRUE)
                  Link->Save ();
               delete Link;
            }
            break;
         }
         case 122: {
            CHAR Key[16], Search[48];

            if (wopen (10, 19, 12, 60, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
               wshadow (DGREY|_BLACK);
               wtitle (" Search Area ", TCENTER, WHITE|_LGREY);

               wprints (0, 1, WHITE|_GREEN, " String ");
               Search[0] = '\0';
               winpbeg (WHITE|_BLUE, WHITE|_BLUE);
               winpdef (0, 10, Search, "???????????????????????????", 0, 2, NULL, 0);
               if (winpread () != W_ESCPRESS && Search[0] != '\0') {
                  hidecur ();
                  wclose ();

                  strtrim (Search);
                  strcpy (Key, Data->Key);
                  if (CMessageListDlg (Key, Search) == TRUE)
                     Data->Read (Key, FALSE);
               }
               else {
                  hidecur ();
                  wclose ();
               }
            }
            break;
         }
         case 123:
            CMessageSecurityDlg (Data);
            break;
         case 124: {
            CHAR Key[16];
            class TMsgData *NewData;

            strcpy (Key, Data->Key);
            if (CMessageListDlg (Key, NULL) == TRUE) {
               if ((NewData = new TMsgData (Cfg->SystemPath)) != NULL) {
                  NewData->Read (Data->Key);
                  NewData->Delete ();
                  NewData->Read (Key);
                  NewData->Insert (Data);
                  delete NewData;
               }
            }
            if (Data->Read (Data->Key, FALSE) == FALSE)
               Data->Read (Key, FALSE);
            break;
         }
         case 996:
            Data->Update ();
            break;
      }
   } while (menu_sel != -1);

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

USHORT COfflineDlg (VOID)
{
   short menu_sel = 996;
   CHAR Temp[16];
   USHORT RetVal = FALSE;

   wopen (9, 5, 16, 74, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Offline Reader ", TCENTER, WHITE|_LGREY);

   DisplayButton (4, 2, "   Ok   ");
   DisplayButton (4, 12, " Cancel ");
   DisplayButton (4, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Work Directory ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Packet Name    ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Max Messages   ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (4, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (4, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (4, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 18, Cfg->TempPath, sizeof (Cfg->TempPath), 49);
      DisplayTextField (1, 18, Cfg->OLRPacketName, sizeof (Cfg->OLRPacketName), 49);
      sprintf (Temp, "%u", Cfg->OLRMaxMessages);
      DisplayTextField (2, 18, Temp, 5, 5);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 18, Cfg->TempPath, sizeof (Cfg->TempPath), 49);
            break;
         case 2:
            GetTextField (1, 18, Cfg->OLRPacketName, sizeof (Cfg->OLRPacketName), 49);
            break;
         case 3:
            sprintf (Temp, "%u", Cfg->OLRMaxMessages);
            GetTextField (2, 18, Temp, 5, 5);
            Cfg->OLRMaxMessages = (USHORT)atoi (Temp);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

// ----------------------------------------------------------------------

USHORT CAddUserDlg (PSZ Name, PSZ Pwd)
{
   short menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (9, 15, 15, 64, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Add User ", TCENTER, WHITE|_LGREY);

   DisplayButton (3, 2, "   Ok   ");
   DisplayButton (3, 12, " Cancel ");
   DisplayButton (3, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Name     ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Password ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (3, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (3, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (3, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 12, Name, 35, 35);
      DisplayTextField (1, 12, "", 15, 15);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 12, Name, 35, 35);
            break;
         case 2:
            winpbeg (WHITE|_BLUE, WHITE|_BLUE);
            winpdef (1, 12, Pwd, "PPPPPPPPPPPPPPP", 0, 2, NULL, 0);
            if (winpread () == W_ESCPRESS)
               Pwd[0] = '\0';
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CUserListDlg (PSZ Name, PSZ Search)
{
   short i, start = 0;
   CHAR Temp[128], Call[32], *p, **Array;
   USHORT RetVal = FALSE, DoAdd = TRUE;
   struct tm *ltm;
   class TUser *Data;
   class TCollection List;

   wopen (7, 2, 23, 77, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" User List ", TCENTER, WHITE|_LGREY);

   sprintf (Temp, " %-27.27s %-5s %-10.10s %-16.16s %s ", "Name", "Level", "Limit", "City", "Last Call");
   wprints (0, 0, YELLOW|_LGREY, Temp);

   if (Search != NULL)
      strupr (Search);

   Data = new TUser (Cfg->UserFile);
   if (Data->First () == TRUE) {
      i = 0;
      List.Clear ();
      do {
         if (Search != NULL) {
            DoAdd = FALSE;
            strcpy (Temp, Data->Name);
            if (strstr (strupr (Temp), Search) != NULL)
               DoAdd = TRUE;
            else {
               strcpy (Temp, Data->RealName);
               if (strstr (strupr (Temp), Search) != NULL)
                  DoAdd = TRUE;
               else {
                  strcpy (Temp, Data->City);
                  if (strstr (strupr (Temp), Search) != NULL)
                     DoAdd = TRUE;
               }
            }
         }
         if (DoAdd == TRUE) {
            if (Data->LastCall == 0L)
               strcpy (Call, "Never");
            else {
               ltm = localtime ((time_t *)&Data->LastCall);
               sprintf (Call, "%02d/%02d/%04d", ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900, ltm->tm_hour, ltm->tm_min);
            }
            sprintf (Temp, " %-27.27s %5u %-10.10s %-16.16s %s ", Data->Name, Data->Level, Data->LimitClass, Data->City, Call);
            List.Add (Temp);
            if (!strcmp (Data->Name, Name))
               start = i;
            i++;
         }
      } while (Data->Next () == TRUE);

      if (List.Elements > 0) {
         i = 0;
         Array = (CHAR **)malloc ((List.Elements + 1) * sizeof (CHAR *));
         if ((p = (CHAR *)List.First ()) != NULL)
            do {
               Array[i++] = p;
            } while ((p = (CHAR *)List.Next ()) != NULL);
         Array[i] = NULL;
         if ((i = wpickstr (9, 3, 22, 76, 5, WHITE|_LGREY, WHITE|_LGREY, WHITE|_BLUE, Array, start, NULL)) != -1) {
            p = Array[i] + 1;
            p[27] = '\0';
            strcpy (Name, strtrim (p));
            RetVal = TRUE;
         }
         if (Array != NULL)
            free (Array);
      }
      else
         getxch ();
   }
   else
      getxch ();

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

USHORT CUserMoreDlg (class TUser *Data)
{
   short menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (8, 8, 16, 70, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" More User Options ", TCENTER, WHITE|_LGREY);

   DisplayButton (5, 2, "   Ok   ");
   DisplayButton (5, 12, " Cancel ");
   DisplayButton (5, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " E-Mail Address     ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Import E-Mail      ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " POP3 Password      ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Use E-Mail Address ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (5, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (5, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (5, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 22, Data->InetAddress, sizeof (Data->InetAddress), 38);
      wprints (1, 22, WHITE|_BLUE, (Data->ImportPOP3Mail == TRUE) ? "Yes" : "No ");
      DisplayTextField (2, 22, Data->Pop3Pwd, sizeof (Data->Pop3Pwd), 36);
      wprints (3, 22, WHITE|_BLUE, (Data->UseInetAddress == TRUE) ? "Yes" : "No ");

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 22, Data->InetAddress, sizeof (Data->InetAddress), 38);
            break;
         case 2:
            Data->ImportPOP3Mail = (Data->ImportPOP3Mail == TRUE) ? FALSE : TRUE;
            break;
         case 3:
            GetTextField (2, 22, Data->Pop3Pwd, sizeof (Data->Pop3Pwd), 36);
            break;
         case 4:
            Data->UseInetAddress = (Data->UseInetAddress == TRUE) ? FALSE : TRUE;
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CUserDlg (VOID)
{
   short menu_sel = 1;
   CHAR Temp[48];
   USHORT RetVal = FALSE;
   class TUser *Data;

   Data = new TUser (Cfg->UserFile);
   if (Data->First () == FALSE)
      Data->Clear ();

   wopen (5, 5, 20, 74, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" User Editor ", TCENTER, WHITE|_LGREY);

   DisplayButton (10, 12, "  More  ");
   DisplayButton (10, 22, "  Pwd.  ");
   DisplayButton (10, 32, " Search ");
   DisplayButton (10, 47, "    Security     ");

   DisplayButton (12,  2, " Change ");
   DisplayButton (12, 12, "  Add   ");
   DisplayButton (12, 22, " Delete ");
   DisplayButton (12, 32, "  List  ");
   DisplayButton (12, 47, "  Prev. ");
   DisplayButton (12, 57, "  Next  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Name        ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Real Name   ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Company     ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Address     ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " City        ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Phone       ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " Limit Class ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (7, 1, " MailBox     ", 0, 8, 0, NULL, 0, 0);
      wmenuitem (8, 1, " Language    ", 0, 9, 0, NULL, 0, 0);

      wmenuitem (0, 51, " Gender   ", 0, 10, 0, NULL, 0, 0);
      wmenuitem (1, 51, " Width    ", 0, 11, 0, NULL, 0, 0);
      wmenuitem (2, 51, " Height   ", 0, 12, 0, NULL, 0, 0);
      wmenuitem (3, 51, " Calls    ", 0, 13, 0, NULL, 0, 0);
      wmenuitem (5, 51, " ANSI     ", 0, 14, 0, NULL, 0, 0);
      wmenuitem (6, 51, " Avatar   ", 0, 15, 0, NULL, 0, 0);
      wmenuitem (7, 51, " Colors   ", 0, 16, 0, NULL, 0, 0);
      wmenuitem (8, 51, " Hot-keys ", 0, 17, 0, NULL, 0, 0);

      wmenuitem (10, 12, "  More  ", 'M', 120, 0, NULL, 0, 0);
      wmenuitem (10, 22, "  Pwd.  ", 'w', 121, 0, NULL, 0, 0);
      wmenuitem (10, 32, " Search ", 'r', 122, 0, NULL, 0, 0);
      wmenuitem (10, 47, "    Security      ", 'S', 123, 0, NULL, 0, 0);

      wmenuitem (12,  2, " Change ", 'C', 996, 0, NULL, 0, 0);
      wmenuitem (12, 12, "  Add   ", 'A', 115, 0, NULL, 0, 0);
      wmenuitem (12, 22, " Delete ", 'D', 116, 0, NULL, 0, 0);
      wmenuitem (12, 32, "  List  ", 'L', 117, 0, NULL, 0, 0);
      wmenuitem (12, 47, "  Prev. ", 'P', 118, 0, NULL, 0, 0);
      wmenuitem (12, 57, "  Next  ", 'N', 119, 0, NULL, 0, 0);

      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 15, Data->Name, sizeof (Data->Name), 35);
      DisplayTextField (1, 15, Data->RealName, sizeof (Data->RealName), 35);
      DisplayTextField (2, 15, Data->Company, sizeof (Data->Company), 35);
      DisplayTextField (3, 15, Data->Address, sizeof (Data->Address), 35);
      DisplayTextField (4, 15, Data->City, sizeof (Data->City), 35);
      DisplayTextField (5, 15, Data->DayPhone, sizeof (Data->DayPhone), 35);
      DisplayTextField (6, 15, Data->LimitClass, sizeof (Data->LimitClass), 35);
      DisplayTextField (7, 15, Data->MailBox, sizeof (Data->MailBox), 35);
      DisplayTextField (8, 15, Data->Language, sizeof (Data->Language), 35);

      wprints (0, 62, WHITE|_BLUE, (Data->Sex == 0) ? "M" : "F");
      sprintf (Temp, "%u", Data->ScreenWidth);
      DisplayTextField (1, 62, Temp, 3, 3);
      sprintf (Temp, "%u", Data->ScreenHeight);
      DisplayTextField (2, 62, Temp, 3, 3);
      sprintf (Temp, "%lu", Data->TotalCalls);
      DisplayTextField (3, 62, Temp, 5, 5);
      wprints (5, 62, WHITE|_BLUE, (Data->Ansi == TRUE) ? "Yes" : "No ");
      wprints (6, 62, WHITE|_BLUE, (Data->Avatar == TRUE) ? "Yes" : "No ");
      wprints (7, 62, WHITE|_BLUE, (Data->Color == TRUE) ? "Yes" : "No ");
      wprints (8, 62, WHITE|_BLUE, (Data->HotKey == TRUE) ? "Yes" : "No ");

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 15, Data->Name, sizeof (Data->Name), 35);
            break;
         case 2:
            GetTextField (1, 15, Data->RealName, sizeof (Data->RealName), 35);
            break;
         case 3:
            GetTextField (2, 15, Data->Company, sizeof (Data->Company), 35);
            break;
         case 4:
            GetTextField (3, 15, Data->Address, sizeof (Data->Address), 35);
            break;
         case 5:
            GetTextField (4, 15, Data->City, sizeof (Data->City), 35);
            break;
         case 6:
            GetTextField (5, 15, Data->DayPhone, sizeof (Data->DayPhone), 35);
            break;
         case 7:
            GetTextField (6, 15, Data->LimitClass, sizeof (Data->LimitClass), 35);
            break;
         case 8:
            GetTextField (7, 15, Data->MailBox, sizeof (Data->MailBox), 35);
            break;
         case 9:
            GetTextField (8, 15, Data->Language, sizeof (Data->Language), 35);
            break;
         case 10:
            Data->Sex = (Data->Sex == 0) ? 1 : 0;
            break;
         case 11:
            sprintf (Temp, "%u", Data->ScreenWidth);
            GetTextField (1, 62, Temp, 3, 3);
            Data->ScreenWidth = (USHORT)atoi (Temp);
            break;
         case 12:
            sprintf (Temp, "%u", Data->ScreenHeight);
            GetTextField (2, 62, Temp, 3, 3);
            Data->ScreenHeight = (USHORT)atoi (Temp);
            break;
         case 13:
            sprintf (Temp, "%lu", Data->TotalCalls);
            GetTextField (3, 62, Temp, 5, 5);
            Data->TotalCalls = atol (Temp);
            break;
         case 14:
            Data->Ansi = (Data->Ansi == TRUE) ? FALSE : TRUE;
            break;
         case 15:
            Data->Avatar = (Data->Avatar == TRUE) ? FALSE : TRUE;
            break;
         case 16:
            Data->Color = (Data->Color == TRUE) ? FALSE : TRUE;
            break;
         case 17:
            Data->HotKey = (Data->HotKey == TRUE) ? FALSE : TRUE;
            break;
         case 115: {
            CHAR Name[48], Pwd[32];

            Name[0] = Pwd[0] = '\0';
            if (CAddUserDlg (Name, Pwd) == TRUE) {
               Data->Clear ();
               Data->ScreenWidth = 80;
               Data->ScreenHeight = 24;
               strcpy (Data->Name, Name);
               strcpy (Data->RealName, Name);
               Data->SetPassword (Pwd);
               Data->Level = Cfg->NewUserLevel;
               Data->AccessFlags = Cfg->NewUserFlags;
               Data->DenyFlags = Cfg->NewUserDenyFlags;
               strcpy (Data->LimitClass, Cfg->NewUserLimits);
               Data->Add ();
            }
            break;
         }
         case 116:
            if (MessageBox ("Delete User", "Are you really sure ?") == TRUE)
               Data->Delete ();
            break;
         case 117: {
            CHAR Name[48];

            strcpy (Name, Data->Name);
            if (CUserListDlg (Name, NULL) == TRUE)
               Data->GetData (Name, FALSE);
            break;
         }
         case 118:
            Data->Previous ();
            break;
         case 119:
            Data->Next ();
            break;
         case 120:
            CUserMoreDlg (Data);
            break;
         case 121:
            if (wopen (10, 25, 12, 54, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
               wshadow (DGREY|_BLACK);
               wtitle (" Set Password ", TCENTER, WHITE|_LGREY);

               wprints (0, 1, WHITE|_GREEN, " Password ");
               Temp[0] = '\0';
               winpbeg (WHITE|_BLUE, WHITE|_BLUE);
               winpdef (0, 12, Temp, "PPPPPPPPPPPPPPP", 0, 2, NULL, 0);
               if (winpread () != W_ESCPRESS && Temp[0] != '\0')
                  Data->SetPassword (Temp);
               hidecur ();
               wclose ();
            }
            break;
         case 122: {
            CHAR Name[48], Search[48];

            if (wopen (10, 19, 12, 60, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
               wshadow (DGREY|_BLACK);
               wtitle (" Search User ", TCENTER, WHITE|_LGREY);

               wprints (0, 1, WHITE|_GREEN, " String ");
               Search[0] = '\0';
               winpbeg (WHITE|_BLUE, WHITE|_BLUE);
               winpdef (0, 10, Search, "???????????????????????????", 0, 2, NULL, 0);
               if (winpread () != W_ESCPRESS && Search[0] != '\0') {
                  hidecur ();
                  wclose ();

                  strtrim (Search);
                  strcpy (Name, Data->Name);
                  if (CUserListDlg (Name, Search) == TRUE)
                     Data->GetData (Name, FALSE);
               }
               else {
                  hidecur ();
                  wclose ();
               }
            }
         }
         case 123:
            CSecurityDlg (" User Security ", &Data->Level, &Data->AccessFlags, &Data->DenyFlags);
            break;
         case 996:
            Data->Update ();
            break;
      }
   } while (menu_sel != -1);

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

VOID DisplayYesNo (USHORT y, USHORT x, USHORT flag, USHORT Required = FALSE)
{
   if (Required == FALSE) {
      if (flag == YES)
         wprints (y, x, WHITE|_BLUE, "Yes");
      else if (flag == NO)
         wprints (y, x, WHITE|_BLUE, "No ");
   }
   else {
      if (flag == YES)
         wprints (y, x, WHITE|_BLUE, "Yes     ");
      else if (flag == NO)
         wprints (y, x, WHITE|_BLUE, "No      ");
      else if (flag == REQUIRED)
         wprints (y, x, WHITE|_BLUE, "Required");
   }
}

VOID DisplayChoice1 (USHORT y, USHORT x, USHORT flag)
{
   if (flag == 1)
      wprints (y, x, WHITE|_BLUE, "Yes");
   else if (flag == 0)
      wprints (y, x, WHITE|_BLUE, "No ");
   else if (flag == 2)
      wprints (y, x, WHITE|_BLUE, "Ask");
}

VOID DisplayChoice2 (USHORT y, USHORT x, USHORT flag)
{
   if (flag == 1)
      wprints (y, x, WHITE|_BLUE, "Yes   ");
   else if (flag == 0)
      wprints (y, x, WHITE|_BLUE, "No    ");
   else if (flag == 2)
      wprints (y, x, WHITE|_BLUE, "Ask   ");
   else if (flag == 3)
      wprints (y, x, WHITE|_BLUE, "Detect");
}

VOID DisplayChoice3 (USHORT y, USHORT x, USHORT flag)
{
   if (flag == YES)
      wprints (y, x, WHITE|_BLUE, "Yes     ");
   else if (flag == NO)
      wprints (y, x, WHITE|_BLUE, "No      ");
   else if (flag == REQUIRED)
      wprints (y, x, WHITE|_BLUE, "Required");
}

VOID LimitClassShadow (VOID)
{
   wtitle (" Limit Classes ", TCENTER, WHITE|_CYAN);
   wshadow (DGREY|_BLACK);
}

USHORT CNewUsersDlg (VOID)
{
   short i, menu_sel = 1, maxlen, start;
   CHAR Temp[64], **Array, *p;
   USHORT RetVal = FALSE;
   class TCollection List;
   class TLimits *Limits;

   wopen (4, 9, 21, 70, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" New Users ", TCENTER, WHITE|_LGREY);

   DisplayButton (14,  2, "   Ok   ");
   DisplayButton (14, 12, " Cancel ");
   DisplayButton (14, 22, "  Help  ");

   DisplayButton (14, 40, "    Security     ");

   do {
      wmenubegc ();

      wmenuitem ( 0,  1, " Use ANSI            ", 0, 1, 0, NULL, 0, 0);
      wmenuitem ( 1,  1, " Use AVATAR          ", 0, 2, 0, NULL, 0, 0);
      wmenuitem ( 2,  1, " Use colors          ", 0, 3, 0, NULL, 0, 0);
      wmenuitem ( 3,  1, " Use fullscr. editor ", 0, 4, 0, NULL, 0, 0);
      wmenuitem ( 4,  1, " Use fullscr. reader ", 0, 5, 0, NULL, 0, 0);
      wmenuitem ( 5,  1, " Use fullscr. lists  ", 0, 6, 0, NULL, 0, 0);
      wmenuitem ( 6,  1, " Ask hotkey          ", 0, 7, 0, NULL, 0, 0);
      wmenuitem ( 7,  1, " Ask IBM chars.      ", 0, 8, 0, NULL, 0, 0);
      wmenuitem ( 8,  1, " Ask lines           ", 0, 9, 0, NULL, 0, 0);
      wmenuitem ( 9,  1, " Page pause          ", 0, 10, 0, NULL, 0, 0);
      wmenuitem (10,  1, " Ask scrn. clear     ", 0, 11, 0, NULL, 0, 0);

      wmenuitem ( 2, 27, " Ask birthdate       ", 0, 12, 0, NULL, 0, 0);
      wmenuitem ( 3, 27, " New mail check      ", 0, 13, 0, NULL, 0, 0);
      wmenuitem ( 4, 27, " New files check     ", 0, 14, 0, NULL, 0, 0);
      wmenuitem ( 5, 27, " Ask real name       ", 0, 15, 0, NULL, 0, 0);
      wmenuitem ( 6, 27, " Company name        ", 0, 16, 0, NULL, 0, 0);
      wmenuitem ( 7, 27, " Address             ", 0, 17, 0, NULL, 0, 0);
      wmenuitem ( 8, 27, " City                ", 0, 18, 0, NULL, 0, 0);
      wmenuitem ( 9, 27, " Phone number        ", 0, 19, 0, NULL, 0, 0);
      wmenuitem (10, 27, " Gender              ", 0, 20, 0, NULL, 0, 0);

      wmenuitem (12, 1, " Limit Class     ", 0, 21, 0, NULL, 0, 0);
      wmenuitem (14, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (14, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (14, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuitem (14, 40, "    Security      ", 'S', 123, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayChoice2 (0, 23, Cfg->UseAnsi);
      DisplayChoice2 (1, 23, Cfg->UseAvatar);
      DisplayChoice1 (2, 23, Cfg->UseColor);
      DisplayChoice1 (3, 23, Cfg->UseFullScreenEditor);
      DisplayChoice1 (4, 23, Cfg->UseFullScreenReader);
      DisplayChoice1 (5, 23, Cfg->UseFullScreenLists);
      DisplayChoice1 (6, 23, Cfg->UseHotKey);
      DisplayChoice1 (7, 23, Cfg->UseIBMChars);
      DisplayChoice1 (8, 23, Cfg->AskLines);
      DisplayChoice1 (9, 23, Cfg->UsePause);
      DisplayChoice1 (10, 23, Cfg->UseScreenClear);

      DisplayChoice1 (2, 49, Cfg->AskBirthDate);
      DisplayChoice1 (3, 49, Cfg->AskMailCheck);
      DisplayChoice1 (4, 49, Cfg->AskFileCheck);
      DisplayChoice3 (5, 49, Cfg->AskAlias);
      DisplayChoice3 (6, 49, Cfg->AskCompanyName);
      DisplayChoice3 (7, 49, Cfg->AskAddress);
      DisplayChoice3 (8, 49, Cfg->AskCity);
      DisplayChoice3 (9, 49, Cfg->AskPhoneNumber);
      DisplayChoice3 (10, 49, Cfg->AskGender);

      sprintf (Temp, "%-16.16s", Cfg->NewUserLimits);
      wprints (12, 23, WHITE|_BLUE, Temp);

      switch (menu_sel = wmenuget ()) {
         case 1:
            Cfg->UseAnsi = (UCHAR)((Cfg->UseAnsi + 1) % 4);
            break;
         case 2:
            Cfg->UseAvatar = (UCHAR)((Cfg->UseAvatar + 1) % 4);
            break;
         case 3:
            Cfg->UseColor = (UCHAR)((Cfg->UseColor + 1) % 3);
            break;
         case 4:
            Cfg->UseFullScreenEditor = (UCHAR)((Cfg->UseFullScreenEditor + 1) % 3);
            break;
         case 5:
            Cfg->UseFullScreenReader = (UCHAR)((Cfg->UseFullScreenReader + 1) % 3);
            break;
         case 6:
            Cfg->UseFullScreenLists = (UCHAR)((Cfg->UseFullScreenLists + 1) % 3);
            break;
         case 7:
            Cfg->UseHotKey = (UCHAR)((Cfg->UseHotKey + 1) % 3);
            break;
         case 8:
            Cfg->UseIBMChars = (UCHAR)((Cfg->UseIBMChars + 1) % 3);
            break;
         case 9:
            Cfg->AskLines = (Cfg->AskLines == TRUE) ? FALSE : TRUE;
            break;
         case 10:
            Cfg->UsePause = (UCHAR)((Cfg->UsePause + 1) % 3);
            break;
         case 11:
            Cfg->UseScreenClear = (UCHAR)((Cfg->UseScreenClear + 1) % 3);
            break;
         case 12:
            Cfg->AskBirthDate = (Cfg->AskBirthDate == TRUE) ? FALSE : TRUE;
            break;
         case 13:
            Cfg->AskMailCheck = (UCHAR)((Cfg->AskMailCheck + 1) % 3);
            break;
         case 14:
            Cfg->AskFileCheck = (UCHAR)((Cfg->AskFileCheck + 1) % 3);
            break;
         case 15:
            Cfg->AskAlias = (UCHAR)((Cfg->AskAlias + 1) % 3);
            break;
         case 16:
            Cfg->AskCompanyName = (UCHAR)((Cfg->AskCompanyName + 1) % 3);
            break;
         case 17:
            Cfg->AskAddress = (UCHAR)((Cfg->AskAddress + 1) % 3);
            break;
         case 18:
            Cfg->AskCity = (UCHAR)((Cfg->AskCity + 1) % 3);
            break;
         case 19:
            Cfg->AskPhoneNumber = (UCHAR)((Cfg->AskPhoneNumber + 1) % 3);
            break;
         case 20:
            Cfg->AskGender = (UCHAR)((Cfg->AskGender + 1) % 3);
            break;
         case 21: {
            List.Clear ();
            start = maxlen = i = 0;
            if ((Limits = new TLimits (Cfg->SystemPath)) != NULL) {
               if (Limits->First () == TRUE)
                  do {
                     sprintf (Temp, " %-16.16s ", Limits->Key);
                     List.Add (Temp);
                     maxlen = (short)strlen (Temp);
                     if (!strcmp (Limits->Key, Cfg->NewUserLimits))
                        start = i;
                     i++;
                  } while (Limits->Next () == TRUE);
               delete Limits;
            }
            if (List.Elements > 0) {
               i = 0;
               Array = (CHAR **)malloc ((List.Elements + 1) * sizeof (CHAR *));
               if ((p = (CHAR *)List.First ()) != NULL)
                  do {
                     Array[i++] = p;
                  } while ((p = (CHAR *)List.Next ()) != NULL);
               Array[i] = NULL;
               if ((i = wpickstr (7, 40, 19, (short)(40 + maxlen + 1), 1, WHITE|_CYAN, BLACK|_CYAN, WHITE|_GREEN, Array, start, LimitClassShadow)) != -1) {
                  if ((p = strtok (Array[i], " ")) != NULL)
                     strcpy (Cfg->NewUserLimits, p);
               }
               if (Array != NULL)
                  free (Array);
            }
            break;
         }
         case 123:
            CSecurityDlg (" New Users Security ", &Cfg->NewUserLevel, &Cfg->NewUserFlags, &Cfg->NewUserDenyFlags);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CSelectNodeFlagDlg (class TNodeFlags *Data)
{
   short i, a, start = 0;
   CHAR Temp[128], *p, **Array;
   USHORT RetVal = FALSE;
   class TCollection List;

   if (Data->First () == TRUE) {
      i = 1;
      List.Clear ();
      do {
         sprintf (Temp, "%-9.9s ³ %-25.25s", Data->Flags, Data->Cmd);
         List.Add (Temp);
         i++;
      } while (Data->Next () == TRUE);

      if (List.Elements > 0) {
         i = 0;
         Array = (CHAR **)malloc ((List.Elements + 1) * sizeof (CHAR *));
         if ((p = (CHAR *)List.First ()) != NULL)
            do {
               Array[i++] = p;
            } while ((p = (CHAR *)List.Next ()) != NULL);
         Array[i] = NULL;
         if ((i += 4) > 15)
            i = 15;
         if ((i = wpickstr (5, 15, i, 52, 5, BLACK|_CYAN, BLACK|_CYAN, YELLOW|_CYAN, Array, start, NULL)) != -1) {
            a = 0;
            if (Data->First () == TRUE)
               do {
                  if (a++ == i)
                     break;
               } while (Data->Next () == TRUE);
            RetVal = TRUE;
         }
         if (Array != NULL)
            free (Array);
      }
      else
         getxch ();
   }
   else
      getxch ();

   return (RetVal);
}

VOID CNodeFlagsDlg (VOID)
{
   short i, menu_sel = 996, sub_menu_sel;
   CHAR Temp[64], TempCmd[64];
   class TNodeFlags *Data;

   Data = new TNodeFlags (Cfg->SystemPath);

   wopen (3, 12, 21, 66, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Nodelist flags ", TCENTER, WHITE|_LGREY);

   DisplayButton (1, 42, "  Add   ");
   DisplayButton (3, 42, " Remove ");
   DisplayButton (5, 42, " Modify ");

   DisplayButton (15, 2, "   Ok   ");
   DisplayButton (15, 12, " Cancel ");
   DisplayButton (15, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (1, 42, "  Add   ", 'A', 102, 0, NULL, 0, 0);
      wmenuitem (3, 42, " Remove ", 'R', 103, 0, NULL, 0, 0);
      wmenuitem (5, 42, " Modify ", 'M', 104, 0, NULL, 0, 0);

      wmenuitem (12, 1, " Call if        ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (13, 1, " Don't call if  ", 0, 2, 0, NULL, 0, 0);

      wmenuitem (15, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (15, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (15, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      sprintf (Temp, " %-9.9s ³ %-25.25s ", "", "");
      for (i = 0; i <= 9; i++)
         wprints ((short)(i + 1), 1, BLACK|_CYAN, Temp);

      sprintf (Temp, " %-9.9s ³ %-25.25s ", "Flags", "Command");
      wprints (0, 1, BLACK|_GREEN, Temp);

      i = 0;
      if (Data->First () == TRUE)
         do {
            if (i <= 9) {
               sprintf (Temp, " %-9.9s ³ %-25.25s ", Data->Flags, Data->Cmd);
               wprints ((short)(i + 1), 1, BLACK|_CYAN, Temp);
               i++;
            }
         } while (Data->Next () == TRUE);

      DisplayTextField (12, 18, Cfg->CallIf, 25, 25);
      DisplayTextField (13, 18, Cfg->DontCallIf, 25, 25);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (12, 18, Cfg->CallIf, 25, 25);
            break;
         case 2:
            GetTextField (13, 18, Cfg->DontCallIf, 25, 25);
            break;
         case 102:
            if (wopen (9, 18, 15, 58, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
               wshadow (DGREY|_BLACK);
               wtitle (" Add Flag ", TCENTER, WHITE|_LGREY);

               DisplayButton (3, 2, "   Ok   ");
               DisplayButton (3, 12, " Cancel ");

               sub_menu_sel = 996;
               Temp[0] = TempCmd[0] = '\0';

               do {
                  wmenubegc ();
                  wmenuitem (0, 1, " Flags    ", 0, 1, 0, NULL, 0, 0);
                  wmenuitem (1, 1, " Command  ", 0, 2, 0, NULL, 0, 0);

                  wmenuitem (3, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
                  wmenuitem (3, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
                  wmenuend ((short)sub_menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

                  DisplayTextField (0, 12, Temp, sizeof (Temp), 9);
                  DisplayTextField (1, 12, TempCmd, sizeof (TempCmd), 25);

                  switch (sub_menu_sel = wmenuget ()) {
                     case 1:
                        GetTextField (0, 12, Temp, sizeof (Temp), 9);
                        break;
                     case 2:
                        GetTextField (1, 12, TempCmd, sizeof (TempCmd), 25);
                        break;
                 }
               } while (sub_menu_sel != -1 && sub_menu_sel != 996 && sub_menu_sel != 997);

               if (sub_menu_sel == 996) {
                  strcpy (Data->Flags, Temp);
                  strcpy (Data->Cmd, TempCmd);
                  Data->Add ();
               }

               hidecur ();
               wclose ();
            }
            break;
         case 103:
            if (CSelectNodeFlagDlg (Data) == TRUE) {
               if (MessageBox ("Delete Item", "Are you really sure ?") == TRUE)
                  Data->Delete ();
            }
            break;
         case 104:
            if (CSelectNodeFlagDlg (Data) == TRUE) {
               if (wopen (9, 18, 15, 58, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
                  wshadow (DGREY|_BLACK);
                  wtitle (" Modify Flag ", TCENTER, WHITE|_LGREY);

                  DisplayButton (3, 2, "   Ok   ");
                  DisplayButton (3, 12, " Cancel ");

                  strcpy (Temp, Data->Flags);
                  strcpy (TempCmd, Data->Cmd);

                  do {
                     wmenubegc ();
                     wmenuitem (0, 1, " Flags    ", 0, 1, 0, NULL, 0, 0);
                     wmenuitem (1, 1, " Command  ", 0, 2, 0, NULL, 0, 0);

                     wmenuitem (3, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
                     wmenuitem (3, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
                     wmenuend ((short)sub_menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

                     DisplayTextField (0, 12, Temp, sizeof (Temp), 9);
                     DisplayTextField (1, 12, TempCmd, sizeof (TempCmd), 25);

                     switch (sub_menu_sel = wmenuget ()) {
                        case 1:
                           GetTextField (0, 12, Temp, sizeof (Temp), 9);
                           break;
                        case 2:
                           GetTextField (1, 12, TempCmd, sizeof (TempCmd), 25);
                           break;
                    }
                  } while (sub_menu_sel != -1 && sub_menu_sel != 996 && sub_menu_sel != 997);

                  if (sub_menu_sel == 996) {
                     strcpy (Data->Flags, Temp);
                     strcpy (Data->Cmd, TempCmd);
                     Data->Update ();
                  }

                  hidecur ();
                  wclose ();
               }
            }
            break;
         case 996:
            Data->Save ();
            break;
         case 997:
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   delete Data;

   wclose ();
}

