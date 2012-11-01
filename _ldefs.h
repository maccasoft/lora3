
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

#ifndef __LDEFS_H
#define __LDEFS_H

#if defined(_MSC_VER)
#undef _WINDOWS
#endif

#if !defined(__LINUX__)
#include <conio.h>
#include <dos.h>
#if defined(__BORLANDC__)
#include <dirent.h>
#endif
#include <direct.h>
#if defined(__OS2__) && defined(__BORLANDC__)
#define __IN_IOCTL
#endif
#include <io.h>
#include <process.h>
#include <share.h>
#include <sys\stat.h>
#endif

#include <ctype.h>
#include <fcntl.h>
#include <malloc.h>
#if defined(_MSC_VER) || defined(__LINUX__)
#include <memory.h>
#else
#include <mem.h>
#endif
#if defined(__BORLANDC__) || defined(__LINUX__)
#include <utime.h>
#else
#include <sys\utime.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__LINUX__)
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#pragma pack(1)
#define cprintf         printf

#define O_BINARY        0
#define SH_DENYNO       0
#define SH_DENYRW       0

struct dosdate_t {
   unsigned char day;
   unsigned char month;
   unsigned int year;
   unsigned char dayofweek;
};

struct dostime_t {
   unsigned char hour;
   unsigned char minute;
   unsigned char second;
   unsigned char hsecond;
};

void chsize (int fd, long size);
void _dos_getdate (struct dosdate_t *ddate);
void _dos_gettime (struct dostime_t *dtime);
long filelength (int fd);
FILE *_fsopen (char *name, char *mode, int shflags);
int sopen (char *file, int flags, int shmode, int mode);
int stricmp (char *s1, char *s2);
int strnicmp (char *s1, char *s2, size_t maxlen);
char *strlwr (char *s);
char *strupr (char *s);
long tell (int fd);

#endif

#if defined(__OS2__) && !defined(INCL_DOS)
#define INCL_DOS
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS
#define INCL_PM
#define INCL_VIO
#define INCL_KBD
#include <os2.h>

typedef struct {
   ULONG SessionID;
   ULONG ResultCode;
} TERMNOTIFY, *PTERMNOTIFY;

#elif defined(__NT__)
#include <windows.h>
#endif

#if defined(__OS2__) || defined(__NT__)
#if defined(_MSC_VER)
#define DLL_EXPORT
#else
#define DLL_EXPORT      _export
#endif
#else
#define DLL_EXPORT
#endif

#ifndef TRUE
#define TRUE            1
#define FALSE           0
#endif

#ifndef VOID
typedef void            VOID;
typedef void *          PVOID;
typedef char            CHAR;
typedef unsigned char   UCHAR;
typedef unsigned char   BYTE;
typedef char *          PSZ;
typedef short           SHORT;
typedef unsigned short  USHORT;
typedef long            LONG;
typedef unsigned long   ULONG;
typedef int             INT;
#endif

#define CTRLA           0x01
#define CTRLB           0x02
#define CTRLC           0x03
#define CTRLD           0x04
#define CTRLE           0x05
#define CTRLF           0x06
#define CTRLG           0x07
#define CTRLH           0x08
#define CTRLI           0x09
#define CTRLJ           0x0a
#define CTRLK           0x0b
#define CTRLL           0x0c
#define CTRLM           0x0d
#define CTRLN           0x0e
#define CTRLO           0x0f
#define CTRLP           0x10
#define CTRLQ           0x11
#define CTRLR           0x12
#define CTRLS           0x13
#define CTRLT           0x14
#define CTRLU           0x15
#define CTRLV           0x16
#define CTRLW           0x17
#define CTRLX           0x18
#define CTRLY           0x19
#define CTRLZ           0x1a
#define ESC             0x1b
#define DEL             0x7f

#define BLACK           0
#define BLUE            1
#define GREEN           2
#define CYAN            3
#define RED             4
#define MAGENTA         5
#define BROWN           6
#define LGREY           7
#define DGREY           8
#define LBLUE           9
#define LGREEN          10
#define LCYAN           11
#define LRED            12
#define LMAGENTA        13
#define YELLOW          14
#define WHITE           15

#define _BLACK          0
#define _BLUE           16
#define _GREEN          32
#define _CYAN           48
#define _RED            64
#define _MAGENTA        80
#define _BROWN          96
#define _LGREY          112

#define BLINK           128

#define SOH             0x01
#define ACK             0x06
#define NAK             0x15
#define SYN             0x16
#define STX             0x02
#define EOT             0x04
#define ENQ             0x05

typedef struct {
   USHORT Zone;
   USHORT Net;
   USHORT Node;
   USHORT Point;
   CHAR   Domain[32];
   USHORT Flags;
} ADDR;

USHORT DLL_EXPORT Crc16 (UCHAR Byte, USHORT Crc);
ULONG  DLL_EXPORT Crc32 (UCHAR Byte, ULONG Crc);
USHORT DLL_EXPORT StringCrc16 (CHAR *String, USHORT Crc);
ULONG  DLL_EXPORT StringCrc32 (CHAR *String, ULONG Crc);

VOID   DLL_EXPORT Pause (LONG lHund);
LONG   DLL_EXPORT TimerSet (LONG lHund);
USHORT DLL_EXPORT TimeUp (LONG lEndtime);

PSZ    DLL_EXPORT AdjustPath (PSZ pszPath);
USHORT DLL_EXPORT BuildPath (PSZ pszPath);

char * DLL_EXPORT strsrep (char *str, char *search, char *replace);

VOID   DLL_EXPORT RunExternal (PSZ Command, USHORT TimeLimit = 0);
VOID   DLL_EXPORT SpawnExternal (PSZ Command);
ULONG  DLL_EXPORT AvailableMemory (VOID);

#endif

