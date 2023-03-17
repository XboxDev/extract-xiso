/*
	extract-xiso.c

	An xdvdfs .iso (xbox iso) file extraction and creation tool by in <in@fishtank.com>
		written March 10, 2003


	View this file with your tab stops set to 4 spaces or it it will look wacky.


	Regarding licensing:

	I think the GPL sucks!  (it stands for Generosity Poor License)

	My open-source code is really *FREE* so you can do whatever you want with it,
	as long as 1) you don't claim that you wrote my code and 2) you retain a notice
	that some parts of the code are copyright in@fishtank.com and 3) you understand
	there are no warranties.  I only guarantee that it will take up disk space!

	If you want to help out with this project it would be welcome, just email me at
	in@fishtank.com.

	This code is copyright in@fishtank.com and is licensed under a slightly modified
	version of the Berkeley Software License, which follows:

	/*
	 * Copyright (c) 2003 in <in@fishtank.com>
	 * All rights reserved.
	 *
	 * Redistribution and use in source and binary forms, with or without
	 * modification, are permitted provided that the following conditions
	 * are met:
	 *
	 * 1. Redistributions of source code must retain the above copyright
	 *    notice, this list of conditions and the following disclaimer.
	 *
	 * 2. Redistributions in binary form must reproduce the above copyright
	 *    notice, this list of conditions and the following disclaimer in the
	 *    documentation and/or other materials provided with the distribution.
	 *
	 * 3. All advertising materials mentioning features or use of this software
	 *    must display the following acknowledgement:
	 *
	 *    This product includes software developed by in <in@fishtank.com>.
	 *
	 * 4. Neither the name "in" nor the email address "in@fishtank.com"
	 *    may be used to endorse or promote products derived from this software
	 *    without specific prior written permission.
	 *
	 * THIS SOFTWARE IS PROVIDED `AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES
	 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
	 * AUTHOR OR ANY CONTRIBUTOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	 *\
	
	
	Last modified:
	
		03.29.03 in:	Fixed a path display bug, changed the tree descent algorithm
						and added ftp to xbox support (rev to v1.2)
						
		04.04.03 in:	Added a counter for total number of files in xiso (rev to
						v1.2.1)  THIS VERSION NOT FOR RELEASE!
						
		04.18.03 in:	Added xoff_t typecasts for __u32 * __u32 manipulations.
						This fixed a bug with very large iso's where the directory
						table was at the end of the iso--duh! (rev to v1.3)
						
		04.19.03 in:	A user pointed out that the program is increasing its
						memory usage over large iso's.  I've tracked this to the buffer
						allocation in extract_file() during traverse_xiso()
						recursions.  As a fix I've moved the copy buffer to a static
						variable.  Not as encapsulated as I'd like but hey, this is
						C after all.
						
						Also added support for FreeBSD (on Intel x86) (rev to v1.4)
						
		04.21.03 in:	It looks like whomever is making xiso creation tools out there
						has never heard of a binary tree and is sticking *every*
						directory entry off the right subnode (at least on all the iso's
						I've found so far).  This causes extremely deep recursion for
						iso's with lots of files (and also causes these iso's, when
						burned to DVD, to behave as a linked list for file lookups, thus
						providing *worst case* lookup performance at all times).
						
						Not only do I find this annoying and extremely bad programming,
						I've noticed that it is causing sporadic stack overflows with
						my (formerly) otherwise good tree traversal code.
						
						In order to combat such bad implementations, I've re-implemented
						the traverse_xiso() routine to get rid of any non-directory
						recursion.  Additionally, I've made a few extra tweaks to
						conserve even more memory.  I can see now that I'm going to need
						to write xiso creation as well and do it right. (rev to v1.5 beta)
						NOT FOR RELEASE
		
		04.22.03 in:	Making some major changes...
		
						Working on the optimization algorithm, still needs some tweaks
						apparently.  DO NOT RELEASE THIS SOURCE BUILD!
						
						NOTE:  I'm building this as 1.5 beta and sending the source to
						Emil only, this source is not yet for release.

		04.28.03 in:	I've finally decided that optimizing in-place just *isn't* going
						to happen.  The xbox is *really* picky about how its b-trees
						are laid out.  I've noticed that it will read the directory if
						I lay the entries out in prefix order.  Seems kind of weird to
						me that it would *have* to be that way but whatever.  So, I guess
						I'll write xiso creation and then piggyback a rewrite type op
						on top of it.  Not quite as nice since it means you need extra
						disk space but such is life.

		05.01.03 in:	Well it looks like I got the creation code working tonight, what
						a pain in the ass *that* was.  I've been working on it in my free
						time (which is almost non-existent) for a week now, bleh.  Also
						decided to implement rewriting xisos and I think I'll add build
						xiso from ftp-server, just to be *really* lazy.  I guess this
						means I'll have to read the stat code in the ftp tree.  Hmmm,
						probably need to dig around in there anyway...  A user reported
						that newer builds of evox are barfing with ftp upload so I guess
						I'll go debug that.  
						
						Also cleaned up the code quite a bit tonight just for posterity.
						I'd just like to point out that I *know* I'm being really lazy with
						all these big-ass functions and no header files and such.  The fact
						is I just can't seem to bring myself to care woohaahaa!
						
						(rev to 2.0 beta)  NOT FOR RELEASE until I get the other goodies
						written ;)
						
		05.03.03 in:	Added support for create xiso from ftp server.  Still need to debug
						evox to see what the problem is-- looks like something to do for
						tomorrow!
						
		05.06.03 in:	Finally got back to this little project ;0 -- the ftp bug was that
						FtpWriteBlock() in the libftp kit was timing out on occasion and returning
						less than a complete buffer.  So I fixed that and some other little
						bugs here and there, plus I changed the handling of the create mode
						so that you can now specify an iso name.  Hopefully that's a bit more
						intuitive.
						
		05.10.03 in:	Fixed a lot of stuff by now, I think it's solid for 2.0 release.
						(rev to 2.0, release)

		05.13.03 in:	Oops, fixed a bug in main() passing an (essentially) nil pointer to
						create_xiso that was causing a core dump and cleaned up the avl_fetch()
						and avl_insert() routines.  (rev to 2.1, release)
						
		05.14.03 in:	Added media check fix, fixed a bug in the ftp library where FtpStat was
						failing on filenames with spaces in them.
						
		06.16.03 in:	Based on code from zeek, I added support for win32, minus ftp
						functionality.  Neither he nor I have the time to port the ftp library
						to windows right now, but at least the creation code will work.  Big thanks
						to zeek for taking the time to wade through the source and figure out
						what needed to be tweaked for the windows build.

		06.20.03 in:	Well I just couldn't release the windows build without ftp support (can
						you say OCD <g> ;-), anyway I sat down today and ported the ftp library
						to win32.  That was a major pain let me tell you as I don't have a decent
						PC to run windows on (all my decent PC's run linux) and I've never really
						programmed anything on Windows.  Who'd have known that I couldn't just use
						fdopen() to convert a socket descriptor to a FILE *!  Anyway, whining aside
						I think it all works the way it's supposed to.  I'm sure once I throw it on
						the PC community I'll have plenty of bug reports, but such is life.  I also
						fixed a few other minor glitches here and there that gcc never complained
						about but that vc++ didn't like.

		07.15.03 in:	Fixed a bug first reported by Metal Maniac (thanks) where the path string was
						being generated improperly during xiso creation on windows.  Special thanks to
						Hydra for submitting code that mostly fixed the problem, I needed to make a few
						more tweaks but nothing much.  Hopefully this will solve the problem.  Also,
						thanks to Huge for doing a Win32 GUI around extract-xiso 2.2!  Rev to 2.3, release.
						
		07.16.03 in:	Changed some of the help text, looks like I introduced a copy-paste
						bug somewhere along the line.  Oops.
						
		07.28.03 in:	Added support for progress updating to create_xiso, now just pass in
						a pointer to a progress_callback routine (see typedef below).  Also added
						support on darwin for burning the iso to cd/dvd.  For some reason right now
						everything works fine if I try to burn an image to a DVD, but if I try to
						insert a cd it dies.  I have no idea as of yet what's wrong.  I am strongly
						considering *not* opensourcing my cd-burning stuff once I get it working
						as I can think of a few commercial uses for it.  Have to mull that one
						over a bit more.  This version only for release to UI developers.
						
		12.02.03 in:	Fixed a few bugs in the ftp subsystem and increased the read-write buffer size
						to 2Mb.  That should help ftp performance quite a bit.
						
		10.29.04 in:	Well, it's been a looooong time since I've worked on this little program...
						I've always been irritated by the fact that extract-xiso would never create an
						iso that could be auto-detected by CD/DVD burning software.  To burn iso's I've
						always had to go in and select a manual sector size of 2048 bytes, etc.  What
						a pain!  As a result, I've been trying to get my hands on the Yellow Book for
						ages.  I never did manage that as I didn't want to pay for it but I did some
						research the other day and came across the ECMA-119 specification.  It lays
						out the exact volume format that I needed to use.  Hooray!  Now xiso's are
						autodetected and burned properly by burning software...
						
						If you try to follow what I've done and find the write_volume_descriptors()
						method cryptic, just go download the ecma-119 specification from the ecma
						website.  Read about primary volume descriptors and it'll make sense.
						
						Bleh! This code is ugly ;-)

		10.25.05 in:	Added in  patch from Nordman.  Thanks.
						Added in security patch from Chris Bainbridge.  Thanks.
						Fixed a few minor bugs.

		01.18.10 aiyyo:	XBox 360 iso extraction supported.

		10.04.10 aiyyo:	Added new command line switch (-s skip $SystemUpdate folder).
						Display file progress in percent.
						Try to create destination directory.

		10.11.10 aiyyo:	Fix -l bug (empty list).

		05.02.11 aiyyo:	Remove security patch.

		09.30.11 somski: Added XGD3 support

		01.11.14 twillecomme: Intégration of aiyyo's and somski's work. 
						Minor warn fixes.

	enjoy!
	
	in
*/

#if defined(__linux__)
	#define _LARGEFILE_SOURCE
	#define	_FILE_OFFSET_BITS	64
	#define	_TIME_BITS			64
#endif

#if defined(__GNUC__)
	#define _GNU_SOURCE
	#define unused	__attribute__((__unused__))
#elif defined(_MSC_VER)
	#pragma warning(disable: 4706)	// C4706: assignment within conditional expression
	#define unused	__pragma(warning(suppress:4100))	/* This unfortunately disables the warning for the whole line and the next one */
#else
	#define unused
#endif

#ifndef DEBUG
	#define DEBUG	0
#endif

#if DEBUG
	#define unused_release
#else
	#define unused_release	unused
#endif

#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(__FreeBSD__) || defined(__OpenBSD__)
	#include <sys/limits.h>
#endif

#if defined(_WIN32)
	#include <direct.h>
	#include "win32/dirent.c"
	#include "win32/getopt.c"
#else
	#include <dirent.h>
	#include <limits.h>
	#include <unistd.h>
#endif

#if defined(_MSC_VER)
	#include "win32/asprintf.c"
	#include <BaseTsd.h>
#else
	#include <strings.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
	#define exiso_target				"macOS"

	#include <libkern/OSByteOrder.h>
	#define bswap_16(x)					OSSwapInt16(x)
	#define bswap_32(x)					OSSwapInt32(x)
	#define bswap_64(x)					OSSwapInt64(x)
#elif defined(__FreeBSD__)
	#define exiso_target				"FreeBSD"

	#include <sys/endian.h>
	#define bswap_16(x)					bswap16(x)
	#define bswap_32(x)					bswap32(x)
	#define bswap_64(x)					bswap64(x)
