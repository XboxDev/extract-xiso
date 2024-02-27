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
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <locale.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "cp1252/cp1252.c"

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
		#define lstat					_stat64
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

typedef int64_t							file_time_t;

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

#ifndef min
	#define min(a, b)					( (a) < (b) ? (a) : (b) )
	#define max(a, b)					( (a) > (b) ? (a) : (b) )
#endif

#define exiso_version					"2.7.1 (01.11.14)"
#define VERSION_LENGTH					16

#define banner							"extract-xiso v" exiso_version " for " exiso_target " - written by in <in@fishtank.com>\n"

#define usage_and_exit(n)				print_usage_and_exit(n, argv[0])

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


#define START_LSEEK_OFFSET				0x00000000ul
#define XGD3_LSEEK_OFFSET				0x02080000ul
#define XGD2_LSEEK_OFFSET				0x0FD90000ul
#define XGD1_LSEEK_OFFSET				0x18300000ul
#define XGD2_ALT_LSEEK_OFFSET			0x89D80000ul
#define LSEEK_OFFSETS_LEN				5
/* The offsets should be in ascending order, otherwise we could get a seek error before checking the correct one */
const xoff_t lseek_offsets[LSEEK_OFFSETS_LEN] = {START_LSEEK_OFFSET, XGD3_LSEEK_OFFSET, XGD2_LSEEK_OFFSET, XGD1_LSEEK_OFFSET, XGD2_ALT_LSEEK_OFFSET};

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

#define READWRITE_BUFFER_SIZE			0x00200000ul	// Must be at least XISO_HEADER_OFFSET

#define DEBUG_DUMP_DIRECTORY			"/Volumes/c/xbox/iso/exiso"

#define GETOPT_STRING					"c:d:Dhlmp:qQrsvx"

const char* const cp1252_locales[] = { ".1252" /* Windows */, "C.CP1252", "en_US.CP1252", "de_DE.CP1252", NULL };
const char* const utf8_locales[] = { ".UTF-8" /* Windows */, "C.UTF-8", "en_US.UTF-8", NULL };


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
	char*								filename;			// User encoded
};

struct dir_node_avl {
	uint32_t							offset;
	xoff_t								dir_start;

	/* filename_cp1252 is always present. If filename is also present, then the encoding differs from filename_cp1252 */
	char*								filename_cp1252;	// Windows-1252 encoded
	char*								filename;			// User encoded
	uint32_t							file_size;
	uint32_t							start_sector;
	uint8_t								attributes;
	dir_node_avl*						subdirectory;
	
	uint32_t							old_start_sector;
	
