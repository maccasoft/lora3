
#include "_ldefs.h"

FILE *_fsopen (char *name, char *mode, int shflags)
{
   shflags = shflags;
   return (fopen (name, mode));
}

char *strlwr (char *s)
{
   char *p = s;

   while (*p != '\0') {
      *p = (char)tolower (*p);
      p++;
   }

   return (s);
}

char *strupr (char *s)
{
   char *p = s;

   while (*p != '\0') {
      *p = (char)toupper (*p);
      p++;
   }

   return (s);
}

void chsize (int fd, long size)
{
   fd = fd;
   size = size;
}

int strnicmp (char *s1, char *s2, size_t maxlen)
{
   int i = maxlen;

   do {
      if (toupper (*s1) != toupper (*s2))
         return ((int)(toupper (*s1) - toupper (*s2)));
      s1++;
      s2++;
   } while (--maxlen > 0);

   return (0);
}

int stricmp (char *s1, char *s2)
{
   while (*s1 != '\0' && *s2 != '\0') {
      if (toupper (*s1) != toupper (*s2))
         return ((int)(toupper (*s1) - toupper (*s2)));
      s1++;
      s2++;
   }

   if (toupper (*s1) != toupper (*s2))
      return ((int)(toupper (*s1) - toupper (*s2)));

   return (0);
}

void _dos_getdate (struct dosdate_t *ddate)
{
   time_t t;
   struct tm *ltm;

   t = time (NULL);
   ltm = localtime (&t);

   ddate->day = (unsigned char)ltm->tm_mday;
   ddate->month = (unsigned char)(ltm->tm_mon + 1);
   ddate->year = (unsigned int)(ltm->tm_year + 1900);
   ddate->dayofweek = (unsigned char)ltm->tm_wday;
}

void _dos_gettime (struct dostime_t *dtime)
{
   time_t t;
   struct tm *ltm;

   t = time (NULL);
   ltm = localtime (&t);

   dtime->hour = (unsigned char)ltm->tm_hour;
   dtime->minute = (unsigned char)ltm->tm_min;
   dtime->second = (unsigned char)ltm->tm_sec;
   dtime->hsecond = 0;
}

long filelength (int fd)
{
   long retval = -1L;
   struct stat buf;

   if (fd != -1) {
      fstat (fd, &buf);
      retval = buf.st_size;
   }

   return (retval);
}

long tell (int fd)
{
   long retval = -1L;

   if (fd != -1)
      retval = lseek (fd, 0L, SEEK_CUR);

   return (retval);
}

int sopen (char *file, int flags, int shmode, int mode)
{
   return (open (file, flags|shmode, mode));
}