#elif defined(__OpenBSD__)
	#define exiso_target				"OpenBSD"

	#include <sys/types.h>
	#define bswap_16(x)					swap16(x)
	#define bswap_32(x)					swap32(x)
	#define bswap_64(x)					swap64(x)
#elif defined(__NetBSD__)
	#define exiso_target				"NetBSD"

	#include <sys/types.h>
	#include <machine/bswap.h>
	#if defined(__BSWAP_RENAME) && !defined(__bswap_16)
		#define bswap_16(x)				bswap16(x)
		#define bswap_32(x)				bswap32(x)
		#define bswap_64(x)				bswap64(x)
	#endif
#elif defined(__linux__)
	#define exiso_target				"Linux"

	#include <byteswap.h>
#elif defined(_WIN32) || defined(__CYGWIN__)
	#define exiso_target				"Windows"

	#if defined(_MSC_VER)
		#define S_ISDIR(x)				((x) & _S_IFDIR)
		#define S_ISREG(x)				((x) & _S_IFREG)

		typedef SSIZE_T					ssize_t;
		#define strcasecmp				_stricmp
		#define strncasecmp				_strnicmp
	#endif

	#if defined(__CYGWIN__)
		#include <byteswap.h>
	#else
		#include <stdlib.h>

		#define lseek					_lseeki64
		#define mkdir(a, b)				_mkdir(a)
		#define stat					_stat64
		#define realpath(a, b)			_fullpath(b, a, _MAX_PATH)

		#define bswap_16(x)				_byteswap_ushort(x)
		#define bswap_32(x)				_byteswap_ulong(x)
		#define bswap_64(x)				_byteswap_uint64(x)
	#endif
#else
	#error unknown target, cannot compile!
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
	#define PATH_CHAR					'\\'
	#define PATH_CHAR_STR				"\\"

	#define READFLAGS					O_RDONLY | O_BINARY
	#define WRITEFLAGS					O_WRONLY | O_CREAT | O_TRUNC | O_BINARY
	#define READWRITEFLAGS				O_RDWR   | O_BINARY

	typedef int64_t                     xoff_t;
#else
	#define PATH_CHAR					'/'
	#define PATH_CHAR_STR				"/"

	#define READFLAGS					O_RDONLY
	#define WRITEFLAGS					O_WRONLY | O_CREAT | O_TRUNC
	#define READWRITEFLAGS				O_RDWR

	typedef	off_t						xoff_t;
#endif

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)	/* All BSD systems, but this macro is unused anyways */
	#define FORCE_ASCII					1
#else
	#define FORCE_ASCII					0
#endif

#if CHAR_BIT != 8
	#error unsupported char size, cannot compile!
#endif

#if !defined(BIG_ENDIAN)
	#define BIG_ENDIAN		1
	#define LITTLE_ENDIAN	0
#endif
#define UNK_ENDIAN			-1

#if defined(CMAKE_ENDIANNESS)
	#if CMAKE_ENDIANNESS == BIG_ENDIAN
		#define USE_BIG_ENDIAN
	#elif CMAKE_ENDIANNESS != LITTLE_ENDIAN
		#error unknown endianness, cannot compile!
	#endif
#endif

#if defined(USE_BIG_ENDIAN)
	#define big16(n)
	#define big32(n)
	#define big64(n)
	#define little16(n)					( (n) = bswap_16(n) )
	#define little32(n)					( (n) = bswap_32(n) )
	#define little64(n)					( (n) = bswap_64(n) )
#else
	#define big16(n)					( (n) = bswap_16(n) )
	#define big32(n)					( (n) = bswap_32(n) )
	#define big64(n)					( (n) = bswap_64(n) )
	#define	little16(n)
	#define little32(n)
	#define little64(n)
#endif


#define DEBUG_VERIFY_XISO				0
#define DEBUG_OPTIMIZE_XISO				0
#define DEBUG_TRAVERSE_XISO_DIR			0


#if ! defined( __cplusplus ) && ! defined( bool )
	typedef int bool;
	enum { false, true };
#endif

typedef int64_t							file_time_t;

#ifndef nil
	#define nil							0
#endif


#define exiso_version					"2.7.1 (01.11.14)"
#define VERSION_LENGTH					16

#define banner							"extract-xiso v" exiso_version " for " exiso_target " - written by in <in@fishtank.com>\n"

#define usage() 						fprintf( stderr, \
"%s\n\
  Usage:\n\
\n\
    %s [options] [-[lrx]] <file1.xiso> [file2.xiso] ...\n\
    %s [options] -c <dir> [name] [-c <dir> [name]] ...\n\
\n\
  Mutually exclusive modes:\n\
\n\
    -c <dir> [name]     Create xiso from file(s) starting in <dir>.  If the\n\
                          [name] parameter is specified, the xiso will be\n\
                          created with the (path and) name given, otherwise\n\
                          the xiso will be created in the current directory\n\
                          with the name <dir>.iso.  The -c option may be\n\
                          specified multiple times to create multiple xiso\n\
                          images.\n\
    -l                  List files in xiso(s).\n\
    -r                  Rewrite xiso(s) as optimized xiso(s).\n\
    -x                  Extract xiso(s) (the default mode if none is given).\n\
                          If no directory is specified with -d, a directory\n\
                          with the name of the xiso (minus the .iso portion)\n\
                          will be created in the current directory and the\n\
                          xiso will be expanded there.\n\
\n\
  Options:\n\
\n\
    -d <directory>      In extract mode, expand xiso in <directory>.\n\
                        In rewrite mode, rewrite xiso in <directory>.\n\
    -D                  In rewrite mode, delete old xiso after processing.\n\
    -h                  Print this help text and exit.\n\
    -m                  In create or rewrite mode, disable automatic .xbe\n\
                          media enable patching (not recommended).\n\
    -q                  Run quiet (suppress all non-error output).\n\
    -Q                  Run silent (suppress all output).\n\
    -s                  Skip $SystemUpdate folder.\n\
    -v                  Print version information and exit.\n\
", banner, argv[ 0 ], argv[ 0 ] );

#define exiso_log(...)					do{ if ( ! s_quiet ) { printf(__VA_ARGS__); } } while(0)
#define exiso_warn(...)					do{ if ( ! s_quiet ) { printf("\nWARNING: " __VA_ARGS__); s_warned = true; } } while(0)
#define flush()							do{ if ( ! s_quiet ) { fflush( stdout ); } } while(0)

#define mem_err()						do{ log_err( __FILE__, __LINE__, "out of memory error" ); err = 1; } while(0)
#define read_err()						do{ log_err( __FILE__, __LINE__, "read error: %s", strerror( errno ) ); err = 1; } while(0)
#define seek_err()						do{ log_err( __FILE__, __LINE__, "seek error: %s", strerror( errno ) ); err = 1; } while(0)
#define write_err()						do{ log_err( __FILE__, __LINE__, "write error: %s", strerror( errno ) ); err = 1; } while(0)
#define rread_err()						do{ log_err( __FILE__, __LINE__, "unable to read remote file" ); err = 1; } while(0)
#define rwrite_err()					do{ log_err( __FILE__, __LINE__, "unable to write to remote file" ); err = 1; } while(0)
#define unknown_err()					do{ log_err( __FILE__, __LINE__, "an unrecoverable error has occurred" ); err = 1; } while(0)
#define open_err( in_file )				do{ log_err( __FILE__, __LINE__, "open error: %s %s", ( in_file ), strerror( errno ) ); err = 1; } while(0)
#define chdir_err( in_dir )				do{ log_err( __FILE__, __LINE__, "unable to change to directory %s: %s", ( in_dir ), strerror( errno ) ); err = 1; } while(0)
#define mkdir_err( in_dir )				do{ log_err( __FILE__, __LINE__, "unable to create directory %s: %s", ( in_dir ), strerror( errno ) ); err = 1; } while(0)
#define ropen_err( in_file )			do{ log_err( __FILE__, __LINE__, "unable to open remote file %s", ( in_file ) ); err = 1; } while(0)
#define rchdir_err( in_dir )			do{ log_err( __FILE__, __LINE__, "unable to change to remote directory %s", ( in_dir ) ); err = 1; } while(0)
#define rmkdir_err( in_dir )			do{ log_err( __FILE__, __LINE__, "unable to create remote directory %s", ( in_dir ) ); err = 1; } while(0)
#define misc_err( ... )					do{ log_err( __FILE__, __LINE__, __VA_ARGS__ ); err = 1; } while(0)


#ifndef min
	#define min( a , b )				( ( a ) < ( b ) ? ( a ) : ( b ) )
	#define max( a , b )				( ( a ) > ( b ) ? ( a ) : ( b ) )
#endif


#define START_LSEEK_OFFSET				0x00000000ul
#define XGD3_LSEEK_OFFSET				0x02080000ul
#define GLOBAL_LSEEK_OFFSET				0x0FD90000ul
#define XGD1_LSEEK_OFFSET				0x18300000ul
#define LSEEK_OFFSETS_LEN				4
/* The offsets should be in ascending order, otherwise we could get a seek error before checking the correct one */
xoff_t lseek_offsets[LSEEK_OFFSETS_LEN] = {START_LSEEK_OFFSET, XGD3_LSEEK_OFFSET, GLOBAL_LSEEK_OFFSET, XGD1_LSEEK_OFFSET};

#define	XISO_HEADER_DATA				"MICROSOFT*XBOX*MEDIA"
#define XISO_HEADER_DATA_LENGTH			20
#define XISO_HEADER_OFFSET				0x10000

#define XISO_FILE_MODULUS				0x10000
	
#define XISO_ROOT_DIRECTORY_SECTOR		0x108

#define XISO_OPTIMIZED_TAG_OFFSET		31337
#define XISO_OPTIMIZED_TAG				"in!xiso!" exiso_version
#define XISO_OPTIMIZED_TAG_LENGTH		( 8 + VERSION_LENGTH )
#define XISO_OPTIMIZED_TAG_LENGTH_MIN	7

#define XISO_ATTRIBUTES_SIZE			1
#define XISO_FILENAME_LENGTH_SIZE		1
#define XISO_TABLE_OFFSET_SIZE			2
#define XISO_SECTOR_OFFSET_SIZE			4
#define XISO_DIRTABLE_SIZE				4
#define XISO_FILESIZE_SIZE				4
#define XISO_DWORD_SIZE					4
#define XISO_FILETIME_SIZE				sizeof(file_time_t)

#define XISO_SECTOR_SIZE				2048
#define XISO_UNUSED_SIZE				0x7c8

#define XISO_FILENAME_OFFSET			14
#define XISO_FILENAME_LENGTH_OFFSET		( XISO_FILENAME_OFFSET - 1 )
#define XISO_FILENAME_MAX_CHARS			255

#define XISO_ATTRIBUTE_RO				0x01
#define XISO_ATTRIBUTE_HID				0x02
#define XISO_ATTRIBUTE_SYS				0x04
#define XISO_ATTRIBUTE_DIR				0x10
#define	XISO_ATTRIBUTE_ARC				0x20
#define XISO_ATTRIBUTE_NOR				0x80

#define XISO_PAD_BYTE					0xff
#define XISO_PAD_SHORT					0xffff

#define XISO_MEDIA_ENABLE				"\xe8\xca\xfd\xff\xff\x85\xc0\x7d"
#define XISO_MEDIA_ENABLE_BYTE			'\xeb'
#define XISO_MEDIA_ENABLE_LENGTH		8
#define XISO_MEDIA_ENABLE_BYTE_POS		7

