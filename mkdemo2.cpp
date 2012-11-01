
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.06
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_defs.h"
#include "config.h"
#include "files.h"
#include "forum.h"
#include "menu.h"
#include "msgdata.h"

void set_top_mnu (void)
{
   int fd;
   MENUHDR menuhdr;
   ITEM item;

   fd = open ("top.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "TOP");
   menuhdr.usItems = 11;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nMain System Menu (TOP)\n^Make your selection (T,I,F,E,C,L,A,D,O,? for help, or X to exit): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nPlease select one of the following:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^T^ ... Teleconference\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "T");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^I^ ... Information Center\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "I");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^E^ ... Electronic Mail\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "E");
   item.usCommand = MNU_GOEMAIL;
   strcpy (item.szArgument, "EMAIL");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^F^ ... Forums (Public Message Bases)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_GOMSG;
   strcpy (item.szArgument, "MSG");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^L^ ... File Libraries\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "L");
   item.usCommand = MNU_GOFILE;
   strcpy (item.szArgument, "FILE");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^A^ ... Account Display/Edit\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "A");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^S^ ... Internet Services\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "S");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "INTERNET");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... Doors\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "DOORS");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^O^ ... Off-Line Mail\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "O");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "OFFLINE");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^X^ ... Exit System (Logoff)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_LOGOFF;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);
}

