TARGET_OS=$(shell echo __`uname | sed 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'`__)
LIBFTP_DIR=libftp-5.0.1.modified.by.in

extract-xiso: extract-xiso.c
	@echo "compiling extract-xiso for ${TARGET_OS}"
	@echo
	make libftp
ifeq (${TARGET_OS},__DARWIN__)
	gcc -O2 -o extract-xiso -D${TARGET_OS} extract-xiso.c ${LIBFTP_DIR}/libftp.a -framework CoreFoundation -framework DiscRecording
else
	gcc -O2 -o extract-xiso -D${TARGET_OS} extract-xiso.c ${LIBFTP_DIR}/libftp.a
endif

static:
	@echo "compiling static extract-xiso for ${TARGET_OS}"
	@echo
	make libftp
	gcc -O2 -static -o extract-xiso-static -D${TARGET_OS} extract-xiso.c ${LIBFTP_DIR}/libftp.a
	
debug:
	@echo "compiling debug extract-xiso for ${TARGET_OS}"
	@echo
	make libftp-debug
ifeq (${TARGET_OS},__DARWIN__)
	gcc -g -o extract-xiso -D${TARGET_OS} -DDEBUG extract-xiso.c ${LIBFTP_DIR}/libftp.a -framework CoreFoundation -framework DiscRecording
else
	gcc -g -o extract-xiso -D${TARGET_OS} -DDEBUG extract-xiso.c ${LIBFTP_DIR}/libftp.a
endif

libftp:
	@rm -f ${LIBFTP_DIR}/libftp.a
	/bin/bash -c 'declare -x CFLAGS="-I. -O2 -D${TARGET_OS}" ; make -C ${LIBFTP_DIR}'

libftp-debug:
	@rm -f ${LIBFTP_DIR}/libftp.a
	/bin/bash -c 'declare -x CFLAGS="-I. -g -D${TARGET_OS}" ; make -C ${LIBFTP_DIR}'

clean:
	make -C ${LIBFTP_DIR} clean
	rm -f extract-xiso
	rm -f extract-xiso-static
	rm -f .gdb_history

dist:
	make clean
	make
ifeq (${TARGET_OS},__LINUX__)
	make static
endif
