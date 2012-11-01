@echo off
del src.zip
pkzip -ex -rp src *.* -x*.sym -x*.obj -x*.pch -x*.exe -x*.dll -x*.err -x*.mk? -x*.lk? -x*.rwp -x*.bak -x*.map -x*.dsk
pkzip -ex -rP src \tcpip\lib\*.lib \tcpip\include\*.*
