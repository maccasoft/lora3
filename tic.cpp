
#include "_ldefs.h"
#include "filebase.h"
#include "files.h"
#include "tools.h"

void main (void)
{
   FILE *fp;
   CHAR Temp[128], *p, Area[32], Name[32];
   struct find_t blk;
   class TCollection Text;

   while (!_dos_findfirst ("*.tic", 0, &blk)) {
      if ((fp = fopen (blk.name, "rt")) != NULL) {
         Text.Clear ();
         Area[0] = Name[0] = '\0';
         while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
            while ((p = strchr (Temp, '\n')) != NULL)
               *p = '\0';
            if ((p = strtok (Temp, " ")) != NULL) {
               if (!stricmp (p, "Area")) {
                  if ((p = strtok (NULL, " ")) != NULL)
                     strcpy (Area, strupr (p));
               }
               else if (!stricmp (p, "File")) {
                  if ((p = strtok (NULL, " ")) != NULL)
                     strcpy (Name, strupr (p));
               }
               else if (!stricmp (p, "Desc") || !stricmp (p, "LDesc")) {
                  if ((p = strtok (NULL, "")) != NULL) {
                     while (*p == ' ')
                        p++;
                     Text.Add (p, (USHORT)(strlen (p) + 1));
                  }
               }
            }
         }
         fclose (fp);

         if (Area[0] != '\0' && Name[0] != '\0') {
            if ((fp = fopen ("tic.cfg", "rt")) != NULL) {
               while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                  while ((p = strchr (Temp, '\n')) != NULL)
                     *p = '\0';
               }
               fclose (fp);
            }
         }
      }
   }
}


