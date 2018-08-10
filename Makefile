# this makefile requires GNU make.  on FreeBSD run 'gmake' instead of make

TARGET_OS=$(shell echo __`uname | sed 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'`__)

ifeq (${TARGET_OS},__FREEBSD__)
MAKE=gmake
LD_FLAGS="-lc_r"
else
MAKE=make
LD_FLAGS=
endif

ifeq (${TARGET_OS},__OPENBSD__)
MAKE=gmake
LD_FLAGS=
endif


extract-xiso: extract-xiso.c
	@echo "compiling extract-xiso for ${TARGET_OS}"
	@echo
ifeq (${TARGET_OS},__DARWIN__)
	@echo "NOTE:  If you want to build a universal binary on Mac OS X, do not use this target!"
	@echo "       Instead, do \"make clean\" then \"make fat\"."
	@echo "       Additionally, you MUST have the 10.4u SDK installed (XCode >= 2.2.1)."
	@echo
endif
ifeq (${TARGET_OS},__DARWIN__)
	gcc -O2 ${LD_FLAGS} -o extract-xiso -D${TARGET_OS} extract-xiso.c -framework CoreFoundation -framework DiscRecording
else
	gcc -O2 ${LD_FLAGS} -o extract-xiso -D${TARGET_OS} extract-xiso.c
endif

ifeq (${TARGET_OS},__DARWIN__)
fat:
	@echo "compiling extract-xiso as universal binary for ${TARGET_OS}"
	@echo
#	old code to build on Mac OS X for PPC
#	CFLAGS="-I. -O2 -D${TARGET_OS} -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -mtune=G5" ${MAKE} -C ${LIBFTP_DIR}
#	gcc -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -mtune=G5 -O2 ${LD_FLAGS} -o extract-xiso.ppc -D${TARGET_OS} extract-xiso.c ${LIBFTP_DIR}/libftp.a -framework CoreFoundation -framework DiscRecording
	CFLAGS="-I. -O2 -D${TARGET_OS} -isysroot /Developer/SDKs/MacOSX10.7.sdk -arch i386 -mtune=pentium4" ${MAKE}
	gcc -isysroot /Developer/SDKs/MacOSX10.7.sdk -arch i386 -mtune=pentium4 -O2 ${LD_FLAGS} -o extract-xiso.i386 -D${TARGET_OS} extract-xiso.c -framework CoreFoundation -framework DiscRecording
#	old code to build on Mac OS X for PPC
#	CFLAGS="-I. -O2 -D${TARGET_OS} -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -mtune=pentium4" ${MAKE} -C ${LIBFTP_DIR}
#	gcc -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -mtune=pentium4 -O2 ${LD_FLAGS} -o extract-xiso.i386 -D${TARGET_OS} extract-xiso.c ${LIBFTP_DIR}/libftp.a -framework CoreFoundation -framework DiscRecording
#	lipo -create extract-xiso.ppc extract-xiso.i386 -output extract-xiso
#	rm -f extract-xiso.ppc extract-xiso.i386
	CFLAGS="-I. -O2 -D${TARGET_OS} -isysroot /Developer/SDKs/MacOSX10.7.sdk -arch x86_64" ${MAKE}
	gcc -isysroot /Developer/SDKs/MacOSX10.7.sdk -arch x86_64 -O2 ${LD_FLAGS} -o extract-xiso.x86_64 -D${TARGET_OS} extract-xiso.c -framework CoreFoundation -framework DiscRecording
	lipo -create extract-xiso.x86_64 extract-xiso.i386 -output extract-xiso
	rm -f extract-xiso.ppc extract-xiso.i386 extract-xiso.x86_64
endif

static:
	@echo "compiling static extract-xiso for ${TARGET_OS}"
	@echo
	gcc -O2 -static -o extract-xiso-static -D${TARGET_OS} extract-xiso.c -lpthread
	
debug:
	@echo "compiling debug extract-xiso for ${TARGET_OS}"
	@echo
ifeq (${TARGET_OS},__DARWIN__)
	gcc -g -o extract-xiso -D${TARGET_OS} -DDEBUG extract-xiso.c -framework CoreFoundation -framework DiscRecording
else
	gcc -g -o extract-xiso -D${TARGET_OS} -DDEBUG extract-xiso.c -lpthread
endif

clean:
	rm -f .gdb_history extract-xiso extract-xiso-static extract-xiso.ppc extract-xiso.i386

dist:
	${MAKE} clean
	${MAKE}
ifeq (${TARGET_OS},__LINUX__)
	${MAKE} static
endif
