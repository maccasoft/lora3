
#include "_ldefs.h"
#include "combase.h"

void main (int argc, char *argv[])
{
   USHORT Readed;
   UCHAR Temp[256];
   class TPipe *Pipe;
   class TScreen *Stdio;

   if ((Pipe = new TPipe) != NULL) {
      if (Pipe->ConnectServer (argv[1], argv[2]) == TRUE) {
         Pipe->Time = 0L;
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
               if (Pipe->Time != 0L) {
                  Stdio->SetName (Pipe->Name);
                  Stdio->SetCity (Pipe->City);
                  Stdio->SetLevel (Pipe->Level);
                  Stdio->SetTimeLeft (Pipe->TimeLeft);
                  Stdio->SetTime (Pipe->Time);
                  Pipe->Time = 0L;
               }
            }
            delete Stdio;
         }
      }
      delete Pipe;
   }
}

