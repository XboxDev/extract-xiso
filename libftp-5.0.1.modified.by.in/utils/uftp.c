/* File Transfer Protocol Toolkit based on libftp */

#include "uftp.h"
#include <varargs.h>


FTP *ftp[NFRAMES];
LINKINFO iftp[NFRAMES];
int frame=0;


int status;
jmp_buf start;
int lastcmd=0;
int trymode=1;
int restmode=1;
int hashmode=0;
int sleeptime=30;
int winsize;
int interactive=0;

time_t noopinterval=0;
time_t nooptimeout=1;
time_t prevtime=0;

FtpString prompt="%T %u@%H:%d> ";
FtpString defaultuser;

FtpString www_gateway="www.cern.ch";
int www_port=8080;

ALIAS *firstalias=NULL;
LIST *hosts=NULL;


/* if main have any arguments, interprets each it as command with args */


main(int argc, char **argv)
{
  register int i;
  register char *p1;

  FtpString params;
  FtpString tmp1,tmp2;
  
  if (setjmp(start)!=0) 
    goto main_loop;
  
  setsignals();

#if defined(TIOCGWINSZ) && defined(SIGWINCH)
  getwinsz();
#endif
  
  FtpSetErrorHandler(&FtpInit,my_error);
  FtpSetIOHandler(&FtpInit,my_error);
  
  strcpy(defaultuser,getpwuid(getuid())->pw_name);

  
  memset(ftp,0,sizeof(FTP*)*NFRAMES);
  memset(iftp,0,sizeof(LINKINFO)*NFRAMES);
  
  fprintf(stderr,"Welcome to uftp version %s (%s)\n",
	  strcpy(tmp1,word(rcsid,3)),
	  strcpy(tmp2,word(rcsid,4)));
  
  batch(SYSTEMRC);

  for (i=1, params[0]=0; i< argc; i++)
    {
      strcat(params,argv[i]);
      if (i+1!=argc) strcat(params," ");
    }
  
  load_link("ftp","open");
  load_link("ftp","connect");
  load_link("ftp","preconnect");
  
  
  batch(getrcname());
  batch(getaliasrcname());
  batch(getsetsrcname());

  load_compctl();

  if (params[0]!=0)
    {
      FtpString new;

      strcpy(new,"open ");
      
      if (ifalias(params)||ifcmd(params)) 
	  execute (params);
      else
	strcat(new,params),
	execute(new);
    }


  gl_tab_hook=/*tab*/compctl_hook;
  
 main_loop:

  interactive=1;
  
  setsignals();
  
  while (1)
    {
      
      setjmp(start);
      if (lastcmd) quit(0);
      
      
      p1=getline(getprompt());

      if (p1==NULL||*p1==0) 
	{
	  quit(0);
	}
      
      if (strchr(p1,'\n')!=NULL) *(char*)strchr(p1,'\n')=0;

      if (*word(p1,1)==0) continue;
      
      gl_histadd(p1);
      execute(p1);
    }
}
/* Exacute few command separated by ';' . The character ' must use for mark complex 
   works*/  

execute (char *cmd)
{
  FtpString w1,w2,w3,w4,w5,w6;
  FtpString newcmd;
  char *p;

  
  if (!*cmd || *cmd=='#' ) return;

  for ( p=newcmd ; *cmd; cmd++)
    {
      if ( *cmd == '\'' )
	{
	  *p++ = *cmd++;
	  while ( *cmd != '\'' && *cmd != 0 ) *p++ = *cmd++;
	  if ( *cmd == 0 ) 
	    return puts("Unbalanced \', please corrected!\n");
	  *p++ = *cmd;
	  continue;
	}
      
      if ( *cmd == ';' ) 
	{
	  *p=0;
	  execute(newcmd);
	  p=newcmd;
	  continue;
	}
      *p++ = *cmd;
    }

  
  *p=0;
  cmd=newcmd;
  
  if ( *cmd=='\\' ) 
    cmd++;
  else
    {
      FtpString new;
      strcpy(new,"\\");
      strcat(new,expandalias(cmd));
      return execute(new);
    }

  if ( *cmd == '!' ) 
    {
      int pid,_pid;
      int status;

      if (!(pid=fork()))
	{
	  execlp(((char*)getenv("SHELL")==NULL)?"/bin/sh":(char *)getenv("SHELL"),
		 "shell","-c",cmd+1,NULL);
	  exit(-1);
	}
      
      while(1)
	{
	  _pid=wait(&status);
	  if (_pid==pid)
	    return;
	}
    }
  
      
  redir(cmd);

  if (cmd[strlen(cmd)-1]=='&')
    {
      FtpString tmp;

      cmd[strlen(cmd)-1]=0;

      strcpy(tmp,"bg ");
      strcat(tmp,cmd);

      strcpy(cmd,tmp);
    }
  
  strcpy(w1,word(cmd,1));
  strcpy(w2,word(cmd,2));
  strcpy(w3,word(cmd,3));
  strcpy(w4,word(cmd,4));
  strcpy(w5,word(cmd,5));
  strcpy(w6,word(cmd,6));
  
  return executev(w1,w2,w3,w4,w5,w6);

  
}

