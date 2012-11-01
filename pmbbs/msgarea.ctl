%
%          LoraBBS Professional Edition Configuration File
%     Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
%
%
%           ----------------------------------------------
%           MSGAREA.CTL - sample message area control file
%           ----------------------------------------------
%
%
%  This file describes your system's message areas.  Each message
%  area can be given a name.  This name consists of letters,
%  numbers, and any of the following characters:
%
%       . / _ ~ ! @ # $ % ^ & * ( ) - + = |

Area LOCAL

        % The MsgInfo keyword gives a verbose description of the message
        % appear.  This will be shown to users who enter the area.  The
        % description must be less than 60 characters long.
 
        MsgInfo         Local Messages

        %       Squish          Use the Squish file format for storing
        %                       messages in this area.  This is the default.
        %
        %       Jam             Use the FidoNet-standard *.MSG format for
        %                       storing messages in this area.
        %

        Type            Squish

%       Conference      C:\Msg\Local
        Local           Msg\Local
%       EchoMail        C:\Msg\Local
%       Matrix          C:\Msg\Local

        % The 'MsgAccess' keyword defines the privilege level required to
        % access this area.  If the user's priv is not high enough, they
        % won't be able to get in to this area, and Lora will pretend
        % that it doesn't even exist.

        MsgAccess       0
End Area

% This defines an EchoMail area called "LORA", which is the
% Lora support echo.  This area will be stored in the
% Squish message format.

Area LORA
        MsgInfo         LoraBBS Support Conference
        Type            Squish

        EchoMail        C:\Lora\Msg\LoraInt

        % The "MsgName" keyword specifies the canonical message area
        % name for EchoMail areas.  This tag should be the same
        % as defined in your EchoMail processor.

        MsgName         LORA

        MsgAccess       10
End Area

Area N1
        MsgInfo         comp.arch.embedded
        Type            Squish

        EchoMail        C:\Lora\Msg\ArchEmb
        MsgName         SCI.ELECTRONICS

        MsgAccess       10
End Area

Area N2
        MsgInfo         comp.home.automation
        Type            Squish

        EchoMail        C:\Lora\Msg\HomeAuto
        MsgName         SCI.ELECTRONICS

        MsgAccess       10
End Area

Area N3
        MsgInfo         comp.realtime
        Type            Squish

        EchoMail        C:\Lora\Msg\CompReal
        MsgName         SCI.ELECTRONICS

        MsgAccess       10
End Area

Area N4
        MsgInfo         comp.robotics.misc
        Type            Squish

        EchoMail        C:\Lora\Msg\RobMisc
        MsgName         SCI.ELECTRONICS

        MsgAccess       10
End Area

Area N5
        MsgInfo         sci.electronics
        Type            Squish

        EchoMail        C:\Lora\Msg\SciElect
        MsgName         SCI.ELECTRONICS

        MsgAccess       10
End Area

Area N6
        MsgInfo         sci.electronics.misc
        Type            Squish

        EchoMail        C:\Lora\Msg\SciElMsc
        MsgName         SCI.ELECTRONICS

        MsgAccess       10
End Area

Area N7
        MsgInfo         sci.electronics.components
        Type            Squish

        EchoMail        C:\Lora\Msg\SciComp
        MsgName         SCI.ELECTRONICS.COMPONENTS

        MsgAccess       10
End Area

Area N8
        MsgInfo         sci.electronics.design
        Type            Squish

        EchoMail        C:\Lora\Msg\SciDesi
        MsgName         SCI.ELECTRONICS.DESIGN

        MsgAccess       10
End Area

%
%
