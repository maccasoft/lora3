
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.20
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/19/95 - Initial coding.
// ----------------------------------------------------------------------

#define NAME            "LoraBBS"
#define VERSION         "0.20"
#define MAJ_VERSION     0
#define MIN_VERSION     20

#if defined(__OS2__)
#define STD_NAME        NAME"/Std-OS/2"
#define STD_COMPLETE    NAME"/Std-OS/2 v"VERSION
#define PRO_NAME        NAME"/Pro-OS/2"
#define PRO_COMPLETE    NAME"/Pro-OS/2 v"VERSION
#elif defined(__NT__)
#define STD_NAME        NAME"/Std-NT"
#define STD_COMPLETE    NAME"/Std-NT v"VERSION
#define PRO_NAME        NAME"/Pro-NT"
#define PRO_COMPLETE    NAME"/Pro-NT v"VERSION
#elif defined(__DOS__) || defined(__BORLANDC__)
#define STD_COMPLETE    NAME"/Std-DOS v"VERSION
#define STD_NAME        NAME"/Std-DOS"
#endif