executev(ARGS)
{
  CMDS *xcmd = &cmds[0];

  
  if (isdigit(*w1))
    return 
      atoi(w1)<NFRAMES?frame=atoi(w1):0,
      executev(w2,w3,w4,w5,w6,"");
  
  while ( xcmd -> cmd != NULL )
    {
      if ( !strcmp(xcmd->cmd,w1) && (xcmd -> func != NULL) )
	{
	  int status;
	  
	  if ( xcmd -> need && LINK == NULL)
	    return puts("Need connection to server");
	  iftp[frame].lock=1; unsetsignals();
	  status = (*xcmd->func)(w1,w2,w3,w4,w5,w6);
	  iftp[frame].lock=0; setsignals();
	  redirback();
	  return status;
	}
      xcmd++;
    }
  
  fprintf(stderr,"%s: unknown command\n",w1);
  return -1;
}


void intr(int sig)
{
  fprintf(stderr,"Interupted by signal %d\n",sig);
  if (LINK!=NULL) FtpSetHashHandler(LINK,NULL);
  setsignals();
  prevtime = time((time_t *)0);
  longjmp(start,1);
}

newframe(int connecteble)
{
  register int i;
  
  if (connecteble)
    for (i=0; i<NFRAMES; i++) if (ftp[i]!=NULL) return frame=i;
  for (i=0; i<NFRAMES; i++) if (ftp[i]==NULL) return frame=i;
  return -1;
}

STATUS my_error(FTP *ftp, int code, char *msg)
{

  if (code==LQUIT||(ftp==NULL)) log(msg);
  else
    FtpLog(ftp->title,msg);
  
  if ( abs(code) == 530 && (strstr(msg,"anonymous")!=NULL))
    {
      Ftp_reopen();
      longjmp(start,1);
    }
  longjmp(start,1);
}

char *getrcname()
{
  static FtpString rcpath;
  struct passwd *pwd=getpwuid(getuid());
  
  sprintf(rcpath,"%s/.uftprc",pwd->pw_dir);
  return rcpath;
}

char *getaliasrcname()
{
  static FtpString rcpath;
  struct passwd *pwd=getpwuid(getuid());
  
  sprintf(rcpath,"%s/.uftp_aliases",pwd->pw_dir);
  return rcpath;
}

char *getsetsrcname()
{
  static FtpString rcpath;
  struct passwd *pwd=getpwuid(getuid());
  
  sprintf(rcpath,"%s/.uftp_sets",pwd->pw_dir);
  return rcpath;
}

char *gethostsname()
{
  static FtpString rcpath;
  struct passwd *pwd=getpwuid(getuid());
  
  sprintf(rcpath,"%s/.uftp_hosts",pwd->pw_dir);
  return rcpath;
}



char *makestr(va_alist)
 va_dcl
{
  char *p1;
  va_list args;
  static FtpString new;

  new[0]=0;
  
  va_start(args);

  while(1)
    {
      p1=va_arg(args,char *);
      if (p1==NULL) break;
      if (*p1!=0) 
	{
	  if (new[0]!=0) strcat(new," ");
	  strcat(new,p1);
	}
    }
  va_end(args);
  return new;
}

  
#define ADD(str,chr) ( *(str)++ = (chr),*(str)=0)

