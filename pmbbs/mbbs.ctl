
Menu MAIN

        MenuColour      10 11 10 11
        Prompt          \nMain System Menu (TOP)\n^Make your selection (E,F,L,S,O,? for help, or X to exit): ^

        Display                                 Demoted "\nPlease select one of the following:\n\n"

        MenuColour      10 11 14 11
        Mail_Read                               Demoted "  ^E^ ... Electronic Mail\n"
        Display_Menu    Message                 Demoted "  ^F^ ... Forums (Public Message Bases)\n"
        Display_Menu    File                    Demoted "  ^L^ ... File Libraries\n"
        Display_Menu    Internet                Demoted "  ^S^ ... Internet Services\n"
        Display_Menu    Reader                  Demoted "  ^O^ ... Off-Line Mail\n"
        Goodbye                               Transient "  ^X^ ... Exit System (Logoff)\n"

  NoDsp Display_File    Hlp\Main                Demoted "^?^"

End Menu


Menu MESSAGE

        MenuColour      10 11 10 11
        Prompt          \nConferences\n^Make your selection (R,W,L,S,E or X to exit): ^

        Display                                 Demoted "\nYour current Conference is : Y\nThere are 9 messages.\n\n"

        MenuColour      10 11 14 11
        Read_Msg                                Demoted "  ^R^ ... Read Messages\n"
        Msg_Enter                               Demoted "  ^W^ ... Write a Messages\n"
        Msg_List                                Demoted "  ^L^ ... List messages\n"
        Msg_Area                                Demoted "  ^S^ ... Select a new Conference\n"
        Msg_Kill                                Demoted "  ^E^ ... Erase a Message\n"

  NoDsp Display_Menu    Main                  Transient "^X^"
  NoDsp Display_File    Hlp\Msg                 Demoted "^?^"

End Menu


Menu FILE

        MenuColour      10 11 10 11
        Prompt          \nFile Libraries\n^Make your selection ( or X to exit): ^

        Display                                 Demoted "\nWelcome to the 2 Library\n"
        Display                                 Demoted "Description: Z\n"
        Display                                 Demoted "There are 8 files available for download.\n"

        Display                                 Demoted "\nPlease select one of the following:\n\n"

        MenuColour      10 11 14 11
        Display_Menu    Listing                 Demoted "  ^F^ ... Find Files\n"
        File_Download                           Demoted "  ^D^ ... Download a File\n"
        File_Tag                                Demoted "  ^T^ ... Manage tagged files (7 tagged)\n"
        File_Area                               Demoted "  ^S^ ... Select a new Library\n"
        Display_Menu    Main                  Transient "  ^X^ ... Exit File Libraries\n"

  NoDsp Display_File    Hlp\FileHelp            Demoted "^?^"

End Menu


Menu LISTING

        MenuColour      10 11 10 11
        Prompt          \n^Select an option, or X to exit: ^

        Display                                 Demoted "\nList Files:\n\n"

        MenuColour      10 11 14 11
        File_Titles                             Demoted "  ^F^ ... By file name, alphabetically\n"
  NoDsp Display_Menu    File                  Transient "^F^"
        File_Titles     /D                      Demoted "  ^D^ ... By date, most recently added to Library\n"
  NoDsp Display_Menu    File                  Transient "^D^"
        File_Newfiles                           Demoted "  ^Q^ ... Quickscan for new files\n"
  NoDsp Display_Menu    File                  Transient "^Q^"
        File_Titles     /N                      Demoted "  ^N^ ... By number of times the file has been downloaded\n"
  NoDsp Display_Menu    File                  Transient "^N^"

        MenuColour      10 11 10 11
        Display                                 Demoted "\nSearch Files:\n\n"

        MenuColour      10 11 14 11
        File_Locate     /N                      Demoted "  ^W^ ... By file name, or partial file name (wildcards)\n"
  NoDsp Display_Menu    File                  Transient "^W^"
        File_Locate                             Demoted "  ^K^ ... Keyword search\n"
  NoDsp Display_Menu    File                  Transient "^K^"

  NoDsp Display_Menu    File                  Transient "^X^"
  NoDsp Display_Menu    File                  Transient "^|^"
  NoDsp Display_File    Hlp\FileHelp            Demoted "^?^"

End Menu


Menu EDIT

        MenuColour      10 11 10 11
        Prompt          \nEditor\n^Make your selection (S,R,A,D,L,I,C,N,H,B or X to exit): ^

        Display                                 Demoted "\nEDITOR OPTIONS:\n\n"

        MenuColour      10 11 14 11
        Edit_Save                             Transient "  ^S^ ... Save message  "
        Edit_Edit                             Transient "  ^R^ ... Retype a line\n"
        Edit_Continue                         Transient "  ^A^ ... Append message"
        Edit_Delete                           Transient "  ^D^ ... Delete line\n"
        Edit_List                             Transient "  ^L^ ... List message  "
        Edit_Insert                           Transient "  ^I^ ... Insert line(s)\n"
        Edit_Edit                             Transient "  ^C^ ... Change text   "
        Edit_New                              Transient "  ^N^ ... New message\n"
        Display_File    Hlp\Editor            Transient "  ^H^ ... Help          "
        Edit_Subj                             Transient "  ^B^ ... Change subject\n"

  NoDsp Edit_Abort                            Transient "^X^"

End Menu


Menu READER

        % This option defines what the name of the menu will look like
        % on-screen, to the user.  This doesn't necessarily have to be
        % the same name as you specified above.

        MenuColour      10 11 10 11
        Prompt          \nOffline Mail (QWK/BlueWave)\n^Select an option (Q,B,A,U,R,C,? for help or X to exit): ^

        Display                                 Demoted "\nWelcome to Offline-mail!\n\n"

        MenuColour      10 11 14 11
        Msg_Download_QWK                        Demoted "  ^Q^ ... Download ^QWK^ packet\n"
        Msg_Download_BW                         Demoted "  ^B^ ... Download ^BlueWave^ packet\n"
        Msg_Tag                                 Demoted "  ^C^ ... Configure you Offline Reader\n"
        Display_Menu    Main                  Transient "  ^X^ ... Exit Offline-mail\n"
  NoDsp Display_File    Hlp\OLR                 Demoted "^?^"
  NoDsp Display_Menu    Main                  Transient "^|^"

End Menu



