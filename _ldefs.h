
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef __LDEFS_H
#define __LDEFS_H

#include <conio.h>
#include <ctype.h>
#include <direct.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#if defined(_MSC_VER)
#include <memory.h>
#else
#include <mem.h>
#endif
#include <process.h>
#include <share.h>
#include <sys\stat.h>
#if defined(__BORLANDC__)
#include <utime.h>
#else
#include <sys\utime.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__OS2__) && !defined(INCL_DOS)
#define INCL_DOS
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS
#define INCL_PM
#define INCL_VIO
#include <os2.h>
#elif defined(__NT__)
#include <windows.h>
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
#define LF              0x0a
#define CTRLK           0x0b
#define CTRLL           0x0c
#define CR              0x0d
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

#if defined(__OS2__) || defined(__NT__)
#define DLL_EXPORT      _export
#else
#define DLL_EXPORT
#endif

typedef struct {
   USHORT Zone;
   USHORT Net;
   USHORT Node;
   USHORT Point;
   CHAR   Domain[32];
   USHORT Flags;
} ADDR;

#endif

