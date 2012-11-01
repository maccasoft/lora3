
#include "_ldefs.h"
#include "combase.h"

void main (int argc, char *argv[])
{
   USHORT Readed;
   UCHAR Temp[256];
   class TPipe *Pipe;
   class TScreen *Stdio;

   if ((Pipe = new TPipe) != NULL) {
#if defined(__OS2__)
      if (Pipe->ConnectServer (argv[1]) == TRUE) {
#elif defined(__NT__)
      if (Pipe->ConnectServer (GetStdHandle (STD_INPUT_HANDLE), GetStdHandle (STD_OUTPUT_HANDLE)) == TRUE) {
         SetStdHandle (STD_INPUT_HANDLE, CreateFile ("CONIN$", GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL));
         SetStdHandle (STD_OUTPUT_HANDLE, CreateFile ("CONOUT$", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL));
         printf ("Readpipe = %lu\n", atol (argv[1]));
         printf ("Writepipe = %lu\n", atol (argv[2]));
#endif
         if ((Stdio = new TScreen) != NULL) {
            Stdio->Initialize ();
            while (Stdio->Carrier () == TRUE && Pipe->Carrier () == TRUE) {
               if (Stdio->BytesReady () == TRUE) {
                  Readed = Stdio->ReadBytes (Temp, sizeof (Temp));
                  Pipe->SendBytes (Temp, Readed);
               }
               if (Pipe->BytesReady () == TRUE) {
                  Readed = Pipe->ReadBytes (Temp, sizeof (Temp));
                  Stdio->SendBytes (Temp, Readed);
               }
#if defined(__OS2__)
               DosSleep (1L);
#elif defined(__NT__)
               Sleep (1L);
#endif
            }
            delete Stdio;
         }
      }
      delete Pipe;
   }
}