INLINE ADDSTR(char **str, char *str1)
{
  while (*str1) *(*str)++ = *str1++;
  **str='\0';
}

char *expandalias(char *str)
{
  ALIAS *a=firstalias;
  static FtpString new;
  FtpString w1={0};
  char *p,*p1=new,*args;
  int dollar=0;

  new[0]=0;
      
  strcpy(w1,word(str,1));

  if ( (p=strchr(str,' '))!=NULL )
    args=p+1;
  else
    args="";
  
  while (a) 
    {
      if (!strcmp(a->name,w1))
	break;
      a=a->next;
    }
  
  if (!a) 
    return str;
  
  for ( p=a->str; *p; p++)
    {
      if ( *p != '$' ) 
	{
	  ADD(p1,*p);
	  continue;
	}
      
      dollar=1;
      p++;
      
      if (isdigit(*p)) 
	{
	  ADDSTR(&p1,word(str,(*p)-'0'+1));
	  continue;
	}

      switch (*p) 
	{ 
	  
	case '\0':
	case '$':  ADD(p1,'$');continue;
	case '*':  ADDSTR(&p1,args);continue;
	default:   ADD(p1,'$');ADD(p1,*p);continue;
	}
    }
  
  if (!dollar) 
    {
      ADD(p1,' ');
      ADDSTR(&p1,args);
    }

  *p=0;
  
  return new;
}

ifalias(char *cmd)
{
  if (*getalias(cmd)!=0)
    return 1;
  return 0;
}

char *getalias(char *cmd)
{
  FtpString what;
  ALIAS *a=firstalias;


  strcpy(what,word(cmd,1));
  
  while ( a!=NULL)
    {
      if (!strcmp(a->name,what))
	return a->str;
      a=a->next;
    }
  return "";
}


ifcmd(char *arg)
{
  CMDS *_cmds = &cmds[0];
  FtpString cmd;
  
  strcpy(cmd,word(arg,1));
  
  while (1)
    {
      if (_cmds->cmd==NULL) return 0;
      if (!strcmp(cmd,_cmds->cmd)) return 1;
      _cmds++;
    }
}


