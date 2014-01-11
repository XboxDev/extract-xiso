extract-xiso v2.7.1 

integration of somsky's and Aiyyo's improvements
minor fixes 

extract-xiso v2.7.0 

Added XGD3 support (somsky)

extract-xiso v2.6.2 

Fix extraction bug (Aiyyo)

extract-xiso v2.6.1 

Fix -l (list files in iso) bug (My mistake).(Aiyyo)

extract-xiso v2.6 

Added new command line switch (-s -> skip $SystemUpdate folder). (Aiyyo)
Try to create destination directory and display progressbar during the extraction. (Aiyyo)

extract-xiso v2.5

Added a few patches from sourceforge, fixed some bugs.

NOTE:  I've added some documentation to the project page on sourceforge.
       Please look there to read about commonly used commands, it'll be
       worth your time to do so!

extract-xiso v2.4b2 by in:

Fixed some buffering problems in the ftp library and added beta support for
CD/DVD burning on MacOS X.  run the program with -h to see the command-line
options for burning.  I also added progress callbacks for UI implementors, of
course you'll have to be writing your UI in C or you won't be able to use
this feature.

extract-xiso v2.3 by in <in@fishtank.com>

This is a maintenance release only, fixed a bug in the path generation code.
Thanks to Hydra for submitting a fix.

extract-xiso v2.2 by in <in@fishtank.com>

New in this version:

*	Finally ported the code to Win32 as a command line tool!
*	Added automatic media check patching during xISO creation.
*	Fixed a couple of minor bugs and made a few optimizations.

Please read the following carefully as I want to answer some support requests
I get fairly commonly and let everyone know a few things that might be helpful.

First, I hardly ever do any programming on the PC.  I *think* I've got the code
ported pretty cleanly and it's worked in my testing, but there may be some bugs.
If you find that something doesn't work correctly, *PLEASE PLEASE* email me and
let me know.  If you can, please include the specific steps I would need to
take to recreate the program.  I'll do what I can to get it corrected.

Ok, now for some answers to some common questions:

ALL these answers assume you're running a modded xbox with an FTP capable
version of EvoX running as your dashboard.  extract-xiso assumes a default
username and password of "xbox" and "xbox" respectively.

Q:	I purchased an XBox DVD game and am worried that it might become scratched.
	How do I make a single legal backup so that I can protect my investment?
	
A:	One way to do this is to use an ftp client to copy the files from the
	D:\ drive of your XBox to your hard disk, say to a folder called:
	
	C:\backup		(unix and mac users think /backup)
	
	Then you would run:
	
	PC Users:			exiso.exe -c C:\backup
	Mac/Unix Users:  	extract-xiso -c /backup
	
	this would create a file in the local directory called backup.iso which
	you can then burn to a DVD.
	
Q:	Ok, that's great, but doesn't extract-xiso support builtin ftp?

A:	Yep.  Assume your xbox is at xbox.mydomain.com, or 10.1.2.3 if you don't
	have DNS support.  Then you would do:
	
	PC Users:			exiso.exe -f xbox.mydomain.com -c d:\ backup.iso
	Mac/Unix Users:		extract-xiso -f xbox.mydomain.com -c /d backup.iso
	
	An alternate form if you have a different ftp username and password might
	be:
	
	extract-xiso -u username -p password -f 10.1.2.3 -c /d /backupdir/backup.iso
	
	Note that there really isn't much of a difference between the PC version
	of this tool and the unix version.  With respect to specifying paths on
	the xbox, you can either use unix style paths or DOS style paths, it
	shouldn't matter.  What this means is that F:\GAMES\ on the xbox can also be
	thought of (or written ) as /f/games/.

Q:	How do I extract files from an xISO I backed up?

A:	The command lines for extraction are listed later in this document in
	other tutorials, and you can always just run the tool with no arguments
	for help text.  Read on!
	
Q:	What's the point of "rewriting" xISO's?

