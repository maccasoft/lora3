
Menu MAIN

        % This option defines what the name of the menu will look like
        % on-screen, to the user.  This doesn't necessarily have to be
        % the same name as you specified above.

        Prompt          Select: 

        MenuColour      15 11 14 14

        % Name of cmd    Optional arguments      Priv.  Command as it
        % to execute     for cmd, if any         Level  appears to user
        % ------------  --------------------- --------- -----------------

        Display                                 Demoted "\nMAIN (%t mins):\n"

        MenuColour      15 11 7 14

        Display_Menu    Message                 Demoted "^M^essage areas       "
        Display_Menu    File                    Demoted "^F^ile areas          "
        Mail_Read                               Demoted "^R^ead E-Mail         "
        Display_Menu    Change                  Demoted "^C^hange setup        "

        % The above three options tell Maximus to chain to a different
        % *.Mnu file, whose name is specified as an argument.  Please
        % make sure NOT to include a path before the name of your menu,
        % since Maximus will add that according to the currently-defined
        % menu path.

        Goodbye                               Transient "^G^oodbye (log off)   "
%       MEX             M\Stats                 Demoted "^S^tatistics          "
% NoDsp Press_Enter                             Demoted "^S^"
%       Userlist                                Demoted "^U^serList            "
        Version                                 Demoted "^V^ersion of BBS      "

%       Display_File    Misc\YellReq            Demoted "^Y^ell for SysOp      "
% NoDsp Yell                                    Demoted "^Y^"

        % The above options are fairly generic, and basically do what
        % they appear to do.

        % When selected, this option displays a *.BBS file to the user.
        % You can have as many of these custom files as you wish.

        Display_File    Misc\Bulletin           Demoted "^B^ulletins           "

        % These commands display the off-line reader and SysOp menus.

        Display_Menu    Reader                  Demoted "^O^ff-line reader     "
%       Display_Menu    Sysop                     Sysop "^#^Sysop menu         "

        % The next commands are only of use to multi-line systems.
        % PLEASE SEE THE DOCUMENTATION BEFORE USING THESE
        % COMMANDS!

        Who_Is_On                               Demoted "^W^ho is On           "
  NoDsp Press_Enter                             Demoted "^W^"
