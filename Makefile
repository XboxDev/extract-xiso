# this makefile requires GNU make.  on FreeBSD run 'gmake' instead of make

TARGET_OS=$(shell echo __`uname | sed 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'`__)
LIBFTP_DIR=libftp-5.0.1.modified.by.in

ifeq (${TARGET_OS},__FREEBSD__)
MAKE=gmake
LD_FLAGS="-lc_r"
else
MAKE=make
LD_FLAGS=
endif

extract-xiso: extract-xiso.c
	@echo "compiling extract-xiso for ${TARGET_OS}"
	@echo
	${MAKE} libftp
ifeq (${TARGET_OS},__DARWIN__)
	gcc -O2 ${LD_FLAGS} -o extract-xiso -D${TARGET_OS} extract-xiso.c ${LIBFTP_DIR}/libftp.a -framework CoreFoundation -framework DiscRecording
else
	gcc -O2 ${LD_FLAGS} -o extract-xiso -D${TARGET_OS} extract-xiso.c ${LIBFTP_DIR}/libftp.a
endif

static:
	@echo "compiling static extract-xiso for ${TARGET_OS}"
	@echo
	${MAKE} libftp
	gcc -O2 -static -o extract-xiso-static -D${TARGET_OS} extract-xiso.c ${LIBFTP_DIR}/libftp.a
	
debug:
	@echo "compiling debug extract-xiso for ${TARGET_OS}"
	@echo
	${MAKE} libftp-debug
ifeq (${TARGET_OS},__DARWIN__)
	gcc -g -o extract-xiso -D${TARGET_OS} -DDEBUG extract-xiso.c ${LIBFTP_DIR}/libftp.a -framework CoreFoundation -framework DiscRecording
else
	gcc -g -o extract-xiso -D${TARGET_OS} -DDEBUG extract-xiso.c ${LIBFTP_DIR}/libftp.a
endif

libftp:
	@rm -f ${LIBFTP_DIR}/libftp.a
	bash -c 'declare -x CFLAGS="-I. -O2 -D${TARGET_OS}" ; ${MAKE} -C ${LIBFTP_DIR}'

libftp-debug:
	@rm -f ${LIBFTP_DIR}/libftp.a
	bash -c 'declare -x CFLAGS="-I. -g -D${TARGET_OS}" ; ${MAKE} -C ${LIBFTP_DIR}'

clean:
	${MAKE} -C ${LIBFTP_DIR} clean
	rm -f extract-xiso
	rm -f extract-xiso-static
	rm -f .gdb_history

dist:
	${MAKE} clean
	${MAKE}
ifeq (${TARGET_OS},__LINUX__)
	${MAKE} static
endif