#define n_sectors(size)					( (size) / XISO_SECTOR_SIZE + ( (size) % XISO_SECTOR_SIZE ? 1 : 0 ) )
#define n_dword(offset)					( (offset) / XISO_DWORD_SIZE + ( (offset) % XISO_DWORD_SIZE ? 1 : 0 ) )

#define EMPTY_SUBDIRECTORY				( (dir_node_avl *) 1 )

#define READWRITE_BUFFER_SIZE			0x00200000

#define DEBUG_DUMP_DIRECTORY			"/Volumes/c/xbox/iso/exiso"

#define GETOPT_STRING					"c:d:Dhlmp:qQrsvx"


typedef enum avl_skew { k_no_skew , k_left_skew , k_right_skew } avl_skew;
typedef enum avl_result { no_err, k_avl_error, k_avl_balanced } avl_result;
typedef enum avl_traversal_method { k_prefix, k_infix, k_postfix } avl_traversal_method;

typedef enum bm_constants { k_default_alphabet_size = 256 } bm_constants;

typedef enum modes { k_generate_avl, k_extract, k_list, k_rewrite } modes;
typedef enum errors { err_end_of_sector = -5001, err_iso_rewritten = -5002, err_iso_no_files = -5003 } errors;
typedef enum strategies { tree_strategy, discover_strategy } strategies;

typedef void (*progress_callback)( long long in_current_value, long long in_final_value );
typedef int (*traversal_callback)( void *in_node, void *in_context, int in_depth );

typedef struct dir_node dir_node;
typedef struct create_list create_list;
typedef struct dir_node_avl dir_node_avl;

struct dir_node {
	uint32_t							file_size;
	uint32_t							start_sector;
	uint8_t								attributes;
	uint8_t								filename_length;
	char*								filename;
};

struct dir_node_avl {
	uint32_t							offset;
	xoff_t								dir_start;

	char							   *filename;
	uint32_t							file_size;
	uint32_t							start_sector;
	dir_node_avl					   *subdirectory;
	
	uint32_t							old_start_sector;
	
	avl_skew							skew;
	dir_node_avl					   *left;
	dir_node_avl					   *right;
};

struct create_list {
	char							   *path;
	char							   *name;
	create_list						   *next;
};

typedef struct wdsafp_context {
	xoff_t								dir_start;
	uint32_t							*current_sector;
} wdsafp_context;

typedef struct write_tree_context {
	int									xiso;
	char							   *path;
	int									from;
	progress_callback					progress;
	long long							final_bytes;
} write_tree_context;


xoff_t lseek_with_error(int fd, xoff_t offset, int whence);
int log_err( const char *in_file, int in_line, const char *in_format, ... );
void avl_rotate_left( dir_node_avl **in_root );
void avl_rotate_right( dir_node_avl **in_root );
int avl_compare_key( char *in_lhs, char *in_rhs );
avl_result avl_left_grown( dir_node_avl **in_root );
avl_result avl_right_grown( dir_node_avl **in_root );
dir_node_avl *avl_fetch( dir_node_avl *in_root, char *in_filename );
avl_result avl_insert( dir_node_avl **in_root, dir_node_avl *in_node );
int avl_traverse_depth_first( dir_node_avl *in_root, traversal_callback in_callback, void *in_context, avl_traversal_method in_method, int in_depth );

void boyer_moore_done();
char *boyer_moore_search( char *in_text, long in_text_len );
int boyer_moore_init( char *in_pattern, long in_pat_len, long in_alphabet_size );

int free_dir_node_avl(void* in_dir_node_avl, void* in_context, int in_depth);
int extract_file( int in_xiso, dir_node *in_file, modes in_mode, char *path );
int decode_xiso( char *in_xiso, char *in_path, modes in_mode, char **out_iso_path );
int verify_xiso( int in_xiso, uint32_t *out_root_dir_sector, uint32_t *out_root_dir_size, char *in_iso_name );
int traverse_xiso(int in_xiso, xoff_t in_dir_start, uint16_t entry_offset, uint16_t end_offset, char* in_path, modes in_mode, dir_node_avl** in_root, strategies strategy);
int process_node(int in_xiso, dir_node* node, char* in_path, modes in_mode, dir_node_avl** in_root, strategies strategy);
int create_xiso( char *in_root_directory, char *in_output_directory, dir_node_avl *in_root, int in_xiso, char **out_iso_path, char *in_name, progress_callback in_progress_callback );

int get_filetime_now( file_time_t *ft );
int generate_avl_tree_local( dir_node_avl **out_root, int *io_n );
int generate_avl_tree_remote( dir_node_avl **out_root, int *io_n );
int write_directory( dir_node_avl *in_avl, write_tree_context* in_context, int in_depth );
int write_file( dir_node_avl *in_avl, write_tree_context *in_context, int in_depth );
int write_tree( dir_node_avl *in_avl, write_tree_context *in_context, int in_depth );
int calculate_total_files_and_bytes( dir_node_avl *in_avl, void *in_context, int in_depth );
int calculate_directory_size( dir_node_avl *in_avl, uint32_t *out_size, int in_depth );
int calculate_directory_requirements( dir_node_avl *in_avl, void *in_context, int in_depth );
int calculate_directory_offsets( dir_node_avl *in_avl, uint32_t *io_context, int in_depth );
int write_dir_start_and_file_positions( dir_node_avl *in_avl, wdsafp_context *io_context, int in_depth );
int write_volume_descriptors( int in_xiso, uint32_t in_total_sectors );

#if DEBUG
void write_sector( int in_xiso, xoff_t in_start, char *in_name, char *in_extension );
#endif


static long								s_pat_len;
static bool								s_quiet = false;
static char							   *s_pattern = nil;
static long							   *s_gs_table = nil;
static long							   *s_bc_table = nil;
static long long						s_total_bytes = 0;
static int								s_total_files = 0;
static char							   *s_copy_buffer = nil;
static bool								s_real_quiet = false;
static bool								s_media_enable = true;
static long long						s_total_bytes_all_isos = 0;
static int								s_total_files_all_isos = 0;
static bool								s_warned = false;

static bool				                s_remove_systemupdate = false;
static char				               *s_systemupdate = "$SystemUpdate";

static xoff_t							s_xbox_disc_lseek = 0;


#if 0		// #pragma mark - inserts a spacer in the function popup of certain text editors (i.e. mine ;-)
#pragma mark -
#endif


int main( int argc, char **argv ) {
	struct stat		sb;
	create_list	   *create = nil, *p, *q, **r;
	int				i, fd, opt_char, err = 0, isos = 0;
	bool			extract = true, rewrite = false, x_seen = false, delete = false, optimized;
	char		   *path = nil, *buf = nil, *new_iso_path = nil, tag[XISO_OPTIMIZED_TAG_LENGTH + 1];

	if ( argc < 2 ) { usage(); exit( 1 ); }
	
	while ( ! err && ( opt_char = getopt( argc, argv, GETOPT_STRING ) ) != -1 ) {
		switch ( opt_char ) {
			case 'c': {
				if ( x_seen || rewrite || ! extract ) {
					usage();
					exit( 1 );
				}
			
				for ( r = &create; *r != nil; r = &(*r)->next ) ;

				if ( ( *r = (create_list *) malloc( sizeof(create_list) ) ) == nil ) mem_err();
				if ( ! err ) {
					(*r)->name = nil;
					(*r)->next = nil;
					
					if ( ( (*r)->path = strdup( optarg ) ) == nil ) mem_err();
				}
				if ( ! err && argv[ optind ] && *argv[ optind ] != '-' && *argv[ optind ] && ( (*r)->name = strdup( argv[ optind++ ] ) ) == nil ) mem_err();
			} break;
			
			case 'd': {
				if ( path ) free( path );
				if ( ( path = strdup( optarg ) ) == nil ) mem_err();
			} break;
			
			case 'D': {
				delete = true;
			} break;

			case 'h': {
				usage();
				exit( 0 );
			} break;
			
			case 'l': {
				if ( x_seen || rewrite || create ) {
					usage();
					exit( 1 );
				}
				extract = false;
			} break;
			
			case 'm': {
				if ( x_seen || ! extract ) {
					usage();
					exit( 1 );
				}
				s_media_enable = false;
			} break;
			
			case 'q': {
				s_quiet = true;
			} break;
			
			case 'Q': {
				s_quiet = s_real_quiet = true;
			} break;
			
			case 'r': {
				if ( x_seen || ! extract || create ) {
					usage();
					exit( 1 );
				}
				rewrite = true;
			} break;

			case 's': {
				s_remove_systemupdate = true;
			} break;

			case 'v': {
				printf( "%s", banner );
				exit( 0 );
			} break;
			
			case 'x': {
				if ( ! extract || rewrite || create ) {
					usage();
					exit( 1 );
				}
				x_seen = true;
			} break;

			default: {
				usage();
				exit( 1 );
			} break;
		}
	}
	
	if ( ! err ) {

		if ( create ) { if ( optind < argc ) { usage(); exit( 1 ); } }
		else if ( optind >= argc ) { usage(); exit( 1 ); }
	
		exiso_log( "%s", banner );
	
		if ( ( extract ) && ( s_copy_buffer = (char *) malloc( READWRITE_BUFFER_SIZE ) ) == nil ) mem_err();
	}
	
	if ( ! err && ( create || rewrite ) ) err = boyer_moore_init( XISO_MEDIA_ENABLE, XISO_MEDIA_ENABLE_LENGTH, k_default_alphabet_size );

	if ( ! err && create ) {
		for ( p = create; ! err && p != nil; ) {
			char*		tmp = nil;
			ptrdiff_t	diff = 0;

			if ( p->name && (tmp = strrchr(p->name, PATH_CHAR)) ) {
				diff = tmp - p->name;
				if ( ( tmp = (char *) malloc( diff + 1 ) ) == nil ) mem_err();
				if ( ! err ) {
					strncpy( tmp, p->name, diff );
					tmp[ diff ] = 0;
				}
				diff += 1;
			}
			
			if ( ! err ) err = create_xiso( p->path, tmp, nil, -1, nil, p->name ? p->name + diff : nil, nil );

			if ( tmp ) free( tmp );

			q = p->next;

			if ( p->name ) free( p->name );
			free( p->path );
			free( p );
			
			p = q;
		}
	} else for ( i = optind; ! err && i < argc; ++i ) {
		++isos;
		s_total_bytes = s_total_files = 0;
		
		
		if ( ! err ) {
			optimized = false;
		
			if ( ( fd = open( argv[ i ], READFLAGS, 0 ) ) == -1 ) open_err( argv[ i ] );
			if ( ! err && lseek_with_error( fd, (xoff_t) XISO_OPTIMIZED_TAG_OFFSET, SEEK_SET ) == -1 ) seek_err();
			if ( ! err && read( fd, tag, XISO_OPTIMIZED_TAG_LENGTH ) != XISO_OPTIMIZED_TAG_LENGTH ) read_err();
			
			if ( fd != -1 ) close( fd );

			if ( ! err ) {
				tag[ XISO_OPTIMIZED_TAG_LENGTH ] = 0;

				if ( ! strncmp( tag, XISO_OPTIMIZED_TAG, XISO_OPTIMIZED_TAG_LENGTH_MIN ) ) optimized = true;

				if ( rewrite ) {
					if ( optimized ) {
						exiso_log( "\n%s is already optimized, skipping...\n", argv[ i ] );
						continue;
					}

					if ( ! err ) {
						if (asprintf(&buf, "%s.old", argv[i]) == -1) mem_err();
						if ( ! err && stat( buf, &sb ) != -1 ) misc_err( "%s already exists, cannot rewrite %s", buf, argv[ i ] );
						if ( ! err && rename( argv[ i ], buf ) == -1 ) misc_err( "cannot rename %s to %s", argv[ i ], buf );
						
						if ( err ) { err = 0; if ( buf ) free( buf ); continue; }
					}
					if ( ! err ) err = decode_xiso( buf, path, k_rewrite, &new_iso_path );
					if ( ! err && delete && unlink( buf ) == -1 ) log_err( __FILE__, __LINE__, "unable to delete %s", buf );
					
					if ( buf ) free( buf );
				} else {
					// the order of the mutually exclusive options here is important, the extract ? k_extract : k_list test *must* be the final comparison
					if ( ! err ) err = decode_xiso( argv[ i ], path, extract ? k_extract : k_list, nil );
				}
			}
		}
		
		if ( ! err ) exiso_log( "\n\n%u files in %s total %lld bytes\n", s_total_files, rewrite ? new_iso_path : argv[ i ], s_total_bytes );
		
		if ( new_iso_path ) {
			if ( ! err ) exiso_log( "\n%s successfully rewritten%s%s\n", argv[ i ], path ? " as " : ".", path ? new_iso_path : "" );
		
			free( new_iso_path );
			new_iso_path = nil;
		}
		
		if ( err == err_iso_no_files ) err = 0;
	}
	
	if ( ! err && isos > 1 ) exiso_log( "\n%u files in %u xiso's total %lld bytes\n", s_total_files_all_isos, isos, s_total_bytes_all_isos );
	if ( s_warned ) exiso_warn( "Warning(s) were issued during execution--review stderr!\n" );
	
	boyer_moore_done();
	
	if ( s_copy_buffer ) free( s_copy_buffer );
	if ( path ) free( path );
		
	return err;
}