char *getprompt()
{
  
  static FtpString _prompt;
  FtpString tmp;
  char *s;

  _prompt[0]=0;

  for(s=prompt;*s;s++)
    switch (*s)
      {
      case '%':
	switch (*++s)
	  {
	  
	  case 'H': 
	    strcat(_prompt,iftp[frame].host);
	    break;
	    
	  case 'h':
	    strcpy(tmp,iftp[frame].host);
	    if (strchr(tmp,'.')!=NULL) *(char *)strchr(tmp,'.')=0;
	    strcat(_prompt,tmp);
	    break;

	  case 'M': 
	    gethostname(tmp, sizeof tmp);
	    strcat(_prompt,gethostbyname(tmp)->h_name);
	    break;
	    
	  case 'm':
	    gethostname(tmp, sizeof tmp);
	    strcpy(tmp,gethostbyname(tmp)->h_name);
	    if (strchr(tmp,'.')!=NULL) *(char *)strchr(tmp,'.')=0;
	    strcat(_prompt,tmp);
	    break;

	  case 'u':
	    strcat(_prompt,iftp[frame].user);
	    break;
	    
	  case 'd':
	    strcat(_prompt,iftp[frame].pwd);
	    break;

	  case 'D':
	    strcat(_prompt,(char *)getcwd(tmp,sizeof(tmp)));
	    break;

	  case 'f':
	    sprintf(tmp,"%d",frame);
	    strcat(_prompt,tmp);
	    break;

	  case 'p':
	    sprintf(tmp,"%d",(LINK==NULL)?0:LINK->port);
	    strcat(_prompt,tmp);
	    break;
	    
	  case 't':
	    
	    sprintf(tmp,"%d",(LINK==NULL)?0:LINK->timeout.tv_sec);
	    strcat(_prompt,tmp);
	    break;
	    

	  case 'T':
	    
	    {
	      time_t t=time((time_t *)0);
	      struct tm *lt=localtime(&t);
	      sprintf(tmp,"%02d:%02d:%02d",lt->tm_hour,
		      lt->tm_min,lt->tm_sec);
	      strcat(_prompt,tmp);
	    }
	    break;

	  case 'P':

	    sprintf(tmp,"%d",getpid());
	    strcat(_prompt,tmp);
	    break;
	    
	  default:
	    sprintf(tmp,"%%%c",*s);
	    strcat(_prompt,tmp);
	    break;
	  }
	break;

      case '^':
	
	++s;
	if (isalpha(*s))
	  {
	    sprintf(tmp,"%c",toupper(*s)-'A'+1);
	    strcat(_prompt,tmp);
	  }
	break;
	
      default:
	
	sprintf(tmp,"%c",*s);
	strcat(_prompt,tmp);
	break;
      }
  return _prompt;
}
	
	    
void noop()
{
  int i;
  time_t curtime,save;
  STATUS (*func1)(),(*func2)(),(*func3)();


  if (noopinterval==0) return;
  
  curtime = time((time_t *)0);
  
  signal(SIGALRM,noop);
  
  if (prevtime==0) 
    {
      prevtime=curtime;
      alarm(noopinterval);
      return;
    }
  
  if (curtime-prevtime < noopinterval) 
    {
      alarm(prevtime+noopinterval-curtime);
      return;
    }
 
  fprintf(stderr,"Waiting...");fflush(stderr);
 
  for (i=0;i<NFRAMES;i++)
    {
      if ( ftp[i]==NULL || FTPCMD(ftp[i]) == NULL || iftp[i].lock )
	continue;

      func1=ftp[i]->debug; ftp[i]->debug=NULL;
      func2=ftp[i]->error; ftp[i]->error=NULL;
      func3=ftp[i]->IO; ftp[i]->IO=NULL;
      save = ftp[i]->timeout.tv_sec;
      ftp[i]->timeout.tv_sec = nooptimeout;
      
      FtpCommand(ftp[i],"NOOP","",0,EOF);

      ftp[i]->timeout.tv_sec = save;
      ftp[i]->debug=func1;
      ftp[i]->error=func2;
      ftp[i]->IO=func3;

    }
  
  alarm(noopinterval);
  prevtime=curtime;
  
  for (i=0;i<10;i++) putc(8,stderr),putc(' ',stderr),putc(8,stderr);
  fflush(stderr);
}


setsignals()
{
  signal(SIGINT,intr);
  signal(SIGQUIT,intr);

#ifdef SIGURG
  signal(SIGURG,SIG_IGN);
#endif

  signal(SIGIO,SIG_IGN);
  signal(SIGCHLD,SIG_IGN);
  signal(SIGPIPE,SIG_IGN);

#if defined(TIOCGWINSZ) && defined(SIGWINCH)
  signal(SIGWINCH,getwinsz);
#endif

  noop();
}

unsetsignals()
{
  signal(SIGALRM,SIG_IGN);
  alarm(0);
}


int myhash(FTP *ftp,unsigned int chars)
{
  
  if (chars==0) return ftp -> counter=0;
  
  ftp -> counter += chars;
  
  if (hashmode)
    {
      fprintf(stderr,"%10u bytes transfered\r",ftp -> counter);
      fflush(stderr);
    }

  if (!lastcmd) 
    {
      noop();
      alarm(0);
    }
}



char *makefilename(char *f1, char *f2)
{
  char *p;
  
  if (*f2!=0)
    return f2;

  if ( (p=strrchr(f1,'/'))!=NULL)
    return p+1;
  return f1;
}

redir(char *cmdline)
{
  char *p=cmdline;
  FtpString result;
  char *r=result;
  
  for ( ; *p ; p++ , r++ )
    {
      if ( *p == '\\' ) 
	{
	  *r = * ++ p ;
	  continue;
	}
      
      if ( *p == '>' || *p == '<' )
	{
	  FtpString filename;
	  char *q=filename;
	  char c = *p;
	  
	  for (p++;isspace(*p)&&*p!=0;p++);
	  if (*p=='"')
	    {
	      for (p++; *p!='"' && *p!=0 ; p++,q++) *q = *p;
	      if (*p!='"') p++;
	    }
	  else
	    for (; !isspace(*p) && *p!=0 ; p++,q++) *q = *p;
	    
	  *q=0;
	  
	  if ( c == '>' ) 
	    output(filename);
	  else
	    input(filename);
	}
      *r = *p;
    }
  *r=0;
  strcpy(cmdline,result);
}