	avl_skew							skew;
	dir_node_avl*						left;
	dir_node_avl*						right;
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


void print_usage_and_exit(int ret, const char* name);
xoff_t lseek_with_error(int fd, xoff_t offset, int whence);
int log_err( const char *in_file, int in_line, const char *in_format, ... );
void avl_rotate_left( dir_node_avl **in_root );
void avl_rotate_right( dir_node_avl **in_root );
int avl_compare_key( const char *in_lhs, const char *in_rhs );
avl_result avl_left_grown( dir_node_avl **in_root );
avl_result avl_right_grown( dir_node_avl **in_root );
dir_node_avl *avl_fetch( dir_node_avl *in_root, const char *in_filename );
avl_result avl_insert( dir_node_avl **in_root, dir_node_avl *in_node );
int avl_traverse_depth_first( dir_node_avl *in_root, traversal_callback in_callback, void *in_context, avl_traversal_method in_method, int in_depth );

void boyer_moore_done();
char *boyer_moore_search( char *in_text, size_t in_text_len );
int boyer_moore_init( const char *in_pattern, size_t in_pat_len, size_t in_alphabet_size );

int free_dir_node_avl(dir_node_avl* in_dir_node_avl, void* in_context, int in_depth);
int extract_file( int in_xiso, dir_node *in_file, modes in_mode, const char *path );
int decode_xiso( char *in_xiso, char *in_path, modes in_mode, char **out_iso_path );
int verify_xiso( int in_xiso, uint32_t *out_root_dir_sector, uint32_t *out_root_dir_size, const char *in_iso_name );
int traverse_xiso(int in_xiso, xoff_t in_dir_start, uint16_t entry_offset, uint16_t end_offset, const char* in_path, modes in_mode, dir_node_avl** in_root, strategies strategy);
int process_node(int in_xiso, dir_node* node, const char* in_path, modes in_mode, dir_node_avl** in_root, strategies strategy);
int create_xiso( char *in_root_directory, const char *in_output_directory, dir_node_avl *in_root, int in_xiso, char **out_iso_path, char *in_name, progress_callback in_progress_callback );

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
void write_sector( int in_xiso, xoff_t in_start, const char *in_name, const char *in_extension );
#endif


static size_t							s_pat_len;
static bool								s_quiet = false;
static const char					   *s_pattern = NULL;
static size_t							*s_gs_table = NULL;
static size_t							*s_bc_table = NULL;
static long long						s_total_bytes = 0;
static int								s_total_files = 0;
static char							   *s_copy_buffer = NULL;
static bool								s_real_quiet = false;
static bool								s_media_enable = true;
static long long						s_total_bytes_all_isos = 0;
static int								s_total_files_all_isos = 0;
static bool								s_warned = false;
static bool								s_cp1252 = false;

static bool				                s_remove_systemupdate = false;
static char				               *s_systemupdate = "$SystemUpdate";

static xoff_t							s_xbox_disc_lseek = 0;


#if 0		// #pragma mark - inserts a spacer in the function popup of certain text editors (i.e. mine ;-)
#pragma mark -
#endif


int main( int argc, char **argv ) {
	struct stat			sb;
	create_list			*create = NULL, *p, *q;
	int					i, fd, opt_char, err = 0, isos = 0;
	bool				extract = true, rewrite = false, x_seen = false, delete = false, optimized;
	ptrdiff_t			diff;
	char				*path = NULL, *buf = NULL, *new_iso_path = NULL;
	const char			*locale = NULL;
	const char* const	*locale_arr = NULL;
	char				tag[XISO_OPTIMIZED_TAG_LENGTH + 1];

	if (argc < 2) usage_and_exit(1);
	
	while ( ! err && ( opt_char = getopt( argc, argv, GETOPT_STRING ) ) != -1 ) {
		switch ( opt_char ) {
			case 'c': {
				if (x_seen || rewrite || !extract) usage_and_exit(1);

				if (create == NULL) {
					if ((p = create = (create_list*)calloc(1, sizeof(create_list))) == NULL) mem_err();
				}
				else {
					p = create;
					while (p != NULL && p->next != NULL) p = p->next;
					if ((p = p->next = (create_list*)calloc(1, sizeof(create_list))) == NULL) mem_err();
				}
				if (!err && (p->path = strdup(optarg)) == NULL) mem_err();
				if (!err && argv[optind] && *argv[optind] != '-' && *argv[optind] && (p->name = strdup(argv[optind++])) == NULL) mem_err();
			} break;
			
			case 'd': {
				if ( path ) free( path );
				if ( ( path = strdup( optarg ) ) == NULL ) mem_err();
			} break;
			
			case 'D': {
				delete = true;
			} break;

			case 'h': {
				usage_and_exit(0);
			} break;
			
			case 'l': {
				if ( x_seen || rewrite || create ) usage_and_exit(1);
				extract = false;
			} break;
			
			case 'm': {
				if ( x_seen || ! extract ) usage_and_exit(1);
				s_media_enable = false;
			} break;
			
			case 'q': {
				s_quiet = true;
			} break;
			
			case 'Q': {
				s_quiet = s_real_quiet = true;
			} break;
			
			case 'r': {
				if ( x_seen || ! extract || create ) usage_and_exit(1);
				rewrite = true;
			} break;

			case 's': {
				s_remove_systemupdate = true;
			} break;

			case 'v': {
				printf(banner);
				exit(0);
			} break;
			
			case 'x': {
				if ( ! extract || rewrite || create ) usage_and_exit(1);
				x_seen = true;
			} break;

			default: {
				usage_and_exit(1);
			} break;
		}
	}
	
	if ( ! err ) {

		if (create) {
			if (optind < argc) usage_and_exit(1);
		}
		else if (optind >= argc) usage_and_exit(1);
	
		exiso_log(banner);
	
		if ((extract || rewrite || create) && (s_copy_buffer = (char*)malloc(READWRITE_BUFFER_SIZE)) == NULL) mem_err();

	}

	if (!err) {
		locale_arr = cp1252_locales;
		while (*locale_arr && !locale) locale = setlocale(LC_ALL, *locale_arr++);
		if (locale) s_cp1252 = true;
		else {
			locale_arr = utf8_locales;
			while (*locale_arr && !locale) locale = setlocale(LC_ALL, *locale_arr++);
			if (!locale) exiso_warn("Using %s locale. Non-ASCII characters will probably not be represented correctly.", setlocale(LC_ALL, NULL));
		}
	}
	
	if ( ! err && ( create || rewrite ) ) err = boyer_moore_init( XISO_MEDIA_ENABLE, XISO_MEDIA_ENABLE_LENGTH, k_default_alphabet_size );

	if ( ! err && create ) {
		p = create;
		while (!err && p != NULL) {
			diff = 0;
			if ( p->name && (buf = strrchr(p->name, PATH_CHAR)) ) {
				diff = buf - p->name;
				if ( ( buf = (char *) malloc( diff + 1 ) ) == NULL ) mem_err();
				if ( ! err ) {
					strncpy( buf, p->name, diff );
					buf[ diff ] = 0;
				}
				diff += 1;
			}
			
			if ( ! err ) err = create_xiso( p->path, buf, NULL, -1, NULL, p->name ? p->name + diff : NULL, NULL );

			if (buf) { free(buf); buf = NULL; }

			q = p->next;

			if ( p->name ) free( p->name );
			if ( p->path ) free( p->path );
			free( p );
			
			p = q;
		}
	} else for ( i = optind; ! err && i < argc; ++i ) {
		++isos;
		s_total_bytes = s_total_files = 0;
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

				if (asprintf(&buf, "%s.old", argv[i]) == -1) mem_err();
				if ( ! err && stat( buf, &sb ) != -1 ) misc_err( "%s already exists, cannot rewrite %s", buf, argv[ i ] );
				if ( ! err && rename( argv[ i ], buf ) == -1 ) misc_err( "cannot rename %s to %s", argv[ i ], buf );

				if (err) {
					err = 0;
					if (buf) { free(buf); buf = NULL; }
					continue;
				}

				err = decode_xiso(buf, path, k_rewrite, &new_iso_path);
				if (!err && delete && unlink(buf) == -1) log_err(__FILE__, __LINE__, "unable to delete %s", buf);

				if (buf) { free(buf); buf = NULL; }
			} else {
				// the order of the mutually exclusive options here is important, the extract ? k_extract : k_list test *must* be the final comparison
				err = decode_xiso( argv[ i ], extract ? path : NULL, extract ? k_extract : k_list, NULL );
			}
		}
		
		if (!err) {
			exiso_log("\n\n%u files in %s total %lld bytes\n", s_total_files, rewrite ? new_iso_path : argv[i], s_total_bytes);
			s_total_files_all_isos += s_total_files;
			s_total_bytes_all_isos += s_total_bytes;
		}
		