/* Wrapper to avoid changing old code, since lseek will not return error if offset is beyond end of file. Use only on input. */
xoff_t lseek_with_error(int fd, xoff_t offset, int whence) {
	xoff_t pos = lseek(fd, offset, whence);	// First execute the seek, to save the offset
	if (pos == -1) return -1;
	xoff_t end = lseek(fd, 0, SEEK_END);	// Then save the end of the file
	if (end == -1) return -1;
	if (pos > end) {
		errno = EINVAL;
		return -1;
	}
	return lseek(fd, pos, SEEK_SET);		// Finally seek again to saved offset
}


int log_err(unused_release const char* in_file, unused_release int in_line, const char* in_format, ...) {
	va_list			ap;
	char			*format;
	int				ret;

#if DEBUG
	asprintf(&format, "%s:%u %s", in_file, in_line, in_format);
#else
	format = (char*)in_format;
#endif

	if (s_real_quiet) ret = 0;
	else if(format){
		va_start(ap, in_format);
		fprintf(stderr, "\n");
		ret = vfprintf(stderr, format, ap);
		fprintf(stderr, "\n");
		va_end(ap);
	}
	else ret = 1;

#if DEBUG
	if(format) free(format);
#endif

	return ret;
}


#if 0
#pragma mark -
#endif


int verify_xiso( int in_xiso, uint32_t *out_root_dir_sector, uint32_t *out_root_dir_size, char *in_iso_name ) {
	int				i, err = 0;
	char			buffer[XISO_HEADER_DATA_LENGTH];

	for (i = 0; !err && i < LSEEK_OFFSETS_LEN; i++) {
		if (lseek_with_error(in_xiso, (xoff_t)XISO_HEADER_OFFSET + lseek_offsets[i], SEEK_SET) == -1) seek_err();
		if (!err && read(in_xiso, buffer, XISO_HEADER_DATA_LENGTH) != XISO_HEADER_DATA_LENGTH) read_err();
		if (!err && !memcmp(buffer, XISO_HEADER_DATA, XISO_HEADER_DATA_LENGTH)) {
			s_xbox_disc_lseek = lseek_offsets[i];
			break;	// Found
		}
	}
	if (!err && i == LSEEK_OFFSETS_LEN) misc_err("%s does not appear to be a valid xbox iso image", in_iso_name);

	// read root directory information
	if ( ! err && read( in_xiso, out_root_dir_sector, XISO_SECTOR_OFFSET_SIZE ) != XISO_SECTOR_OFFSET_SIZE ) read_err();
	if ( ! err && read( in_xiso, out_root_dir_size, XISO_DIRTABLE_SIZE ) != XISO_DIRTABLE_SIZE ) read_err();

	little32( *out_root_dir_sector );
	little32( *out_root_dir_size );
	
	// seek to header tail and verify media tag
	if ( ! err && lseek_with_error(in_xiso, (xoff_t)XISO_FILETIME_SIZE + XISO_UNUSED_SIZE, SEEK_CUR) == -1) seek_err();
	if ( ! err && read( in_xiso, buffer, XISO_HEADER_DATA_LENGTH ) != XISO_HEADER_DATA_LENGTH ) read_err();
	if ( ! err && memcmp( buffer, XISO_HEADER_DATA, XISO_HEADER_DATA_LENGTH ) ) misc_err( "%s appears to be corrupt", in_iso_name );

	// seek to root directory sector
	if (!err && lseek_with_error(in_xiso, (xoff_t)*out_root_dir_sector * XISO_SECTOR_SIZE, SEEK_SET) == -1) seek_err();
	
	return err;
}




int create_xiso( char *in_root_directory, char *in_output_directory, dir_node_avl *in_root, int in_xiso, char **out_iso_path, char *in_name, progress_callback in_progress_callback ) {
	xoff_t					pos = 0;
	dir_node_avl			root = { 0 };
	file_time_t				ft = 0;
	write_tree_context		wt_context = { 0 };
	uint32_t				start_sector = 0;
	int						i = 0, n = 0, xiso = -1, err = 0;
	char				   *cwd = nil, *buf = nil, *iso_name = nil, *xiso_path = nil, *iso_dir = nil, *real_path = nil;

	s_total_bytes = s_total_files = 0;

	if ( ( cwd = getcwd( nil, 0 ) ) == nil ) mem_err();
	if ( ! err ) {
		if ( ! in_root ) {
			i = (int)strlen(in_root_directory) - 1;
			if ( in_root_directory[i] == '/' || in_root_directory[i] == '\\' ) in_root_directory[i] = 0;
			if ((iso_dir = strrchr(in_root_directory, PATH_CHAR))) iso_dir++;
			else iso_dir = in_root_directory;

			iso_name = in_name ? in_name : iso_dir;
		} else {
			iso_dir = iso_name = in_root_directory;
		}
	}
	if ( ! err ) {
		if ( ! *iso_dir ) iso_dir = PATH_CHAR_STR;
		if ( ! iso_name || ! *iso_name ) iso_name = "root";
		else if ( iso_name[ 1 ] == ':' ) { iso_name[ 1 ] = iso_name[ 0 ]; ++iso_name; }
		if (!err && (real_path = realpath(in_output_directory ? in_output_directory : ".", nil)) == nil) misc_err("unable to get absolute path of %s: %s", real_path, strerror(errno));
		if (!err && (asprintf(&xiso_path, "%s%c%s%s", real_path, PATH_CHAR, iso_name, in_name ? "" : ".iso")) == -1) mem_err();
		if (real_path) {
			free(real_path);
			real_path = nil;
		}

		if (!err && !in_root && chdir(in_root_directory) == -1) chdir_err(in_root_directory);
	}
	if ( ! err ) {
		exiso_log( "\n%s %s%s:\n", in_root ? "rewriting" : "creating", iso_name, in_name ? "" : ".iso" );

		root.start_sector = XISO_ROOT_DIRECTORY_SECTOR;		

		s_total_bytes = s_total_files = 0;

		if ( in_root ) {
			root.subdirectory = in_root;
			avl_traverse_depth_first( &root, (traversal_callback) calculate_total_files_and_bytes, nil, k_prefix, 0 );
		} else {
			n = 0;

			exiso_log("\ngenerating avl tree from filesystem: "); flush();
			
			err = generate_avl_tree_local( &root.subdirectory, &n );

			for ( i = 0; i < n; ++i ) exiso_log( "\b" );
			for ( i = 0; i < n; ++i ) exiso_log( " " );
			for ( i = 0; i < n; ++i ) exiso_log( "\b" );

			exiso_log( "%s\n", err ? "failed!" : "[OK]" );
		}
	}
	if ( ! err && in_progress_callback ) (*in_progress_callback)( 0, s_total_bytes );
	if ( ! err ) {
		wt_context.final_bytes = s_total_bytes;
		
		s_total_bytes = s_total_files = 0;
		
		start_sector = root.start_sector;
		
		avl_traverse_depth_first( &root, (traversal_callback) calculate_directory_requirements, nil, k_prefix, 0 );
		avl_traverse_depth_first( &root, (traversal_callback) calculate_directory_offsets, &start_sector, k_prefix, 0 );
	}
	if ( ! err && ( buf = (char *) malloc( n = max( READWRITE_BUFFER_SIZE, XISO_HEADER_OFFSET ) ) ) == nil ) mem_err();
	if ( ! err ) {
		if ( ( xiso = open( xiso_path, WRITEFLAGS, 0644 ) ) == -1 ) open_err( xiso_path );
		if (out_iso_path) *out_iso_path = xiso_path;
		else {
			free(xiso_path);
			xiso_path = nil;
		}
	}
	if ( ! err ) {
		memset( buf, 0, n );
		if ( write( xiso, buf, XISO_HEADER_OFFSET ) != XISO_HEADER_OFFSET ) write_err();
	}
	if ( ! err && write( xiso, XISO_HEADER_DATA, XISO_HEADER_DATA_LENGTH ) != XISO_HEADER_DATA_LENGTH ) write_err();
	if ( ! err ) {
		little32( root.start_sector );
		if ( write( xiso, &root.start_sector, XISO_SECTOR_OFFSET_SIZE ) != XISO_SECTOR_OFFSET_SIZE ) write_err();
		little32( root.start_sector );
	}
	if ( ! err ) {
		little32( root.file_size );
		if ( write( xiso, &root.file_size, XISO_DIRTABLE_SIZE ) != XISO_DIRTABLE_SIZE ) write_err();
		little32( root.file_size );
	}
	if ( ! err ) {
		if ( in_root ) {
			if (lseek_with_error(in_xiso, (xoff_t)XISO_HEADER_OFFSET + XISO_HEADER_DATA_LENGTH + XISO_SECTOR_OFFSET_SIZE + XISO_DIRTABLE_SIZE + s_xbox_disc_lseek, SEEK_SET) == -1) seek_err();
			if ( ! err && read( in_xiso, buf, XISO_FILETIME_SIZE ) != XISO_FILETIME_SIZE ) read_err();
			if ( ! err && write( xiso, buf, XISO_FILETIME_SIZE ) != XISO_FILETIME_SIZE ) write_err();			

			memset( buf, 0, XISO_FILETIME_SIZE );
		} else {
			if ( ( err = get_filetime_now(&ft) ) ) misc_err("cannot get current time");
			if ( ! err && write( xiso, &ft, XISO_FILETIME_SIZE ) != XISO_FILETIME_SIZE ) write_err();
		}
	}
	if ( ! err && write( xiso, buf, XISO_UNUSED_SIZE ) != XISO_UNUSED_SIZE ) write_err();
	if ( ! err && write( xiso, XISO_HEADER_DATA, XISO_HEADER_DATA_LENGTH ) != XISO_HEADER_DATA_LENGTH ) write_err();
	
	if ( ! err && ! in_root ) {
		if ( chdir( ".." ) == -1 ) chdir_err( ".." );
	}
	if ( ! err && ( root.filename = strdup( iso_dir ) ) == nil ) mem_err();

	if ( ! err && lseek( xiso, (xoff_t) root.start_sector * XISO_SECTOR_SIZE, SEEK_SET ) == -1 ) seek_err();
	if ( ! err ) {
		wt_context.path = nil;
		wt_context.xiso = xiso;
		wt_context.from = in_root ? in_xiso : -1;
		wt_context.progress = in_progress_callback;

		err = avl_traverse_depth_first( &root, (traversal_callback) write_tree, &wt_context, k_prefix, 0 );
	}

	if ( ! err && ( pos = lseek( xiso, (xoff_t) 0, SEEK_END ) ) == -1 ) seek_err();
	if (!err) {
		i = (int)((XISO_FILE_MODULUS - pos % XISO_FILE_MODULUS) % XISO_FILE_MODULUS);
		if (write(xiso, buf, i) != i) write_err();
	}

	if ( ! err ) err = write_volume_descriptors( xiso, (uint32_t)((pos + (xoff_t)i) / XISO_SECTOR_SIZE) );

	if ( ! err && lseek( xiso, (xoff_t) XISO_OPTIMIZED_TAG_OFFSET, SEEK_SET ) == -1 ) seek_err();
	if ( ! err && write( xiso, XISO_OPTIMIZED_TAG, XISO_OPTIMIZED_TAG_LENGTH ) != XISO_OPTIMIZED_TAG_LENGTH ) write_err();

	if ( ! in_root ) {
		if ( err ) exiso_log( "\ncould not create %s%s\n", iso_name ? iso_name : "xiso", iso_name && ! in_name ? ".iso" : "" );
		else exiso_log( "\nsucessfully created %s%s (%u files totalling %lld bytes added)\n", iso_name ? iso_name : "xiso", iso_name && ! in_name ? ".iso" : "", s_total_files, s_total_bytes );
	}
			
	if ( root.subdirectory != EMPTY_SUBDIRECTORY ) avl_traverse_depth_first( root.subdirectory, free_dir_node_avl, nil, k_postfix, 0 );
	
	if ( xiso != -1 ) {
		close( xiso );
		if (err && xiso_path) unlink(xiso_path);
	}
	
	if ( root.filename ) free( root.filename );
	if ( buf ) free( buf );

	if ( cwd ) {
		if ( chdir( cwd ) == -1 ) chdir_err( cwd );
		free( cwd );
	}
	
	return err;
}


