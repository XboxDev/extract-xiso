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

#if defined( __LINUX__ )
	#define _LARGEFILE64_SOURCE
#endif


#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


#if defined( __FREEBSD__ ) || defined( __OPENBSD__ )
	#include <machine/limits.h>
#endif


#if ! defined( NO_FTP )
	#define libftp_client
	#include "libftp-5.0.1.modified.by.in/FtpLibrary.h"
#else
	typedef 							struct FTP_STAT { char type; unsigned long size; char *name; void *next; } FTP_STAT;

	#define FTP							void
	#define STATUS						int
	
	#define FtpBye( a )					do { } while ( 0 )
	#define FtpClose( a )				do { } while ( 0 )
	#define FtpBinary( a )				do { } while ( 0 )
	#define FtpChdir( a, b )			do { } while ( 0 )
	#define FtpMkdir( a, b )			do { } while ( 0 )
	#define FtpStatFree( a )			do { } while ( 0 )
	#define FtpStat( a, b, c )			do { } while ( 0 )
	#define FtpOpenRead( a, b )			do { } while ( 0 )
	#define FtpOpenWrite( a, b )		do { } while ( 0 )
	#define FtpReadBlock( a, b, c )		do { } while ( 0 )
	#define FtpWriteBlock( a, b, c )	do { } while ( 0 )
	#define FtpLogin( a, b, c, d, e )	do { } while ( 0 )
#endif

#if defined( _WIN32 )
	#include <direct.h>
	#include "win32/dirent.c"
#else
	#include <dirent.h>
	#include <limits.h>
	#include <unistd.h>
#endif


#if defined( __DARWIN__ )
	#define exiso_target				"macos-x"

	#define PATH_CHAR					'/'
	#define PATH_CHAR_STR				"/"

	// I'm not planning on distributing the cd-burning code on darwin, so BURN_ENABLED is 0
	#define BURN_ENABLED				0
	#define FORCE_ASCII					1
	#define READFLAGS					O_RDONLY
	#define WRITEFLAGS					O_WRONLY | O_CREAT | O_TRUNC
	#define READWRITEFLAGS				O_RDWR

	typedef	off_t						xoff_t;
#elif defined( __FREEBSD__ )
	#define exiso_target				"freebsd"

	#define PATH_CHAR					'/'
	#define PATH_CHAR_STR				"/"
	
	#define BURN_ENABLED				0
	#define FORCE_ASCII					1
	#define READFLAGS					O_RDONLY
	#define WRITEFLAGS					O_WRONLY | O_CREAT | O_TRUNC
	#define READWRITEFLAGS				O_RDWR

	typedef	off_t						xoff_t;
#elif defined( __LINUX__ )
	#define exiso_target				"linux"

	#define PATH_CHAR					'/'
	#define PATH_CHAR_STR				"/"
	
	#define BURN_ENABLED				0
	#define FORCE_ASCII					0
	#define READFLAGS					O_RDONLY | O_LARGEFILE
	#define WRITEFLAGS					O_WRONLY | O_CREAT | O_TRUNC | O_LARGEFILE
	#define READWRITEFLAGS				O_RDWR | O_LARGEFILE

	#define lseek						lseek64
	#define stat						stat64
	
	typedef __off64_t					xoff_t;
#elif defined( __OPENBSD__ )
	#define exiso_target				"openbsd"
#elif defined( _WIN32 )
	#define exiso_target				"win32"

	#define PATH_CHAR					'\\'
	#define PATH_CHAR_STR				"\\"
	
	#define BURN_ENABLED				0
	#define FORCE_ASCII					0
	#define READFLAGS					O_RDONLY | O_BINARY
	#define WRITEFLAGS					O_WRONLY | O_CREAT | O_TRUNC | O_BINARY
	#define READWRITEFLAGS				O_RDWR   | O_BINARY

	#define S_ISDIR( x )				( ( x ) & _S_IFDIR )
	#define S_ISREG( x )				( ( x ) & _S_IFREG )

	#define ULONG_MAX					0xfffffffful

	#include "win32/getopt.c"
	#include "win32/asprintf.c"
	
	#define lseek						_lseeki64
	#define mkdir( a, b )				mkdir( a )

	typedef __int32						int32_t;
    typedef __int64                     xoff_t;
#else
	#error unknown target, cannot compile!
#endif


#define swap16( n )						( ( n ) = ( n ) << 8 | ( n ) >> 8 )
#define swap32( n )						( ( n ) = ( n ) << 24 | ( n ) << 8 & 0xff0000 | ( n ) >> 8 & 0xff00 | ( n ) >> 24 )


#if BYTE_ORDER == BIG_ENDIAN
	#define big16( n )
	#define big32( n )
	#define little16( n )				swap16( n )
	#define little32( n )				swap32( n )
#else
	#define big16( n )					swap16( n )
	#define big32( n )					swap32( n )
	#define	little16( n )
	#define little32( n )
#endif


#if BURN_ENABLED
	#define BURN_OPTION_CHAR			"b"
	#define BURN_OPTION_TEXT			"    -b                  Burn xiso image to disc.\n"
#else
	#define BURN_OPTION_CHAR			""
	#define BURN_OPTION_TEXT			""
	
	enum { err_burn_aborted = -5004 };
#endif


#define DEBUG_VERIFY_XISO				0
#define DEBUG_OPTIMIZE_XISO				0
#define DEBUG_TRAVERSE_XISO_DIR			0


#ifndef DEBUG
	#define DEBUG						0
#endif


#if ! defined( __cplusplus ) && ! defined( bool )
	typedef int bool;
	enum { false, true };
#endif


#ifndef nil
	#define nil							0
#endif


#define exiso_version					"2.7.1 (01.11.14)"
#define VERSION_LENGTH					16

#define banner							"extract-xiso v" exiso_version " for " exiso_target " - written by in <in@fishtank.com>\n"

#if ! defined( NO_FTP )
#define usage() 						fprintf( stderr, \
"%s\n\
  Usage:\n\
\n\
    %s [options] [-[" BURN_OPTION_CHAR "lrx]] <file1.xiso> [file2.xiso] ...\n\
    %s [options] -c <dir> [name] [-c <dir> [name]] ...\n\
\n\
  Mutually exclusive modes:\n\
\n\
"	BURN_OPTION_TEXT	"\
    -c <dir> [name]     Create xiso from file(s) starting in (local or remote)\n\
                          <dir>.  If the [name] parameter is specified, the\n\
                          xiso will be created with the (path and) name given,\n\
                          otherwise the xiso will be created in the current\n\
                          directory with the name <dir>.iso.  The -c option\n\
                          may be specified multiple times to create multiple\n\
                          xiso images.\n\
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
                        This option is required when extracting to an ftp\n\
                          server.\n\
    -D                  In rewrite mode, delete old xiso after processing.\n\
    -h                  Print this help text and exit.\n\
    -f <ftp_server>     In create or extract mode, use <ftp_server> instead of\n\
                          the local filesystem.\n\
    -m                  In create or rewrite mode, disable automatic .xbe\n\
                          media enable patching (not recommended).\n\
    -p <password>       Ftp password (defaults to \"xbox\")\n\
    -q                  Run quiet (suppress all non-error output).\n\
    -Q                  Run silent (suppress all output).\n\
    -s                  Skip $SystemUpdate folder.\n\
    -u <user name>      Ftp user name (defaults to \"xbox\")\n\
    -v                  Print version information and exit.\n\
", banner, argv[ 0 ], argv[ 0 ] );
#else
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
#endif

#define exiso_log						if ( ! s_quiet ) printf
#define flush()							if ( ! s_quiet ) fflush( stdout )

#define mem_err()						{ log_err( __FILE__, __LINE__, "out of memory error\n" ); err = 1; }
#define read_err()						{ log_err( __FILE__, __LINE__, "read error: %s\n", strerror( errno ) ); err = 1; }
#define seek_err()						{ log_err( __FILE__, __LINE__, "seek error: %s\n", strerror( errno ) ); err = 1; }
#define write_err()						{ log_err( __FILE__, __LINE__, "write error: %s\n", strerror( errno ) ); err = 1; }
#define rread_err()						{ log_err( __FILE__, __LINE__, "unable to read remote file\n" ); err = 1; }
#define rwrite_err()					{ log_err( __FILE__, __LINE__, "unable to write to remote file\n" ); err = 1; }
#define unknown_err()					{ log_err( __FILE__, __LINE__, "an unrecoverable error has occurred\n" ); err = 1; }
#define open_err( in_file )				{ log_err( __FILE__, __LINE__, "open error: %s %s\n", ( in_file ), strerror( errno ) ); err = 1; }
#define chdir_err( in_dir )				{ log_err( __FILE__, __LINE__, "unable to change to directory %s: %s\n", ( in_dir ), strerror( errno ) ); err = 1; }
#define mkdir_err( in_dir )				{ log_err( __FILE__, __LINE__, "unable to create directory %s: %s\n", ( in_dir ), strerror( errno ) ); err = 1; }
#define ropen_err( in_file )			{ log_err( __FILE__, __LINE__, "unable to open remote file %s\n", ( in_file ) ); err = 1; }
#define rchdir_err( in_dir )			{ log_err( __FILE__, __LINE__, "unable to change to remote directory %s\n", ( in_dir ) ); err = 1; }
#define rmkdir_err( in_dir )			{ log_err( __FILE__, __LINE__, "unable to create remote directory %s\n", ( in_dir ) ); err = 1; }
#define misc_err( in_format, a, b, c )	{ log_err( __FILE__, __LINE__, ( in_format ), ( a ), ( b ), ( c ) ); err = 1; }