int itty = -1,otty = -1;
FILE *is=NULL, *os=NULL;


input(char *filename)
{
  
  if ((is=Ftpfopen(filename,"r"))==NULL)
    {
      perror(filename);
      return;
    }

  fflush(stdin);
  itty=dup(0);
  close(0);
  dup2(fileno(is),0);

}

output(char *filename)
{
  
  if ((os=Ftpfopen(filename,"w"))==NULL)
    {
      perror(filename);
      return;
    }
  
  fflush(stdout);
  otty=dup(1);
  close(1);
  dup2(fileno(os),1);
}

redirback()
{

  if (itty!= -1)
    {
      fflush(stdin);
      close(0);
      Ftpfclose(is);
      dup2(itty,0);
      is=NULL;
      itty = -1;
    }

  if (otty!= -1)
    {
      fflush(stdout);
      close(1);
      Ftpfclose(os);
      dup2(otty,1);
      os=NULL;
      otty = -1;
    }
}


batch(char *filename)
{
  FILE *fp;
  FtpString tmp;
  
  if ((fp=fopen(filename,"r"))!=NULL)
    {
      
      while ( fgets(tmp, sizeof tmp, fp) != NULL)
	{
	  tmp[strlen(tmp)-1]=0;
	  execute(tmp);
	  if (tmp[0]) gl_histadd(tmp);
	}
      fclose(fp);
    }
}
  

quit(int code)
{
  if (ifalias("autoquit"))
    execute("autoquit");
  exit(code);
}

#if defined(TIOCGWINSZ) && defined(SIGWINCH)

void getwinsz()
{
  struct winsize win;

  ioctl(0,TIOCGWINSZ,&win);
  
  winsize = win.ws_col;
  if (winsize==0) winsize=80;
  gl_setwidth(winsize);
  gl_redraw_r();
}
#endif


load_compctl()
{
  /* Load command(s) */

  static char *sets[] = {
    "frame","timeout","sleep","debug","bin","try","hash",
    "port","prompt","rest","noopinterval","nooptimeout","user",
    "wwwgateway","wwwport",
    NULL};
  char **p;

  CMDS *xcmd = &cmds[0];

  ALIAS *a=firstalias;
  
  while (xcmd->cmd != NULL) load_key(xcmd->cmd),xcmd++;

  /* Load alias(es) */

  while (a!=NULL) load_key(a->name),a=a->next;

  for (p=sets;*p!=NULL;p++)
    {
      FtpString tmp;

      strcpy(tmp,"set ");
      strcat(tmp,*p);
      load_key(tmp);
    }

  load_hosts();
  
}

load_hosts()
{
  FILE *in=fopen(gethostsname(),"r");
  FtpString s,ss;
  
  if (in==NULL) return;

  while ( fgets(s,sizeof s,in)!=NULL)
    {
      s[strlen(s)-1]=0;
      strcpy(ss,"open ");
      strcat(ss,s);
      load_key(ss);
    }

  fclose(in);
}

log_traffic ( char *hf, char *ht, char *ff, char *ft, int counter)
{
  time_t t=time((time_t *)0);
  struct tm *lt=localtime(&t);
  FtpString hostname,txt;
  int fd=open(LOG_TRAFFIC_FILE,O_WRONLY|O_CREAT|O_APPEND);

  if (fd==-1) return;

  while ((status=flock(fd,LOCK_EX)==-1) && errno==EWOULDBLOCK);

  if (status==-1) return;

  gethostname(hostname, sizeof hostname);  
  
  
  sprintf(txt,"%02d:%02d:%02d %02d:%02d:%02d %s@%s %s:%s -> %s:%s %d bytes\n",
	  lt->tm_mday,lt->tm_mon+1,lt->tm_year+1900,
	  lt->tm_hour,lt->tm_min,lt->tm_sec,
	  getpwuid(getuid())->pw_name,hostname, hf, ff, ht, ft, counter) ;

  write(fd,txt,strlen(txt));

  close(fd);
}