int decode_xiso( char *in_xiso, char *in_path, modes in_mode, char **out_iso_path ) {
	dir_node_avl		   *root = nil;
	bool					repair = false;
	xoff_t					root_dir_start;
	uint32_t				root_dir_sect = 0, root_dir_size = 0;
	uint16_t				root_end_offset;
	int						xiso = 0, err = 0, len = 0, path_len = 0, add_slash = 0;
	char				   *buf = nil, *cwd = nil, *name = nil, *short_name = nil, *iso_name = nil;

	if ( ( xiso = open( in_xiso, READFLAGS, 0 ) ) == -1 ) open_err( in_xiso );
	
	if ( ! err ) {
		len = (int) strlen( in_xiso );
		
		if ( in_mode == k_rewrite ) {
			in_xiso[ len -= 4 ] = 0;
			repair = true;
		}
		
		if ((name = strrchr(in_xiso, PATH_CHAR))) name++;
		else name = in_xiso;
		
		len = (int) strlen( name );
		
		// create a directory of the same name as the file we are working on, minus the ".iso" portion
		if (len > 4 && strcasecmp(&name[len - 4], ".iso") == 0) {
			name[ len -= 4 ] = 0;
			if ( ( short_name = strdup( name ) ) == nil ) mem_err();
			name[ len ] = '.';
		}
	}

	if ( ! err && ! len ) misc_err( "invalid xiso image name: %s", in_xiso );

	if ( ! err && in_mode == k_extract && in_path ) {
		if ( ( cwd = getcwd( nil, 0 ) ) == nil ) mem_err();
		if ( ! err && mkdir( in_path, 0755 ) == -1 ) mkdir_err( in_path );
		if ( ! err && chdir( in_path ) == -1 ) chdir_err( in_path );
	}

	if ( ! err ) err = verify_xiso( xiso, &root_dir_sect, &root_dir_size, name );

	iso_name = short_name ? short_name : name;

	if ( ! err && in_mode != k_rewrite ) {
		exiso_log( "\n%s %s:\n", in_mode == k_extract ? "extracting" : "listing", name );

		if ( in_mode == k_extract ) {
			if ( ! in_path ) {
				if ( ( err = mkdir( iso_name, 0755 ) ) ) mkdir_err( iso_name );
				if ( ! err && ( err = chdir( iso_name ) ) ) chdir_err( iso_name );
			}
		}
	}

	if ( ! err ) {						
		if ( in_path ) {
			path_len = (int) strlen( in_path );
			if ( in_path[ path_len - 1 ] != PATH_CHAR ) ++add_slash;
		}
		
		if (!err) {
			if (asprintf(&buf, "%s%s%s%c", in_path ? in_path : "", add_slash && (!in_path) ? PATH_CHAR_STR : "", in_mode != k_list && (!in_path) ? iso_name : "", PATH_CHAR) == -1) mem_err();

			root_dir_start = (xoff_t)root_dir_sect * XISO_SECTOR_SIZE + s_xbox_disc_lseek;
			root_end_offset = (uint16_t)(n_sectors(root_dir_size) * XISO_SECTOR_SIZE / XISO_DWORD_SIZE);

			if (!err && lseek_with_error(xiso, root_dir_start, SEEK_SET) == -1) seek_err();
			
			if ( in_mode == k_rewrite ) {
				if (!err && root_dir_size == 0) root = EMPTY_SUBDIRECTORY;
				else {
					if (!err) err = traverse_xiso(xiso, root_dir_start, 0, root_end_offset, buf, k_generate_avl, &root, tree_strategy);
					if (!err) err = traverse_xiso(xiso, root_dir_start, 0, root_end_offset, buf, k_generate_avl, &root, discover_strategy);
				}
				if (!err) err = create_xiso(iso_name, in_path, root, xiso, out_iso_path, nil, nil);
			}
			else {
				if (!err && root_dir_size != 0) err = traverse_xiso(xiso, root_dir_start, 0, root_end_offset, buf, in_mode, nil, discover_strategy);
			}
			
			if(buf) free(buf);
		}
	}
	
	if ( err == err_iso_rewritten ) err = 0;
	if ( err ) misc_err( "failed to %s xbox iso image %s", in_mode == k_rewrite ? "rewrite" : in_mode == k_extract ? "extract" : "list", name );

	if ( xiso != -1 ) close( xiso );
	
	if ( short_name ) free( short_name );
	if ( cwd ) {
		if (chdir(cwd) == -1) chdir_err(cwd);
		free( cwd );
	}
	
	if ( repair ) in_xiso[ strlen( in_xiso ) ] = '.';

	return err;
}


int traverse_xiso(int in_xiso, xoff_t in_dir_start, uint16_t entry_offset, uint16_t end_offset, char* in_path, modes in_mode, dir_node_avl** in_root, strategies strategy) {
	dir_node_avl	*avl = nil;
	dir_node		*node = nil;
	uint16_t		l_offset = 0, r_offset = 0;
	int				err = 0;

	if (entry_offset >= end_offset) misc_err("attempt to read node entry beyond directory end");

	do {
		if (!err && lseek_with_error(in_xiso, in_dir_start + (xoff_t)entry_offset * XISO_DWORD_SIZE, SEEK_SET) == -1) seek_err();

		if (!err && read(in_xiso, &l_offset, XISO_TABLE_OFFSET_SIZE) != XISO_TABLE_OFFSET_SIZE) read_err();
		if (!err && l_offset == XISO_PAD_SHORT) {
			if (entry_offset == 0) {	// Empty directories have padding starting at the beginning
				if (in_mode == k_generate_avl) *in_root = EMPTY_SUBDIRECTORY;
				return err;				// Done
			}
			else if (strategy != discover_strategy) {			// When discovering, the padding means end of sector
				exiso_warn("Invalid node found and skipped!");	// When not discovering, the padding means a bad entry, skip it without failing
				return err;										// We're done if not discovering
			}
			// We're discovering, so set the offset to the start of the next sector
			if (!err) entry_offset = n_sectors(entry_offset * XISO_DWORD_SIZE) * XISO_SECTOR_SIZE / XISO_DWORD_SIZE;
			continue;
		}

		// Read node
		if (!err) if ((node = calloc(1, sizeof(dir_node))) == nil) mem_err();
		if (!err && read(in_xiso, &r_offset, XISO_TABLE_OFFSET_SIZE) != XISO_TABLE_OFFSET_SIZE) read_err();
		if (!err && read(in_xiso, &node->start_sector, XISO_SECTOR_OFFSET_SIZE) != XISO_SECTOR_OFFSET_SIZE) read_err();
		if (!err && read(in_xiso, &node->file_size, XISO_FILESIZE_SIZE) != XISO_FILESIZE_SIZE) read_err();
		if (!err && read(in_xiso, &node->attributes, XISO_ATTRIBUTES_SIZE) != XISO_ATTRIBUTES_SIZE) read_err();
		if (!err && read(in_xiso, &node->filename_length, XISO_FILENAME_LENGTH_SIZE) != XISO_FILENAME_LENGTH_SIZE) read_err();

		if (!err && (entry_offset * XISO_DWORD_SIZE + XISO_FILENAME_OFFSET + node->filename_length) > (end_offset * XISO_DWORD_SIZE)) misc_err("node entry spans beyond directory end");

		if (!err) {
			little16(l_offset);
			little16(r_offset);
			little32(node->file_size);
			little32(node->start_sector);

			if ((node->filename = (char*)malloc((size_t)node->filename_length + 1)) == nil) mem_err();
		}

		if (!err) {
			if (read(in_xiso, node->filename, node->filename_length) != node->filename_length) read_err();
			if (!err) {
				node->filename[node->filename_length] = 0;

				// security patch (Chris Bainbridge), modified by in to support "...", etc. 02.14.06 (in)
				if (!strcmp(node->filename, ".") || !strcmp(node->filename, "..") || strchr(node->filename, '/') || strchr(node->filename, '\\')) {
					log_err(__FILE__, __LINE__, "filename '%s' contains invalid character(s), aborting.", node->filename);
					exit(1);
				}
			}
		}

		// Process the node according to the mode
		if (!err) {
			if (in_mode == k_generate_avl) {
				if ((avl = (dir_node_avl*)calloc(1, sizeof(dir_node_avl))) == nil) mem_err();
				if (!err) if ((avl->filename = strdup(node->filename)) == nil) mem_err();
				if (!err) {
					avl->file_size = node->file_size;
					avl->old_start_sector = node->start_sector;
					if (avl_insert(in_root, avl) == k_avl_error) {	// Insert node in tree
						// If we're discovering files outside trees, we don't care about avl_insert errors,
						// since they represent nodes already discovered before, and we don't want to process them again
						if (strategy != discover_strategy) misc_err("this iso appears to be corrupt");
					}
					else err = process_node(in_xiso, node, in_path, in_mode, &avl->subdirectory, strategy);
				}
			}
			else err = process_node(in_xiso, node, in_path, in_mode, nil, strategy);
		}

		// Save next offset for discovery
		if (!err) entry_offset = n_dword(entry_offset * XISO_DWORD_SIZE + XISO_FILENAME_OFFSET + node->filename_length);

		// Free memory before recurring or iterating
		if (node) {
			if (node->filename) free(node->filename);
			free(node);
		}

	} while (!err && entry_offset < end_offset && strategy == discover_strategy);	// Iterate only if using discover_strategy

	if (strategy != discover_strategy) {
		// Recurse on left node
		if (!err && l_offset) {
			if (lseek_with_error(in_xiso, in_dir_start + (xoff_t)l_offset * XISO_DWORD_SIZE, SEEK_SET) == -1) seek_err();
			if (!err) err = traverse_xiso(in_xiso, in_dir_start, l_offset, end_offset, in_path, in_mode, &avl, strategy);
		}

		// Recurse on right node
		if (!err && r_offset) {
			if (lseek_with_error(in_xiso, in_dir_start + (xoff_t)r_offset * XISO_DWORD_SIZE, SEEK_SET) == -1) seek_err();
			if (!err) err = traverse_xiso(in_xiso, in_dir_start, r_offset, end_offset, in_path, in_mode, &avl, strategy);
		}
	}

	return err;
}