#ifndef min
	#define min( a , b )				( ( a ) < ( b ) ? ( a ) : ( b ) )
	#define max( a , b )				( ( a ) > ( b ) ? ( a ) : ( b ) )
#endif


#define GLOBAL_LSEEK_OFFSET       0xFD90000ul
#define XGD3_LSEEK_OFFSET         0x2080000ul

#define n_sectors( size )				( ( size ) / XISO_SECTOR_SIZE + ( ( size ) % XISO_SECTOR_SIZE ? 1 : 0 ) )

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
#define XISO_FILETIME_SIZE				8

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

#define EMPTY_SUBDIRECTORY				( (dir_node_avl *) 1 )

#define READWRITE_BUFFER_SIZE			0x00200000

#define FTP_DEFAULT_USERPASS			"xbox"

#define DEBUG_DUMP_DIRECTORY			"/Volumes/c/xbox/iso/exiso"

#if ! defined( NO_FTP )
#define GETOPT_STRING					BURN_OPTION_CHAR "c:d:Df:hlmp:qQrsu:vx"
#else
#define GETOPT_STRING					BURN_OPTION_CHAR "c:d:Dhlmp:qQrsvx"
#endif


typedef enum avl_skew { k_no_skew , k_left_skew , k_right_skew } avl_skew;
typedef enum avl_result { no_err, k_avl_error, k_avl_balanced } avl_result;
typedef enum avl_traversal_method { k_prefix, k_infix, k_postfix } avl_traversal_method;

typedef enum bm_constants { k_default_alphabet_size = 256 } bm_constants;

typedef enum modes { k_generate_avl, k_extract, k_upload, k_list, k_rewrite, k_burn } modes;
typedef enum errors { err_end_of_sector = -5001, err_iso_rewritten = -5002, err_iso_no_files = -5003 } errors;

typedef void (*progress_callback)( xoff_t in_current_value, xoff_t in_final_value );
typedef int (*traversal_callback)( void *in_node, void *in_context, long in_depth );

typedef struct dir_node dir_node;
typedef struct create_list create_list;
typedef struct dir_node_avl dir_node_avl;

struct dir_node {
	dir_node						   *left;
	dir_node						   *parent;
	dir_node_avl					   *avl_node;

	char							   *filename;

	unsigned short						r_offset;
	unsigned char						attributes;
	unsigned char						filename_length;
	
	unsigned long						file_size;
	unsigned long						start_sector;
};

struct dir_node_avl {
	unsigned long						offset;
	xoff_t								dir_start;

	char							   *filename;
	unsigned long						file_size;
	unsigned long						start_sector;
	dir_node_avl					   *subdirectory;
	
	unsigned long						old_start_sector;
	
	avl_skew							skew;	
	dir_node_avl					   *left;
	dir_node_avl					   *right;
};

struct create_list {
	char							   *path;
	char							   *name;
	create_list						   *next;
};

typedef struct FILE_TIME {
	unsigned long						l;
	unsigned long						h;
} FILE_TIME;

typedef struct wdsafp_context {
	xoff_t								dir_start;
	unsigned long					   *current_sector;
} wdsafp_context;

typedef struct write_tree_context {
	int									xiso;
	char							   *path;
	int									from;
	progress_callback					progress;
	xoff_t								final_bytes;
} write_tree_context;


int log_err( const char *in_file, int in_line, const char *in_format, ... );
void avl_rotate_left( dir_node_avl **in_root );
void avl_rotate_right( dir_node_avl **in_root );
int avl_compare_key( char *in_lhs, char *in_rhs );
avl_result avl_left_grown( dir_node_avl **in_root );
avl_result avl_right_grown( dir_node_avl **in_root );
dir_node_avl *avl_fetch( dir_node_avl *in_root, char *in_filename );
avl_result avl_insert( dir_node_avl **in_root, dir_node_avl *in_node );
int avl_traverse_depth_first( dir_node_avl *in_root, traversal_callback in_callback, void *in_context, avl_traversal_method in_method, long in_depth );

void boyer_moore_done();
char *boyer_moore_search( char *in_text, long in_text_len );
int boyer_moore_init( char *in_pattern, long in_pat_len, long in_alphabet_size );

int free_dir_node_avl( void *in_dir_node_avl, void *, long );
int extract_file( int in_xiso, dir_node *in_file, modes in_mode, char *path );
int open_ftp_connection( char *in_host, char *in_user, char *in_password, FTP **out_ftp );
int decode_xiso( char *in_xiso, char *in_path, modes in_mode, char **out_iso_path, bool in_ll_compat );
int verify_xiso( int in_xiso, int32_t *out_root_dir_sector, int32_t *out_root_dir_size, char *in_iso_name );
int traverse_xiso( int in_xiso, dir_node *in_dir_node, xoff_t in_dir_start, char *in_path, modes in_mode, dir_node_avl **in_root, bool in_ll_compat );
int create_xiso( char *in_root_directory, char *in_output_directory, dir_node_avl *in_root, int in_xiso, char **out_iso_path, char *in_name, progress_callback in_progress_callback );

FILE_TIME *alloc_filetime_now( void );
int generate_avl_tree_local( dir_node_avl **out_root, int *io_n );
int generate_avl_tree_remote( dir_node_avl **out_root, int *io_n );
int write_directory( dir_node_avl *in_avl, int in_xiso, int in_depth );
int write_file( dir_node_avl *in_avl, write_tree_context *in_context, int in_depth );
int write_tree( dir_node_avl *in_avl, write_tree_context *in_context, int in_depth );
int calculate_total_files_and_bytes( dir_node_avl *in_avl, void *in_context, int in_depth );
int calculate_directory_size( dir_node_avl *in_avl, unsigned long *out_size, long in_depth );
int calculate_directory_requirements( dir_node_avl *in_avl, void *in_context, int in_depth );
int calculate_directory_offsets( dir_node_avl *in_avl, unsigned long *io_context, int in_depth );
int write_dir_start_and_file_positions( dir_node_avl *in_avl, wdsafp_context *io_context, int in_depth );
int write_volume_descriptors( int in_xiso, unsigned long in_total_sectors );

#if DEBUG
void write_sector( int in_xiso, xoff_t in_start, char *in_name, char *in_extension );
#endif


static long								s_pat_len;
static FTP							   *s_ftp = nil;
static bool								s_quiet = false;
static char							   *s_pattern = nil;
static long							   *s_gs_table = nil;
static long							   *s_bc_table = nil;
static xoff_t							s_total_bytes = 0;
static int								s_total_files = 0;
static char							   *s_copy_buffer = nil;
static bool								s_real_quiet = false;
static bool								s_media_enable = true;
static xoff_t							s_total_bytes_all_isos = 0;
static int								s_total_files_all_isos = 0;
static bool								s_warned = 0;

static bool				                s_remove_systemupdate = false; 
static char				               *s_systemupdate = "$SystemUpdate"; 

static xoff_t							s_xbox_disc_lseek = 0;

#if BURN_ENABLED
	#if defined( __DARWIN__ )
		#include "darwin/burn.c"
	#endif
#endif


#if 0		// #pragma mark - inserts a spacer in the function popup of certain text editors (i.e. mine ;-)
#pragma mark -
#endif


