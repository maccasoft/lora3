
#include "_ldefs.h"
#include "bbs.h"

TStatus::TStatus (PSZ pszDataPath)
{
   CurrentTask = 0xFFFFU;
   LastTask = 0xFFFFU;
   strcpy (DataFile, pszDataPath);
   if (DataFile[0] != '\0' && DataFile[strlen (DataFile) - 1] != '\\')
      strcat (DataFile, "\\");
   strcat (DataFile, "Status.Dat");
}

TStatus::~TStatus (void)
{
}

VOID TStatus::Activate (USHORT usTask)
{
   int fd;
   USHORT Found = FALSE;

   if ((fd = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
      while (read (fd, &Stat, sizeof (CHSTAT)) == sizeof (CHSTAT)) {
         if (Stat.Task == usTask) {
            Stat.Active = TRUE;
            Stat.UpTime = time (NULL);
            lseek (fd, tell (fd) - sizeof (CHSTAT), SEEK_SET);
            write (fd, &Stat, sizeof (CHSTAT));
            Found = TRUE;
         }
      }
      if (Found == FALSE) {
         memset (&Stat, 0, sizeof (CHSTAT));
         Stat.Active = TRUE;
         Stat.Task = usTask;
         Stat.UpTime = Stat.ChangeTime = time (NULL);
         write (fd, &Stat, sizeof (CHSTAT));
      }

      Active = Stat.Active;
      Task = Stat.Task;
      strcpy (User, Stat.User);
      Speed = Stat.Speed;
      strcpy (Status, Stat.Status);
      strcpy (Location, Stat.Location);
      ChangeTime = Stat.ChangeTime;
      UpTime = Stat.UpTime;

      close (fd);
   }

   CurrentTask = usTask;
}

VOID TStatus::Deactivate (USHORT usTask)
{
   int fd;

   if (usTask == 0xFFFFU)
      usTask = CurrentTask;

   if ((fd = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
      while (read (fd, &Stat, sizeof (CHSTAT)) == sizeof (CHSTAT)) {
         if (Stat.Task == usTask) {
            Stat.Active = FALSE;
            lseek (fd, tell (fd) - sizeof (CHSTAT), SEEK_SET);
            write (fd, &Stat, sizeof (CHSTAT));
         }
      }
      close (fd);
   }
}

USHORT TStatus::First (VOID)
{
   LastTask = 0xFFFFU;
   return (Next ());
}

USHORT TStatus::Next (VOID)
{
   int fd;
   USHORT Found = FALSE, Highest = 0xFFFFU;
   ULONG Position;

   if ((fd = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
      while (read (fd, &Stat, sizeof (CHSTAT)) == sizeof (CHSTAT)) {
         if ((Stat.Task > LastTask || LastTask == 0xFFFFU) && Stat.Task < Highest) {
            Highest = Stat.Task;
            Position = tell (fd) - sizeof (CHSTAT);
            Found = TRUE;
         }
      }
      if (Found == TRUE) {
         lseek (fd, Position, SEEK_SET);
         read (fd, &Stat, sizeof (CHSTAT));
         Active = Stat.Active;
         Task = Stat.Task;
         strcpy (User, Stat.User);
         Speed = Stat.Speed;
         strcpy (Status, Stat.Status);
         strcpy (Location, Stat.Location);
         ChangeTime = Stat.ChangeTime;
         UpTime = Stat.UpTime;
         LastTask = Task;
      }
      close (fd);
   }

   return (Found);
}

USHORT TStatus::Read (USHORT usTask)
{
   int fd;
   USHORT Found = FALSE;

   if ((fd = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
      while (read (fd, &Stat, sizeof (CHSTAT)) == sizeof (CHSTAT)) {
         if (Stat.Task == usTask) {
            Active = Stat.Active;
            Task = Stat.Task;
            strcpy (User, Stat.User);
            Speed = Stat.Speed;
            strcpy (Status, Stat.Status);
            strcpy (Location, Stat.Location);
            ChangeTime = Stat.ChangeTime;
            UpTime = Stat.UpTime;
            LastTask = Task;
            Found = TRUE;
         }
      }
      close (fd);
   }

   return (Found);
}

VOID TStatus::Update (VOID)
{
   int fd;

   if ((fd = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
      while (read (fd, &Stat, sizeof (CHSTAT)) == sizeof (CHSTAT)) {
         if (Stat.Task == CurrentTask) {
            memset (Stat.Status, 0, sizeof (Stat.Status));
            Stat.Active = Active;
            Stat.Task = Task;
            strcpy (Stat.User, User);
            Stat.Speed = Speed;
            strcpy (Stat.Status, Status);
            strcpy (Stat.Location, Location);
            Stat.ChangeTime = time (NULL);
            Stat.UpTime = UpTime;
            lseek (fd, tell (fd) - sizeof (CHSTAT), SEEK_SET);
            write (fd, &Stat, sizeof (CHSTAT));
         }
      }
      close (fd);
   }
}

