
# -----------------------------------------------------------------------------
# LoraBBS Professional Edition - Version 3.00.14
# Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
#
# History:
#    16/08/96 - Initial coding
# -----------------------------------------------------------------------------

cc = g++
link = g++
lib = ar
copts = -g -c -D__LINUX__ -O6 -funsigned-char -Wno-sign-compare -fno-exceptions -fpermissive
#copts = -c -D__LINUX__ -O2
#copts = -c -D__NT__ -O2 -I /cygdrive/c/borland/bcc55/include
libopt = rcs

all: libcomm.a libmsgbase.a liblora_api.a libbbs.a \
     lfile lmsg luser lora reader


# -----------------------------------------------------------------------------
# Dynamic link libraries dependencies
# -----------------------------------------------------------------------------

comm = serial.o screen.o stdio.o tcpip.o

msgbase = adept.o dupes.o fidosdm.o hudson.o inetmail.o jam.o passthr.o squish.o \
          packet.o usenet.o

bbs = bbs.o detect.o editor.o email.o embedded.o ftrans.o internet.o \
      janus.o library.o message.o offline.o zmodem.o

lora_api = address.o collect.o config.o events.o filebase.o filedata.o \
      language.o limits.o log.o menu.o \
      misc.o msgdata.o nodes.o okfile.o outbound.o packer.o protocol.o \
      stats.o uulib.o user.o


# -----------------------------------------------------------------------------
# Dynamic link libraries (shared libraries)
# -----------------------------------------------------------------------------

libcomm.a : $(comm)
	$(lib) $(libopt) libcomm.a $(comm)

libmsgbase.a : $(msgbase)
	$(lib) $(libopt) libmsgbase.a $(msgbase)

liblora_api.a : $(lora_api)
	$(lib) $(libopt) liblora_api.a $(lora_api)

libbbs.a : $(bbs)
	$(lib) $(libopt) libbbs.a $(bbs)


# -----------------------------------------------------------------------------
# Executables
# -----------------------------------------------------------------------------

lora : areamgr.o import.o export.o lcfgtxt.o lcfgtxt2.o loratxt.o modem.o mailwin.o linux.o rcvfax.o tic.o
	$(link) -o lora -L. areamgr.o import.o export.o lcfgtxt.o lcfgtxt2.o loratxt.o modem.o mailwin.o rcvfax.o tic.o linux.o -lcxl -lmsgbase -lbbs -llora_api -lncurses -lcomm

luser : luser.o linux.o
	$(link) -o luser -L. luser.o linux.o -llora_api

lmsg : lmsg.o linux.o
	$(link) -o lmsg -L. lmsg.o linux.o -lcxl -lcomm -lmsgbase -llora_api

lfile : lfile.o linux.o liblora_api.a
	$(link) -o lfile -L. lfile.o linux.o -llora_api

reader : readtxt.o linux.o libcomm.a libcxl.a liblora_api.a libmsgbase.a
	$(link) -o reader -L. readtxt.o linux.o -lcxl -llora_api -lmsgbase -lncurses -lcomm

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

hudson.o: ../hudson.cpp ../_ldefs.h ../msgbase.h ../collect.h ../combase.h ../jam.h ../jamsys.h
	$(cc) $(copts) ../hudson.cpp

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

okfile.o : ../okfile.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../okfile.cpp

outbound.o : ../outbound.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../outbound.cpp

packer.o : ../packer.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../packer.cpp

protocol.o : ../protocol.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../protocol.cpp

stats.o : ../stats.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../stats.cpp

uulib.o : ../uulib.cpp ../_ldefs.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../uulib.cpp

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

import.o: ../import.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../import.cpp

export.o: ../export.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../export.cpp

lcfgtxt.o: ../lcfgtxt.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../lcfgtxt.cpp

lcfgtxt2.o: ../lcfgtxt2.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../lcfgtxt2.cpp

loratxt.o: ../loratxt.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../loratxt.cpp

mailwin.o: ../mailwin.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../mailwin.cpp

modem.o: ../modem.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../modem.cpp

rcvfax.o: ../rcvfax.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../rcvfax.cpp

tic.o: ../tic.cpp ../_ldefs.h ../lorawin.h ../combase.h ../lora_api.h ../collect.h
	$(cc) $(copts) ../tic.cpp

readtxt.o: ../readtxt.cpp ../_ldefs.h ../lora.h ../bluewave.h ../combase.h ../collect.h ../ftrans.h ../lora_api.h ../menu.h ../msgbase.h ../jam.h ../jamsys.h ../lorawin.h
	$(cc) $(copts) ../readtxt.cpp