int main( int argc, char **argv ) {
	struct stat		sb;
	create_list	   *create = nil, *p, *q, **r;
	int				i, fd, opt_char, err = 0, isos = 0;
	bool			burn = false, extract = true, rewrite = false, free_user = false, free_pass = false, x_seen = false, delete = false, optimized;
	char		   *cwd = nil, *server = nil, *pass, *path = nil, *user, *buf = nil, *new_iso_path = nil, tag[ XISO_OPTIMIZED_TAG_LENGTH * sizeof(long) ];

	if ( argc < 2 ) { usage(); exit( 1 ); }
	
	user = pass = FTP_DEFAULT_USERPASS;
	
	while ( ! err && ( opt_char = getopt( argc, argv, GETOPT_STRING ) ) != -1 ) {
		switch ( opt_char ) {
			case 'b': {
				if ( x_seen || rewrite || ! extract || create ) {
					usage();
					exit( 1 );
				}
				burn = true;
			} break;
		
			case 'c': {
				if ( burn || x_seen || rewrite || ! extract ) {
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

			case 'f': {
				if ( server ) free( server );
				if ( ( server = strdup( optarg ) ) == nil ) mem_err();
			} break;

			case 'h': {
				usage();
				exit( 0 );
			} break;
			
			case 'l': {
				if ( burn || x_seen || rewrite || create ) {
					usage();
					exit( 1 );
				}
				extract = false;
			} break;
			
			case 'm': {
				if ( burn || x_seen || ! extract ) {
					usage();
					exit( 1 );
				}
				s_media_enable = false;
			} break;
			
			case 'p': {
				if ( pass && free_pass ) free( pass );
				if ( ( pass = strdup( optarg ) ) == nil ) mem_err();
				free_pass = true;
			} break;
			
			case 'q': {
				s_quiet = true;
			} break;
			
			case 'Q': {
				s_quiet = s_real_quiet = true;
			} break;
			
			case 'r': {
				if ( burn || x_seen || ! extract || create ) {
					usage();
					exit( 1 );
				}
				rewrite = true;
			} break;

			case 's': {
				s_remove_systemupdate = true;
			} break;

			case 'u': {
				if ( user && free_user ) free( user );
				if ( ( user = strdup( optarg ) ) == nil ) mem_err();
				free_user = true;
			} break;

			case 'v': {
				printf( "%s", banner );
				exit( 0 );
			} break;
			
			case 'x': {
				if ( burn || ! extract || rewrite || create ) {
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
		if ( ( ! extract || rewrite ) && server ) { free( server ); server = nil; }

		if ( create ) { if ( optind < argc ) { usage(); exit( 1 ); } }
		else if ( optind >= argc || server && ! path ) { usage(); exit( 1 ); }
	
		exiso_log( "%s", banner );
	
		if ( ( server || extract ) && ( s_copy_buffer = (char *) malloc( READWRITE_BUFFER_SIZE ) ) == nil ) mem_err();
	}
	
	if ( ! err && server ) {
	#if defined( _WIN32 )
		WSADATA		ws_data;
		WORD		ws_version_requested = MAKEWORD( 2, 0 );

		if ( ( err = WSAStartup( ws_version_requested, &ws_data ) ) || LOBYTE( ws_data.wVersion ) != 2 || HIBYTE( ws_data.wVersion ) != 0 ) misc_err( "unable to initialize winsock v2 dll, aborting ftp operation\n", 0, 0, 0 );
	#endif

		if ( ! err ) err = open_ftp_connection( server, user, pass, &s_ftp );
	}

	if ( ! err && ( create || rewrite ) ) err = boyer_moore_init( XISO_MEDIA_ENABLE, XISO_MEDIA_ENABLE_LENGTH, k_default_alphabet_size );

	if ( ! err && create ) {
		for ( p = create; ! err && p != nil; ) {
			char			*tmp = nil;

			if ( p->name ) {
				for ( i = (int) strlen( p->name ); i >= 0 && p->name[ i ] != PATH_CHAR; --i ) ; ++i;

				if ( i ) {
					if ( ( tmp = (char *) malloc( i + 1 ) ) == nil ) mem_err();
					if ( ! err ) {
						strncpy( tmp, p->name, i );
						tmp[ i ] = 0;
					}
				}
			}
			
			if ( ! err ) err = create_xiso( p->path, tmp, nil, -1, nil, p->name ? p->name + i : nil, nil );

			if ( tmp ) free( tmp );

			q = p->next;

			if ( p->name ) free( p->name );
			free( p->path );
			free( p );
			
			p = q;
		}
	} else for ( i = optind; ! err && i < argc; ++i ) {
		++isos;
		exiso_log( "\n" );
		s_total_bytes = s_total_files = 0;
		
		if ( server && path ) {
			char				*tmp;
			
			if ( ( tmp = strdup( path ) ) == NULL ) { mem_err(); }
			else if ( tmp[ 1 ] == ':' ) {
				int				i, n;

				tmp[ 1 ] = tmp[ 0 ];
				tmp[ 0 ] = '/';

				for ( i = 2, n = (int) strlen( tmp ); i < n; ++i ) if ( tmp[ i ] == '\\' ) tmp[ i ] = '/';
			}
			if ( ! err && FtpChdir( s_ftp, tmp ) < 0 ) rchdir_err( tmp );
			if ( tmp ) free( tmp );
		}
		
		if ( ! err ) {
			optimized = false;
		
			if ( ( fd = open( argv[ i ], READFLAGS, 0 ) ) == -1 ) open_err( argv[ i ] );
			if ( ! err && lseek( fd, (xoff_t) XISO_OPTIMIZED_TAG_OFFSET, SEEK_SET ) == -1 ) seek_err();
			if ( ! err && read( fd, tag, XISO_OPTIMIZED_TAG_LENGTH ) != XISO_OPTIMIZED_TAG_LENGTH ) read_err();
			
			if ( fd != -1 ) close( fd );

			if ( ! err ) {
				tag[ XISO_OPTIMIZED_TAG_LENGTH ] = 0;

				if ( ! strncmp( tag, XISO_OPTIMIZED_TAG, XISO_OPTIMIZED_TAG_LENGTH_MIN ) ) optimized = true;

				if ( rewrite ) {
					if ( optimized ) {
						exiso_log( "%s is already optimized, skipping...\n", argv[ i ] );
						continue;
					}
				
					if ( ! err && ( buf = (char *) malloc( strlen( argv[ i ] ) + 5 ) ) == nil ) mem_err();	// + 5 magic number is for ".old\0"
					if ( ! err ) {
						sprintf( buf, "%s.old", argv[ i ] );
						if ( stat( buf, &sb ) != -1 ) misc_err( "%s already exists, cannot rewrite %s\n", buf, argv[ i ], 0 );
						if ( ! err && rename( argv[ i ], buf ) == -1 ) misc_err( "cannot rename %s to %s\n", argv[ i ], buf, 0 );
						
						if ( err ) { err = 0; free( buf ); continue; }
					}
					if ( ! err ) err = decode_xiso( buf, path, k_rewrite, &new_iso_path, true );
					if ( ! err && delete && unlink( buf ) == -1 ) log_err( __FILE__, __LINE__, "unable to delete %s\n", buf );
					
					if ( buf ) free( buf );
				} else {
					if ( burn && ! optimized ) {
						if ( s_quiet ) misc_err( "refusing to burn a non-optimized xiso in quiet mode\n", 0, 0, 0 );
						if ( ! err ) {
							exiso_log( "%s is not optimized!!\n", argv[ i ] );
							exiso_log( "... you should rewrite it with the -r option before you burn it.\n" );
							exiso_log( "\ncontinue burn [y/N]? " );
							
							*tag = 0;
							if ( *fgets( tag, sizeof(tag), stdin ) != 'y' && *tag != 'Y' ) err = err_burn_aborted;
						}
					}
				
					// the order of the mutually exclusive options here is important, the extract ? k_extract : k_list test *must* be the final comparison
					if ( ! err ) err = decode_xiso( argv[ i ], path, server ? k_upload : burn ? k_burn : extract ? k_extract : k_list, nil, ! optimized );

					if ( burn && err == err_burn_aborted ) exiso_log( "burn aborted.\n" );
				}
			}
		}
		
		if ( ! err && ! burn ) exiso_log( "\n%u files in %s total %lld bytes\n", s_total_files, rewrite ? new_iso_path : argv[ i ], (long long int) s_total_bytes );
		
		if ( new_iso_path ) {
			if ( ! err ) exiso_log( "\n%s successfully rewritten%s%s\n", argv[ i ], path ? " as " : ".", path ? new_iso_path : "" );
		
			free( new_iso_path );
			new_iso_path = nil;
		}
		
		if ( err == err_iso_no_files ) err = 0;
	}
	
	if ( ! err && isos > 1  && ! burn ) exiso_log( "\n%u files in %u xiso's total %lld bytes\n", s_total_files_all_isos, isos, (long long int) s_total_bytes_all_isos );
	if ( s_warned ) exiso_log( "\nWARNING:  Warning(s) were issued during execution--review stderr!\n" );
	
	if ( s_ftp ) FtpBye( s_ftp );
	
	boyer_moore_done();
	
	if ( s_copy_buffer ) free( s_copy_buffer );
	if ( user && free_user ) free( user );
	if ( pass && free_pass ) free( pass );
	if ( server ) {
	#if defined( _WIN32 )
		WSACleanup();
	#endif
		free( server );
	}
	if ( path ) free( path );
		
	return err;
}


int log_err( const char *in_file, int in_line, const char *in_format, ... ) {
	va_list			ap;
	char		   *format;
	int				ret;

#if DEBUG
	asprintf( &format, "%s:%u %s", in_file, in_line, in_format );
#else
	format = (char *) in_format;
#endif
	
	if ( s_real_quiet ) ret = 0;
	else {
		va_start( ap, in_format );
		ret = vfprintf( stderr, format, ap );
		va_end( ap );
	}

#if DEBUG
	free( format );
#endif

	return ret;
}


#if 0
#pragma mark -
#endif


int verify_xiso( int in_xiso, int32_t *out_root_dir_sector, int32_t *out_root_dir_size, char *in_iso_name ) {
	int				err = 0;
	char			buffer[ XISO_HEADER_DATA_LENGTH ];

	if ( lseek( in_xiso, (xoff_t) XISO_HEADER_OFFSET, SEEK_SET ) == -1 ) seek_err();
	if ( ! err && read( in_xiso, buffer, XISO_HEADER_DATA_LENGTH ) != XISO_HEADER_DATA_LENGTH ) read_err();
	if ( ! err && memcmp( buffer, XISO_HEADER_DATA, XISO_HEADER_DATA_LENGTH ) ) 
	{
    if ( lseek( in_xiso, (xoff_t) XISO_HEADER_OFFSET + GLOBAL_LSEEK_OFFSET, SEEK_SET ) == -1 ) seek_err();
    if ( ! err && read( in_xiso, buffer, XISO_HEADER_DATA_LENGTH ) != XISO_HEADER_DATA_LENGTH ) read_err();
    if ( ! err && memcmp( buffer, XISO_HEADER_DATA, XISO_HEADER_DATA_LENGTH ) ) {
    
      if ( lseek( in_xiso, (xoff_t) XISO_HEADER_OFFSET + XGD3_LSEEK_OFFSET, SEEK_SET ) == -1 ) seek_err();
      if ( ! err && read( in_xiso, buffer, XISO_HEADER_DATA_LENGTH ) != XISO_HEADER_DATA_LENGTH ) read_err();
      if ( ! err && memcmp( buffer, XISO_HEADER_DATA, XISO_HEADER_DATA_LENGTH ) ) misc_err( "%s does not appear to be a valid xbox iso image\n", in_iso_name, 0, 0 )
      else s_xbox_disc_lseek = XGD3_LSEEK_OFFSET;
    }
    else s_xbox_disc_lseek = GLOBAL_LSEEK_OFFSET;
  }
  else s_xbox_disc_lseek = 0;

	// read root directory information
	if ( ! err && read( in_xiso, out_root_dir_sector, XISO_SECTOR_OFFSET_SIZE ) != XISO_SECTOR_OFFSET_SIZE ) read_err();
	if ( ! err && read( in_xiso, out_root_dir_size, XISO_DIRTABLE_SIZE ) != XISO_DIRTABLE_SIZE ) read_err();

	little32( *out_root_dir_sector );
	little32( *out_root_dir_size );
	
	// seek to header tail and verify media tag
	if ( ! err && lseek( in_xiso, (xoff_t) XISO_FILETIME_SIZE + XISO_UNUSED_SIZE, SEEK_CUR ) == -1 ) seek_err();
	if ( ! err && read( in_xiso, buffer, XISO_HEADER_DATA_LENGTH ) != XISO_HEADER_DATA_LENGTH ) read_err();
	if ( ! err && memcmp( buffer, XISO_HEADER_DATA, XISO_HEADER_DATA_LENGTH ) ) misc_err( "%s appears to be corrupt\n", in_iso_name, 0, 0 );

	// seek to root directory sector
	if ( ! err ) {
		if ( ! *out_root_dir_sector && ! *out_root_dir_size ) {
			exiso_log( "xbox image %s contains no files.\n", in_iso_name );
			err = err_iso_no_files;
		} else {
			if ( lseek( in_xiso, (xoff_t) *out_root_dir_sector * XISO_SECTOR_SIZE, SEEK_SET ) == -1 ) seek_err();
		}
	}
	
	return err;
}




int create_xiso( char *in_root_directory, char *in_output_directory, dir_node_avl *in_root, int in_xiso, char **out_iso_path, char *in_name, progress_callback in_progress_callback ) {
	xoff_t					pos;
	dir_node_avl			root;
	FILE_TIME			   *ft = nil;
	write_tree_context		wt_context;
	unsigned long			start_sector;
	int						i, n, xiso = -1, err = 0;
	char				   *cwd = nil, *buf = nil, *iso_name, *xiso_path, *iso_dir;

	s_total_bytes = s_total_files = 0;

	memset( &root, 0, sizeof(dir_node_avl) );

	if ( ( cwd = getcwd( nil, 0 ) ) == nil ) mem_err();
	if ( ! err ) {
		if ( ! in_root ) {
			if ( ! s_ftp ) { if ( chdir( in_root_directory ) == -1 ) chdir_err( in_root_directory ); }
			else {
				if ( in_root_directory[ 1 ] == ':' ) {
					in_root_directory[ 1 ] = in_root_directory[ 0 ];
					in_root_directory[ 0 ] = '/';

					for ( i = 2, n = (int) strlen( in_root_directory ); i < n; ++i ) if ( in_root_directory[ i ] == '\\' ) in_root_directory[ i ] = '/';
				}
				if ( FtpChdir( s_ftp, in_root_directory ) < 0 ) rchdir_err( in_root_directory );
			}
			if ( ! err ) {
				if ( in_root_directory[ i = (int) strlen( in_root_directory ) - 1 ] == '/' || in_root_directory[ i ] == '\\' ) in_root_directory[ i-- ] = 0;
				for ( iso_dir = &in_root_directory[ i ]; iso_dir >= in_root_directory && *iso_dir != PATH_CHAR; --iso_dir ) ; ++iso_dir;

				iso_name = in_name ? in_name : iso_dir;
			}
		} else {
			iso_dir = iso_name = in_root_directory;
		}
	}
	if ( ! err ) {
		if ( ! *iso_dir ) iso_dir = PATH_CHAR_STR;
		if ( ! in_output_directory ) in_output_directory = cwd;
		if ( in_output_directory[ i = (int) strlen( in_output_directory ) - 1 ] == PATH_CHAR ) in_output_directory[ i-- ] = 0;
		if ( ! iso_name || ! *iso_name ) iso_name = "root";
		else if ( iso_name[ 1 ] == ':' ) { iso_name[ 1 ] = iso_name[ 0 ]; ++iso_name; }
#if defined( _WIN32 )
		if ( ( asprintf( &xiso_path, "%s%c%s%s", *in_output_directory ? in_output_directory : cwd, PATH_CHAR, iso_name, in_name ? "" : ".iso" ) ) == -1 ) mem_err();
#else
		if ( ( asprintf( &xiso_path, "%s%s%s%c%s%s", *in_output_directory == PATH_CHAR ? "" : cwd, *in_output_directory == PATH_CHAR ? "" : PATH_CHAR_STR, in_output_directory, PATH_CHAR, iso_name, in_name ? "" : ".iso" ) ) == -1 ) mem_err();
#endif
	}
	if ( ! err ) {
		exiso_log( "%s %s%s:\n\n", in_root ? "rewriting" : "\ncreating", iso_name, in_name ? "" : ".iso" );

		root.start_sector = XISO_ROOT_DIRECTORY_SECTOR;		

		s_total_bytes = s_total_files = 0;

		if ( in_root ) {
			root.subdirectory = in_root;
			avl_traverse_depth_first( in_root, (traversal_callback) calculate_total_files_and_bytes, nil, k_prefix, 0 );
		} else {
			int		i, n = 0;

			exiso_log( "generating avl tree from %sfilesystem: ", s_ftp ? "remote " : "" ); flush();
			
			err = s_ftp ? generate_avl_tree_remote( &root.subdirectory, &n ) : generate_avl_tree_local( &root.subdirectory, &n );

			for ( i = 0; i < n; ++i ) exiso_log( "\b" );
			for ( i = 0; i < n; ++i ) exiso_log( " " );
			for ( i = 0; i < n; ++i ) exiso_log( "\b" );

			exiso_log( "%s\n\n", err ? "failed!" : "[OK]" );
		}
	}
	if ( ! err && in_progress_callback ) (*in_progress_callback)( 0, s_total_bytes );
	if ( ! err ) {
		wt_context.final_bytes = s_total_bytes;
		
		s_total_bytes = s_total_files = 0;
		
		if ( root.subdirectory == EMPTY_SUBDIRECTORY ) root.start_sector = root.file_size = 0;
		
		start_sector = root.start_sector;
		
		avl_traverse_depth_first( &root, (traversal_callback) calculate_directory_requirements, nil, k_prefix, 0 );
		avl_traverse_depth_first( &root, (traversal_callback) calculate_directory_offsets, &start_sector, k_prefix, 0 );
	}
	if ( ! err && ( buf = (char *) malloc( n = max( READWRITE_BUFFER_SIZE, XISO_HEADER_OFFSET ) ) ) == nil ) mem_err();
	if ( ! err ) {
		if ( ( xiso = open( xiso_path, WRITEFLAGS, 0644 ) ) == -1 ) open_err( xiso_path );
		if ( out_iso_path ) *out_iso_path = xiso_path;
		else free( xiso_path );
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
			if ( lseek( in_xiso, (xoff_t) XISO_HEADER_OFFSET + XISO_HEADER_DATA_LENGTH + XISO_SECTOR_OFFSET_SIZE + XISO_DIRTABLE_SIZE, SEEK_SET ) == -1 ) seek_err();
			if ( ! err && read( in_xiso, buf, XISO_FILETIME_SIZE ) != XISO_FILETIME_SIZE ) read_err();
			if ( ! err && write( xiso, buf, XISO_FILETIME_SIZE ) != XISO_FILETIME_SIZE ) write_err();			

			memset( buf, 0, XISO_FILETIME_SIZE );
		} else {
			if ( ( ft = alloc_filetime_now() ) == nil ) mem_err();
			if ( ! err && write( xiso, ft, XISO_FILETIME_SIZE ) != XISO_FILETIME_SIZE ) write_err();
		}
	}
	if ( ! err && write( xiso, buf, XISO_UNUSED_SIZE ) != XISO_UNUSED_SIZE ) write_err();
	if ( ! err && write( xiso, XISO_HEADER_DATA, XISO_HEADER_DATA_LENGTH ) != XISO_HEADER_DATA_LENGTH ) write_err();
	
	if ( ! err && ! in_root ) {
		if ( s_ftp ) { if ( FtpChdir( s_ftp, ".." ) < 0 ) rchdir_err( ".." ); }
		else { if ( chdir( ".." ) == -1 ) chdir_err( ".." ); }
	}
	if ( ! err && ( root.filename = strdup( iso_dir ) ) == nil ) mem_err();

	if ( ! err && root.start_sector && lseek( xiso, (xoff_t) root.start_sector * XISO_SECTOR_SIZE, SEEK_SET ) == -1 ) seek_err();
	if ( ! err ) {
		wt_context.path = nil;
		wt_context.xiso = xiso;
		wt_context.from = in_root ? in_xiso : -1;
		wt_context.progress = in_progress_callback;

		err = avl_traverse_depth_first( &root, (traversal_callback) write_tree, &wt_context, k_prefix, 0 );
	}

	if ( ! err && ( pos = lseek( xiso, (xoff_t) 0, SEEK_END ) ) == -1 ) seek_err();
	if ( ! err && write( xiso, buf, i = (int) (( XISO_FILE_MODULUS - pos % XISO_FILE_MODULUS ) % XISO_FILE_MODULUS) ) != i ) write_err();

	if ( ! err ) err = write_volume_descriptors( xiso, ( pos + (xoff_t) i ) / XISO_SECTOR_SIZE );

	if ( ! err && lseek( xiso, (xoff_t) XISO_OPTIMIZED_TAG_OFFSET, SEEK_SET ) == -1 ) seek_err();
	if ( ! err && write( xiso, XISO_OPTIMIZED_TAG, XISO_OPTIMIZED_TAG_LENGTH ) != XISO_OPTIMIZED_TAG_LENGTH ) write_err();

	if ( ! in_root ) {
		if ( err ) { exiso_log( "\ncould not create %s%s\n", iso_name ? iso_name : "xiso", iso_name && ! in_name ? ".iso" : "" ); }
		else exiso_log( "\nsucessfully created %s%s (%u files totalling %lld bytes added)\n", iso_name ? iso_name : "xiso", iso_name && ! in_name ? ".iso" : "", s_total_files, (long long int) s_total_bytes );
	}
			
	if ( root.subdirectory != EMPTY_SUBDIRECTORY ) avl_traverse_depth_first( root.subdirectory, free_dir_node_avl, nil, k_postfix, 0 );
	
	if ( xiso != -1 ) {
		close( xiso );
		if ( err ) unlink( xiso_path );
	}
	
	if ( root.filename ) free( root.filename );
	if ( buf ) free( buf );
	if ( ft ) free( ft );

	if ( cwd ) {
		if ( chdir( cwd ) == -1 ) chdir_err( cwd );
		free( cwd );
	}
	
	return err;
}


int decode_xiso( char *in_xiso, char *in_path, modes in_mode, char **out_iso_path, bool in_ll_compat ) {
	dir_node_avl		   *root = nil;
	bool					repair = false;
	int32_t					root_dir_sect, root_dir_size;
	int						xiso, err = 0, len, path_len = 0, add_slash = 0;
	char				   *buf, *cwd = nil, *name = nil, *short_name = nil, *iso_name, *folder = nil;

	if ( ( xiso = open( in_xiso, READFLAGS, 0 ) ) == -1 ) open_err( in_xiso );
	
	if ( ! err ) {
		len = (int) strlen( in_xiso );
	
		if ( in_mode == k_rewrite ) {
			in_xiso[ len -= 4 ] = 0;
			repair = true;
		}
	
		for ( name = &in_xiso[ len ]; name >= in_xiso && *name != PATH_CHAR; --name ) ; ++name;

		len = (int) strlen( name );

		// create a directory of the same name as the file we are working on, minus the ".iso" portion
		if ( len > 4 && name[ len - 4 ] == '.' && ( name[ len - 3 ] | 0x20 ) == 'i' && ( name[ len - 2 ] | 0x20 ) == 's' && ( name[ len - 1 ] | 0x20 ) == 'o' ) {
			name[ len -= 4 ] = 0;
			if ( ( short_name = strdup( name ) ) == nil ) mem_err();
			name[ len ] = '.';
		}
	}
	
	if ( ! err && ! len ) misc_err( "invalid xiso image name: %s\n", in_xiso, 0, 0 );

	if ( ! err && in_mode == k_extract && in_path ) {
		if ( ( cwd = getcwd( nil, 0 ) ) == nil ) mem_err();
		if ( ! err && mkdir( in_path, 0755 ) );
		if ( ! err && chdir( in_path ) == -1 ) chdir_err( in_path );
	}

	if ( ! err ) err = verify_xiso( xiso, &root_dir_sect, &root_dir_size, name );

	iso_name = short_name ? short_name : name;

	if ( ! err && in_mode != k_rewrite ) {
		exiso_log( "%s %s:\n\n", in_mode == k_extract ? "extracting" : in_mode == k_upload ? "uploading" : in_mode == k_burn ? "burning" : "listing", name );

		if ( in_mode == k_extract ) {
			if ( ! in_path ) {
				if ( ( err = mkdir( iso_name, 0755 ) ) ) mkdir_err( iso_name );
				if ( ! err && ( err = chdir( iso_name ) ) ) chdir_err( iso_name );
			}
		} else if ( in_mode == k_upload ) {
			if ( FtpMkdir( s_ftp, iso_name ) < 0 ) rmkdir_err( iso_name );
			if ( ! err && FtpChdir( s_ftp, iso_name ) < 0 ) rchdir_err( iso_name );
		}
	}
	
	if ( ! err && root_dir_sect && root_dir_size ) {						
		if ( in_path ) {
			path_len = (int) strlen( in_path );
			if ( in_path[ path_len - 1 ] != PATH_CHAR ) ++add_slash;
		}
		
		if ( ( buf = (char *) malloc( path_len + add_slash + strlen( iso_name ) + 2 ) ) == nil ) mem_err();
		
		if ( ! err ) {
			sprintf( buf, "%s%s%s%c", in_path ? in_path : "", add_slash && ( ! in_path || in_mode == k_upload ) ? PATH_CHAR_STR : "", in_mode != k_list && ( ! in_path || in_mode == k_upload ) ? iso_name : "", PATH_CHAR );

			if ( in_mode == k_rewrite ) {
	
	if ( ! err && lseek( xiso, (xoff_t) root_dir_sect * XISO_SECTOR_SIZE + s_xbox_disc_lseek, SEEK_SET ) == -1 ) seek_err();
	if ( ! err ) err = traverse_xiso( xiso, nil, (xoff_t) root_dir_sect * XISO_SECTOR_SIZE + s_xbox_disc_lseek, buf, k_generate_avl, &root, in_ll_compat );
	if ( ! err ) err = create_xiso( iso_name, in_path, root, xiso, out_iso_path, nil, nil );
			
			} else if ( in_mode == k_burn ) {
			#if BURN_ENABLED
				#if __DARWIN__
					static DRDeviceRef			device = nil;
				#else
					void					   *device = nil;
				#endif
				
					if ( ! device ) {
					#if __DARWIN__
						err = darwin_select_burn_device( &device );
					#endif
					}
					
					if ( ! err ) {
					#if __DARWIN__
						err = darwin_burn_xiso( in_xiso, xiso, device );
					#endif
					}
			#endif
			} else {
	      if ( ! err && lseek( xiso, (xoff_t) root_dir_sect * XISO_SECTOR_SIZE + s_xbox_disc_lseek, SEEK_SET ) == -1) seek_err();
	      if ( ! err ) err = traverse_xiso( xiso, nil, (xoff_t) root_dir_sect * XISO_SECTOR_SIZE + s_xbox_disc_lseek, buf, in_mode, nil, in_ll_compat );
			}

			free( buf );
		}
	}
	
	if ( err == err_iso_rewritten ) err = 0;
	if ( err && err != err_burn_aborted ) misc_err( "failed to %s xbox iso image %s\n", in_mode == k_rewrite ? "rewrite" : in_mode == k_burn ? "burn" : in_mode == k_extract ? "extract" : in_mode == k_upload ? "upload" : "list", name, 0 );

	if ( xiso != -1 ) close( xiso );
		
	if ( short_name ) free( short_name );
	if ( cwd ) {
		chdir( cwd );
		free( cwd );
	}
	
	if ( repair ) in_xiso[ strlen( in_xiso ) ] = '.';

	return err;
}


int traverse_xiso( int in_xiso, dir_node *in_dir_node, xoff_t in_dir_start, char *in_path, modes in_mode, dir_node_avl **in_root, bool in_ll_compat ) {
	dir_node_avl		   *avl;
	char				   *path;
	xoff_t					curpos;
	dir_node				subdir;
	dir_node			   *dir, node;
	int						err = 0, sector;
	unsigned short			l_offset = 0, tmp;

	if ( in_dir_node == nil ) in_dir_node = &node;

	memset( dir = in_dir_node, 0, sizeof(dir_node) );

read_entry:

    if ( ! err && read( in_xiso, &tmp, XISO_TABLE_OFFSET_SIZE ) != XISO_TABLE_OFFSET_SIZE ) read_err();

	if ( ! err ) {
		if ( tmp == XISO_PAD_SHORT ) {
			l_offset = l_offset * XISO_DWORD_SIZE + ( XISO_SECTOR_SIZE - ( l_offset * XISO_DWORD_SIZE ) % XISO_SECTOR_SIZE );
			err = lseek( in_xiso, in_dir_start + (xoff_t) l_offset, SEEK_SET ) == -1 ? 1 : 0;
		
			if ( ! err ) goto read_entry;		// me and my silly comments
		} else {
			l_offset = tmp;
		}
	}

	if ( ! err && read( in_xiso, &dir->r_offset, XISO_TABLE_OFFSET_SIZE ) != XISO_TABLE_OFFSET_SIZE ) read_err();
	if ( ! err && read( in_xiso, &dir->start_sector, XISO_SECTOR_OFFSET_SIZE ) != XISO_SECTOR_OFFSET_SIZE ) read_err();
	if ( ! err && read( in_xiso, &dir->file_size, XISO_FILESIZE_SIZE ) != XISO_FILESIZE_SIZE ) read_err();
	if ( ! err && read( in_xiso, &dir->attributes, XISO_ATTRIBUTES_SIZE ) != XISO_ATTRIBUTES_SIZE ) read_err();
	if ( ! err && read( in_xiso, &dir->filename_length, XISO_FILENAME_LENGTH_SIZE ) != XISO_FILENAME_LENGTH_SIZE ) read_err();

	if ( ! err ) {
		little16( l_offset );
		little16( dir->r_offset );
		little32( dir->file_size );
		little32( dir->start_sector );

		if ( ( dir->filename = (char *) malloc( dir->filename_length + 1 ) ) == nil ) mem_err();
	}
	
	if ( ! err ) {
		if ( read( in_xiso, dir->filename, dir->filename_length ) != dir->filename_length ) read_err();
		if ( ! err ) {
			dir->filename[ dir->filename_length ] = 0;
	
			// security patch (Chris Bainbridge), modified by in to support "...", etc. 02.14.06 (in)
			if ( ! strcmp( dir->filename, "." ) || ! strcmp( dir->filename, ".." ) || strchr( dir->filename, '/' ) || strchr( dir->filename, '\\' ) ) {
				log_err( __FILE__, __LINE__, "filename '%s' contains invalid character(s), aborting.", dir->filename );
				exit( 1 );
			}
		}
	}
	
	if ( ! err && in_mode == k_generate_avl ) {
		if ( ( avl = (dir_node_avl *) malloc( sizeof(dir_node_avl) ) ) == nil ) mem_err();
		if ( ! err ) {
			memset( avl, 0, sizeof(dir_node_avl) );
			
			if ( ( avl->filename = strdup( dir->filename ) ) == nil ) mem_err();
		}
		if ( ! err ) {
			dir->avl_node = avl;

			avl->file_size = dir->file_size;
			avl->old_start_sector = dir->start_sector;
						
			if ( avl_insert( in_root, avl ) == k_avl_error ) misc_err( "this iso appears to be corrupt\n", 0, 0, 0 );
		}
	}

	if ( ! err && l_offset ) {
		in_ll_compat = false;
	
		if ( ( dir->left = (dir_node *) malloc( sizeof(dir_node) ) ) == nil ) mem_err();
		if ( ! err ) {
			memset( dir->left, 0, sizeof(dir_node) );
			if ( lseek( in_xiso, in_dir_start + (xoff_t) l_offset * XISO_DWORD_SIZE, SEEK_SET ) == -1 ) seek_err();
		}
		if ( ! err ) {
			dir->left->parent = dir;
			dir = dir->left;
			
			goto read_entry;
		}
	}

left_processed:
			
	if ( dir->left ) { free( dir->left ); dir->left = nil; }

	if ( ! err && ( curpos = lseek( in_xiso, 0, SEEK_CUR ) ) == -1 ) seek_err();
	
	if ( ! err ) {
		if ( dir->attributes & XISO_ATTRIBUTE_DIR ) {
			if ( in_path ) {
				if ( ( path = (char *) malloc( strlen( in_path ) + dir->filename_length + 2 ) ) == nil ) mem_err();
			
				if ( ! err ) {
					sprintf( path, "%s%s%c", in_path, dir->filename, PATH_CHAR );
					if ( dir->start_sector && lseek( in_xiso, (xoff_t) dir->start_sector * XISO_SECTOR_SIZE + s_xbox_disc_lseek, SEEK_SET ) == -1 ) seek_err();
				}
			} else path = nil;
	
			if ( ! err ) {
				if ( !s_remove_systemupdate || !strstr( dir->filename, s_systemupdate ) )
				{
				if ( in_mode == k_extract ) {
					if ( ( err = mkdir( dir->filename, 0755 ) ) ) mkdir_err( dir->filename );
					if ( ! err && dir->start_sector && ( err = chdir( dir->filename ) ) ) chdir_err( dir->filename );
				} else if ( in_mode == k_upload ) {
					if ( FtpMkdir( s_ftp, dir->filename ) < 0 ) rmkdir_err( dir->filename );
					if ( ! err && dir->start_sector && FtpChdir( s_ftp, dir->filename ) < 0 ) rchdir_err( dir->filename );
				}
				if ( ! err && in_mode != k_list && in_mode != k_generate_avl ) exiso_log( "creating %s (0 bytes) [OK]\n", path );
			}
			}
			
			if ( ! err && dir->start_sector ) {
				memcpy( &subdir, dir, sizeof(dir_node) );
				
				subdir.parent = nil;
				if ( ! err && dir->file_size > 0 ) err = traverse_xiso( in_xiso, &subdir, (xoff_t) dir->start_sector * XISO_SECTOR_SIZE + s_xbox_disc_lseek, path, in_mode, in_mode == k_generate_avl ? &dir->avl_node->subdirectory : nil, in_ll_compat );        

				if ( !s_remove_systemupdate || !strstr( dir->filename, s_systemupdate ) )
				{
	
				if ( ! err && in_mode == k_extract && ( err = chdir( ".." ) ) ) chdir_err( ".." );
				if ( ! err && in_mode == k_upload && FtpChdir( s_ftp, ".." ) < 0 ) rchdir_err( ".." );
			}
			}
	
			if ( path ) free( path );
		} else if ( in_mode != k_generate_avl ) {
			if ( ! err ) {
				if ( !s_remove_systemupdate || !strstr( in_path, s_systemupdate ) )
				{

				if ( in_mode == k_extract || in_mode == k_upload ) {
						err = extract_file( in_xiso, dir, in_mode, in_path );
				} else {
					exiso_log( "%s%s%s (%lu bytes)%s", in_mode == k_extract ? "extracting " : in_mode == k_upload ? "uploading " : "", in_path, dir->filename, dir->file_size , in_mode == k_extract || in_mode == k_upload ? " " : "" ); flush();
					exiso_log( "\n" );
				}

				++s_total_files;
				++s_total_files_all_isos;
				s_total_bytes += dir->file_size;
				s_total_bytes_all_isos += dir->file_size;
				}
			}
		}
	}
	
	if ( ! err && dir->r_offset ) {
		// compatibility for iso's built as linked lists (bleh!)
		if ( in_ll_compat && (xoff_t) dir->r_offset * XISO_DWORD_SIZE / XISO_SECTOR_SIZE > ( sector = (int) (( curpos - in_dir_start ) / XISO_SECTOR_SIZE )) ) dir->r_offset = sector * ( XISO_SECTOR_SIZE / XISO_DWORD_SIZE ) + ( XISO_SECTOR_SIZE / XISO_DWORD_SIZE );
		
		if ( ! err && lseek( in_xiso, in_dir_start + (xoff_t) dir->r_offset * XISO_DWORD_SIZE, SEEK_SET ) == -1 ) seek_err();
		if ( ! err ) {
			if ( dir->filename ) { free( dir->filename ); dir->filename = nil; }

			l_offset = dir->r_offset;
			
			goto read_entry;
		}
	}

	if ( dir->filename ) free( dir->filename );
	
	if ( ( dir = dir->parent ) ) goto left_processed;
	
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


int avl_traverse_depth_first( dir_node_avl *in_root, traversal_callback in_callback, void *in_context, avl_traversal_method in_method, long in_depth ) {
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
		for ( i = 0; i < in_pat_len - 1; ++i ) s_bc_table[ (unsigned char) in_pattern[ i ] ] = in_pat_len - i - 1;
	
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
			l = s_bc_table[ (unsigned char) in_text[ j ] ];

			j += max( k, l );
			
			i = s_pat_len - 1;
		}
	}
	
	return i < 0 ? in_text + j + 1 : nil;
}


#if 0
#pragma mark -
#endif


int extract_file( int in_xiso, dir_node *in_file, modes in_mode , char* path) {
	char					c;
	int						err = 0;
	bool					ftp_open = false, warn = false;
	unsigned long			i, size, totalsize = 0, totalpercent = 0;
	int						out;

	if ( s_remove_systemupdate && strstr( path, s_systemupdate ) )
	{
		if ( ! err && lseek( in_xiso, (xoff_t) in_file->start_sector * XISO_SECTOR_SIZE + s_xbox_disc_lseek, SEEK_SET ) == -1 ) seek_err();
		}
	else
	{

	if ( in_mode == k_extract ) {
		if ( ( out = open( in_file->filename, WRITEFLAGS, 0644 ) ) == -1 ) open_err( in_file->filename );
	} else if ( in_mode == k_upload ) {
		if ( FtpOpenWrite( s_ftp, in_file->filename ) < 0 ) { ropen_err( in_file->filename ); }
		else ftp_open = true;
	} else err = 1;
	
		if ( ! err && lseek( in_xiso, (xoff_t) in_file->start_sector * XISO_SECTOR_SIZE + s_xbox_disc_lseek, SEEK_SET ) == -1 ) seek_err();

	if ( ! err ) {
			if ( in_file->file_size == 0 )
				exiso_log( "%s%s%s (0 bytes) [100%%]%s\r", in_mode == k_extract ? "extracting " : in_mode == k_upload ? "uploading " : "", path, in_file->filename, in_mode == k_extract || in_mode == k_upload ? " " : "" );
		if ( in_mode == k_extract ) {
			for ( i = 0, size = min( in_file->file_size, READWRITE_BUFFER_SIZE );
				  i < in_file->file_size && read( in_xiso, s_copy_buffer, size ) == (int) size;
				  i += size, size = min( in_file->file_size - i, READWRITE_BUFFER_SIZE ) )
			{
				if ( write( out, s_copy_buffer, size ) != (int) size ) {
					write_err();
					break;
				}
					totalsize += size;
					totalpercent = ( totalsize * 100.0 ) / in_file->file_size;
					exiso_log( "%s%s%s (%lu bytes) [%lu%%]%s\r", in_mode == k_extract ? "extracting " : in_mode == k_upload ? "uploading " : "", path, in_file->filename, in_file->file_size , totalpercent, in_mode == k_extract || in_mode == k_upload ? " " : "" );
			}
			
			close( out );
		} else {
			for ( i = 0, size = min( in_file->file_size, READWRITE_BUFFER_SIZE );
				  i < in_file->file_size && read( in_xiso, s_copy_buffer, size ) == (int) size;
				  i += size, size = min( in_file->file_size - i, READWRITE_BUFFER_SIZE ) )
			{
				if ( FtpWriteBlock( s_ftp, s_copy_buffer, size ) != (int) size ) {
					rwrite_err();
					break;
				}
					totalsize += size;
					totalpercent = ( totalsize * 100.0 ) / in_file->file_size;
					exiso_log( "%s%s%s (%lu bytes) [%lu%%]%s\r", in_mode == k_extract ? "extracting " : in_mode == k_upload ? "uploading " : "", path, in_file->filename, in_file->file_size , totalpercent, in_mode == k_extract || in_mode == k_upload ? " " : "" );
			}
		}
	}

	if ( ftp_open ) FtpClose( s_ftp );
	}

	if ( ! err ) exiso_log( "\n" );

	return err;
}


int open_ftp_connection( char *in_host, char *in_user, char *in_password, FTP **out_ftp ) {
	STATUS			err = 0;
	
	exiso_log( "\nlogging in to ftp server %s... ", in_host ); flush();

	if ( FtpLogin( out_ftp, in_host, in_user, in_password, nil ) < 0 ) err = 1;
	if ( ! err && FtpBinary( *out_ftp ) < 0 ) err = 1;
	
	exiso_log( "%s\n", err ? "failed!" : "[OK]" );

	if ( err && s_quiet ) misc_err( "unable to exiso_log in to ftp server %s\n", in_host, 0, 0 );

	return err;
}


int free_dir_node_avl( void *in_dir_node_avl, void *in_context, long in_depth ) {
	dir_node_avl	   *avl = (dir_node_avl *) in_dir_node_avl;

	if ( avl->subdirectory && avl->subdirectory != EMPTY_SUBDIRECTORY ) avl_traverse_depth_first( avl->subdirectory, free_dir_node_avl, nil, k_postfix, 0 );
	
	free( avl->filename );
	free( avl );

	return 0;
}


int write_tree( dir_node_avl *in_avl, write_tree_context *in_context, int in_depth ) {
	xoff_t					pos;
	write_tree_context		context;
	int						err = 0, pad;
	char					sector[ XISO_SECTOR_SIZE ];

	if ( in_avl->subdirectory ) {
		if ( in_context->path ) { if ( asprintf( &context.path, "%s%s%c", in_context->path, in_avl->filename, PATH_CHAR ) == -1 ) mem_err(); }
		else { if ( asprintf( &context.path, "%c", PATH_CHAR ) == -1 ) mem_err(); }

		if ( ! err ) {
			exiso_log( "adding %s (0 bytes) [OK]\n", context.path );
	
			if ( in_avl->subdirectory != EMPTY_SUBDIRECTORY ) {
				context.xiso = in_context->xiso;
				context.from = in_context->from;
				context.progress = in_context->progress;
				context.final_bytes = in_context->final_bytes;
		
				if ( in_context->from == -1 ) {
					if ( s_ftp ) { if ( FtpChdir( s_ftp, in_avl->filename ) < 0 ) rchdir_err( in_avl->filename ); }
					else { if ( chdir( in_avl->filename ) == -1 ) chdir_err( in_avl->filename ); }
				}
				if ( ! err && lseek( in_context->xiso, (xoff_t) in_avl->start_sector * XISO_SECTOR_SIZE, SEEK_SET ) == -1 ) seek_err();
				if ( ! err ) err = avl_traverse_depth_first( in_avl->subdirectory, (traversal_callback) write_directory, (void *) in_context->xiso, k_prefix, 0 );
				if ( ! err && ( pos = lseek( in_context->xiso, 0, SEEK_CUR ) ) == -1 ) seek_err();
				if ( ! err && ( pad = (int) (( XISO_SECTOR_SIZE - ( pos % XISO_SECTOR_SIZE ) ) % XISO_SECTOR_SIZE) ) ) {
					memset( sector, XISO_PAD_BYTE, pad );
					if ( write( in_context->xiso, sector, pad ) != pad ) write_err();
				}
				if ( ! err ) err = avl_traverse_depth_first( in_avl->subdirectory, (traversal_callback) write_file, &context, k_prefix, 0 );
				if ( ! err ) err = avl_traverse_depth_first( in_avl->subdirectory, (traversal_callback) write_tree, &context, k_prefix, 0 );
				if ( ! err && in_context->from == -1 ) {
					if ( s_ftp ) { if ( FtpChdir( s_ftp, ".." ) < 0 ) rchdir_err( ".." ); }
					else { if ( chdir( ".." ) == -1 ) chdir_err( ".." ); }
				}
				
				if ( context.path ) free( context.path );
			}
		}
	}

	return err;
}


int write_file( dir_node_avl *in_avl, write_tree_context *in_context, int in_depth ) {
	char		   *buf, *p;
	unsigned long	bytes, n, size;
	int				err = 0, fd = -1, i;

	if ( ! in_avl->subdirectory ) {
		if ( lseek( in_context->xiso, (xoff_t) in_avl->start_sector * XISO_SECTOR_SIZE, SEEK_SET ) == -1 ) seek_err();
		
		if ( ! err && ( buf = (char *) malloc( ( size = max( XISO_SECTOR_SIZE, READWRITE_BUFFER_SIZE ) ) + 1 ) ) == nil ) mem_err();
		if ( ! err ) {
			if ( in_context->from == -1 ) {
				if ( s_ftp ) { if ( FtpOpenRead( s_ftp, in_avl->filename ) < 0 ) ropen_err( in_avl->filename ); }
				else { if ( ( fd = open( in_avl->filename, READFLAGS, 0 ) ) == -1 ) open_err( in_avl->filename ); }
			} else {
				if ( lseek( fd = in_context->from, (xoff_t) in_avl->old_start_sector * XISO_SECTOR_SIZE, SEEK_SET ) == -1 ) seek_err();
			}
		}

		if ( ! err ) {
			exiso_log( "adding %s%s (%lu bytes) ", in_context->path, in_avl->filename, in_avl->file_size ); flush();

			if ( s_media_enable && ( i = (int) strlen( in_avl->filename ) ) >= 4 && in_avl->filename[ i - 4 ] == '.' && ( in_avl->filename[ i - 3 ] | 0x20 ) == 'x' && ( in_avl->filename[ i - 2 ] | 0x20 ) == 'b' && ( in_avl->filename[ i - 1 ] | 0x20 ) == 'e' ) {
				for ( bytes = in_avl->file_size, i = 0; ! err && bytes; ) {
					if ( s_ftp ) { if ( ( n = FtpReadBlock( s_ftp, buf + i, min( bytes, size - i ) ) ) < 0 ) rread_err(); }
					else { if ( ( n = read( fd, buf + i, min( bytes, size - i ) ) ) == -1 ) read_err(); }
					
					bytes -= n;
										
					if ( ! err ) {
						for ( buf[ n += i ] = 0, p = buf; ( p = boyer_moore_search( p, n - ( p - buf ) ) ) != nil; p += XISO_MEDIA_ENABLE_LENGTH ) p[ XISO_MEDIA_ENABLE_BYTE_POS ] = XISO_MEDIA_ENABLE_BYTE;
					
						if ( bytes ) {
							if ( write( in_context->xiso, buf, n - ( i = XISO_MEDIA_ENABLE_LENGTH - 1 ) ) != (int) n - i ) write_err();
							
							if ( ! err ) memcpy( buf, &buf[ n - ( XISO_MEDIA_ENABLE_LENGTH - 1 ) ], XISO_MEDIA_ENABLE_LENGTH - 1 );
						} else {
							if ( write( in_context->xiso, buf, n + i ) != (int) n + i ) write_err();
						}
					}
				}
			} else {
				for ( bytes = in_avl->file_size; ! err && bytes; bytes -= n ) {
					if ( s_ftp ) { if ( ( n = FtpReadBlock( s_ftp, buf, min( bytes, size ) ) ) < 0 ) rread_err(); }
					else { if ( ( n = read( fd, buf, min( bytes, size ) ) ) == -1 ) read_err(); }
	
					if ( ! err && write( in_context->xiso, buf, n ) != (int) n ) write_err();
				}
			}
			
			if ( ! err && ( bytes = ( XISO_SECTOR_SIZE - ( in_avl->file_size % XISO_SECTOR_SIZE ) ) % XISO_SECTOR_SIZE ) ) {
				memset( buf, XISO_PAD_BYTE, bytes );
				if ( write( in_context->xiso, buf, bytes ) != (int) bytes ) write_err();
			}
			
			if ( err ) { exiso_log( "failed\n" ); }
			else {
				exiso_log( "[OK]\n" );

				++s_total_files;
				s_total_bytes += in_avl->file_size;

				if ( in_context->progress ) (*in_context->progress)( s_total_bytes, in_context->final_bytes );
			}
		}
				
		if ( in_context->from == -1 && fd != -1 ) close( fd );
		if ( s_ftp ) FtpClose( s_ftp );
		if ( buf ) free( buf );
	}
	
	return err;
}


int write_directory( dir_node_avl *in_avl, int in_xiso, int in_depth ) {
	xoff_t				pos;
	int					err = 0, pad;
	unsigned short		l_offset, r_offset;
	char				length = (char) strlen( in_avl->filename ), attributes = in_avl->subdirectory ? XISO_ATTRIBUTE_DIR : XISO_ATTRIBUTE_ARC, sector[ XISO_SECTOR_SIZE ];
		
	little32( in_avl->file_size );
	little32( in_avl->start_sector );
	
	l_offset = (unsigned short) (in_avl->left ? in_avl->left->offset / XISO_DWORD_SIZE : 0);
	r_offset = (unsigned short) (in_avl->right ? in_avl->right->offset / XISO_DWORD_SIZE : 0);
	
	little16( l_offset );
	little16( r_offset );
	
	memset( sector, XISO_PAD_BYTE, XISO_SECTOR_SIZE );
	
	if ( ( pos = lseek( in_xiso, 0, SEEK_CUR ) ) == -1 ) seek_err();
	if ( ! err && ( pad = (int) ( (xoff_t) in_avl->offset + in_avl->dir_start - pos ) ) && write( in_xiso, sector, pad ) != pad ) write_err();
	if ( ! err && write( in_xiso, &l_offset, XISO_TABLE_OFFSET_SIZE ) != XISO_TABLE_OFFSET_SIZE ) write_err();
	if ( ! err && write( in_xiso, &r_offset, XISO_TABLE_OFFSET_SIZE ) != XISO_TABLE_OFFSET_SIZE ) write_err();
	if ( ! err && write( in_xiso, &in_avl->start_sector, XISO_SECTOR_OFFSET_SIZE ) != XISO_SECTOR_OFFSET_SIZE ) write_err();
	if ( ! err && write( in_xiso, &in_avl->file_size, XISO_FILESIZE_SIZE ) != XISO_FILESIZE_SIZE ) write_err();
	if ( ! err && write( in_xiso, &attributes, XISO_ATTRIBUTES_SIZE ) != XISO_ATTRIBUTES_SIZE ) write_err();
	if ( ! err && write( in_xiso, &length, XISO_FILENAME_LENGTH_SIZE ) != XISO_FILENAME_LENGTH_SIZE ) write_err();
	if ( ! err && write( in_xiso, in_avl->filename, length ) != length ) write_err();
	
	little32( in_avl->start_sector );
	little32( in_avl->file_size );
	
	return err;	
}


int calculate_directory_offsets( dir_node_avl *in_avl, unsigned long *io_current_sector, int in_depth ) {
	wdsafp_context			context;

	if ( in_avl->subdirectory ) {
		if ( in_avl->subdirectory == EMPTY_SUBDIRECTORY ) in_avl->start_sector = 0;
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


int write_dir_start_and_file_positions( dir_node_avl *in_avl, wdsafp_context *io_context, int in_depth ) {
	in_avl->dir_start = io_context->dir_start;

	if ( ! in_avl->subdirectory ) {
		in_avl->start_sector = *io_context->current_sector;
		*io_context->current_sector += n_sectors( in_avl->file_size );
	}
	
	return 0;
}


int calculate_total_files_and_bytes( dir_node_avl *in_avl, void *in_context, int in_depth ) {
	if ( in_avl->subdirectory ) {
		if ( in_avl->subdirectory != EMPTY_SUBDIRECTORY ) {
			avl_traverse_depth_first( in_avl->subdirectory, (traversal_callback) calculate_total_files_and_bytes, nil, k_prefix, 0 );
		}
	} else {
		++s_total_files;
		s_total_bytes += in_avl->file_size;
	}
	
	return 0;
}


int calculate_directory_requirements( dir_node_avl *in_avl, void *in_context, int in_depth ) {
	if ( in_avl->subdirectory && in_avl->subdirectory != EMPTY_SUBDIRECTORY ) {
		avl_traverse_depth_first( in_avl->subdirectory, (traversal_callback) calculate_directory_size, &in_avl->file_size, k_prefix, 0 );
		avl_traverse_depth_first( in_avl->subdirectory, (traversal_callback) calculate_directory_requirements, in_context, k_prefix, 0 );
	}
	
	return 0;
}


int calculate_directory_size( dir_node_avl *in_avl, unsigned long *out_size, long in_depth ) {
	unsigned long			length;

	if ( in_depth == 0 ) *out_size = 0;
	
	length = XISO_FILENAME_OFFSET + strlen( in_avl->filename );
	length += ( XISO_DWORD_SIZE - ( length % XISO_DWORD_SIZE ) ) % XISO_DWORD_SIZE;
	
	if ( n_sectors( *out_size + length ) > n_sectors( *out_size ) ) {
		*out_size += ( XISO_SECTOR_SIZE - ( *out_size % XISO_SECTOR_SIZE ) ) % XISO_SECTOR_SIZE;
	}

	in_avl->offset = *out_size;

	*out_size += length;

	return 0;
}


int generate_avl_tree_local( dir_node_avl **out_root, int *io_n ) {
	struct dirent	   *p;
	struct stat			sb;	
	dir_node_avl	   *avl;
	DIR				   *dir = nil;
	int					err = 0, i, j;
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
				empty_dir = false;
				if ( sb.st_size > ULONG_MAX ) {
					log_err( __FILE__, __LINE__, "file %s is too large for xiso, skipping...\n", avl->filename );
					free( avl->filename );
					free( avl );
					continue;
				}
				s_total_bytes += avl->file_size = (unsigned long) sb.st_size;
				++s_total_files;
			} else {
				free( avl->filename );
				free( avl );
				continue;
			}
		}
		if ( ! err ) {
			if ( avl_insert( out_root, avl ) == k_avl_error ) misc_err( "error inserting file %s into tree (duplicate filename?)\n", avl->filename, 0, 0 );
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


int generate_avl_tree_remote( dir_node_avl **out_root, int *io_n ) {
	dir_node_avl	   *avl;
	int					err = 0, i, j;
	FTP_STAT		   *ftp_stat = nil, *p;
	bool				empty_dir = true;

	if ( FtpStat( s_ftp, "", &ftp_stat ) < 0 ) misc_err( "unable to stat remote directory", 0, 0, 0 );

	for ( p = ftp_stat; ! err && p != nil; p = p->next ) {
		if ( ! strcmp( p->name, "." ) || ! strcmp( p->name, ".." ) ) continue;

		for ( i = *io_n; i; --i ) exiso_log( "\b" );
		exiso_log( "%s", p->name );
		for ( j = i = (int) strlen( p->name ); j < *io_n; ++j ) exiso_log( " " );
		for ( j = i; j < *io_n; ++j ) exiso_log( "\b" );
		*io_n = i;
		flush();

		if ( ( avl = (dir_node_avl *) malloc( sizeof(dir_node_avl) ) ) == nil ) mem_err();
		if ( ! err ) {
			memset( avl, 0, sizeof(dir_node_avl) );
			if ( ( avl->filename = strdup( p->name ) ) == nil ) mem_err();

			if ( ! err ) {
				if ( p->type == 'd' ) {
					empty_dir = false;
	
					if ( FtpChdir( s_ftp, avl->filename ) < 0 ) rchdir_err( avl->filename );
	
					if ( ! err ) err = generate_avl_tree_remote( &avl->subdirectory, io_n );
					if ( ! err && FtpChdir( s_ftp, ".." ) <= 0 ) rchdir_err( ".." );
				} else if ( p->type == '-' ) {
					empty_dir = false;
					s_total_bytes += avl->file_size = (unsigned long) p->size;
					++s_total_files;
				} else {
					free( avl->filename );
					free( avl );
					continue;
				}
			}
		}
		if ( ! err ) {
			if ( avl_insert( out_root, avl ) == k_avl_error ) misc_err( "error inserting file %s into tree (duplicate filename?)\n", avl->filename, 0, 0 );
		} else {
			if ( avl ) {
				if ( avl->filename ) free( avl->filename );
				free( avl );
			}
		}
	}
	
	if ( empty_dir ) *out_root = EMPTY_SUBDIRECTORY;
	
	if ( ftp_stat ) FtpStatFree( ftp_stat );
	
	return err;
}


FILE_TIME *alloc_filetime_now( void ) {
	FILE_TIME		   *ft;
	double				tmp;
	time_t				now;
	int					err = 0;

	if ( ( ft = (FILE_TIME *) malloc( sizeof(struct FILE_TIME) ) ) == nil ) mem_err();
	if ( ! err && ( now = time( nil ) ) == -1 ) unknown_err();
	if ( ! err ) {
		tmp = ( (double) now + ( 369.0 * 365.25 * 24 * 60 * 60 - ( 3.0 * 24 * 60 * 60 + 6.0 * 60 * 60 ) ) ) * 1.0e7;

		ft->h = (unsigned long) ( tmp * ( 1.0 / ( 4.0 * (double) ( 1 << 30 ) ) ) );
		ft->l = (unsigned long) ( tmp - ( (double) ft->h ) * 4.0 * (double) ( 1 << 30 ) );
		
		little32( ft->h );		// convert to little endian here because this is a PC only struct and we won't read it anyway
		little32( ft->l );
	} else if ( ft ) {
		free( ft );
		ft = nil;
	}
	
	return ft;
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

int write_volume_descriptors( int in_xiso, unsigned long in_total_sectors ) {
	int				big, err = 0, little;
	char			date[] = "0000000000000000";
	char			spaces[ ECMA_119_VOLUME_CREATION_DATE - ECMA_119_VOLUME_SET_IDENTIFIER ];

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
	xoff_t			curpos;
	int				fp = -1, err = 0;
	char		   *cwd, *sect = nil, buf[ 256 ];

	if ( ( cwd = getcwd( nil, 0 ) ) == nil ) mem_err();
	if ( ! err && chdir( DEBUG_DUMP_DIRECTORY ) == -1 ) chdir_err( DEBUG_DUMP_DIRECTORY );

	sprintf( buf, "%llu.%s.%s", in_start, in_name, in_extension ? in_extension : "" );

	if ( ! err && ( fp = open( buf, WRITEFLAGS, 0644 ) ) == -1 ) open_err( buf );
	if ( ! err && ( curpos = lseek( in_xiso, 0, SEEK_CUR ) ) == -1 ) seek_err();
	if ( ! err && lseek( in_xiso, in_start, SEEK_SET ) == -1 ) seek_err();
	
	if ( ! err && ( sect = (char *) malloc( XISO_SECTOR_SIZE ) ) == nil ) mem_err();
	
	if ( ! err && read( in_xiso, sect, XISO_SECTOR_SIZE ) != XISO_SECTOR_SIZE ) read_err();
	if ( ! err && ( wrote = write( fp, sect, XISO_SECTOR_SIZE ) ) != XISO_SECTOR_SIZE ) write_err();
	
	if ( ! err && lseek( in_xiso, curpos, SEEK_SET ) == -1 ) seek_err();
	
	if ( sect ) free( sect );
	if ( fp != -1 ) close( fp );

	if ( cwd ) {
		if ( chdir( cwd ) == -1 ) chdir_err( cwd );
		free( cwd );
	}
}

#endif
