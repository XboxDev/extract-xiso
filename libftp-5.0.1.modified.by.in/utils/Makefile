# Generated automatically from Makefile.in by configure.

CC=gcc -O0
CO=co

CFLAGS = -g -I. -I.. -L. -L..

ALL=uftp

OBJS=uftp.o uftpcmd.o uftp_tab.o

SRCS=uftp.c uftpcmd.c uftp_tab.c uftp.h list.h list.c env.c getline.c getline.h glob.h cdefs.h glob.c

LIBS=../libftp.a libetc.a

###############################################################################
#
# Basic Rules
#
###############################################################################


all: $(ALL)

uftp:	$(OBJS) ../libftp.a libetc.a
	$(CC) $(CFLAGS) -o uftp $(OBJS) -lftp -letc 

clean: 
	rm -f $(ALL) $(OBJS) libetc.a $(LIBOBJS) *~ core
	
###############################################################################
#
# Depending from main library (libftp)
#
###############################################################################



../libftp.a: $(shell echo ../*.[ch])
	@cd ..;$(MAKE) CFLAGS="$(CFLAGS)" CC="$(CC)" 

###############################################################################
#
# Library with help functions
#
###############################################################################



LIBOBJS= getline.o glob.o env.o list.o

libetc.a: $(LIBOBJS)	
	ar cruv libetc.a  $(LIBOBJS)
	ranlib libetc.a


	

$(SRCS):
	$(CO) $@

# DO NOT DELETE THIS LINE -- make depend depends on it.

getline.o: getline.h
glob.o: glob.h cdefs.h
glob.o: cdefs.h
list.o: list.h uftp.h glob.h cdefs.h getline.h
uftp.o: uftp.h glob.h cdefs.h getline.h list.h
uftp.o: glob.h cdefs.h getline.h list.h
uftp_tab.o: uftp.h glob.h cdefs.h getline.h list.h
uftpcmd.o: uftp.h glob.h cdefs.h getline.h list.h
