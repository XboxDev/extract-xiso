# Generated automatically from Makefile.in by configure.
CC=gcc
CO=co

OBJS=\
FtpAbort.o \
FtpArchie.o \
FtpBye.o \
FtpClose.o \
FtpChmod.o \
FtpCommand.o \
FtpConnect.o \
FtpCopy.o \
FtpData.o \
FtpDebug.o \
FtpFilenameChecker.o \
FtpFull.o \
FtpGetHost.o \
FtpGood.o \
FtpHTTP.o \
FtpIO.o \
FtpInit.o \
FtpLogin.o \
FtpMessage.o \
FtpMove.o \
FtpOpenDir.o \
FtpPasv.o \
FtpPort.o \
FtpPwd.o \
FtpRetr.o \
FtpSize.o \
FtpStat.o \
FtpStor.o \
FtpSyst.o \
FtpType.o \
Ftpfopen.o \
Ftp_etclib.o

SRCS=\
FtpAbort.c \
FtpArchie.c \
FtpBye.c \
FtpClose.c \
FtpChmod.c \
FtpCommand.c \
FtpConnect.c \
FtpCopy.c \
FtpData.c \
FtpDebug.c \
FtpFilenameChecker.c \
FtpFull.c \
FtpGetHost.c \
FtpGood.c \
FtpHTTP.c \
FtpIO.c \
FtpInit.c \
FtpLogin.c \
FtpMessage.c \
FtpMove.c \
FtpOpenDir.c \
FtpPasv.c \
FtpPort.c \
FtpPwd.c \
FtpRetr.c \
FtpSize.c \
FtpStat.c \
FtpStor.c \
FtpSyst.c \
FtpType.c \
Ftpfopen.c \
Ftp_etclib.c \
FtpLibrary.h






all:	libftp.a

libftp.a:	$(OBJS)
	ar cruv libftp.a $(OBJS)
	ranlib libftp.a

clean:
	rm -f libftp.a $(OBJS) Make.log* *~


$(OBJS): FtpLibrary.h

$(SRCS):
	$(CO) $@
