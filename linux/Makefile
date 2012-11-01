
# -----------------------------------------------------------------------------
# LoraBBS Professional Edition - Version 3.00.14
# Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
#
# History:
#    16/08/96 - Initial coding
# -----------------------------------------------------------------------------

cc = gcc
link = gcc
copts = -c -D__LINUX__ -fPIC

all: comm.dll msgbase.dll lora_api.dll bbs.dll \
     lc lfile lmsg luser lora


# -----------------------------------------------------------------------------
# Dynamic link libraries dependencies
# -----------------------------------------------------------------------------

comm = serial.o screen.o stdio.o tcpip.o

msgbase = adept.o dupes.o fidosdm.o inetmail.o jam.o passthr.o squish.o \
          packet.o usenet.o

bbs = bbs.o detect.o editor.o email.o embedded.o ftrans.o internet.o \
      janus.o library.o message.o offline.o zmodem.o

lora_api = address.o collect.o config.o events.o filebase.o filedata.o \
      language.o limits.o log.o menu.o \
      misc.o msgdata.o nodes.o outbound.o packer.o protocol.o stats.o user.o


# -----------------------------------------------------------------------------
# Dynamic link libraries (shared libraries)
# -----------------------------------------------------------------------------

comm.dll : $(comm)
	$(link) -o comm.dll -shared $(comm)

msgbase.dll : $(msgbase)
	$(link) -o msgbase.dll -shared $(msgbase)

lora_api.dll : $(lora_api)
	$(link) -o lora_api.dll -shared $(lora_api)

bbs.dll : $(bbs)
	$(link) -o bbs.dll -shared $(bbs)


# -----------------------------------------------------------------------------
# Executables
# -----------------------------------------------------------------------------

lora : areamgr.o loratxt.o modem.o mailwin.o linux.o tic.o /sources/ncxl/linux/cxl.dll comm.dll msgbase.dll lora_api.dll bbs.dll
	$(link) -o lora -L. areamgr.o loratxt.o modem.o mailwin.o /sources/ncxl/linux/cxl.dll tic.o linux.o comm.dll msgbase.dll lora_api.dll bbs.dll

luser : luser.o linux.o lora_api.dll
	$(link) -o luser -L. luser.o linux.o lora_api.dll

lmsg : lmsg.o linux.o msgbase.dll lora_api.dll
	$(link) -o lmsg -L. lmsg.o linux.o /sources/ncxl/linux/cxl.dll comm.dll msgbase.dll lora_api.dll

lfile : lfile.o linux.o lora_api.dll
	$(link) -o lfile -L. lfile.o linux.o lora_api.dll

lc : lc.o linux.o lora_api.dll
	$(link) -o lc -L. lc.o linux.o lora_api.dll


# -----------------------------------------------------------------------------
# Communication objects
# -----------------------------------------------------------------------------

serial.o: ../serial.cpp ../_ldefs.h ../combase.h
	$(cc) $(copts) ../serial.cpp

screen.o: ../screen.cpp ../_ldefs.h ../combase.h
	$(cc) $(copts) ../screen.cpp

stdio.o: ../stdio.cpp ../_ldefs.h ../combase.h
	$(cc) $(copts) ../stdio.cpp

tcpip.o: ../tcpip.cpp ../_ldefs.h ../combase.h
	$(cc) $(copts) ../tcpip.cpp


# -----------------------------------------------------------------------------
# Message base objects
# -----------------------------------------------------------------------------

adept.o: ../adept.cpp ../_ldefs.h ../msgbase.h ../collect.h ../combase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../adept.cpp

dupes.o: ../dupes.cpp ../_ldefs.h ../msgbase.h ../collect.h ../combase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../dupes.cpp

fidosdm.o: ../fidosdm.cpp ../_ldefs.h ../msgbase.h ../collect.h ../combase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../fidosdm.cpp

inetmail.o: ../inetmail.cpp ../_ldefs.h ../msgbase.h ../collect.h ../combase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../inetmail.cpp