		if ( new_iso_path ) {
			if ( ! err ) exiso_log( "\n%s successfully rewritten%s%s\n", argv[ i ], path ? " as " : ".", path ? new_iso_path : "" );
		
			free( new_iso_path );
			new_iso_path = NULL;
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

void print_usage_and_exit(int ret, const char* name) {
	fprintf(ret ? stderr : stdout, banner "\n\
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
", name, name);
	fflush(ret ? stderr : stdout);
	exit(ret);
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


int verify_xiso( int in_xiso, uint32_t *out_root_dir_sector, uint32_t *out_root_dir_size, const char *in_iso_name ) {
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
	if (!err && lseek_with_error(in_xiso, (xoff_t)*out_root_dir_sector * XISO_SECTOR_SIZE + s_xbox_disc_lseek, SEEK_SET) == -1) seek_err();
	
	return err;
}




int create_xiso( char *in_root_directory, const char *in_output_directory, dir_node_avl *in_root, int in_xiso, char **out_iso_path, char *in_name, progress_callback in_progress_callback ) {
	xoff_t					pos = 0;
	dir_node_avl			root = { 0 };
	file_time_t				ft = 0;
	write_tree_context		wt_context = { 0 };
	uint32_t				start_sector = 0;
	int						i = 0, n = 0, xiso = -1, err = 0;
	char				   *cwd = NULL, *iso_name = NULL, *xiso_path = NULL, *iso_dir = NULL, *real_path = NULL, *in_dir_path = NULL;

	s_total_bytes = s_total_files = 0;

	if ( ( cwd = getcwd( NULL, 0 ) ) == NULL ) mem_err();
	if (!err) {
		if (!in_root) {
			if ((in_dir_path = realpath(in_root_directory, NULL)) == NULL) misc_err("unable to get absolute path of %s: %s", in_root_directory, strerror(errno));
			if (!err) {
				// Remember not to free in_dir_path until iso_dir is not needed anymore
				if ((iso_dir = strrchr(in_dir_path, PATH_CHAR)) != NULL) iso_dir++;
				else iso_dir = in_dir_path;
				iso_name = in_name ? in_name : iso_dir;
			}
		}
		else {
			iso_dir = iso_name = in_root_directory;
		}
	}
	if(!err) {
		if (!iso_dir || !*iso_dir) iso_dir = PATH_CHAR_STR;
		if (!iso_name || !*iso_name) iso_name = "root";
		else if (iso_name[1] == ':') { iso_name[1] = iso_name[0]; ++iso_name; }

		if (in_output_directory == NULL) in_output_directory = ".";
		if (!err && (real_path = realpath(in_output_directory, NULL)) == NULL) misc_err("unable to get absolute path of %s: %s", in_output_directory, strerror(errno));
		if (!err && (asprintf(&xiso_path, "%s%c%s%s", real_path, PATH_CHAR, iso_name, in_name ? "" : ".iso")) == -1) mem_err();
		if (real_path) {
			free(real_path);
			real_path = NULL;
		}

		if (!err && !in_root && chdir(in_dir_path) == -1) chdir_err(in_dir_path);
	}
	if ( ! err ) {
		exiso_log( "\n%s %s%s:\n", in_root ? "rewriting" : "creating", iso_name, in_name ? "" : ".iso" );

		root.start_sector = XISO_ROOT_DIRECTORY_SECTOR;
		root.attributes = XISO_ATTRIBUTE_DIR;

		s_total_bytes = s_total_files = 0;

		if ( in_root ) {
			root.subdirectory = in_root;
			avl_traverse_depth_first( &root, (traversal_callback) calculate_total_files_and_bytes, NULL, k_prefix, 0 );
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
		
		avl_traverse_depth_first( &root, (traversal_callback) calculate_directory_requirements, NULL, k_prefix, 0 );
		avl_traverse_depth_first( &root, (traversal_callback) calculate_directory_offsets, &start_sector, k_prefix, 0 );
	}
	if ( ! err ) {
		if ( ( xiso = open( xiso_path, WRITEFLAGS, 0644 ) ) == -1 ) open_err( xiso_path );
		if (out_iso_path) *out_iso_path = xiso_path;
		else {
			free(xiso_path);
			xiso_path = NULL;
		}
	}
	if ( ! err ) {
		memset(s_copy_buffer, 0, XISO_HEADER_OFFSET);
		if ( write( xiso, s_copy_buffer, XISO_HEADER_OFFSET ) != XISO_HEADER_OFFSET ) write_err();
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
			if ( ! err && read( in_xiso, s_copy_buffer, XISO_FILETIME_SIZE ) != XISO_FILETIME_SIZE ) read_err();
			if ( ! err && write( xiso, s_copy_buffer, XISO_FILETIME_SIZE ) != XISO_FILETIME_SIZE ) write_err();
		} else {
			if ( ( err = get_filetime_now(&ft) ) ) misc_err("cannot get current time");
			if ( ! err && write( xiso, &ft, XISO_FILETIME_SIZE ) != XISO_FILETIME_SIZE ) write_err();
		}
	}
	memset(s_copy_buffer, 0, XISO_UNUSED_SIZE);
	if ( ! err && write( xiso, s_copy_buffer, XISO_UNUSED_SIZE ) != XISO_UNUSED_SIZE ) write_err();
	if ( ! err && write( xiso, XISO_HEADER_DATA, XISO_HEADER_DATA_LENGTH ) != XISO_HEADER_DATA_LENGTH ) write_err();
	
	if ( ! err && ! in_root ) {
		if (chdir("..") == -1) chdir_err("..");
	}
	if (!err && (root.filename = strdup(iso_dir)) == NULL) mem_err();
	if (!err) {
		if (s_cp1252) {
			root.filename_cp1252 = root.filename;
			root.filename = NULL;
		}
		else if ((root.filename_cp1252 = getCP1252String(iso_dir)) == NULL) mem_err();
	}

	if ( ! err && lseek( xiso, (xoff_t) root.start_sector * XISO_SECTOR_SIZE, SEEK_SET ) == -1 ) seek_err();
	if ( ! err ) {
		wt_context.path = NULL;
		wt_context.xiso = xiso;
		wt_context.from = in_root ? in_xiso : -1;
		wt_context.progress = in_progress_callback;

		err = avl_traverse_depth_first( &root, (traversal_callback) write_tree, &wt_context, k_prefix, 0 );
	}

	if ( ! err && ( pos = lseek( xiso, (xoff_t) 0, SEEK_END ) ) == -1 ) seek_err();
	if (!err) {
		i = (int)((XISO_FILE_MODULUS - pos % XISO_FILE_MODULUS) % XISO_FILE_MODULUS);
		memset(s_copy_buffer, 0, i);
		if (write(xiso, s_copy_buffer, i) != i) write_err();
	}

	if ( ! err ) err = write_volume_descriptors( xiso, (uint32_t)((pos + (xoff_t)i) / XISO_SECTOR_SIZE) );

	if ( ! err && lseek( xiso, (xoff_t) XISO_OPTIMIZED_TAG_OFFSET, SEEK_SET ) == -1 ) seek_err();
	if ( ! err && write( xiso, XISO_OPTIMIZED_TAG, XISO_OPTIMIZED_TAG_LENGTH ) != XISO_OPTIMIZED_TAG_LENGTH ) write_err();

	if ( ! in_root ) {
		if ( err ) exiso_log( "\ncould not create %s%s\n", iso_name ? iso_name : "xiso", iso_name && ! in_name ? ".iso" : "" );
		else exiso_log( "\n\nsuccessfully created %s%s (%u files totalling %lld bytes added)\n", iso_name ? iso_name : "xiso", iso_name && ! in_name ? ".iso" : "", s_total_files, s_total_bytes );
	}
	
	if ( root.subdirectory ) avl_traverse_depth_first( root.subdirectory, (traversal_callback) free_dir_node_avl, NULL, k_postfix, 0 );
	
	if ( xiso != -1 ) {
		close( xiso );
		if (err && xiso_path) unlink(xiso_path);
	}
	
	if (root.filename) free(root.filename);
	if (root.filename_cp1252) free(root.filename_cp1252);

	if (in_dir_path) {
		free(in_dir_path);
		in_dir_path = NULL;
	}

	if ( cwd ) {
		if ( chdir( cwd ) == -1 ) chdir_err( cwd );
		free( cwd );
	}
	
	return err;
}


int decode_xiso( char *in_xiso, char *in_path, modes in_mode, char **out_iso_path ) {
	dir_node_avl		   *root = NULL;
	xoff_t					root_dir_start;
	uint32_t				root_dir_sect = 0, root_dir_size = 0;
	uint16_t				root_end_offset;
	size_t					len;
	int						xiso = 0, err = 0;
	char				   *path = NULL, *cwd = NULL, *name = NULL, *short_name = NULL, *iso_name = NULL;

	if ((xiso = open(in_xiso, READFLAGS, 0)) == -1) open_err(in_xiso);
	
	if (in_mode == k_rewrite) in_xiso[strlen(in_xiso) - 4] = '\0';

	if (!err) {
		if ((name = strrchr(in_xiso, PATH_CHAR))) name++;
		else name = in_xiso;

		// create a directory of the same name as the file we are working on, minus the ".iso" portion
		if ((len = strlen(name)) >= 4 && strcasecmp(&name[len - 4], ".iso") == 0) {
			name[len - 4] = '\0';
			if ((short_name = strdup(name)) == NULL) mem_err();
			name[len - 4] = '.';
		}

		iso_name = short_name ? short_name : name;
	}

	if (!err && in_path && (len = strlen(in_path)) > 0) {
		if (in_path[len - 1] == '/' || in_path[len - 1] == '\\') in_path[len - 1] = '\0';
	}

	if (!err && in_mode == k_extract) {
		if ((cwd = getcwd(NULL, 0)) == NULL) mem_err();
		if (in_path) {
			if (!err && strlen(in_path) == 0) misc_err("empty destination path");
			if (!err && mkdir(in_path, 0755) == -1) mkdir_err(in_path);
			if (!err && chdir(in_path) == -1) chdir_err(in_path);
		}
		else {
			if (!err && strlen(iso_name) == 0) misc_err("invalid xiso image name: %s", in_xiso);
			if (!err && mkdir(iso_name, 0755) == -1) mkdir_err(iso_name);
			if (!err && chdir(iso_name) == -1) chdir_err(iso_name);
		}
	}

	if (!err) err = verify_xiso(xiso, &root_dir_sect, &root_dir_size, name);

	if (!err && in_mode != k_rewrite) exiso_log("\n%s %s:\n", in_mode == k_extract ? "extracting" : "listing", name);

	if (!err) {
		if (asprintf(&path, "%s%c", in_path ? in_path : (in_mode != k_list ? iso_name : ""), PATH_CHAR) == -1) mem_err();

		root_dir_start = (xoff_t)root_dir_sect * XISO_SECTOR_SIZE + s_xbox_disc_lseek;
		root_end_offset = (uint16_t)(n_sectors(root_dir_size) * XISO_SECTOR_SIZE / XISO_DWORD_SIZE);
		
		if (in_mode == k_rewrite) {
			if (!err && root_dir_size == 0) root = NULL;
			else {
				if (!err) err = traverse_xiso(xiso, root_dir_start, 0, root_end_offset, path, k_generate_avl, &root, tree_strategy);
				if (!err) err = traverse_xiso(xiso, root_dir_start, 0, root_end_offset, path, k_generate_avl, &root, discover_strategy);
			}
			if (!err) err = create_xiso(iso_name, in_path, root, xiso, out_iso_path, NULL, NULL);
		}
		else {
			exiso_log("\n%s%s (0 bytes)%s", in_mode == k_extract ? "creating\t" : "", path, in_mode == k_extract ? " [OK]" : ""); flush();
			if (!err && root_dir_size != 0) err = traverse_xiso(xiso, root_dir_start, 0, root_end_offset, path, in_mode, NULL, discover_strategy);
		}
		
		if(path) free(path);
	}
	
	if ( err == err_iso_rewritten ) err = 0;
	if ( err ) misc_err( "failed to %s xbox iso image %s", in_mode == k_rewrite ? "rewrite" : in_mode == k_extract ? "extract" : "list", name );

	if ( xiso != -1 ) close( xiso );
	
	if ( short_name ) free( short_name );
	if ( cwd ) {
		if (chdir(cwd) == -1) chdir_err(cwd);
		free( cwd );
	}
	
	if (in_mode == k_rewrite) in_xiso[strlen(in_xiso)] = '.';

	return err;
}


int traverse_xiso(int in_xiso, xoff_t in_dir_start, uint16_t entry_offset, uint16_t end_offset, const char* in_path, modes in_mode, dir_node_avl** in_root, strategies strategy) {
	dir_node_avl	*avl = NULL;
	dir_node		*node = NULL;
	uint16_t		l_offset = 0, r_offset = 0;
	int				err = 0;
	char			*filename = NULL;

	if (entry_offset >= end_offset) misc_err("attempt to read node entry beyond directory end");

	do {
		if (!err && lseek_with_error(in_xiso, in_dir_start + (xoff_t)entry_offset * XISO_DWORD_SIZE, SEEK_SET) == -1) seek_err();

		if (!err && read(in_xiso, &l_offset, XISO_TABLE_OFFSET_SIZE) != XISO_TABLE_OFFSET_SIZE) read_err();
		if (!err && l_offset == XISO_PAD_SHORT) {
			if (entry_offset == 0) {	// Empty directories have padding starting at the beginning
				if (in_mode == k_generate_avl) *in_root = NULL;
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
		if (!err) if ((node = calloc(1, sizeof(dir_node))) == NULL) mem_err();
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
			if ((filename = malloc((size_t)node->filename_length + 1)) == NULL) mem_err();
			if (!err && read(in_xiso, filename, node->filename_length) != node->filename_length) read_err();
			if (!err) {
				filename[node->filename_length] = '\0';

				// security patch (Chris Bainbridge), modified by in to support "...", etc. 02.14.06 (in)
				if (!strcmp(filename, ".") || !strcmp(filename, "..") || strchr(filename, '/') || strchr(filename, '\\')) {
					log_err(__FILE__, __LINE__, "filename '%s' contains invalid character(s), aborting.", filename);
					exit(1);
				}

				if (s_cp1252) {
					node->filename = filename;
					filename = NULL;
				}
				else if ((node->filename = getUTF8String(filename)) == NULL) mem_err();
			}
			
		}

		// Process the node according to the mode
		if (!err) {
			if (in_mode == k_generate_avl) {
				if ((avl = (dir_node_avl*)calloc(1, sizeof(dir_node_avl))) == NULL) mem_err();
				if (!err) {
					if (s_cp1252) {
						avl->filename = NULL;
						avl->filename_cp1252 = node->filename;	// Don't set node->filename to NULL yet
					}
					else {
						avl->filename = node->filename;			// Don't set node->filename to NULL yet
						avl->filename_cp1252 = filename;
						filename = NULL;
					}
					avl->file_size = node->file_size;
					avl->old_start_sector = node->start_sector;
					avl->attributes = node->attributes;
					if (avl_insert(in_root, avl) != k_avl_error) {	// Insert node in tree
						err = process_node(in_xiso, node, in_path, in_mode, &avl->subdirectory, strategy);
					} else if (strategy == discover_strategy) {
						// If we're discovering files outside trees, we don't care about avl_insert errors, since
						// they represent nodes already discovered before, and we don't want to process them again.
						// We just free some memory previously allocated
						if (avl->filename) free(avl->filename);
						if (avl->filename_cp1252) free(avl->filename_cp1252);
						free(avl);
					}
					else misc_err("this iso appears to be corrupt");	// When not discovering, a duplicate node is an error
					node->filename = NULL;
				}
			}
			else {
				if (filename) {
					free(filename);
					filename = NULL;
				}
				err = process_node(in_xiso, node, in_path, in_mode, NULL, strategy);
			}
		}

		// Save next offset for discovery
		if (!err) entry_offset = n_dword(entry_offset * XISO_DWORD_SIZE + XISO_FILENAME_OFFSET + node->filename_length);

		// Free memory before recurring or iterating
		if (filename) {
			free(filename);
			filename = NULL;
		}
		if (node) {
			if (node->filename) free(node->filename);
			free(node);
			node = NULL;
		}

	} while (!err && entry_offset < end_offset && strategy == discover_strategy);	// Iterate only if using discover_strategy

	if (strategy != discover_strategy) {
		// Recurse on left node
		if (!err && l_offset) {
			if (lseek_with_error(in_xiso, in_dir_start + (xoff_t)l_offset * XISO_DWORD_SIZE, SEEK_SET) == -1) seek_err();
			if (!err) err = traverse_xiso(in_xiso, in_dir_start, l_offset, end_offset, in_path, in_mode, in_root, strategy);
		}

		// Recurse on right node
		if (!err && r_offset) {
			if (lseek_with_error(in_xiso, in_dir_start + (xoff_t)r_offset * XISO_DWORD_SIZE, SEEK_SET) == -1) seek_err();
			if (!err) err = traverse_xiso(in_xiso, in_dir_start, r_offset, end_offset, in_path, in_mode, in_root, strategy);
		}
	}

	return err;
}

int process_node(int in_xiso, dir_node* node, const char* in_path, modes in_mode, dir_node_avl** in_root, strategies strategy) {
	char		*path = NULL;
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
					exiso_log("\n%s%s%s%s (0 bytes)%s", in_mode == k_extract ? "creating\t" : "", in_path, node->filename, PATH_CHAR_STR, in_mode == k_extract ? " [OK]" : ""); flush();
				}
			}
		}

		if (!err) {
			// Recurse on subdirectory
			if (node->file_size == 0) {
				if (in_mode == k_generate_avl) *in_root = NULL;
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
				s_total_bytes += node->file_size;
			}
		}
	}

	return err;
}


#if 0
#pragma mark -
#endif


dir_node_avl *avl_fetch( dir_node_avl *in_root, const char *in_filename ) {
	int				result;

	for ( ;; ) {
		if ( in_root == NULL ) return NULL;
	
		result = avl_compare_key( in_filename, in_root->filename_cp1252);
	
		if ( result < 0 ) in_root = in_root->left;
		else if ( result > 0 ) in_root = in_root->right;
		else return in_root;
	}
}


avl_result avl_insert( dir_node_avl **in_root, dir_node_avl *in_node ) {
	avl_result		tmp;
	int				result;
	
	if ( *in_root == NULL ) { *in_root = in_node; return k_avl_balanced; }

	result = avl_compare_key( in_node->filename_cp1252, (*in_root)->filename_cp1252);
	
	if ( result < 0 ) return ( tmp = avl_insert( &(*in_root)->left, in_node ) ) == k_avl_balanced ? avl_left_grown( in_root ) : tmp;
	if ( result > 0 ) return ( tmp = avl_insert( &(*in_root)->right, in_node ) ) == k_avl_balanced ? avl_right_grown( in_root ) : tmp;
	
	return k_avl_error;
}


avl_result avl_left_grown( dir_node_avl **in_root ) {
	switch ( (*in_root)->skew ) {
		case k_left_skew:
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
			return no_err;
		
		case k_right_skew:
			(*in_root)->skew = k_no_skew;
			return no_err;
		
		default:
			return k_avl_balanced;
	}
}


avl_result avl_right_grown( dir_node_avl **in_root ) {
	switch ( (*in_root)->skew ) {
		case k_right_skew:
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
			return no_err;

		case k_left_skew:
			(*in_root)->skew = k_no_skew;
			return no_err;
		
		default:
			return k_avl_balanced;
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


int avl_compare_key( const char *in_lhs, const char *in_rhs ) {
	unsigned char		a, b;

	for ( ;; ) {
		a = (unsigned char)toupperCP1252(*in_lhs++);
		b = (unsigned char)toupperCP1252(*in_rhs++);
		
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

	if ( in_root == NULL ) return 0;
	
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


int boyer_moore_init( const char *in_pattern, size_t in_pat_len, size_t in_alphabet_size ) {
	size_t			j, k, t, t1, q, q1, *aux = NULL;
	int				err = 0;

	s_pattern = in_pattern;
	s_pat_len = in_pat_len;

	boyer_moore_done();	// Prepare for a new init

	if (in_pat_len == 0) return 0;
	
	// Delta1 table
	if ((s_bc_table = (size_t*)malloc(in_alphabet_size * sizeof(size_t))) == NULL) mem_err();
	if (!err) {
		for (k = 0; k < in_alphabet_size; k++) s_bc_table[k] = in_pat_len;
		for (k = 0; k < in_pat_len; k++) s_bc_table[(unsigned char)in_pattern[k]] = in_pat_len - 1 - k;
	}

	// Delta2 table (dd' algorithm with Rytter correction)
	if (!err && (s_gs_table = (size_t*)malloc(in_pat_len * sizeof(size_t))) == NULL) mem_err();
	if (!err && (aux = (size_t*)malloc(in_pat_len * sizeof(size_t))) == NULL) mem_err();
	if (!err) {
		// Step A1
		for (k = 1; k <= in_pat_len; k++) s_gs_table[k - 1] = 2 * in_pat_len - k;

		// Step A2
		for (j = in_pat_len, t = in_pat_len + 1; j > 0; j--, t--) {
			aux[j - 1] = t;
			while (t <= in_pat_len && in_pattern[j - 1] != in_pattern[t - 1]) {
				s_gs_table[t - 1] = min(s_gs_table[t - 1], in_pat_len - j);
				t = aux[t - 1];
			}
		}

		// Step B1
		q = t; t = in_pat_len + 1 - q;
		for (j = 1, t1 = 0; j <= t; t1++, j++) {
			aux[j - 1] = t1;
			while (t1 >= 1 && in_pattern[j - 1] != in_pattern[t1 - 1]) t1 = aux[t1 - 1];
		}

		// Step B2
		q1 = 1;
		while (q < in_pat_len) {
			for (k = q1; k <= q; k++) {
				s_gs_table[k - 1] = min(s_gs_table[k - 1], in_pat_len + q - k);
			}
			q1 = q + 1; q = q + t - aux[t - 1]; t = aux[t - 1];
		}
	}

	if (aux) free(aux);
	
	return err;
}


void boyer_moore_done() {
	if ( s_bc_table ) { free( s_bc_table ); s_bc_table = NULL; }
	if ( s_gs_table ) { free( s_gs_table ); s_gs_table = NULL; }
}


char* boyer_moore_search(char* in_text, size_t in_text_len) {
	size_t	i, j;

	if (s_pat_len == 0) return in_text;

	i = s_pat_len - 1;
	while (i < in_text_len) {
		for (j = s_pat_len - 1; in_text[i] == s_pattern[j]; --i, --j) {
			if (j == 0) return in_text + i;
		}

		i += max(s_bc_table[(unsigned char)in_text[i]], s_gs_table[j]);
	}
	return NULL;
}


#if 0
#pragma mark -
#endif


int extract_file(int in_xiso, dir_node *in_file, modes in_mode, const char* path) {
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
			if (in_file->file_size == 0) exiso_log("%s%s%s (0 bytes) [100%%]\r", in_mode == k_extract ? "extracting\t" : "", path, in_file->filename);
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
						exiso_log("%s%s%s (%u bytes) [%.1f%%]\r", in_mode == k_extract ? "extracting\t" : "", path, in_file->filename, in_file->file_size, totalpercent);

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


int free_dir_node_avl( dir_node_avl *in_dir_node_avl, unused void *in_context, unused int in_depth ) {
	if ((in_dir_node_avl->attributes & XISO_ATTRIBUTE_DIR) && in_dir_node_avl->subdirectory) avl_traverse_depth_first(in_dir_node_avl->subdirectory, (traversal_callback)free_dir_node_avl, NULL, k_postfix, 0);
	
	if (in_dir_node_avl->filename) free(in_dir_node_avl->filename);
	free(in_dir_node_avl->filename_cp1252);
	free(in_dir_node_avl);

	return 0;
}


int write_tree( dir_node_avl *in_avl, write_tree_context *in_context, unused int in_depth ) {
	xoff_t					pos = 0, dir_start = (xoff_t)in_avl->start_sector * XISO_SECTOR_SIZE;
	write_tree_context		context = { 0 };
	char*					filename = in_avl->filename ? in_avl->filename : in_avl->filename_cp1252;
	int						err = 0, pad = 0;

	if (in_avl->attributes & XISO_ATTRIBUTE_DIR) {
		if (asprintf(&context.path, "%s%s%c", in_context->path ? in_context->path : "", in_context->path ? filename : "", PATH_CHAR) == -1) mem_err();

		if ( ! err ) {
			exiso_log( "\nadding\t%s (0 bytes) [OK]", context.path );
	
			if (in_avl->subdirectory) {
				context.xiso = in_context->xiso;
				context.from = in_context->from;
				context.progress = in_context->progress;
				context.final_bytes = in_context->final_bytes;
		
				if ( in_context->from == -1 ) {
					if ( chdir(filename) == -1 ) chdir_err(filename);
				}

				if ( ! err ) err = avl_traverse_depth_first( in_avl->subdirectory, (traversal_callback) write_file, &context, k_prefix, 0 );
				if ( ! err ) err = avl_traverse_depth_first( in_avl->subdirectory, (traversal_callback) write_tree, &context, k_prefix, 0 );

				if (!err && lseek(in_context->xiso, dir_start, SEEK_SET) == -1) seek_err();
				if (!err) err = avl_traverse_depth_first(in_avl->subdirectory, (traversal_callback)write_directory, in_context, k_prefix, 0);
				if (!err && (pos = lseek(in_context->xiso, 0, SEEK_CUR)) == -1) seek_err();
				if (!err) {
					pad = (int)((XISO_SECTOR_SIZE - (pos % XISO_SECTOR_SIZE)) % XISO_SECTOR_SIZE);
					memset(s_copy_buffer, XISO_PAD_BYTE, pad);
					if (write(in_context->xiso, s_copy_buffer, pad) != pad) write_err();
				}

				if ( ! err && in_context->from == -1 ) {
					if ( chdir( ".." ) == -1 ) chdir_err( ".." );
				}
			} else {
				memset(s_copy_buffer, XISO_PAD_BYTE, XISO_SECTOR_SIZE);
				if ((pos = lseek(in_context->xiso, dir_start, SEEK_SET)) == -1) seek_err();
				if (!err && write(in_context->xiso, s_copy_buffer, XISO_SECTOR_SIZE) != XISO_SECTOR_SIZE) write_err();
			}
		}

		if (context.path) free(context.path);
	}

	return err;
}


int write_file( dir_node_avl *in_avl, write_tree_context *in_context, unused int in_depth ) {
	char			*p = NULL;
	uint32_t		bytes = 0, size = 0;
	int				err = 0, fd = -1, n = 0, i = 0;
	size_t			len;
	bool			xbe_file;
	char*			filename = in_avl->filename ? in_avl->filename : in_avl->filename_cp1252;

	if (!(in_avl->attributes & XISO_ATTRIBUTE_DIR)) {
		if ( lseek( in_context->xiso, (xoff_t) in_avl->start_sector * XISO_SECTOR_SIZE, SEEK_SET ) == -1 ) seek_err();
		
		if ( ! err ) {
			if ( in_context->from == -1 ) {
				if ( ( fd = open( filename, READFLAGS, 0 ) ) == -1 ) open_err(filename);
			} else {
				if (lseek_with_error(fd = in_context->from, (xoff_t)in_avl->old_start_sector * XISO_SECTOR_SIZE + s_xbox_disc_lseek, SEEK_SET) == -1) seek_err();
			}
		}

		if ( ! err ) {
			exiso_log( "\nadding\t%s%s (%u bytes) ", in_context->path, filename, in_avl->file_size ); flush();

			bytes = in_avl->file_size;
			len = strlen(in_avl->filename_cp1252);
			xbe_file = len >= 4 && strcasecmp(&in_avl->filename_cp1252[len - 4], ".xbe") == 0;
			do {
				n = read(fd, s_copy_buffer + i, min(bytes, READWRITE_BUFFER_SIZE - i));
				if (n < 0) read_err();
				else if (n == 0) {	// Unexpected end of file
					if (i > 0 && write(in_context->xiso, s_copy_buffer, i) != i) write_err();	// Write remaining 'i' bytes
				}
				else {
					bytes -= n;
					if (s_media_enable && xbe_file) {
						n += i;
						for (p = s_copy_buffer; (p = boyer_moore_search(p, n - (long)(p - s_copy_buffer))) != NULL; p += XISO_MEDIA_ENABLE_LENGTH) p[XISO_MEDIA_ENABLE_BYTE_POS] = XISO_MEDIA_ENABLE_BYTE;
						if (bytes) {
							i = XISO_MEDIA_ENABLE_LENGTH - 1;
							n -= i;
						}
					}
					if (write(in_context->xiso, s_copy_buffer, n) != n) write_err();
					if (!err && s_media_enable && xbe_file && bytes) memcpy(s_copy_buffer, &s_copy_buffer[n], i);
				}
			} while (!err && bytes > 0 && n > 0);
			size = in_avl->file_size;
			in_avl->file_size -= bytes;

			if (!err) {
				i = (XISO_SECTOR_SIZE - (in_avl->file_size % XISO_SECTOR_SIZE)) % XISO_SECTOR_SIZE;
				if (i > 0) {
					memset(s_copy_buffer, XISO_PAD_BYTE, i);
					if (write(in_context->xiso, s_copy_buffer, i) != i) write_err();
				}
			}
			exiso_log(err ? "failed" : "[OK]");

			if (!err && size != in_avl->file_size) exiso_warn("File %s is truncated. Reported size: %u bytes, wrote size: %u bytes!", filename, size, in_avl->file_size);

			if (!err) {
				++s_total_files;
				s_total_bytes += in_avl->file_size;
				if (in_context->progress) (*in_context->progress)(s_total_bytes, in_context->final_bytes);
			}
		}
		
		if ( in_context->from == -1 && fd != -1 ) close( fd );
	}
	
	return err;
}


int write_directory( dir_node_avl *in_avl, write_tree_context* in_context, unused int in_depth ) {
	xoff_t		pos;
	uint16_t	l_offset, r_offset;
	uint32_t	file_size = (in_avl->attributes & XISO_ATTRIBUTE_DIR) ? n_sectors(in_avl->file_size) * XISO_SECTOR_SIZE : in_avl->file_size;
	uint8_t		length = (uint8_t)strlen(in_avl->filename_cp1252);
	int			err = 0, pad = 0;
		
	little32( in_avl->file_size );
	little32( in_avl->start_sector );
	
	l_offset = (uint16_t) (in_avl->left ? in_avl->left->offset / XISO_DWORD_SIZE : 0);
	r_offset = (uint16_t) (in_avl->right ? in_avl->right->offset / XISO_DWORD_SIZE : 0);
	
	little16( l_offset );
	little16( r_offset );
	
	memset(s_copy_buffer, XISO_PAD_BYTE, XISO_SECTOR_SIZE);
	
	if ( ( pos = lseek( in_context->xiso, 0, SEEK_CUR ) ) == -1 ) seek_err();
	if ( ! err ) pad = (int)((xoff_t)in_avl->offset + in_avl->dir_start - pos);
	if ( ! err && write( in_context->xiso, s_copy_buffer, pad ) != pad ) write_err();
	if ( ! err && write( in_context->xiso, &l_offset, XISO_TABLE_OFFSET_SIZE ) != XISO_TABLE_OFFSET_SIZE ) write_err();
	if ( ! err && write( in_context->xiso, &r_offset, XISO_TABLE_OFFSET_SIZE ) != XISO_TABLE_OFFSET_SIZE ) write_err();
	if ( ! err && write( in_context->xiso, &in_avl->start_sector, XISO_SECTOR_OFFSET_SIZE ) != XISO_SECTOR_OFFSET_SIZE ) write_err();
	if ( ! err && write( in_context->xiso, &file_size, XISO_FILESIZE_SIZE ) != XISO_FILESIZE_SIZE ) write_err();
	if ( ! err && write( in_context->xiso, &in_avl->attributes, XISO_ATTRIBUTES_SIZE ) != XISO_ATTRIBUTES_SIZE ) write_err();
	if ( ! err && write( in_context->xiso, &length, XISO_FILENAME_LENGTH_SIZE ) != XISO_FILENAME_LENGTH_SIZE ) write_err();
	if ( ! err && write( in_context->xiso, in_avl->filename_cp1252, length ) != length ) write_err();
	
	little32( in_avl->start_sector );
	little32( in_avl->file_size );
	
	return err;	
}


int calculate_directory_offsets( dir_node_avl *in_avl, uint32_t *io_current_sector, unused int in_depth ) {
	wdsafp_context			context = { 0 };

	if ( in_avl->attributes & XISO_ATTRIBUTE_DIR ) {
		if (!in_avl->subdirectory) {
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

	if (!(in_avl->attributes & XISO_ATTRIBUTE_DIR)) {
		in_avl->start_sector = *io_context->current_sector;
		*io_context->current_sector += n_sectors(in_avl->file_size);
	}
	
	return 0;
}


int calculate_total_files_and_bytes( dir_node_avl *in_avl, unused void *in_context, unused int in_depth ) {
	if (in_avl->attributes & XISO_ATTRIBUTE_DIR) {
		if (in_avl->subdirectory) {
			avl_traverse_depth_first(in_avl->subdirectory, (traversal_callback)calculate_total_files_and_bytes, NULL, k_prefix, 0);
		}
	} else {
		++s_total_files;
		s_total_bytes += in_avl->file_size;
	}
	return 0;
}


int calculate_directory_requirements( dir_node_avl *in_avl, void *in_context, unused int in_depth ) {
	if (in_avl->attributes & XISO_ATTRIBUTE_DIR) {
		if (in_avl->subdirectory) {
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
	
	length = XISO_FILENAME_OFFSET + (uint32_t)strlen(in_avl->filename_cp1252);
	length += ( XISO_DWORD_SIZE - ( length % XISO_DWORD_SIZE ) ) % XISO_DWORD_SIZE;
	
	if ( n_sectors( *out_size + length ) > n_sectors( *out_size ) ) {
		*out_size += ( XISO_SECTOR_SIZE - ( *out_size % XISO_SECTOR_SIZE ) ) % XISO_SECTOR_SIZE;
	}

	in_avl->offset = *out_size;

	*out_size += length;

	return 0;
}


int generate_avl_tree_local( dir_node_avl **out_root, int *io_n ) {
	struct dirent	   *p = NULL;
	struct stat			sb = { 0 };
	dir_node_avl	   *avl = NULL;
	DIR				   *dir = NULL;
	int					err = 0, i = 0, j = 0;
	bool				empty_dir = true;

	if ( ( dir = opendir( "." ) ) == NULL ) mem_err();

	while ( ! err && ( p = readdir( dir ) ) != NULL ) {
		if ( ! strcmp( p->d_name, "." ) || ! strcmp( p->d_name, ".." ) ) continue;

		for ( i = *io_n; i; --i ) exiso_log( "\b" );
		exiso_log( "%s", p->d_name );
		for ( j = i = (int) strcplen( p->d_name, !s_cp1252 ); j < *io_n; ++j ) exiso_log( " " );
		for ( j = i; j < *io_n; ++j ) exiso_log( "\b" );
		*io_n = i;
		flush();
		
		if ( ( avl = (dir_node_avl *) calloc( 1, sizeof(dir_node_avl) ) ) == NULL ) mem_err();
		if (!err && (avl->filename = strdup(p->d_name)) == NULL) mem_err();
		if (!err) {
			if (s_cp1252) {
				avl->filename_cp1252 = avl->filename;
				avl->filename = NULL;
			} else if ((avl->filename_cp1252 = getCP1252String(p->d_name)) == NULL) mem_err();
		}
		if ( ! err && lstat( p->d_name, &sb ) == -1 ) read_err();
		if ( ! err ) {
			if ( S_ISDIR( sb.st_mode ) ) {
				empty_dir = false;
				avl->attributes = XISO_ATTRIBUTE_DIR;

				if ( chdir(p->d_name) == -1 ) chdir_err(p->d_name);

				if ( ! err ) err = generate_avl_tree_local( &avl->subdirectory, io_n );
				if ( ! err && chdir( ".." ) == -1 ) chdir_err( ".." );
			} else if ( S_ISREG( sb.st_mode ) ) {
				if ( sb.st_size > (int64_t)UINT32_MAX ) {
					log_err( __FILE__, __LINE__, "file %s is too large for xiso, skipping...", p->d_name);
					if (avl->filename) free(avl->filename);
					free(avl->filename_cp1252);
					free(avl);
					continue;
				}
				empty_dir = false;
				avl->attributes = XISO_ATTRIBUTE_NOR;
				s_total_bytes += avl->file_size = (uint32_t) sb.st_size;
				++s_total_files;
			} else {
				if (avl->filename) free(avl->filename);
				free(avl->filename_cp1252);
				free(avl);
				continue;
			}
		}
		if ( ! err ) {
			if ( avl_insert( out_root, avl ) == k_avl_error ) misc_err( "error inserting file %s into tree (duplicate filename?)", avl->filename);
		} else {
			if ( avl ) {
				if (avl->filename) free(avl->filename);
				free(avl->filename_cp1252);
				free(avl);
			}
		}
	}
	
	if ( empty_dir ) *out_root = NULL;
	
	if ( dir ) closedir( dir );
	
	return err;
}


int get_filetime_now(file_time_t *ft) {
	time_t				now = 0;
	int					err = 0;

	if (ft == NULL) return 1;
	if ( ( now = time( NULL ) ) == -1 ) unknown_err();
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

void write_sector( int in_xiso, xoff_t in_start, const char *in_name, const char *in_extension ) {
	ssize_t			wrote;
	xoff_t			curpos = 0;
	int				fp = -1, err = 0;
	char		   *cwd, *sect = NULL, buf[ 256 ];

	if ( ( cwd = getcwd( NULL, 0 ) ) == NULL ) mem_err();
	if ( ! err && chdir( DEBUG_DUMP_DIRECTORY ) == -1 ) chdir_err( DEBUG_DUMP_DIRECTORY );

	sprintf( buf, "%" PRId64 ".%s.%s", in_start, in_name, in_extension ? in_extension : "");

	if ( ! err && ( fp = open( buf, WRITEFLAGS, 0644 ) ) == -1 ) open_err( buf );
	if ( ! err && ( curpos = lseek_with_error( in_xiso, 0, SEEK_CUR ) ) == -1 ) seek_err();
	if ( ! err && lseek_with_error( in_xiso, in_start, SEEK_SET ) == -1 ) seek_err();
	
	if ( ! err && ( sect = (char *) malloc( XISO_SECTOR_SIZE ) ) == NULL ) mem_err();
	
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
