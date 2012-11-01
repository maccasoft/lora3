/*
**  JAM(mbp) - The Joaquim-Andrew-Mats Message Base Proposal
**
**  C API
**
**  Written by Joaquim Homrighausen and Mats Wallin.
**
**  ----------------------------------------------------------------------
**
**  jamsys.h (JAMmb)
**
**  Compiler and platform dependant definitions
**
**  Copyright 1993 Joaquim Homrighausen, Andrew Milner, Mats Birch, and
**  Mats Wallin. ALL RIGHTS RESERVED.
**
**  93-06-28    JoHo/MW
**  Initial coding.
*/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __JAMSYS_H__
#define __JAMSYS_H__

/*
**  The following assumptions are made about compilers and platforms:
**
**  __MSDOS__       Defined if compiling for MS-DOS
**  _WINDOWS        Defined if compiling for Microsoft Windows
**  __NT__          Defined if compiling for Windows NT
**  __OS2__         Defined if compiling for OS/2 2.x
**  __sparc__       Defined if compiling for Sun Sparcstation
**  __50SERIES      Defined if compiling for Prime with Primos
**
**  __SMALL__       Defined if compiling under MS-DOS in small memory model
**  __MEDIUM__      Defined if compiling under MS-DOS in medium memory model
**  __COMPACT__     Defined if compiling under MS-DOS in compact memory model
**  __LARGE__       Defined if compiling under MS-DOS in large memory model
**
**  __ZTC__         Zortech C++ 3.x
**  __BORLANDC__    Borland C++ 3.x
**  __TURBOC__      Turbo C 2.0
**  __TSC__         JPI TopSpeed C 1.06
**  _MSC_VER        Microsoft C 6.0 and later
**  _QC             Microsoft Quick C
*/
#if defined(_WINDOWS)
    #ifndef _INC_WINDOWS
        #include <windows.h>
    #endif

    #if defined(__MSDOS__)
        #undef __MSDOS__
    #endif

    typedef long                INT32;      /* 32 bits signed integer     */
    typedef unsigned long       UINT32;     /* 32 bits unsigned integer   */
    typedef short int           INT16;      /* 16 bits signed integer     */
    typedef unsigned short int  UINT16;     /* 16 bits unsigned integer   */
    typedef char                CHAR8;      /* 8 bits signed integer      */
    typedef unsigned char       UCHAR8;     /* 8 bits unsigned integer    */
    typedef HFILE               FHANDLE;    /* File handle                */

    #if defined(__ZTC__)               /* Zortech C++ 3.x */
        #define _JAMFAR _far
        #define _HUGE   _huge
        #define _PASCAL _pascal
        #define _EXPORT _export
    #elif defined(__BORLANDC__)        /* Borland C++ 3.x */
        #define _JAMFAR far
        #define _HUGE   huge
        #define _PASCAL pascal
        #define _EXPORT _export
    #elif defined(_MSC_VER) ||         /* Microsoft C 6.0 and later */   \
          defined(_QC)                 /* Microsoft Quick C 2.51 and later */
        #define _JAMFAR _far
        #define _HUGE   _huge
        #define _PASCAL _pascal
        #define _EXPORT _export
        #if defined(_M_I86SM)
            #define __SMALL__
        #elif defined(_M_I86MM)
            #define __MEDIUM__
        #elif defined(_M_I86CM)
            #define __COMPACT__
        #elif defined(_M_I86LM)
            #define __LARGE__
        #endif
    #else
        #error Unsupported compiler
    #endif

    #if defined(JAMCAPI) && !defined(__LARGE__)
        #error Only LARGE memory model supported for the JAM Windows DLL
    #endif

    #define _JAMPROC            _JAMFAR _PASCAL _EXPORT
    #define _JAMDATA            _JAMFAR

#elif defined(__MSDOS__) || defined(__386__) || defined(__DOS__)
    typedef long                INT32;      /* 32 bits signed integer     */
    typedef unsigned long       UINT32;     /* 32 bits unsigned integer   */
    typedef short int           INT16;      /* 16 bits signed integer     */
    typedef unsigned short int  UINT16;     /* 16 bits unsigned integer   */
    typedef char                CHAR8;      /* 8 bits signed integer      */
    typedef unsigned char       UCHAR8;     /* 8 bits unsigned integer    */
    typedef int                 FHANDLE;    /* File handle                */

    #define _JAMPROC
    #define _JAMDATA

    #if defined(__ZTC__)               /* Zortech C++ 3.x */
        #define _JAMFAR    _far
    #elif defined(__BORLANDC__)        /* Borland C++ 3.x */
        #define _JAMFAR    far
    #elif defined(__TURBOC__)          /* Turbo C 2.0 */
        #define _JAMFAR    far
    #elif defined(__TSC__)             /* JPI TopSpeed C 1.06 */
        #define _JAMFAR    far
        #if defined(M_I86SM)
            #define __SMALL__
        #elif defined(M_I86MM)
            #define __MEDIUM__
        #elif defined(M_I86CM)
            #define __COMPACT__
        #elif defined(M_I86LM)
            #define __LARGE__
        #endif
    #elif defined(_MSC_VER) ||         /* Microsoft C 6.0 and later */   \
          defined(_QC)                 /* Microsoft Quick C 2.51 and later */
        #define _JAMFAR    _far
        #if defined(_M_I86SM)
            #define __SMALL__
        #elif defined(_M_I86MM)
            #define __MEDIUM__
        #elif defined(_M_I86CM)
            #define __COMPACT__
        #elif defined(_M_I86LM)
            #define __LARGE__
        #endif
    #else
        #define _JAMFAR    far
    #endif