A:	Let's say you formerly created your backup with another tool, such as one
	that emulates GDFIMAGE.EXE.  Well what you will have then is an xISO that
	is *maximally* inefficient.  You can read below to see just what I mean by
	that.  Naturally you want your DVD to perform as well as it can so that
	when you restore your game it plays as fast as possible.
	
	To rewrite the xISO as an optimized xISO then, you would issue:
	
	extract-xiso -r backup.iso
	
	This will do its magic and you will be left with two files, backup.iso,
	which is now the optimized (burnable) iso, and backup.iso.old, which is
	your old backup image.
	
	If you include the -D option to the rewrite command above, the old image
	will be deleted (if no errors occur) after the operation.
	
Q:	I rewrote the xISO to optimize it and it's a different size now.  What gives?

A:	It is normal for a rewritten xISO to shrink or grow by a few kilobytes.  This
	is due to end-of-sector buffering and is nothing to worry about.  If you need
	more convincing, run extract-xiso with the -l option to list both the old and
	new xISO and compare the file/byte counts at the end of the lists.
	
Q:	I burned the xISO to a CD/DVD but it doesn't work, what gives?

A:	PC users:

	Make sure that the DVD is burned as "CD Mode1 (Blocking 2048) DVD".  I
	have been told that some recorder programs detect the image as a CD
	Mode 2 XA image with 2352 byte sectors.  If burned this way the image won't
	work.
	
	**NOTE**  If anyone can tell me how to write the xISO so that burners
	autodetect the 2048-byte sector format please let me know and I'll
	incorporate it into the program.  As of now I've been too lazy to look
	it up.
	
	Mac Users:
	
	Launch Toast, click "Disk Image", click "Select Image...", turn *OFF*
	"Auto detect format", select the .iso image and choose OK, click OK
	to accept the defaults of 2048 byte sectors, burn.
	
Q:	Cool app dude, will you make a GUI for platform <x> so that I can
	use my mouse and don't have to use the command line?
	
A:	Nope, I've got better things to do.  Since extract-xiso runs on linux,
	freebsd, macos-x, and now windows, should I write 4 GUI's?  Nah.
	
	Anyone who wants to implement a GUI around my tool should feel free to
	do so.  The source code is opensource and you can get a copy by emailing
	me if you for some reason can't get it through the normal distribution
	channels.
	
Q:	So I bought this xbox and now I want to pirate games, can you help?

A:	Get a life.

Q:	Dude, like I typed some stuff and I don't think your program works,
	can you help?
	
A:	See answer to previous question.

Q:	I'm running operating system <x>, and I ran your tool which crashed.  I
	took notes of the exact error message that the program reported, and
	I re-ran it to verify that it happened again.  Here are the exact steps
	I took which caused to problem to recur: <blah>.  Could this be a bug?
	
A:	Thank you, I'll look into it. 

Q:	Do you answer questions in the xbox-scene forums?

A:	Yep, when I can, but sometimes I forget to look there for weeks at a time :)

Q:	On the Mac, I get this error about "_asprintf: unresolved symbol".  What gives?

A:	You need to run the MacOS X installer CD and install the BSD subsystem.


Ok that's it, enjoy!!

in
June 21, 2003


--- PREVIOUS README's FOLLOW ---

extract-xiso v2.1 by in <in@fishtank.com>

*   This is a maintenance release only to resolve a few reported bugs in the
      xISO creation code.


extract-xiso v2.0 by in <in@fishtank.com>

    Greets to Project-X!

Features:

*   Now creates *OPTIMIZED* xISO files!

    - Every XBox xISO creation tool (except this one) is based on GDFIMAGE.EXE,
      the Microsoft xISO creation tool.  That tool (and all that followed) make
      xISO's that when burned to disc give the *WORST POSSIBLE PERFORMANCE* for
      looking up files!!  I suspect Microsoft did this intentionally to prevent
      developers from burning unsigned games that seek quickly, but that's just
      my theory...

      At any rate, the XBox DVD filesystem structure is designed so that
      directory entries can be stored as a binary tree, a data structure that
      provides massively faster search times when reading discs.  I have
      implemented my creation code around an AVL (height balanced) binary tree.

      This creates xISO's that when burned to disc provide *BEST POSSIBLE
      PERFORMANCE* at all times.

      To get an idea for how bad the performance is with GDFIMAGE.EXE and the
      like, take an xISO with say 4,000 files.  On average (meaning half the
      time it's WORSE), the directory entries have to be read 2,000 times to
      find *ANY* file!  In the worst case you have to read the directory
      entries 4,000 times!!  Ever wonder why your backups run slower than your
      "normal" games?  This is why!  With a height balanced AVL tree and an
      xISO with 4,000 files, on *AVERAGE* the directory entries have to be read
      11 times, and at WORST they have to be read 12 times.  Now that's fast
      (and *OPTIMIZED* ;-)!

      Note that if you are just extracting xISO's to a hard disk there is no
      performance gain.  It only matters if you burn it to a CD or DVD.

*   Rewrites existing xISO's in one operation as *OPTIMIZED* xISO's!

*   Creates *OPTIMIZED* xISO's from an FTP server.

*   Extracts both optimized xISO and non-optimized (normal) xISO files to
      either the local filesystem or to a directory on an FTP server.

*   Massive optimizations to the decoding algorithm.

*   Many compatibility bug fixes to the underlying FTP library.

*   Currently runs as a command-line tool on Linux, MacOS-X, and FreeBSD.

*   100% open source software which uses *NO* Microsoft code whatsoever

---- TO USE ----

Read the readme.txt for the 1.0-1.4 versions (below), or just run extract-xiso
on the command line for help text.  Please note that I *CANNOT* provide
technical support for this software to people who do not know how to use a
command line (there simply isn't enough time in the day), but please do send bug
reports or feature requests to the email above.

Enjoy,

in
May 10, 2003

---- OLD README'S ----

This is extract-xiso v1.4 by in <in@fishtank.com>

Hey, it's a new version!

First, read the readme for version 1.1 to get an idea of what the program does:


README.TXT FOR VERSION 1.1:

This is extract-xiso v1.1 by in <in@fishtank.com>

This tool will extract an xdvdfs (xbox iso) image into the current directory,
unless the -d <directory> option is given, in which case it will first change to
the specified directory and then extract.

The top level directory for any extracted xiso will be the name of the iso image
minus any (case-insensitive) '.iso' extension.

Passing the -q option (for quiet) will suppress any output.  Passing the -l
option (for list) will list the contents of the image but not extract.

Any number of iso files may be specified on the command line for extraction. Run
the extract-xiso command with no parameters for help text.

Currently there are three versions:

For darwin/MacOS-X, there is a dynamically linked binary called 'extract-xiso'.

For linux there are two versions, a dynamically linked ELF binary which should
run with libc6 called 'extract-xiso', and a statically linked binary called
'extract-xiso-static' which you should use if you get runtime errors (unresolved
symbols and so on) when you try to run the dynamically linked version.

Finally, if you unzip these files and they won't execute, you might try setting
the execute bit(s) on the file with a command like this from your shell prompt
(represented below by unix:~$):

unix:~$ chmod a+rx extract-xiso

Enjoy,

in - March 11, 2003


README.TXT for version 1.4:

v1.4 adds support for FreeBSD and an optimization to the runtime memory usage.

README.TXT FOR VERSION 1.3:

v1.3 is a maintenance release only--fixed a critical bug in the offset
calculation that was causing a core dump on a few xiso's.  Should work fine on
all xiso's now.

README.TXT FOR VERSION 1.2:

Before we get going, let me first say to everyone, don't you *DARE* send me an
email until you've *at least* read this file in its entirety.  I wrote it to
help you, so check it out.

That having been said, I'll outline the new features/changes and then I'll have
a section for newbies who have questions.

New features for version 1.2:

extract-xiso now supports direct-to-ftp-server extraction.  This is a real boon
for those of you who, like me, are really lazy.  The new switches are:

-f <ftp_server>

Specifying -f and then a hostname or IP address will cause extract-xiso to
connect to the supplied server name as the target of the extraction.

-u <user name> -p <password>

Since you probably have your ftp server set up to use a username and password,
you can specify these options to set those values.

-d <directory>

This option used to set the directory to extract to if you were inclined to
extract to someplace besides the current directory.  It still does this in
normal "extract to local filesystem" mode, but if extract-xiso is running in
upload (-f <ftp_server>) context it will change to the directory on the remote
server prior to uploading.  You'll pretty much want to use this switch whenever
you're using ftp.  Please note that in no case will extract-xiso try and create
the directory specified here.  I put this in as a safety measure because I
thought it was a good idea ;-)

Other than these changes, there isn't anything new other than a few bug fixes
and some optimizations to how it traverses the directory structure.  Things that
would probably bore you to tears, honestly :)