int process_node(int in_xiso, dir_node* node, char* in_path, modes in_mode, dir_node_avl** in_root, strategies strategy) {
	char		*path = nil;
	int			err = 0;
	xoff_t		dir_start = (xoff_t)node->start_sector * XISO_SECTOR_SIZE + s_xbox_disc_lseek;
	uint16_t	end_offset = 0;

	if (node->attributes & XISO_ATTRIBUTE_DIR) {	// Process directory

		if (!err) if (lseek_with_error(in_xiso, dir_start, SEEK_SET) == -1) seek_err();

		if (!err) {
			if (!s_remove_systemupdate || !strstr(node->filename, s_systemupdate))
			{
				if (in_mode == k_extract) {
					if ((err = mkdir(node->filename, 0755))) mkdir_err(node->filename);
					if (!err && (err = chdir(node->filename))) chdir_err(node->filename);
				}
				if (!err && in_mode != k_generate_avl) {
					exiso_log("\n%s%s%s%s (0 bytes)%s", in_mode == k_extract ? "creating " : "", in_path, node->filename, PATH_CHAR_STR, in_mode == k_extract ? " [OK]" : ""); flush();
				}
			}
		}

		if (!err) {
			// Recurse on subdirectory
			if (node->file_size == 0) {
				if (in_mode == k_generate_avl) *in_root = EMPTY_SUBDIRECTORY;
			}
			else {
				if (in_path && asprintf(&path, "%s%s%c", in_path, node->filename, PATH_CHAR) == -1) mem_err();
				if (!err) {
					end_offset = (uint16_t)(n_sectors(node->file_size) * XISO_SECTOR_SIZE / XISO_DWORD_SIZE);
					err = traverse_xiso(in_xiso, dir_start, 0, end_offset, path, in_mode, in_root, strategy);
				}
				if (path) free(path);
			}

			if (!s_remove_systemupdate || !strstr(node->filename, s_systemupdate))
			{
				if (!err && in_mode == k_extract && (err = chdir(".."))) chdir_err("..");
			}
		}
		
	}
	else if (in_mode != k_generate_avl) {	// Write file
		if (!err) {
			if (!s_remove_systemupdate || !strstr(in_path, s_systemupdate))
			{
				if (in_mode == k_extract) err = extract_file(in_xiso, node, in_mode, in_path);
				else {
					exiso_log("\n%s%s (%u bytes)", in_path, node->filename, node->file_size); flush();
				}

				++s_total_files;
				++s_total_files_all_isos;
				s_total_bytes += node->file_size;
				s_total_bytes_all_isos += node->file_size;
			}
		}
	}

	return err;
}


#if 0
#pragma mark -
#endif


dir_node_avl *avl_fetch( dir_node_avl *in_root, char *in_filename ) {
	int				result;

	for ( ;; ) {
		if ( in_root == nil ) return nil;
	
		result = avl_compare_key( in_filename, in_root->filename );
	
		if ( result < 0 ) in_root = in_root->left;
		else if ( result > 0 ) in_root = in_root->right;
		else return in_root;
	}
}


avl_result avl_insert( dir_node_avl **in_root, dir_node_avl *in_node ) {
	avl_result		tmp;
	int				result;
	
	if ( *in_root == nil ) { *in_root = in_node; return k_avl_balanced; }

	result = avl_compare_key( in_node->filename, (*in_root)->filename );
	
	if ( result < 0 ) return ( tmp = avl_insert( &(*in_root)->left, in_node ) ) == k_avl_balanced ? avl_left_grown( in_root ) : tmp;
	if ( result > 0 ) return ( tmp = avl_insert( &(*in_root)->right, in_node ) ) == k_avl_balanced ? avl_right_grown( in_root ) : tmp;
	
	return k_avl_error;
}


avl_result avl_left_grown( dir_node_avl **in_root ) {
	switch ( (*in_root)->skew ) {
		case k_left_skew: {
			if ( (*in_root)->left->skew == k_left_skew ) {
				(*in_root)->skew = (*in_root)->left->skew = k_no_skew;
				avl_rotate_right( in_root );
			} else {
				switch ( (*in_root)->left->right->skew ) {
					case k_left_skew: {
						(*in_root)->skew = k_right_skew;
						(*in_root)->left->skew = k_no_skew;
					} break;
					
					case k_right_skew: {
						(*in_root)->skew = k_no_skew;
						(*in_root)->left->skew = k_left_skew;
					} break;
					
					default: {
						(*in_root)->skew = k_no_skew;
						(*in_root)->left->skew = k_no_skew;
					} break;
				}
				(*in_root)->left->right->skew = k_no_skew;
				avl_rotate_left( &(*in_root)->left );
				avl_rotate_right( in_root );
			}
		} return no_err;
		
		case k_right_skew: {
			(*in_root)->skew = k_no_skew;
		} return no_err;
		
		default: {
			(*in_root)->skew = k_left_skew;
		} return k_avl_balanced;
	}
}


avl_result avl_right_grown( dir_node_avl **in_root ) {
	switch ( (*in_root)->skew ) {
		case k_left_skew: {
			(*in_root)->skew = k_no_skew;
		} return no_err;
		
		case k_right_skew: {
			if ( (*in_root)->right->skew == k_right_skew ) {
				(*in_root)->skew = (*in_root)->right->skew = k_no_skew;
				avl_rotate_left( in_root );
			} else {
				switch ( (*in_root)->right->left->skew ) {
					case k_left_skew: {
						(*in_root)->skew = k_no_skew;
						(*in_root)->right->skew = k_right_skew;
					} break;
					
					case k_right_skew: {
						(*in_root)->skew = k_left_skew;
						(*in_root)->right->skew = k_no_skew;
					} break;
					
					default: {
						(*in_root)->skew = k_no_skew;
						(*in_root)->right->skew = k_no_skew;
					} break;
				}
				(*in_root)->right->left->skew = k_no_skew;
				avl_rotate_right( &(*in_root)->right );
				avl_rotate_left( in_root );
			}
		} return no_err;
		
		default: {
			(*in_root)->skew = k_right_skew;
		} return k_avl_balanced;
	}
}


void avl_rotate_left( dir_node_avl **in_root ) {
	dir_node_avl   *tmp = *in_root;
	
	*in_root = (*in_root)->right;
	tmp->right = (*in_root)->left;
	(*in_root)->left = tmp;
}


void avl_rotate_right( dir_node_avl **in_root ) {
	dir_node_avl   *tmp = *in_root;
	
	*in_root = (*in_root)->left;
	tmp->left = (*in_root)->right;
	(*in_root)->right = tmp;
}


int avl_compare_key( char *in_lhs, char *in_rhs ) {
	char			a, b;

	for ( ;; ) {
		a = *in_lhs++;
		b = *in_rhs++;
		
		if ( a >= 'a' && a <= 'z' ) a -= 32;	// uppercase(a);
		if ( b >= 'a' && b <= 'z' ) b -= 32;	// uppercase(b);
		
		if ( a ) {
			if ( b ) {
				if ( a < b ) return -1;
				if ( a > b ) return 1;
			} else return 1;
		} else return b ? -1 : 0;
	}
}


int avl_traverse_depth_first( dir_node_avl *in_root, traversal_callback in_callback, void *in_context, avl_traversal_method in_method, int in_depth ) {
	int			err;

	if ( in_root == nil ) return 0;
	
	switch ( in_method ) {
		case k_prefix: {
			err = (*in_callback)( in_root, in_context, in_depth );
			if ( ! err ) err = avl_traverse_depth_first( in_root->left, in_callback, in_context, in_method, in_depth + 1 );
			if ( ! err ) err = avl_traverse_depth_first( in_root->right, in_callback, in_context, in_method, in_depth + 1 );		
		} break;

		case k_infix: {
			err = avl_traverse_depth_first( in_root->left, in_callback, in_context, in_method, in_depth + 1 );
			if ( ! err ) err = (*in_callback)( in_root, in_context, in_depth );
			if ( ! err ) err = avl_traverse_depth_first( in_root->right, in_callback, in_context, in_method, in_depth + 1 );		
		} break;

		case k_postfix: {
			err = avl_traverse_depth_first( in_root->left, in_callback, in_context, in_method, in_depth + 1 );
			if ( ! err ) err = avl_traverse_depth_first( in_root->right, in_callback, in_context, in_method, in_depth + 1 );		
			if ( ! err ) err = (*in_callback)( in_root, in_context, in_depth );
		} break;
		
		default:		err = 0;			break;
	}

	return err;
}


#if 0
#pragma mark -
#endif


int boyer_moore_init( char *in_pattern, long in_pat_len, long in_alphabet_size ) {
	long			i, j, k, *backup, err = 0;

	s_pattern = in_pattern;
	s_pat_len = in_pat_len;
	
	if ( ( s_bc_table = (long *) malloc( in_alphabet_size * sizeof(long) ) ) == nil ) mem_err();
	
	if ( ! err ) {
		for ( i = 0; i < in_alphabet_size; ++i ) s_bc_table[ i ] = in_pat_len;
		for ( i = 0; i < in_pat_len - 1; ++i ) s_bc_table[ (uint8_t) in_pattern[ i ] ] = in_pat_len - i - 1;
	
		if ( ( s_gs_table = (long *) malloc( 2 * ( in_pat_len + 1 ) * sizeof(long) ) ) == nil ) mem_err();
	}	

	if ( ! err ) {
		backup = s_gs_table + in_pat_len + 1;
		
		for ( i = 1; i <= in_pat_len; ++i ) s_gs_table[ i ] = 2 * in_pat_len - i;
		for ( i = in_pat_len, j = in_pat_len + 1; i; --i, --j ) {
			backup[ i ] = j;
	
			while ( j <= in_pat_len && in_pattern[ i - 1 ] != in_pattern[ j - 1 ] ) {
				if ( s_gs_table[ j ] > in_pat_len - i ) s_gs_table[ j ] = in_pat_len - i;
				j = backup[ j ];	
			}
		}
		for ( i = 1; i <= j; ++i ) if ( s_gs_table[ i ] > in_pat_len + j - i ) s_gs_table[ i ] = in_pat_len + j - i;
		
		k = backup[ j ];
	
		for ( ; j <= in_pat_len; k = backup[ k ] ) {
			for ( ; j <= k; ++j ) if ( s_gs_table[ j ] >= k - j + in_pat_len ) s_gs_table[ j ] = k - j + in_pat_len;
		}
	}
	
	return err;
}


void boyer_moore_done() {
	if ( s_bc_table ) { free( s_bc_table ); s_bc_table = nil; }
	if ( s_gs_table ) { free( s_gs_table ); s_gs_table = nil; }
}