jam.o: ../jam.cpp ../_ldefs.h ../msgbase.h ../collect.h ../combase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../jam.cpp

passthr.o: ../passthr.cpp ../_ldefs.h ../msgbase.h ../collect.h ../combase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../passthr.cpp

squish.o : ../squish.cpp ../_ldefs.h ../msgbase.h ../collect.h ../combase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../squish.cpp

packet.o : ../packet.cpp ../_ldefs.h ../msgbase.h ../collect.h ../combase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../packet.cpp

usenet.o : ../usenet.cpp ../_ldefs.h ../msgbase.h ../collect.h ../combase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../usenet.cpp


# -----------------------------------------------------------------------------
# LoraBBS Application Programming Interface (API)
# -----------------------------------------------------------------------------

address.o: ../address.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../address.cpp

collect.o: ../collect.cpp ../_ldefs.h ../collect.h
	$(cc) $(copts) ../collect.cpp

config.o: ../config.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../config.cpp

events.o : ../events.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../events.cpp

filebase.o : ../filebase.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../filebase.cpp

filedata.o : ../filedata.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../filedata.cpp

language.o : ../language.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../language.cpp

limits.o : ../limits.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../limits.cpp

log.o : ../log.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../log.cpp

menu.o: ../menu.cpp ../_ldefs.h ../menu.h ../collect.h
	$(cc) $(copts) ../menu.cpp

misc.o: ../misc.cpp ../_ldefs.h
	$(cc) $(copts) ../misc.cpp

msgdata.o: ../msgdata.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../msgdata.cpp

nodes.o : ../nodes.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../nodes.cpp

outbound.o : ../outbound.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../outbound.cpp

packer.o : ../packer.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../packer.cpp

protocol.o : ../protocol.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../protocol.cpp

stats.o : ../stats.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../stats.cpp

user.o : ../user.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../user.cpp


# -----------------------------------------------------------------------------
# Bulletin board objects
# -----------------------------------------------------------------------------

bbs.o: ../bbs.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../bbs.cpp

detect.o: ../detect.cpp ../_ldefs.h ../ftrans.h ../combase.h ../lora_api.h ../collect.h ../lora.h ../bluewave.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../detect.cpp

editor.o: ../editor.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../editor.cpp

email.o: ../email.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../email.cpp

embedded.o: ../embedded.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../embedded.cpp

ftrans.o: ../ftrans.cpp ../_ldefs.h ../ftrans.h ../combase.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../ftrans.cpp

internet.o: ../internet.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../internet.cpp

janus.o: ../janus.cpp ../_ldefs.h ../ftrans.h ../combase.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../janus.cpp

library.o: ../library.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../library.cpp

message.o: ../message.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../message.cpp

offline.o: ../offline.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../offline.cpp

zmodem.o: ../zmodem.cpp ../_ldefs.h ../ftrans.h ../combase.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../zmodem.cpp


# -----------------------------------------------------------------------------
# Linux-specific functions
# -----------------------------------------------------------------------------

linux.o : ../linux.cpp ../_ldefs.h
	$(cc) $(copts) ../linux.cpp


# -----------------------------------------------------------------------------
# Executable-specific objects
# -----------------------------------------------------------------------------

lc.o: ../lc.cpp ../_ldefs.h ../lora_api.h ../collect.h ../menu.h
	$(cc) $(copts) ../lc.cpp

lfile.o: ../lfile.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../lfile.cpp

lmsg.o: ../lmsg.cpp ../_ldefs.h ../lora_api.h ../collect.h ../msgbase.h ../combase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../lmsg.cpp

luser.o: ../luser.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../luser.cpp

areamgr.o: ../areamgr.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../areamgr.cpp

loratxt.o: ../loratxt.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../loratxt.cpp

mailwin.o: ../mailwin.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../mailwin.cpp

modem.o: ../modem.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../modem.cpp

tic.o: ../tic.cpp ../_ldefs.h ../lorawin.h ../combase.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../tic.cpp