Now a few words about emailing me!

My email address is in here for those people that are interested in the
following:

1)  Requesting new features 2)  Reporting bugs 3)  Asking development-related
questions 4)  Telling me how much you like the program

I *DO NOT* have time to answer technical support questions.  If you think this
is *just* the program you've been looking for, but you don't know how to operate
a unix command line, please *DO NOT* waste my time asking me how to use this
program.  I'm sure there are numerous book stores in your area which have
beginner books on Unix and I know there are thousands (millions?) of web sites
dedicated to the topic.  Please help yourself to some of this information, I
assure you it will be worth your time.

Now, since I have received several emails asking the same question ("HOW DO I
USE THIS?") I'll give you the best answer I can:


NEWBIE SECTION

First, this will be targeted at MacOS X users, I'll assume you kids with linux
boxes know what you're doing...

Second, I'm going to assume you know nothing about unix here so pardon me if any
of this is redundant.

Third, I'll also assume you're using an xiso image of public domain software, I
don't/can't/won't condone software piracy, I'm sure you understand.

Fourth, I'll assume you've downloaded the stuffit file and extracted it to your
desktop.

So...

You first need to make sure the extract-xiso file itself is executable.  It
should be if you extract the archive with stuffit, but to make sure, open the
terminal and type:

cd ~/Desktop/macos-x

This will change to the folder on your Desktop that the extract-xiso file is in.

Now type:

ls -l extract-xiso

You should see that the file is executable (it'll have 'x' characters in the
permission string, as in:

-rwxr-xr-x    1 in       staff         21k Mar 14 15:05 extract-xiso*

If it doesn't have any x's, type:

chmod a+x extract_xiso

That'll make it executable (if you want, you can type this command anyway just
to make sure, it won't hurt anything).

Next, copy the file you want to extract to your Desktop.  You can do that in the
Finder.  Just move it to your Desktop from wherever it is now.

Now go back to your open Terminal window which has ~/Desktop/macos-x as the
current directory and type:

./extract-xiso ~/Desktop/file.iso

That's it!  You should see a bunch of files extract and when you click on your
Desktop you'll see a new folder with your files in it.

Now, let's say you wanted to extract directly to a ftp server of some sort... 
Here is how that might work:

Let's say you have an ftp server running at 192.168.1.5, for which your username
is "xbox" and your password is "xbox", and you also have an xiso file called
"my_files.iso".  Let's further surmise that your ftp server has a /f/ directory
which you want to upload the contents of the xiso to.  You would then type
something like this on your command line:

extract-xiso -f 192.168.1.5 -u xbox -p xbox -d /f my_files.iso

And away it would go.  Now for those of you who are *really* new to this, and
who are intending to upload to an ftp server which uses some sort of MS-DOS-like
directory nomenclature (like EvoX for instance ;-), don't worry about
backslashes versus forward slashes, just use a leading forward slash, convert
the backslashes to forward slashes, and omit the colons.

So if the path looks like it should be F:\ or F:\GAMES\ or somesuch, you'll just
use (for the -d argument, remember??) /f/ (or /f) and /f/games/ (or /f/games)
respectively.  Got it?

Until next time,

in
March 30, 2003