char *boyer_moore_search( char *in_text, long in_text_len ) {
	long			i, j, k, l;

	for ( i = j = s_pat_len - 1; j < in_text_len && i >= 0; ) {
		if ( in_text[ j ] == s_pattern[ i ] ) { --i; --j; }
		else {
			k = s_gs_table[ i + 1 ];
			l = s_bc_table[ (uint8_t) in_text[ j ] ];

			j += max( k, l );
			
			i = s_pat_len - 1;
		}
	}
	
	return i < 0 ? in_text + j + 1 : nil;
}


#if 0
#pragma mark -
#endif


int extract_file(int in_xiso, dir_node *in_file, modes in_mode, char* path) {
	int						err = 0;
	xoff_t					file_start = (xoff_t)in_file->start_sector * XISO_SECTOR_SIZE + s_xbox_disc_lseek;
	uint32_t				i, size, totalsize = 0;
	int						read_size;
	float					totalpercent = 0.0f;
	int						out = -1;

	if (lseek_with_error(in_xiso, file_start, SEEK_SET) == -1) seek_err();

	if ( !s_remove_systemupdate || !strstr( path, s_systemupdate ) ) {
		if ( in_mode == k_extract ) {
			if (!err && (out = open(in_file->filename, WRITEFLAGS, 0644)) == -1) open_err(in_file->filename);
		} else err = 1;

		if ( ! err ) {
			exiso_log("\n");
			if (in_file->file_size == 0) exiso_log("%s%s%s (0 bytes) [100%%]\r", in_mode == k_extract ? "extracting " : "", path, in_file->filename);
			else {
				i = 0;
				size = min(in_file->file_size, READWRITE_BUFFER_SIZE);
				do {
					read_size = read(in_xiso, s_copy_buffer, size);
					if (read_size < 0) read_err();
					else if (in_mode == k_extract && read_size != 0) {
						if (write(out, s_copy_buffer, read_size) != read_size) write_err();
					}
					if (!err) {
						totalsize += read_size;
						totalpercent = (totalsize * 100.0f) / in_file->file_size;
						exiso_log("%s%s%s (%u bytes) [%.1f%%]\r", in_mode == k_extract ? "extracting " : "", path, in_file->filename, in_file->file_size, totalpercent);

						i += read_size;
						size = min(in_file->file_size - i, READWRITE_BUFFER_SIZE);
					}
				} while (!err && i < in_file->file_size && read_size > 0);
				if (!err && i < in_file->file_size) {
					exiso_warn("File %s is truncated. Reported size: %u bytes, read size: %u bytes!", in_file->filename, in_file->file_size, i);
					in_file->file_size = i;
				}
			}
			if (in_mode == k_extract) close(out);
		}
	}

	return err;
}


int free_dir_node_avl( void *in_dir_node_avl, unused void *in_context, unused int in_depth ) {
	dir_node_avl	   *avl = (dir_node_avl *) in_dir_node_avl;

	if ( avl->subdirectory && avl->subdirectory != EMPTY_SUBDIRECTORY ) avl_traverse_depth_first( avl->subdirectory, free_dir_node_avl, nil, k_postfix, 0 );
	
	free( avl->filename );
	free( avl );

	return 0;
}


int write_tree( dir_node_avl *in_avl, write_tree_context *in_context, unused int in_depth ) {
	xoff_t					pos = 0, dir_start = (xoff_t)in_avl->start_sector * XISO_SECTOR_SIZE;
	write_tree_context		context = { 0 };
	int						err = 0, pad = 0;
	char					sector[XISO_SECTOR_SIZE];

	if ( in_avl->subdirectory ) {
		if ( in_context->path ) { if ( asprintf( &context.path, "%s%s%c", in_context->path, in_avl->filename, PATH_CHAR ) == -1 ) mem_err(); }
		else { if ( asprintf( &context.path, "%c", PATH_CHAR ) == -1 ) mem_err(); }

		if ( ! err ) {
			exiso_log( "\nadding %s (0 bytes) [OK]", context.path );
	
			if ( in_avl->subdirectory != EMPTY_SUBDIRECTORY ) {
				context.xiso = in_context->xiso;
				context.from = in_context->from;
				context.progress = in_context->progress;
				context.final_bytes = in_context->final_bytes;
		
				if ( in_context->from == -1 ) {
					if ( chdir( in_avl->filename ) == -1 ) chdir_err( in_avl->filename );
				}

				if ( ! err ) err = avl_traverse_depth_first( in_avl->subdirectory, (traversal_callback) write_file, &context, k_prefix, 0 );
				if ( ! err ) err = avl_traverse_depth_first( in_avl->subdirectory, (traversal_callback) write_tree, &context, k_prefix, 0 );

				if (!err && lseek(in_context->xiso, dir_start, SEEK_SET) == -1) seek_err();
				if (!err) err = avl_traverse_depth_first(in_avl->subdirectory, (traversal_callback)write_directory, in_context, k_prefix, 0);
				if (!err && (pos = lseek(in_context->xiso, 0, SEEK_CUR)) == -1) seek_err();
				if (!err) {
					pad = (int)((XISO_SECTOR_SIZE - (pos % XISO_SECTOR_SIZE)) % XISO_SECTOR_SIZE);
					memset(sector, XISO_PAD_BYTE, pad);
					if (write(in_context->xiso, sector, pad) != pad) write_err();
				}

				if ( ! err && in_context->from == -1 ) {
					if ( chdir( ".." ) == -1 ) chdir_err( ".." );
				}
			} else {
				memset(sector, XISO_PAD_BYTE, XISO_SECTOR_SIZE);
				if ((pos = lseek(in_context->xiso, dir_start, SEEK_SET)) == -1) seek_err();
				if (!err && write(in_context->xiso, sector, XISO_SECTOR_SIZE) != XISO_SECTOR_SIZE) write_err();
			}

			if (context.path) free(context.path);
		}
	}

	return err;
}


int write_file( dir_node_avl *in_avl, write_tree_context *in_context, unused int in_depth ) {
	char		   *buf = nil, *p = nil;
	uint32_t		bytes = 0, size = max(XISO_SECTOR_SIZE, READWRITE_BUFFER_SIZE);
	int				err = 0, fd = -1, n = 0, i = 0;
	size_t			len;
	bool			xbe_file;

	if ( ! in_avl->subdirectory ) {
		if ( lseek( in_context->xiso, (xoff_t) in_avl->start_sector * XISO_SECTOR_SIZE, SEEK_SET ) == -1 ) seek_err();
		
		if ( ! err && ( buf = (char *) malloc( (size_t)size + 1 ) ) == nil ) mem_err();
		if ( ! err ) {
			if ( in_context->from == -1 ) {
				if ( ( fd = open( in_avl->filename, READFLAGS, 0 ) ) == -1 ) open_err( in_avl->filename );
			} else {
				if (lseek_with_error(fd = in_context->from, (xoff_t)in_avl->old_start_sector * XISO_SECTOR_SIZE + s_xbox_disc_lseek, SEEK_SET) == -1) seek_err();
			}
		}

		if ( ! err ) {
			exiso_log( "\nadding %s%s (%u bytes) ", in_context->path, in_avl->filename, in_avl->file_size ); flush();

			bytes = in_avl->file_size;
			len = strlen(in_avl->filename);
			xbe_file = len >= 4 && strcasecmp(&in_avl->filename[len - 4], ".xbe") == 0;
			do {
				n = read(fd, buf + i, min(bytes, size - i));
				if (n < 0) read_err();
				else if (n == 0) {	// Unexpected end of file
					if (i > 0 && write(in_context->xiso, buf, i) != i) write_err();	// Write remaining 'i' bytes
				}
				else {
					bytes -= n;
					if (s_media_enable && xbe_file) {
						n += i;
						for (buf[n] = 0, p = buf; (p = boyer_moore_search(p, n - (long)(p - buf))) != nil; p += XISO_MEDIA_ENABLE_LENGTH) p[XISO_MEDIA_ENABLE_BYTE_POS] = XISO_MEDIA_ENABLE_BYTE;
						if (bytes) {
							i = XISO_MEDIA_ENABLE_LENGTH - 1;
							n -= i;
						}
					}
					if (write(in_context->xiso, buf, n) != n) write_err();
					if (!err && s_media_enable && xbe_file && bytes) memcpy(buf, &buf[n], i);
				}
			} while (!err && bytes > 0 && n > 0);
			size = in_avl->file_size;
			in_avl->file_size -= bytes;

			if (!err) {
				i = (XISO_SECTOR_SIZE - (in_avl->file_size % XISO_SECTOR_SIZE)) % XISO_SECTOR_SIZE;
				if (i > 0) {
					memset(buf, XISO_PAD_BYTE, i);
					if (write(in_context->xiso, buf, i) != i) write_err();
				}
			}
			exiso_log(err ? "failed" : "[OK]");

			if (!err && size != in_avl->file_size) exiso_warn("File %s is truncated. Reported size: %u bytes, wrote size: %u bytes!", in_avl->filename, size, in_avl->file_size);

			if (!err) {
				++s_total_files;
				s_total_bytes += in_avl->file_size;
				if (in_context->progress) (*in_context->progress)(s_total_bytes, in_context->final_bytes);
			}
		}
		
		if ( in_context->from == -1 && fd != -1 ) close( fd );
		if ( buf ) free( buf );
	}
	
	return err;
}


int write_directory( dir_node_avl *in_avl, write_tree_context* in_context, unused int in_depth ) {
	xoff_t		pos;
	uint16_t	l_offset, r_offset;
	uint32_t	file_size = in_avl->subdirectory ? n_sectors(in_avl->file_size) * XISO_SECTOR_SIZE : in_avl->file_size;
	uint8_t		length = (uint8_t)strlen(in_avl->filename);
	uint8_t		attributes = in_avl->subdirectory ? XISO_ATTRIBUTE_DIR : XISO_ATTRIBUTE_ARC;
	char		sector[XISO_SECTOR_SIZE];
	int			err = 0, pad = 0;
		
	little32( in_avl->file_size );
	little32( in_avl->start_sector );
	
	l_offset = (uint16_t) (in_avl->left ? in_avl->left->offset / XISO_DWORD_SIZE : 0);
	r_offset = (uint16_t) (in_avl->right ? in_avl->right->offset / XISO_DWORD_SIZE : 0);
	
	little16( l_offset );
	little16( r_offset );
	
	memset( sector, XISO_PAD_BYTE, XISO_SECTOR_SIZE );
	
	if ( ( pos = lseek( in_context->xiso, 0, SEEK_CUR ) ) == -1 ) seek_err();
	if ( ! err ) pad = (int)((xoff_t)in_avl->offset + in_avl->dir_start - pos);
	if ( ! err && write( in_context->xiso, sector, pad ) != pad ) write_err();
	if ( ! err && write( in_context->xiso, &l_offset, XISO_TABLE_OFFSET_SIZE ) != XISO_TABLE_OFFSET_SIZE ) write_err();
	if ( ! err && write( in_context->xiso, &r_offset, XISO_TABLE_OFFSET_SIZE ) != XISO_TABLE_OFFSET_SIZE ) write_err();
	if ( ! err && write( in_context->xiso, &in_avl->start_sector, XISO_SECTOR_OFFSET_SIZE ) != XISO_SECTOR_OFFSET_SIZE ) write_err();
	if ( ! err && write( in_context->xiso, &file_size, XISO_FILESIZE_SIZE ) != XISO_FILESIZE_SIZE ) write_err();
	if ( ! err && write( in_context->xiso, &attributes, XISO_ATTRIBUTES_SIZE ) != XISO_ATTRIBUTES_SIZE ) write_err();
	if ( ! err && write( in_context->xiso, &length, XISO_FILENAME_LENGTH_SIZE ) != XISO_FILENAME_LENGTH_SIZE ) write_err();
	if ( ! err && write( in_context->xiso, in_avl->filename, length ) != length ) write_err();
	
	little32( in_avl->start_sector );
	little32( in_avl->file_size );
	
	return err;	
}