void set_menu (void)
{
   int fd;
   MENUHDR menuhdr;
   ITEM item;

   printf ("Menu System\n");

   set_top_mnu ();

   fd = open ("msg.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "MSG");
   menuhdr.usItems = 11;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nConferences\n^Make your selection (R,W,T,S,M,E,P,A,C,O or X to exit): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nYour current Conference is : Y\nThere are 9 messages.\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^R^ ... Read Messages\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "R");
   item.usCommand = MNU_READMAIL;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^W^ ... Write a Messages\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "W");
   item.usCommand = MNU_WRITEMAIL;
   strcpy (item.szArgument, "EDITOR");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^L^ ... List messages\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "L");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "LISTING");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^T^ ... Teleconference\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "T");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^S^ ... Select a new Conference\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "S");
   item.usCommand = MNU_SELECTAREA;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^M^ ... Modify a Message\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "M");
   item.usCommand = MNU_MODIFYMAIL;
   strcpy (item.szArgument, "EDITOR");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^E^ ... Erase a Message\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "E");
   item.usCommand = MNU_DELETEMAIL;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^P^ ... Set personal preferences\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "P");
   item.usCommand = MNU_READPREFERENCE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^O^ ... Operations Menu\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "O");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // -----------------

   fd = open ("olrconf.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "OLRCONF");
   menuhdr.usItems = 4;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nOffline Reader\n^Make your selection (+,-,V, or X to exit): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^+^ ... Add a Forum to your offline reader\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "+");
   item.usCommand = MNU_QUICKADD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^-^ ... Remove a Forum from your offline reader\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "-");
   item.usCommand = MNU_QUICKREMOVE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^V^ ... View the Forums now in your offline reader\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "V");
   item.usCommand = MNU_QUICKDISPLAY;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // -----------------

   fd = open ("listing.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "LISTING");
   menuhdr.usItems = 9;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nSelect a letter from the above list, or ? for help: ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nThe listing modes available are\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^B^ ... Brief-title   (1 line per message)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "B");
   item.usCommand = MNU_LISTBRIEF;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "B");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^T^ ... Title         (4-line summary of each message)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "T");
   item.usCommand = MNU_LISTTITLE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "T");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^F^ ... Full-text     (summary plus full message text)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_LISTTEXT;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "|");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // -----------------

   fd = open ("file.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "FILE");
   menuhdr.usItems = 13;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nFile Libraries\n^Make your selection (R,W,F,T,S,M,E,A,C,O or X to exit): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nWelcome to the 2 Library\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "Description: Z\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "There are 8 files available for download.\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nPlease select one of the following:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^F^ ... Find Files\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "FILELIST");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... Download a File\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = MNU_DOWNLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^U^ ... Upload a File\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "U");
   item.usCommand = MNU_UPLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^T^ ... Manage tagged files (7 tagged)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "T");
   item.usCommand = MNU_MANAGETAGGED;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^L^ ... Download Lists of Files\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "L");
   item.usCommand = MNU_DOWNLOADLIST;
   strcpy (item.szArgument, "");
   item.usLevel = 10;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^S^ ... Select a new Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "S");
   item.usCommand = MNU_SELECTLIBRARY;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^P^ ... Set your Preferences\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "P");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^O^ ... Operations menu\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "O");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "LIBOP");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^X^ ... Exit File Libraries\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("libop.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "LIBOP");
   menuhdr.usItems = 14;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nLibrary Operations\n^Make your selection (C,D,E,L,J,M,I,A,U,R,V,F,S or X to exit): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nLibrary Operations Menu for the <???> Library.\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^C^ ... Create a Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "C");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... Delete a Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^E^ ... Edit this Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "E");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^L^ ... Add files to this Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "L");
   item.usCommand = MNU_ADDFILES;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^J^ ... Join/Unjoin other Libraries to this Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "J");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^M^ ... Library maintenance\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "M");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^I^ ... List Libraries with files awaiting approval\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "I");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^A^ ... Approve files (0 waiting)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "A");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^U^ ... Unapprove files\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "V");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^R^ ... Remove or rename files\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "R");
   item.usCommand = MNU_REMOVEFILES;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^V^ ... Move or copy files in this Library to another Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "V");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^F^ ... Edit file information\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_EDITFILE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^X^ ... Exit the operations menu\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("filelist.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "FILELIST");
   menuhdr.usItems = 17;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\n^Select an option, or X to exit: ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nList Files:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^F^ ... By file name, alphabetically\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_FILELIST;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... By date, most recently added to Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = MNU_SEARCHDATE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^Q^ ... Quickscan for new files\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "Q");
   item.usCommand = MNU_SEARCHNEW;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "Q");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^N^ ... By number of times the file has been downloaded\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "N");
   item.usCommand = MNU_SEARCHDOWNLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "N");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nSearch Files:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^W^ ... By file name, or partial file name (wildcards)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "W");
   item.usCommand = MNU_SEARCHNAME;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "W");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^K^ ... Keyword search\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "K");
   item.usCommand = MNU_SEARCHKEYWORD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "K");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^S^ ... Keyword search through file names and descriptions\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "S");
   item.usCommand = MNU_SEARCHTEXT;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "S");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("email.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "EMAIL");
   menuhdr.usItems = 6;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nE-Mail\n^Make your selection (R,W,E,P, or X to exit): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nThe following E-Mail services are available:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^R^ ... Read Message(s)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "R");
   item.usCommand = MNU_READMAIL;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^W^ ... Write a Message\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "W");
   item.usCommand = MNU_WRITEMAIL;
   strcpy (item.szArgument, "EDITOR");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^E^ ... Erase a Message\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "E");
   item.usCommand = MNU_DELETEMAIL;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^P^ ... Set personal preferences\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "P");
   item.usCommand = MNU_READPREFERENCE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^X^ ... Exit E-Mail\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("editor.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "EDITOR");
   menuhdr.usItems = 13;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nEditor\n^Make your selection (S,R,A,D,L,I,C,N,H,B,U or X to exit): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nEDITOR OPTIONS:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^S^ ... Save message  ");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "S");
   item.usCommand = MNU_SAVEMSG;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^R^ ... Retype a line\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "R");
   item.usCommand = MNU_RETYPELINE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^A^ ... Append message");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "A");
   item.usCommand = MNU_APPENDMSG;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... Delete line\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = MNU_DELETELINE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^L^ ... List message  ");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "L");
   item.usCommand = MNU_LISTMSG;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^I^ ... Insert line(s)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "I");
   item.usCommand = MNU_INSERTLINE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^C^ ... Change text   ");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "C");
   item.usCommand = MNU_CHANGETEXT;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^N^ ... New message\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "N");
   item.usCommand = MNU_NEWMESSAGE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^H^ ... Help          ");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "H");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^B^ ... Change subject\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "B");
   item.usCommand = MNU_CHANGESUBJ;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^U^ ... Upload file\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "U");
   item.usCommand = MNU_UPLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("offline.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "OFFLINE");
   menuhdr.usItems = 8;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nOffline Mail (QWK/BlueWave)\n^Select an option (Q,B,A,U,R,C,? for help or X to exit): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nWelcome to Offline-mail!\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^Q^ ... Download ^QWK^ packet\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "Q");
   item.usCommand = MNU_QUICKDOWNLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^B^ ... Download ^BlueWave^ packet\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "B");
   item.usCommand = MNU_BWDOWNLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^A^ ... Download ^ASCII^ packet\n\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "A");
   item.usCommand = MNU_ASCIIDOWNLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^U^ ... Upload reply packet\n\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "U");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^R^ ... Reset message pointers\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "R");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^C^ ... Configure you Offline Reader\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "C");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "OLRCONF");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^X^ ... Exit Offline-mail\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("doors.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "DOORS");
   menuhdr.usItems = 7;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nDoors\n^Select a letter from the list (I,E,L, ? for help or X to exit): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nThe following Doors services are available:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^E^ ... Enter a Door\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "E");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^L^ ... List available Doors\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "L");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^A^ ... Add a Door\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "A");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000U;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^M^ ... Modify a Door\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "M");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000U;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... Delete a Door\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000U;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^X^ ... Exit Doors\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("internet.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "INTERNET");
   menuhdr.usItems = 7;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nInternet\n^Select a letter from the list (T,F,I, ? for help or X to exit): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nThe following Internet services are available:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^T^ ... Telnet\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "T");
   item.usCommand = MNU_TELNET;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^F^ ... FTP (File Transfer Protocol)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_FTP;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^G^ ... Gopher\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "G");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^I^ ... Finger (User Information)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "I");
   item.usCommand = MNU_FINGER;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^V^ ... VModem (Virtual Modem Protocol)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "V");
   item.usCommand = MNU_VMODEM;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^X^ ... Exit Internet\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

}


