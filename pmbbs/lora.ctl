%
%              LoraBBS Professional Edition Configuration File
%        Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
%

System Section

        Name            Arci Computer Club Bologna
        Sysop           Marco Maccaferri
        Location        Bologna, Italy

        Address         2:332/402
        Address         2:332/422
        Address         39:102/304

        Path System     .\
        Path Misc       D:\Sources\Bbs\Data\English\

        Path Outbound   C:\Lora\Outbound\
        Path Inbound    C:\Lora\Inbound\

        File Password   User

        Path Home       D:\Sources\Bbs\Data\Home\
        Path EMail      D:\Sources\Bbs\Data\EMail\
        Path Sent Mail  D:\Sources\Bbs\Data\EMail\Sent\

        Menu Path       English\

        Log Path        D:\Sources\Bbs\Data\Logs\

%       Log Mode        Terse
%       Log Mode        Verbose
        Log Mode        Debug

        First Menu      MAIN

        Start           Ftp Server

End System Section


Include Access.Ctl


Channel 0
        Type            Local

        Logon Level     10
        Logon Class     NORMAL
        Logon TimeLimit 10

        Input Timeout   4

        Ask             RealName Required
        Ask             Company
        Ask             Address
        Ask             City Required
        Ask             State Required
        Ask             Phone
        Ask             Sex Required
End Channel


Channel 1
        Type            Modem

        Output          COM2
        Baud Maximum    38400

        Busy            v~~^~~|~ATZ|~ATM0H1|
        Init            ~v~````|~^``ATZ|
        Answer          ATA|
        Dial            ATDT
        Hangup          ~v~~^~+++~~ATH0|

        Logon Level     10
%       Logon           Preregistered

        Logon Timelimit 15
        Logon Class     NORMAL

        Input Timeout   4

        Ask             RealName Required
        Ask             Company
        Ask             Address
        Ask             City Required
        Ask             State Required
        Ask             Phone
        Ask             Sex Required
End Channel


Reader Section

        Packet Name     ARCIBBS
        Work Directory  Temp\
        Phone Number    +39-51-6331730
        Max Messages    1000

End Reader Section


Include MsgArea.Ctl
Include FileArea.Ctl
Include Menus.Ctl