%       Display_Menu    Chat                    Demoted "^/^Chat menu          "

        % The following is the help file for the main menu.  Unless you
        % specify a help file, such as done below, users will NOT be able
        % to press `?' for help.

        Display_File    Hlp\Main                Demoted "^?^help\n"

End Menu


Menu MESSAGE

        % This option defines what the name of the menu will look like
        % on-screen, to the user.  This doesn't necessarily have to be
        % the same name as you specified above.

        Prompt          Select: 

        MenuColour      15 11 14 14

        % Name of cmd    Optional arguments      Priv.  Command as it
        % to execute     for cmd, if any         Level  appears to user
        % ------------  --------------------- --------- -----------------

OnEnter Display_File    Misc\HeadMsg            Demoted ""
   Exec Display_File    Misc\Msg                Demoted ""
        Display                                 Demoted "\nMESSAGE (%t mins):\n"

        MenuColour      15 11 7 14

  NoRIP Msg_Area                                Demoted "^A^rea change         "
    RIP MEX             M\Msgarea               Demoted "^A^rea change         "
  NoDsp Msg_PrevArea                            Demoted "^[^"
  NoDsp Msg_NextArea                            Demoted "^]^"
        Read_Next                               Demoted "^N^ext message        "
        Read_Previous                           Demoted "^P^revious message    "
        Msg_Enter                               Demoted "^E^nter message       "
        Msg_Reply                               Demoted "^R^eply to a message  "
%       Msg_Browse                              Demoted "^B^rowse messages     "
%       Msg_Change                              Demoted "^C^hange current msg  "
        Read_Nonstop                            Demoted "^=^ReadNonStop        "
%       Read_Original                           Demoted "^-^ReadOriginal       "
%       Read_Reply                              Demoted "^+^ReadReply          "
        Msg_Current                             Demoted "^*^ReadCurrent        "
        Msg_List                                Demoted "^L^ist (brief)        "
        Msg_Tag                                 Demoted "^T^ag areas           "
        Display_Menu    Main                  Transient "^M^ain menu           "
        Display_Menu    File                  Transient "^J^ump to file areas  "
        Goodbye                               Transient "^G^oodbye (log off)   "
        Msg_Kill                                Demoted "^K^ill (delete) msg   "
%       Msg_Upload                              Demoted "^U^pload a message    "
        Msg_Forward                             Demoted "^F^orward (copy)      "
%       Msg_Reply_Area  .                       Demoted "^$^Reply Elsewhere    "
%       Msg_Download_Attach                     Demoted "^^^Download Attaches  "
%       Msg_Hurl                                  Sysop "^H^url (move)         "
%       Msg_Xport                                 Sysop "^X^port to disk       "
%       Msg_Edit_User                             Sysop "^@^Edit user          "
        Msg_Kludges                               Sysop "^!^Toggle Kludges     "
%       Msg_Unreceive                             Sysop "^#^Unreceive Msg      "
  NoDsp Same_Direction                          Demoted "^|^"
  NoDsp Read_Individual                         Demoted "^0^"
  NoDsp Read_Individual                         Demoted "^1^"
  NoDsp Read_Individual                         Demoted "^2^"
  NoDsp Read_Individual                         Demoted "^3^"
  NoDsp Read_Individual                         Demoted "^4^"
  NoDsp Read_Individual                         Demoted "^5^"
  NoDsp Read_Individual                         Demoted "^6^"
  NoDsp Read_Individual                         Demoted "^7^"
  NoDsp Read_Individual                         Demoted "^8^"
  NoDsp Read_Individual                         Demoted "^9^"

  Local Display_File    Hlp\Msg                 Demoted "^?^help\n"
% Matrix Display_File    Hlp\Mail                Demoted "^?^help"
%   Echo Display_File    Hlp\Echo                Demoted "^?^help"

End Menu


Menu FILE

        % This option defines what the name of the menu will look like
        % on-screen, to the user.  This doesn't necessarily have to be
        % the same name as you specified above.

        Prompt          Select: 

        MenuColour      15 11 14 14

        % Name of cmd    Optional arguments      Priv.  Command as it
        % to execute     for cmd, if any         Level  appears to user
        % ------------  --------------------- --------- -----------------

OnEnter Display_File    Misc\HeadFile           Demoted ""
   Exec Display_File    Misc\File               Demoted ""
        Display                                 Demoted "\nFILE (%t mins):\n"

        MenuColour      15 11 7 14

  NoRIP File_Area                               Demoted "^A^rea change         "
%   RIP MEX             M\FileArea              Demoted "^A^rea change         "
  NoDsp File_PrevArea                           Demoted "^[^"
  NoDsp File_NextArea                           Demoted "^]^"
        File_Locate                             Demoted "^L^ocate a file       "
        File_Titles                             Demoted "^F^ile titles         "
%       File_View                               Demoted "^V^iew text file      "
        File_Download                           Demoted "^D^ownload (receive)  "
%       File_Upload                             Demoted "^U^pload (send)       "
%       MEX             M\Stats                 Demoted "^S^tatistics          "
% NoDsp Press_Enter                             Demoted "^S^"                  "
%       File_Contents                           Demoted "^C^ontents (archive)  "
        File_Tag                                Demoted "^T^ag (queue) files   "
        File_NewFiles                           Demoted "^N^ew files scan      "
%       File_Raw                                  Sysop "^R^aw directory       "
%       File_Kill                                 Sysop "^K^ill file           "
%       File_Hurl                                 Sysop "^H^url (move)         "
%       File_Override                             Sysop "^O^verride path       "
        Display_Menu    Main                  Transient "^M^ain menu           "
        Display_Menu    Message               Transient "^J^ump to msg. areas  "
        Goodbye                               Transient "^G^oodbye (log off)   "
        Display_File    Hlp\FileHelp            Demoted "^?^help\n"

End Menu


Menu CHANGE

        % This option defines what the name of the menu will look like
        % on-screen, to the user.  This doesn't necessarily have to be
        % the same name as you specified above.

        Prompt          Select: 

        MenuColour      15 11 14 14

        % Name of cmd    Optional arguments      Priv.  Command as it
        % to execute     for cmd, if any         Level  appears to user
        % ------------  --------------------- --------- -----------------

        Display                                 Demoted "\nCHANGE (%t mins):\n"

        MenuColour      15 11 7 14

        Chg_Alias                                Hidden "^A^lias               "
        Chg_Phone                               Demoted "^#^Telephone no.      "
%       Chg_City                                Demoted "^C^ity                "
        Chg_Password                            Demoted "^P^assword            "
%       Chg_Help                                Demoted "^H^elp level          "
%       Chg_Nulls                               Demoted "^N^ulls               "
%       Chg_Width                               Demoted "^W^idth               "
        Chg_Length                              Demoted "^L^ength              "
%       Chg_Tabs                                Demoted "^T^abs                "
        Chg_Video                               Demoted "^V^ideo mode          "
        Chg_RIP                                 Demoted "^R^IP Graphics        "
%       Chg_Editor                              Demoted "^F^ullScrnEdit        "
%       Chg_Clear                               Demoted "^S^creen clear        "
%       Chg_IBM                                 Demoted "^I^BM characters      "
        Chg_Hotkeys                             Demoted "^!^Hotkeys            "
%       Chg_Language                            Demoted "^@^Language           "
%       Chg_Userlist                            Demoted "^%^ShowInUserlist     "
%       Chg_Protocol                            Demoted "^$^Protocol default   "
%       Chg_Archiver                            Demoted "^&^Archiver default   "
%       Chg_FSR                                 Demoted "^^^FullScrnRead       "
        Chg_More                                Demoted "^*^More Prompt        "
        Display_Menu    Main                  Transient "^M^ain Menu           "
  NoDsp Display_Menu    Main                  Transient "^Q^"
  NoDsp Display_Menu    Main                  Transient "^|^"
        Display_File    Hlp\Change              Demoted "^?^help\n"

End Menu


Menu EDIT

        % This option defines what the name of the menu will look like
        % on-screen, to the user.  This doesn't necessarily have to be
        % the same name as you specified above.

        Prompt          Select: 

        MenuColour      15 11 14 14

        % Name of cmd    Optional arguments      Priv.  Command as it
        % to execute     for cmd, if any         Level  appears to user
        % ------------  --------------------- --------- -----------------

        Display                                 Demoted "\nEDIT (%t mins):\n"

        MenuColour      15 11 7 14

        Edit_Save                             Transient "^S^ave message        "
        Edit_Abort                            Transient "^A^bort message       "
        Edit_List                             Transient "^L^ist message        "
        Edit_Edit                             Transient "^E^dit line           "
        Edit_Insert                           Transient "^I^nsert line         "
        Edit_Delete                           Transient "^D^elete line         "
%       Edit_Quote                            Transient "^Q^uote message       "
        Edit_Continue                         Transient "^C^ontinue            "
        Edit_To                               Transient "^T^o                  "
        Edit_Subj                             Transient "^J^subJect            "
%       Edit_From                                 Sysop "^F^rom                "
%       Edit_Handling                             Sysop "^H^andling            "
%       Read_DiskFile                             Sysop "^R^ead from disk      "
        Display_File    Hlp\Editor            Transient "^?^help\n"

End Menu


Menu CHAT

        % This option defines what the name of the menu will look like
        % on-screen, to the user.  This doesn't necessarily have to be
        % the same name as you specified above.

        Prompt          Select: 

        MenuColour      15 11 14 14

        % Name of cmd    Optional arguments      Priv.  Command as it
        % to execute     for cmd, if any         Level  appears to user
        % ------------  --------------------- --------- -----------------

        Display                                 Demoted "\nCHAT (%t mins):\n"

        MenuColour      15 11 7 14

        Chat_CB                                 Demoted "^C^B chat             "
        Chat_Page                               Demoted "^P^age user           "
        Chat_Pvt                                Demoted "^A^nswer page         "
        Chat_Toggle                             Demoted "^T^oggle status       "
        Display_Menu    MAIN                    Demoted "^M^ain menu           "
        Goodbye                                 Demoted "^G^oodbye (log off)   "
        Display_File    Misc\ChatHelp           Demoted "^?^help\n"

End Menu


Menu READER

        % This option defines what the name of the menu will look like
        % on-screen, to the user.  This doesn't necessarily have to be
        % the same name as you specified above.

        Prompt          Select: 

        MenuColour      15 11 14 14

        % Name of cmd    Optional arguments      Priv.  Command as it
        % to execute     for cmd, if any         Level  appears to user
        % ------------  --------------------- --------- -----------------

   Exec Display_File    Misc\Offline            Demoted ""
        Display                                 Demoted "\nREADER (%t mins):\n"

        MenuColour      15 11 7 14

        Msg_Tag                                 Demoted "^T^ag area            "
        Msg_Download_QWK                        Demoted "^Q^WK download        "
        Msg_Download_BW                         Demoted "^B^lueWave download   "
%       Msg_Upload_QWK                          Demoted "^U^pload replies      "
%       Chg_Protocol                            Demoted "^P^rotocol default    "
%       Chg_Archiver                            Demoted "^A^rchiver default    "
%       Msg_Restrict                            Demoted "^R^estrict date       "
        Display_Menu    Main                  Transient "^M^ain menu           "
        Goodbye                               Transient "^G^oodbye (log off)   "
        Display_File    Hlp\OLR                 Demoted "^?^help\n"
  NoDsp Display_Menu    Main                  Transient "^|^"

End Menu


Menu SYSOP

        % This option defines what the name of the menu will look like
        % on-screen, to the user.  This doesn't necessarily have to be
        % the same name as you specified above.

        Prompt          Select: 

        MenuColour      15 11 14 14

        % Name of cmd    Optional arguments      Priv.  Command as it
        % to execute     for cmd, if any         Level  appears to user
        % ------------  --------------------- --------- -----------------

        Display                                 Demoted "\nSYSOP (%t mins):\n"

        MenuColour      15 11 7 14

        % This command invokes the internal Maximus user editor.

        User_Editor                                Sysop "^U^ser editor        "

        % This tells Maximus that you want to execute an external
        % program (in this case, COMMAND.COM) when the `O' menu option
        % is selected.  Other options are available, such as `Xtern_Dos'
        % (which can run .BAT or .CMD files) and `Xtern_Erlvl'
        % (which completely unloads Maximus from memory).
        %
        % Also, when specifying the command/errorlevel to execute,
        % don't forget to use underscores instead of spaces!
        %
        % Examples:
        %
        %   Xtern_Run   C:\Max\Bonk_/XN       Sysop "^N^odelist editor"
        %   Xtern_Dos   D:\Path\Runme.Bat Transient "^R^un XYZ program"
        %   Xtern_Erlvl 45_Ongame.Bat       Demoted "^O^n-line Games"
        %
        % If you're running 4DOS, replace the following
        % "Command.Com" with "4Dos.Com".
        %
        % OS/2 users should use the following statement instead:
        %
        % Xtern_Run     maxpipe.exe_cmd.exe         Sysop "OS shell


UsrRemote Xtern_Run     Cmd.Exe >%P <%P             Sysop "^O^S shell          "
UsrLocal  Xtern_Run     Cmd.Exe                     Sysop "^O^S shell          "

        Telnet                                      Sysop "^T^elnet            "
        Ftp                                         Sysop "^F^tp               "

        Display_Menu    Main                        Sysop "^M^ain menu\n"
  NoDsp Display_Menu    Main                        Sysop "^Q^"
  NoDsp Display_Menu    Main                        Sysop "^|^"

End Menu