int calculate_directory_offsets( dir_node_avl *in_avl, uint32_t *io_current_sector, unused int in_depth ) {
	wdsafp_context			context = { 0 };

	if ( in_avl->subdirectory ) {
		if (in_avl->subdirectory == EMPTY_SUBDIRECTORY) {
			in_avl->start_sector = *io_current_sector;
			*io_current_sector += 1;
		}
		else {
			context.current_sector = io_current_sector;
			context.dir_start = (xoff_t) ( in_avl->start_sector = *io_current_sector ) * XISO_SECTOR_SIZE;
		
			*io_current_sector += n_sectors( in_avl->file_size );
		
			avl_traverse_depth_first( in_avl->subdirectory, (traversal_callback) write_dir_start_and_file_positions, &context, k_prefix, 0 );
			avl_traverse_depth_first( in_avl->subdirectory, (traversal_callback) calculate_directory_offsets, io_current_sector, k_prefix, 0 );
		}
	}
	
	return 0;
}


int write_dir_start_and_file_positions( dir_node_avl *in_avl, wdsafp_context *io_context, unused int in_depth ) {
	in_avl->dir_start = io_context->dir_start;

	if ( ! in_avl->subdirectory ) {
		in_avl->start_sector = *io_context->current_sector;
		*io_context->current_sector += n_sectors( in_avl->file_size );
	}
	
	return 0;
}


int calculate_total_files_and_bytes( dir_node_avl *in_avl, unused void *in_context, unused int in_depth ) {
	if (in_avl->subdirectory) {
		if (in_avl->subdirectory != EMPTY_SUBDIRECTORY) {
			avl_traverse_depth_first(in_avl->subdirectory, (traversal_callback)calculate_total_files_and_bytes, nil, k_prefix, 0);
		}
	} else {
		++s_total_files;
		s_total_bytes += in_avl->file_size;
	}
	return 0;
}


int calculate_directory_requirements( dir_node_avl *in_avl, void *in_context, unused int in_depth ) {
	if ( in_avl->subdirectory ) {
		if (in_avl->subdirectory != EMPTY_SUBDIRECTORY) {
			avl_traverse_depth_first(in_avl->subdirectory, (traversal_callback)calculate_directory_size, &in_avl->file_size, k_prefix, 0);
			avl_traverse_depth_first(in_avl->subdirectory, (traversal_callback)calculate_directory_requirements, in_context, k_prefix, 0);
		} else {
			in_avl->file_size = XISO_SECTOR_SIZE;
		}
	}
	
	return 0;
}


int calculate_directory_size( dir_node_avl *in_avl, uint32_t *out_size, int in_depth ) {
	uint32_t			length;

	if ( in_depth == 0 ) *out_size = 0;
	
	length = XISO_FILENAME_OFFSET + (uint32_t)strlen( in_avl->filename );
	length += ( XISO_DWORD_SIZE - ( length % XISO_DWORD_SIZE ) ) % XISO_DWORD_SIZE;
	
	if ( n_sectors( *out_size + length ) > n_sectors( *out_size ) ) {
		*out_size += ( XISO_SECTOR_SIZE - ( *out_size % XISO_SECTOR_SIZE ) ) % XISO_SECTOR_SIZE;
	}

	in_avl->offset = *out_size;

	*out_size += length;

	return 0;
}


int generate_avl_tree_local( dir_node_avl **out_root, int *io_n ) {
	struct dirent	   *p = nil;
	struct stat			sb = { 0 };
	dir_node_avl	   *avl = nil;
	DIR				   *dir = nil;
	int					err = 0, i = 0, j = 0;
	bool				empty_dir = true;

	if ( ( dir = opendir( "." ) ) == nil ) mem_err();

	while ( ! err && ( p = readdir( dir ) ) != nil ) {
		if ( ! strcmp( p->d_name, "." ) || ! strcmp( p->d_name, ".." ) ) continue;

		for ( i = *io_n; i; --i ) exiso_log( "\b" );
		exiso_log( "%s", p->d_name );
		for ( j = i = (int) strlen( p->d_name ); j < *io_n; ++j ) exiso_log( " " );
		for ( j = i; j < *io_n; ++j ) exiso_log( "\b" );
		*io_n = i;
		flush();
		
		if ( ( avl = (dir_node_avl *) malloc( sizeof(dir_node_avl) ) ) == nil ) mem_err();
		if ( ! err ) {
			memset( avl, 0, sizeof(dir_node_avl) );
			if ( ( avl->filename = strdup( p->d_name ) ) == nil ) mem_err();
		}
		if ( ! err && stat( avl->filename, &sb ) == -1 ) read_err();
		if ( ! err ) {
			if ( S_ISDIR( sb.st_mode ) ) {
				empty_dir = false;

				if ( chdir( avl->filename ) == -1 ) chdir_err( avl->filename );

				if ( ! err ) err = generate_avl_tree_local( &avl->subdirectory, io_n );
				if ( ! err && chdir( ".." ) == -1 ) chdir_err( ".." );
			} else if ( S_ISREG( sb.st_mode ) ) {
				if ( sb.st_size > (int64_t)UINT32_MAX ) {
					log_err( __FILE__, __LINE__, "file %s is too large for xiso, skipping...", avl->filename );
					free( avl->filename );
					free( avl );
					continue;
				}
				empty_dir = false;
				s_total_bytes += avl->file_size = (uint32_t) sb.st_size;
				++s_total_files;
			} else {
				free( avl->filename );
				free( avl );
				continue;
			}
		}
		if ( ! err ) {
			if ( avl_insert( out_root, avl ) == k_avl_error ) misc_err( "error inserting file %s into tree (duplicate filename?)", avl->filename );
		} else {
			if ( avl ) {
				if ( avl->filename ) free( avl->filename );
				free( avl );
			}
		}
	}
	
	if ( empty_dir ) *out_root = EMPTY_SUBDIRECTORY;
	
	if ( dir ) closedir( dir );
	
	return err;
}


int get_filetime_now(file_time_t *ft) {
	time_t				now = 0;
	int					err = 0;

	if (ft == nil) return 1;
	if ( ( now = time( nil ) ) == -1 ) unknown_err();
	if ( ! err ) {
		*ft = (now * 10000000LL) + 116444736000000000LL;	// Magic numbers directly from Microsoft
		little64(*ft);	// convert to little endian here because this is a PC only struct and we won't read it anyway
	}
	
	return err;
}

// Found the CD-ROM layout in ECMA-119.  Now burning software should correctly
// detect the format of the xiso and burn it correctly without the user having
// to specify sector sizes and so on.	in 10.29.04

#define ECMA_119_DATA_AREA_START			0x8000
#define ECMA_119_VOLUME_SPACE_SIZE			( ECMA_119_DATA_AREA_START + 80 )
#define ECMA_119_VOLUME_SET_SIZE			( ECMA_119_DATA_AREA_START + 120 )
#define ECMA_119_VOLUME_SET_IDENTIFIER		( ECMA_119_DATA_AREA_START + 190 )
#define ECMA_119_VOLUME_CREATION_DATE		( ECMA_119_DATA_AREA_START + 813 )


// write_volume_descriptors() assumes that the iso file block from offset
// 0x8000 to 0x8808 has been zeroed prior to entry.

int write_volume_descriptors( int in_xiso, uint32_t in_total_sectors ) {
	uint32_t		big, little;
	char			date[] = "0000000000000000";
	char			spaces[ ECMA_119_VOLUME_CREATION_DATE - ECMA_119_VOLUME_SET_IDENTIFIER ];
	int				err = 0;

	big = little = in_total_sectors;
	
	big32( big );
	little32( little );

	memset( spaces, 0x20, sizeof(spaces) );
	
	if ( lseek( in_xiso, ECMA_119_DATA_AREA_START, SEEK_SET ) == -1 ) seek_err();
	if ( ! err && write( in_xiso, "\x01" "CD001\x01", 7 ) == -1 ) write_err();
	if ( ! err && lseek( in_xiso, ECMA_119_VOLUME_SPACE_SIZE, SEEK_SET ) == -1 ) seek_err();
	if ( ! err && write( in_xiso, &little, 4 ) == -1 ) write_err();
	if ( ! err && write( in_xiso, &big, 4 ) == -1 ) write_err();
	if ( ! err && lseek( in_xiso, ECMA_119_VOLUME_SET_SIZE, SEEK_SET ) == -1 ) seek_err();
	if ( ! err && write( in_xiso, "\x01\x00\x00\x01\x01\x00\x00\x01\x00\x08\x08\x00", 12 ) == -1 ) write_err();
	if ( ! err && lseek( in_xiso, ECMA_119_VOLUME_SET_IDENTIFIER, SEEK_SET ) == -1 ) seek_err();
	if ( ! err && write( in_xiso, spaces, sizeof(spaces) ) == -1 ) write_err();
	if ( ! err && write( in_xiso, date, sizeof(date) ) == -1 ) write_err();
	if ( ! err && write( in_xiso, date, sizeof(date) ) == -1 ) write_err();
	if ( ! err && write( in_xiso, date, sizeof(date) ) == -1 ) write_err();
	if ( ! err && write( in_xiso, date, sizeof(date) ) == -1 ) write_err();
	if ( ! err && write( in_xiso, "\x01", 1 ) == -1 ) write_err();
	if ( ! err && lseek( in_xiso, ECMA_119_DATA_AREA_START + XISO_SECTOR_SIZE, SEEK_SET ) == -1 ) seek_err();
	if ( ! err && write( in_xiso, "\xff" "CD001\x01", 7 ) == -1 ) write_err();
		
	return err;
}


#if DEBUG

void write_sector( int in_xiso, xoff_t in_start, char *in_name, char *in_extension ) {
	ssize_t			wrote;
	xoff_t			curpos = 0;
	int				fp = -1, err = 0;
	char		   *cwd, *sect = nil, buf[ 256 ];

	if ( ( cwd = getcwd( nil, 0 ) ) == nil ) mem_err();
	if ( ! err && chdir( DEBUG_DUMP_DIRECTORY ) == -1 ) chdir_err( DEBUG_DUMP_DIRECTORY );

	sprintf( buf, "%" PRId64 ".%s.%s", in_start, in_name, in_extension ? in_extension : "");

	if ( ! err && ( fp = open( buf, WRITEFLAGS, 0644 ) ) == -1 ) open_err( buf );
	if ( ! err && ( curpos = lseek_with_error( in_xiso, 0, SEEK_CUR ) ) == -1 ) seek_err();
	if ( ! err && lseek_with_error( in_xiso, in_start, SEEK_SET ) == -1 ) seek_err();
	
	if ( ! err && ( sect = (char *) malloc( XISO_SECTOR_SIZE ) ) == nil ) mem_err();
	
	if ( ! err && read( in_xiso, sect, XISO_SECTOR_SIZE ) != XISO_SECTOR_SIZE ) read_err();
	if ( ! err && ( wrote = write( fp, sect, XISO_SECTOR_SIZE ) ) != XISO_SECTOR_SIZE ) write_err();
	
	if ( ! err && lseek_with_error( in_xiso, curpos, SEEK_SET ) == -1 ) seek_err();
	
	if ( sect ) free( sect );
	if ( fp != -1 ) close( fp );

	if ( cwd ) {
		if ( chdir( cwd ) == -1 ) chdir_err( cwd );
		free( cwd );
	}
}

#endif
