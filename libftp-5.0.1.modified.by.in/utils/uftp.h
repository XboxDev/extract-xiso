#include <sys/types.h>
#include <FtpLibrary.h>
#include <strings.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <arpa/telnet.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <errno.h>
#include <glob.h>
#include "getline.h"
#include "list.h"

#define SYSTEMRC "/usr/share/etc/uftprc"
#define LINK ftp[frame]
#define NFRAMES 10
#define TIME(proc) settimer(), status = proc , showtimer(), status
#define ARGS char *w1,char *w2,char *w3,char *w4,char *w5,char *w6
#define NULLARGS "","","","","",""
#define log(x) FtpLog("uftp",x)
#define LOG_TRAFFIC_FILE "/usr/share/etc/uftp.log"

#ifdef MIN
#undef MIN
#endif

#define MIN(x,y) ((x)<(y)?(x):(y))
typedef struct
{
  FtpString host;
  FtpString user;
  FtpString pass;
  FtpString pwd;
  int lock;
} LINKINFO;

typedef struct
{
  char *cmd;
  int (*func)();
  int need;
  char *help;
} CMDS;

typedef struct _alias
{
  FtpString name,str;
  struct _alias *next;
} ALIAS;

typedef struct _token
{
  FtpString word;
  struct _token *next;
  struct _token *shift;
} TOKEN;



extern ALIAS *firstalias;
extern FTP *ftp[NFRAMES];
extern LINKINFO iftp[NFRAMES];
extern LIST *hosts;
extern int frame;
extern int lastcmd;
extern int trymode;
extern int hashmode;
extern int restmode;
extern int sleeptime;
extern int winsize;
extern int interactive;
extern time_t noopinterval,nooptimeout;
extern CMDS cmds[];
extern int status;
extern FtpString prompt;
extern FtpString defaultuser;
extern jmp_buf start;

char *word(char *,int);
char *readline(char *);

#ifndef _AIX
char *getpass(char *);
#endif

char *getrcname();
char *getaliasrcname();
char *getsetsrcname();
char *gethostsname();
char *makestr();
char *expandalias(char *str);
char *getprompt();
char *makefilename(char *,char *);
char *getalias(char *);

int tab_hook(char *,int,int*);
int compctl_hook(char *,int,int*);

int setenv(char *, char*);
int unsetenv(char *);
int printenv();
void getwinsz();

void intr(int);
void noop();
int  setargs(char *);

int myhash(FTP *,unsigned int);
STATUS my_error(FTP *, int, char *);


extern FtpString www_gateway;
extern int www_port;