#elif defined(__OS2__)
    typedef long                INT32;      /* 32 bits signed integer     */
    typedef unsigned long       UINT32;     /* 32 bits unsigned integer   */
    typedef short int           INT16;      /* 16 bits signed integer     */
    typedef unsigned short int  UINT16;     /* 16 bits unsigned integer   */
    typedef char                CHAR8;      /* 8 bits signed integer      */
    typedef unsigned char       UCHAR8;     /* 8 bits unsigned integer    */
    typedef HFILE               FHANDLE;    /* File handle                */

    #define _JAMFAR
    #define _JAMPROC
    #define _JAMDATA

#elif defined(__LINUX__)
    typedef long                INT32;      /* 32 bits signed integer     */
    typedef unsigned long       UINT32;     /* 32 bits unsigned integer   */
    typedef short int           INT16;      /* 16 bits signed integer     */
    typedef unsigned short int  UINT16;     /* 16 bits unsigned integer   */
    typedef char                CHAR8;      /* 8 bits signed integer      */
    typedef unsigned char       UCHAR8;     /* 8 bits unsigned integer    */
    typedef int                 FHANDLE;    /* File handle                */

    #define _JAMFAR
    #define _JAMPROC
    #define _JAMDATA

#elif defined(__NT__)
    typedef long                INT32;      /* 32 bits signed integer     */
    typedef unsigned long       UINT32;     /* 32 bits unsigned integer   */
    typedef short int           INT16;      /* 16 bits signed integer     */
    typedef unsigned short int  UINT16;     /* 16 bits unsigned integer   */
    typedef char                CHAR8;      /* 8 bits signed integer      */
    typedef unsigned char       UCHAR8;     /* 8 bits unsigned integer    */
    typedef int                 FHANDLE;    /* File handle                */

    #define _JAMFAR
    #define _JAMPROC
    #define _JAMDATA

#elif defined(__sparc__) || defined(__50SERIES)
    typedef long                INT32;      /* 32 bits signed integer     */
    typedef unsigned long       UINT32;     /* 32 bits unsigned integer   */
    typedef short int           INT16;      /* 16 bits signed integer     */
    typedef unsigned short int  UINT16;     /* 16 bits unsigned integer   */
    typedef char                CHAR8;      /* 8 bits signed integer      */
    typedef unsigned char       UCHAR8;     /* 8 bits unsigned integer    */
    typedef int                 FHANDLE;    /* File handle                */

    #define _JAMFAR
    #define _JAMPROC
    #define _JAMDATA

#else
    #error Undefined platform
#endif

typedef INT32 _JAMDATA *        INT32ptr;
typedef UINT32 _JAMDATA *       UINT32ptr;
typedef INT16 _JAMDATA *        INT16ptr;
typedef UINT16 _JAMDATA *       UINT16ptr;
typedef CHAR8 _JAMDATA *        CHAR8ptr;
typedef UCHAR8 _JAMDATA *       UCHAR8ptr;
typedef void _JAMDATA *         VOIDptr;

/*
**  Values for "AccessMode" and "ShareMode" parameter to JAMsysSopen.
*/

#if defined(_WINDOWS)
    #define JAMO_RDWR           READ_WRITE
    #define JAMO_RDONLY         READ
    #define JAMO_WRONLY         WRITE
    #define JAMSH_DENYNO        OF_SHARE_DENY_NONE
    #define JAMSH_DENYRD        OF_SHARE_DENY_READ
    #define JAMSH_DENYWR        OF_SHARE_DENY_WRITE
    #define JAMSH_DENYRW        OF_SHARE_EXCLUSIVE
#elif defined(__MSDOS__) || defined(__OS2__) || defined(__NT__)
    #define JAMO_RDWR           O_RDWR
    #define JAMO_RDONLY         O_RDONLY
    #define JAMO_WRONLY         O_WRONLY
    #define JAMSH_DENYNO        SH_DENYNO
    #define JAMSH_DENYRD        SH_DENYRD
    #define JAMSH_DENYWR        SH_DENYWR
    #define JAMSH_DENYRW        SH_DENYRW
#elif defined(__sparc__)
    #define JAMO_RDWR           O_RDWR
    #define JAMO_RDONLY         O_RDONLY
    #define JAMO_WRONLY         O_WRONLY
    #define JAMSH_DENYNO        0
    #define JAMSH_DENYRD        0
    #define JAMSH_DENYWR        0
    #define JAMSH_DENYRW        0
#elif defined(__50SERIES)
    #define JAMO_RDWR           2
    #define JAMO_RDONLY         0
    #define JAMO_WRONLY         1
    #define JAMSH_DENYNO        0
    #define JAMSH_DENYRD        0
    #define JAMSH_DENYWR        0
    #define JAMSH_DENYRW        0
#endif


/*
**  Structure to contain date/time information
*/
typedef struct JAMtm
    {
    int     tm_sec,                    /* Seconds 0..59                     */
            tm_min,                    /* Minutes 0..59                     */
            tm_hour,                   /* Hour of day 0..23                 */
            tm_mday,                   /* Day of month 1..31                */
            tm_mon,                    /* Month 0..11                       */
            tm_year,                   /* Years since 1900                  */
            tm_wday,                   /* Day of week 0..6 (Sun..Sat)       */
            tm_yday,                   /* Day of year 0..365                */
            tm_isdst;                  /* Daylight savings time (not used)  */
    } JAMTM, _JAMDATA * JAMTMptr;

#endif /* __JAMSYS_H__ */

#ifdef __cplusplus
}
#endif

/* end of file "jamsys.h" */
