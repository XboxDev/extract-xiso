#include "uftp.h"

jmp_buf connectstack;



Ftp_connect_hook(FTP *ftp,int code, char *msg)
{
  FtpLog(ftp->title,msg);
  longjmp(connectstack,1);
  
}
  

Ftp_connect(ARGS) 
{
  STATUS (*func1)(),(*func2)();

  if (*w2==0)
    {
      w2=getline("Host:");
      if (strchr(w2,'\n')!=NULL)
	*(char *)strchr(w2,'\n')=0;
      else
	longjmp(start,1);
    }
  
  if (FtpGetHost(w2)==NULL && FtpInit.IO != NULL) 
    {
      char *msg;
      extern int h_errno;
      
      switch(h_errno)
	{
	case HOST_NOT_FOUND:	  msg = "Host unknown";	  break;
	case TRY_AGAIN:	          msg = "Hostname lookup failure";break;
	default:	          msg = "gethostbyname failure";	  
	}
      return (*FtpInit.IO)(LINK,QUIT,msg);
    }
  
  newframe(0);
  
  func1 = FtpInit.error;
  func2 = FtpInit.IO;
  
  if (trymode) 
    {
      FtpSetErrorHandler(&FtpInit,Ftp_connect_hook);
      FtpSetIOHandler(&FtpInit,Ftp_connect_hook);
      setjmp(connectstack);
    }

  list_add(&hosts,w2);
  
  FtpConnect(&LINK,w2);
  strcpy(iftp[frame].host,FtpGetHost(w2)->h_name);
  
  FtpSetErrorHandler(LINK,func1);
  FtpSetErrorHandler(&FtpInit,func1);
  FtpSetIOHandler(LINK,func2);
  FtpSetIOHandler(&FtpInit,func2);
  
  return;
}

Ftp_preconnect(ARGS)
{
  FtpString host;
  FtpString pass;
  
  gethostname(host, sizeof host);
  strcpy(host,FtpGetHost(host)->h_name);
  sprintf(pass,"%s@%s",getpwuid(getuid())->pw_name,host);
  
  newframe(0);


  if (*w2==0)
   {
     w2=getline("Host:");
     if (strchr(w2,'\n')!=NULL)
       *(char *)strchr(w2,'\n')=0;
     else
       longjmp(start,1);
   }
  strcpy(iftp[frame].host,w2);
  
  if (*w3==0)
    w3=defaultuser;
  strcpy(iftp[frame].user,w3);

  if (*w4==0)
    w4=pass;
  strcpy(iftp[frame].pass,w4);

  if (*w5==0)
    w5=".";

  strcpy(iftp[frame].pwd,w5);
  
  iftp[frame].lock=0;

  ftp[frame]=FtpCreateObject();

}


Ftp_user(ARGS)
{
  FtpString tmp;
  
  if (*w2==0)
    {
      sprintf(tmp,"login (default %s):",defaultuser);
      w2=getline(tmp);
      if (strchr(w2,'\n')!=NULL)
	*(char *)strchr(w2,'\n')=0;
      else
	longjmp(start,1);
      if (*w2==0) 
	w2=defaultuser;
    }
  strcpy(iftp[frame].user,w2);
  strcpy(iftp[frame].pass,w3);
  if (FtpUser(LINK,w2)==331) Ftp_pass("",w3,"","","","");
}

Ftp_pass(ARGS)
{
  FtpString tmp;

  if (*w2==0) 
    {
      FtpString pass;
      FtpString host;
  
      gethostname(host, sizeof host);
      strcpy(host,FtpGetHost(host)->h_name);
      sprintf(pass,"%s@%s",getpwuid(getuid())->pw_name,host);
      sprintf(tmp,"Password (default %s):",pass);
      w2=(char*)getpass(tmp);
      if (*w2==0) w2=pass;
    }
  
  strcpy(iftp[frame].pass,w2);
  FtpPassword(LINK,w2);
  strcpy(iftp[frame].pwd,FtpPwd(LINK));
}


Ftp_HTTP_open(ARGS)
{
  int c;
  FtpString protocol,host,path,file;
  FTP_STAT *stat=NULL;
  char *p;
  
  /* ftp://hostname.domain/path/file */

  if ( (c=sscanf ( w2, "%[^:]://%[^/]/%s", protocol, host, path )) <2 )
    return 0;

  if (strcasecmp(protocol,"ftp")!=0) return 0;

  if (c==2) strcpy(path,"/");

  if ((p=strrchr(path,'/'))!=NULL)
    {
      strcpy(file,p+1);
      *(p-1)=0;
    }
  else
    {
      strcpy(file,path);
      strcpy(path,"/");
    }

  if (!*path) strcpy(path,"/");
  
  Ftp_ftp("ftp",host,path,"","","");

  FtpStat(LINK,file,&stat);

  if (stat==NULL) return 1;
  
  if (stat->type=='d') Ftp_cd(LINK,file,"","","","");
  if (stat->type=='-')
    {
      Ftp_get("get",file,"","","","");
      if (!interactive) Ftp_quit("","","","","","");
    }
  return 1;
}

  

Ftp_open(ARGS)
{

  STATUS (*err)();

  /* There two possibilities to describe future connection:
     1st: host user passwd dir
     1nd: HTML-spec

  */

  if (Ftp_HTTP_open(w1,w2,"","","",""))
    return;

  
  Ftp_connect("",w2,"","","","");
  Ftp_user("",w3,w4,"","","" );
  if (ifalias("autologin")) execute("autologin");
  if (*w5) 
    Ftp_cd("cd",w5,"","","","");
}

Ftp_reopen(ARGS)
{
  FtpString host,user,pass,pwd;

  strcpy(host,iftp[frame].host);
  strcpy(user,iftp[frame].user);
  strcpy(pass,iftp[frame].pass);
  strcpy(pwd,iftp[frame].pwd);


  if ( host[0]!=0 )
    {
      Ftp_close(NULLARGS);
      Ftp_open("",host,user,pass,pwd,"");
    }
  else
    {
      log("Need at least hostname");
    }
}

Ftp_ftp(ARGS)
{
  FtpString pass;
  FtpString host;
  
  /* There two possibilities to describe future connection:
     1st: host dir
     1nd: HTML-spec

  */

  if (Ftp_HTTP_open(w1,w2,"","","",""))
    return;


  gethostname(host, sizeof host);
  strcpy(host,FtpGetHost(host)->h_name);
  sprintf(pass,"%s@%s",getpwuid(getuid())->pw_name,host);
  
  Ftp_open("",w2,"anonymous",pass,w3,"");
  return;
}

Ftp_quit(ARGS)
{
  quit(0);
}

Ftp_mput_handler()
{
  log("File(s) or directory not found");
  longjmp(start,1);
}


Ftp_lcd(ARGS)
{
  glob_t gl;
  FtpString tmp;
  
  bzero(&gl,sizeof gl);
 
  glob(w2,GLOB_BRACE|GLOB_TILDE|GLOB_QUOTE,
       Ftp_mput_handler, &gl);
  
  
  if (gl.gl_matchc<1 || chdir(*gl.gl_pathv))
    perror(w2);
  
  fprintf(stderr,"Local directory is \"%s\"\n",getwd(tmp));
}

Ftp_close(ARGS)
{
  register int i;
  
  if (isdigit(*w2)) 
    {
      i=atoi(w2);
    }
  else 
    {
      i=frame;
    }
  
  FtpQuickBye(ftp[i]);
  
  iftp[i].host[0]=0;
  iftp[i].pwd[0]=0;
  ftp[i]=NULL;
  newframe(1);
}


INLINE SetLogicalVar(char arg, int * var, char *msg)
{
  switch(arg)
    {
      
    case 'y':
      
      *var=1;
      break;
      
    case 'n':
      
      *var=0;
      break;
      
    default:
      
      (*var)?(*var=0):(*var=1);
      break;
    }
}

Ftp_set(ARGS)
{
  if (!strcmp("frame",w2))
    return atoi(w3)<NFRAMES?frame=atoi(w3):0;
  
  if (!strcmp("timeout",w2))
    {
      FtpSetTimeout(&FtpInit,atoi(w3));
      if (LINK!=0)
	FtpSetTimeout(LINK,atoi(w3));
      return;
    }
  
  if (!strcmp("sleep",w2))
    {
      sleeptime=atoi(w3);
      return;
    }
  
  if (!strcmp("debug",w2))
    {
      if ( *w3=='y' || *w3==0) 
	{ 
	  if (LINK!=NULL) FtpSetDebugHandler(LINK,FtpDebugDebug);
	  FtpSetDebugHandler(&FtpInit,FtpDebugDebug);
	  return;
	}
      if ( *w3 == 'n')
	{
	  if (LINK!=NULL) FtpSetDebugHandler(LINK,NULL);
	  FtpSetDebugHandler(&FtpInit,NULL);
	  return;
	}
    }
  
  if (!strcmp("bin",w2))
    {
      if ( *w3=='y' || *w3==0) 
	{ 
	  FtpInit.mode='I';
	  return;
	}
      if ( *w3 == 'n')
	{
	  FtpInit.mode='A';
	  return;
	}
    }
  
  if (!strcmp("try",w2))
    return SetLogicalVar(*w3,&trymode,"Try mode");
  if (!strcmp("hash",w2))
    return SetLogicalVar(*w3,&hashmode,"Hash mode");
  if (!strcmp("rest",w2)||!strcmp(w2,"restore"))
    return SetLogicalVar(*w3,&restmode,"Restore mode");
  
  if (!strcmp("prompt",w2))
    {
      strcpy(prompt,w3);
      return;
    }
  
  if (!strcmp("port",w2))
    {
      if ( isdigit(*w3)) 
	return FtpSetPort(&FtpInit,atoi(w3));
      return;
    }

  if (!strcmp("wwwport",w2))
    {
      if ( isdigit(*w3)) 
	return www_port=atoi(w3);
      return;
    }

  if (!strcmp("wwwgateway",w2))
    {
      strcpy(www_gateway,w3);
      return;
    }

  if (!strcmp("noopinterval",w2)||!strcmp("noop",w2))
    {
      if ( isdigit(*w3)) 
	return noopinterval=(time_t)atoi(w3);
      fputs("Time must be number\n",stderr);
      return;
    }

  if (!strcmp("nooptimeout",w2))
    {
      if ( isdigit(*w3)) 
	return nooptimeout=(time_t)atoi(w3);
      fputs("Time must be number\n",stderr);
      return;
    }

      

  if (!strcmp("user",w2))
    {
      strcpy(defaultuser,w3);
      return;
    }
  
  if (!strcmp("",w2))
    {

      fprintf(stderr,"frime %d\n",frame);
      fprintf(stderr,"timeout %d secs\n",FtpInit.timeout.tv_sec);
      fprintf(stderr,"sleep time %d secs\n",sleeptime);
      fprintf(stderr,"debug %s\n",(FtpInit.debug!=NULL)?"enable":"disable");
      fprintf(stderr,"try mode %s\n",trymode?"enable":"disable");
      fprintf(stderr,"hash mode %s\n",hashmode?"enable":"disable");
      fprintf(stderr,"restore mode %s\n",restmode?"enable":"disable");
      fprintf(stderr,"automatic binary mode %s\n",(FtpInit.mode=='I')?"enable":"disable");
      fprintf(stderr,"prompt  \"%s\"\n",prompt);
      fprintf(stderr,"port %d\n",FtpInit.port);
      fprintf(stderr,"noop interval %d\n",noopinterval);
      fprintf(stderr,"noop timeout %d\n",nooptimeout);
      fprintf(stderr,"wwwgateway %s\n",www_gateway);
      fprintf(stderr,"wwwport %d\n",www_port);
      fprintf(stderr,"Default login name \"%s\"\n",defaultuser);
      fflush(stderr);
      return;
    }
  return puts("arg 2 unknown"); 
  
}

jmp_buf getstack;

Ftp_get_hook(FTP *con,int code, char *msg)
{

  if ( abs(code)==553 || (abs(code)==550 && FtpBadReply550(msg)) ) 
    {
      FtpLog(con->title,msg);
      log("Transfer cancel");
      longjmp(getstack,2);
    }
  
  if ( code == LQUIT )
    {
      log(msg);
      log("Transfer leave after I/O error with local file");
      longjmp(getstack,2);
    }


  
  FtpLog(con->title,msg);
  FtpQuickBye(LINK);
  LINK=NULL;

  log("sleeping......");
  sleep(sleeptime);
  log("try again...");
  
  longjmp(getstack,1);
  
}
  
void Ftp_get_intr(int sig)
{
  signal(SIGINT,intr);
  log("Transfer interupt");
  Ftp_abort();
  longjmp(start,3);
}

Ftp_get(ARGS)
{
  FTP OldInit;
  register int status=0;
  char *in="",*out="";
  FtpString myhostname;


  
  OldInit=FtpInit;
  
  if (restmode)
    FtpSetFlag(LINK,FTP_REST);
  else
    FtpClearFlag(LINK,FTP_REST);    
  
  if (trymode)
    {
      FtpSetErrorHandler(LINK,Ftp_get_hook);
      FtpSetIOHandler(LINK,Ftp_get_hook);
      FtpInit= (*LINK);
      FTPCMD(&FtpInit)=FTPCMD(&OldInit);
      FTPDATA(&FtpInit)=FTPDATA(&OldInit);
    }
  
  signal(SIGINT,Ftp_get_intr);
  FtpSetHashHandler(LINK,NULL);
  
  
  if ((status=setjmp(getstack))==2||status==3) 
    goto done;
  
  if ((LINK==NULL)||(LINK->sock==FtpInit.sock))
    {
      FtpLogin(&LINK,iftp[frame].host,iftp[frame].user,
	       iftp[frame].pass,NULL);	  
      FtpChdir(LINK,iftp[frame].pwd);
    }
  
  if ((hashmode && isatty(fileno(stderr)) &&
       strstr(w1,"dir")==NULL && strstr(w1,"ls")==NULL))
    FtpSetHashHandler(LINK,myhash);
  else
    FtpSetHashHandler(LINK,NULL);
    
  myhash(LINK,0);
  
  gethostname(myhostname, sizeof myhostname);
  
  if (strstr(w1,"get")!=NULL)
    {
      FtpGet(LINK,in=w2,out=makefilename(w2,w3));
      log_traffic(LINK->title,myhostname,in,out,LINK->counter);
    }

  if (strstr(w1,"put")!=NULL)
    {
      FtpPut(LINK,in=w2,out=makefilename(w2,w3));
      log_traffic(myhostname,LINK->title,in,out,LINK->counter);
    }
  
  if (strstr(w1,"dir")!=NULL)
    FtpRetr(LINK,(*w2==0)?"LIST":"LIST %s",makestr(w2,w3,w4,w5,w6,NULL),"-");

  if (strstr(w1,"ls")!=NULL)
    FtpRetr(LINK,(*w2==0)?"NLST":"NLST %s",makestr(w2,w3,w4,w5,w6,NULL),"-");

  if (LINK->debug!=NULL) log("Transfer complete");
  
  
done:

  if (LINK!=NULL)
    {
      FtpSetHashHandler(LINK,NULL);
      FtpSetErrorHandler(LINK,my_error);
      FtpSetIOHandler(LINK,my_error);
      FtpClearFlag(LINK,FTP_REST);    
    }
  FtpInit=OldInit;
}


Ftp_mget(ARGS)
{
  FTP_STAT *ftpstat, *first;
  FtpString local;
  FtpString pwd;

  if (LINK==NULL || FTPCMD(LINK)==NULL )
    Ftp_reopen(NULLARGS);

  while (FtpError(FtpStatus(LINK,FtpStat(LINK,w2,&ftpstat))))
    {
      FtpStatFree(ftpstat);
      Ftp_reopen(NULLARGS);
    }
  
  first = ftpstat;

  strcpy(pwd,FtpPwd(LINK));
  strcpy(iftp[frame].pwd,pwd);
  getcwd(local,sizeof local);
  
  while (ftpstat!=NULL)
    {

      if (ftpstat->type=='-')
	{
	  FtpString tmp;
	  struct stat st;

	  if (stat(ftpstat->name,&st)==-1 || st.st_size < ftpstat->size)
	    {
	      
	      sprintf(tmp,"get %s/%s",pwd,ftpstat->name);
	      log(tmp);
	      Ftp_get("get",ftpstat->name,"","","","");
	      chmod(ftpstat->name,ftpstat->mode);
	    }
	}
      
      if (ftpstat->type=='l')
	symlink(ftpstat->link,ftpstat->name);
      
      ftpstat=ftpstat->next;
    }

  ftpstat=first;
  
  while(ftpstat!=NULL)
    {
      if ( ftpstat -> type == 'd' &&
	   strcmp(ftpstat->name,".")!=0 &&
	   strcmp(ftpstat->name,"..")!=0 &&
	  ((mkdir(ftpstat->name,ftpstat->mode),chdir(ftpstat->name))!=-1) && 
	  !FtpError(FtpStatus(LINK,FtpChdir(LINK,ftpstat->name))))
	{
	  Ftp_mget(NULLARGS);
	  FtpChdir(LINK,pwd);
	}

      chdir(local);
      ftpstat=ftpstat->next;
      
    }


  strcpy(iftp[frame].pwd,pwd);

  FtpStatFree(first);
  
}

Ftp_proxyget(ARGS)
{
  switch (FtpHttpGet(www_gateway,www_port,w2,""))
    {

    case LQUIT:
      FtpLog("proxyget","error util work with local file");
      break;
      
    case QUIT:
      FtpLog("proxyget","communication problem");
      break;
      
    case OK:
      FtpLog("proxyget","Transfer complete");
      break;
      
    case ENOENT:
      FtpLog("proxyget","No such file or directory (on remote host)");
      break;
      
    default:
      FtpLog("proxyget","strange answer from libftp");
    }
}



Ftp_rm(ARGS)
{
  FTP_STAT *ftpstat, *first;
  FtpString local;
  FtpString pwd,tmp;
  
  while (FtpError(FtpStatus(LINK,FtpStat(LINK,w2,&ftpstat))))
    {
      FtpStatFree(ftpstat);
      Ftp_reopen(NULLARGS);
    }
  
  first = ftpstat;

  strcpy(pwd,FtpPwd(LINK));
  strcpy(iftp[frame].pwd,pwd);
  getcwd(local,sizeof local);
  
  for (;ftpstat!=NULL;ftpstat=ftpstat->next)
    if (ftpstat->type!='d')
      FtpStatus(LINK,FtpRm(LINK,ftpstat->name)),
      sprintf(tmp,"rm %s/%s",pwd,ftpstat->name),
      log(tmp);

  for (ftpstat=first;ftpstat!=NULL;ftpstat=ftpstat->next)
    if ( ftpstat -> type == 'd' &&
	!FtpError(FtpStatus(LINK,FtpChdir(LINK,ftpstat->name))))
      {
	Ftp_rm(NULLARGS);
	chdir(local);
	FtpChdir(LINK,pwd);
	FtpStatus(LINK,FtpCommand(LINK,"XRMD %s",ftpstat->name,0,EOF));
      }

  strcpy(iftp[frame].pwd,pwd);
  FtpStatFree(first);
  
}

Ftp_copy(ARGS)
{
  char *p;
  int in,out;
  
  if ( !*w2 || !*w3 ) return puts("Must pass two args");
  
  if ((p=strchr(w2,'!'))!=NULL) 
    {
      *p=0;
      in=atoi(w2);
      w2=p+1;
    }
  else
    in=frame;

  if ((p=strchr(w3,'!'))!=NULL) 
    {
      *p=0;
      out=atoi(w3);
      w3=p+1;
    }
  else
    in=frame;
  
  if (in==out) return puts("Files must been from different frames");
  
  FtpCopy(ftp[in],ftp[out],w2,w3);
}

Ftp_ccopy(ARGS)
{
  char *p;
  int in,out;
  
  if ( !*w2 || !*w3 ) return puts("Must pass two args");
  
  if ((p=strchr(w2,'!'))!=NULL) 
    {
      *p=0;
      in=atoi(w2);
      w2=p+1;
    }
  else
    in=frame;

  if ((p=strchr(w3,'!'))!=NULL) 
    {
      *p=0;
      out=atoi(w3);
      w3=p+1;
    }
  else
    in=frame;
  
  if (in==out) return puts("Files must been from different frames");
  
  FtpPassiveTransfer(ftp[in],ftp[out],w2,w3);
}

Ftp_bin(ARGS)
{
  FtpBinary(LINK);
}

Ftp_ascii(ARGS)
{
  FtpAscii(LINK);
}

Ftp_cd(ARGS)
{
  FtpChdir(LINK,w2);
  strcpy(iftp[frame].pwd,FtpPwd(LINK));
  if (ifalias("autocd")) execute("autocd");
}


Ftp_dup(ARGS)
{ 
  LINKINFO oldinfo;
  FTP oldftp;
  
  oldinfo=iftp[frame];
  oldftp=(*LINK);
  
  newframe(0);
  puts("Make alternative connection...");
  Ftp_open("",oldinfo.host,oldinfo.user,oldinfo.pass,"","");
  if (strcmp(oldinfo.pwd,iftp[frame].pwd)) 
    Ftp_cd("",oldinfo.pwd,"","","","");
  if (LINK->mode!=oldftp.mode)
    FtpType(LINK,oldftp.mode);
  LINK -> timeout = oldftp.timeout;
  LINK -> flags = oldftp.flags;
  FtpSetDebugHandler(LINK,oldftp.debug);
  FtpSetErrorHandler(LINK,oldftp.error);
  FtpSetIOHandler(LINK,oldftp.IO);
  FtpSetHashHandler(LINK,oldftp.hash);
}
  

  
Ftp_bg(ARGS)
{
  FtpString tmp;

  if (fork())
    {
      log(makestr("Backgrounding \"",w2,w3,w4,w5,w6,"\"",NULL));
      return;
    }
  else
    {
      int i=frame;
  
      lastcmd=1;
     
      /* Ignoring keypad */

      alarm (0);
      signal(SIGALRM,SIG_IGN);
      signal(SIGURG,SIG_IGN);
      signal(SIGPIPE,SIG_IGN);
      signal(SIGTSTP,SIG_IGN);
      signal(SIGINT,SIG_IGN);
      signal(SIGQUIT,SIG_IGN);
      signal(SIGCHLD,SIG_IGN);
      signal(SIGIO,SIG_IGN);

      /* Droping output*/

      sprintf(tmp,"/tmp/uftp-%s",getpwuid(getuid())->pw_name);
      mkdir(tmp,0700);
      sprintf(tmp,"/tmp/uftp-%s/logfile.XXXXXX",getpwuid(getuid())->pw_name);
      mktemp(tmp);
      close(0);close(1);close(2);
      open(tmp,O_RDWR|O_TRUNC|O_CREAT,0600);
      dup(0);dup(0);

      if (LINK!=NULL)
	{
	  Ftp_dup(NULLARGS);
	  free(ftp[i]);
	  ftp[i]=NULL;
	}
      
      executev(w2,w3,w4,w5,w6,"");
      exit(0);
    }
}
  

Ftp_list()
{
  register int i;
      
#define _FMT "%-5s %-15s %-10s %-25s %-7s %-4s\n" 
#define  FMT "%-5d %-15s %-10s %-25s %-7d %-4d\n" 
      
  fprintf(stderr,_FMT,"Frame","Host name","User's name","Working directory","Timeout","Port");
  
  for ( i = 0 ; i < NFRAMES ; i++ )
    if (ftp[i]!=NULL)
      fprintf(stderr,FMT,i,iftp[i].host,iftp[i].user,iftp[i].pwd,
	     ftp[i]->timeout.tv_sec,ftp[i]->port);
  fflush(stderr);
  return;
}

Ftp_abort(ARGS)
{
  time_t save;

  if (LINK!=NULL)
    {
      save = LINK ->timeout.tv_sec;
      LINK->timeout.tv_sec = nooptimeout;
      FtpAbort(LINK);
      LINK->timeout.tv_sec = save;
    }
}

Ftp_type(ARGS)
{
  FtpGet(LINK,w2,"*STDOUT*");
}


Ftp_page(ARGS)
{
  register char *pager;
  FtpString out={'|',0};
  
  if ((pager=(char *)getenv("PAGER"))==NULL) 
    pager="more";
 
  strcat(out,pager);
  FtpGet(LINK,w2,out);
}


Ftp_mkdir(ARGS)
{
  FtpMkdir(LINK,w2);
}

Ftp_move(ARGS)
{
  FtpMove(LINK,w2,w3);
}

Ftp_help(ARGS)
{
  LIST *list=NULL;
  register int i,ii,w2_len=strlen(w2);
  
  if ( !*w2 )
    {
      for ( i = 0 ; cmds[i].cmd!=NULL ; i++)
	list_add(&list,cmds[i].cmd);
    }
  
  else

    for ( i = 0 ; cmds[i].cmd!=NULL; i++)
      {
	int len=strlen(cmds[i].cmd);
	if (!strncmp(cmds[i].cmd,w2,MIN(len,w2_len)))
	  list_add(&list,cmds[i].help);
      }
  
  list_sort(&list);
  printlist(&list);
  list_init(&list);
}

Ftp_quote(ARGS)
{
  FtpString new;
  
  new[0]=0;

  strcpy(new,makestr(w2,w3,w4,w5,w6,NULL));
  FtpCommand(LINK,new,"",0,EOF);
}

Ftp_alias(ARGS)
{
  ALIAS *a=firstalias;
  
  if ( *w2==0 )
    {
      while (a!=NULL)
	{
	  fprintf(stderr,"%s=%s\n",a->name,a->str);
	  a=a->next;
	}
      return;
    }


  while (1) 
    {

      if ( a == NULL )
	{ 
	  firstalias = a = (ALIAS *) malloc(sizeof(ALIAS));
	  memset(a,0,sizeof(ALIAS));
	  a -> next = NULL;
	  break;
	}
      
      if (!strcmp(a->name,w2))
	break;
      
      if ( a->next == NULL)
	{
	  a -> next = (ALIAS *) malloc(sizeof(ALIAS));
	  a = a->next;
	  memset(a,0,sizeof(ALIAS));
	  a -> next = NULL;
	  break;
	}
      a=a->next;
    }

  strcpy(a -> name,w2);
  strcpy(a -> str,makestr(w3,w4,w5,w6,NULL));
}

Ftp_mkalias(ARGS)
{
  FtpString new;

  if (!*w2) return puts("Arg must present\n");

  sprintf(new,"open \"%s\" \"%s\" \"%s\" \"%s\"",
	  iftp[frame].host,iftp[frame].user,
	  iftp[frame].pass,iftp[frame].pwd);
  
  Ftp_alias("alias",w2,new,"","","");
}

Ftp_unalias(ARGS)
{
  ALIAS *cur,*prev;

  cur=prev=firstalias;

  while ( cur != NULL )
    {
      if (!strcmp(cur->name,w2))
	{
	  if ( cur == firstalias )
	    {
	      firstalias = cur->next;
	      free(cur);
	      return;
	    }
	  prev -> next = cur -> next;
	  free(cur);
	}
      prev=cur;
      cur=cur->next;
    }
}


Ftp_save_aliases(ARGS)
{
  ALIAS *a=firstalias;
  FILE *out;
  
  if ((out=fopen (getaliasrcname(),"w"))==NULL)
    {
      perror(getaliasrcname());
      return;
    }

  while (a!=NULL)
    {
      fprintf(out,"alias %s '%s'\n",a->name,a->str);
      a=a->next;
    }

  fclose(out);
  
  chmod ( getaliasrcname(), 0600);
}

Ftp_save_sets(ARGS)
{
  FILE *out;
  FtpString newprompt;
  char *p1=prompt, *p2=newprompt;
  
  if ((out=fopen (getsetsrcname(),"w"))==NULL)
    {
      perror(getaliasrcname());
      return;
    }

  fprintf(out,"set timeout %d\n",FtpInit.timeout.tv_sec);
  fprintf(out,"set sleep %d\n",sleeptime);
  fprintf(out,"set debug %c\n",(FtpInit.debug!=NULL)?'y':'n');
  fprintf(out,"set try %c\n",trymode?'y':'n');
  fprintf(out,"set hash %c\n",hashmode?'y':'n');
  fprintf(out,"set rest %c\n",restmode?'y':'n');
  fprintf(out,"set bin %c\n", (FtpInit.mode=='I')?'y':'n');

  while (*p1)
    {
      if (*p1=='>'||*p1=='<') *p2++='\\';
      *p2++ = *p1++;
    }

  *p2=0;
  
  fprintf(out,"set prompt '%s'\n",newprompt);
  
  fprintf(out,"set port %d\n",FtpInit.port);
  fprintf(out,"set noopinterval %d\n",noopinterval);
  fprintf(out,"set nooptimeout %d\n",nooptimeout);
  fprintf(out,"set user '%s'\n",defaultuser);
  
  fclose(out);
  
  chmod ( getsetsrcname(), 0600);
  
}


#define ARCHIE_MAX_TARGETS 20

Ftp_acd(ARGS)
{
  static int targets=0;
  static FtpString what={0};
  static ARCHIE result[ARCHIE_MAX_TARGETS];
  FtpString tmp;
  
  int i, selected_target;

  char *p;

  
  if ( (what[0] == 0 || strcmp(w2,what) != 0) && *w2!=0 )
    {
      if ((targets=FtpArchie(w2,result,ARCHIE_MAX_TARGETS))<1) 
	return puts("Archie failure or target not found");
      strcpy(what,w2);
    }
  
  for (i=0;i<targets;i++)
    fprintf(stderr,"%2d %s:%s\n",i,result[i].host,result[i].file);
  
  if (strcmp(w1,"archie")==0)
    return;
  
  
  p = getline("Your selection? ");

  if (strchr(p,'\n')!=NULL)
    *(char *)strchr(p,'\n')=0;
  else
    longjmp(start,1);
  
  if (*word(p,1)==0) return;
  
  selected_target = atoi(p);


  if (strcmp(w1,"acd")==0 || *(strrchr(result[selected_target].file,'/')+1)=='\0' )
    {
      strcpy(tmp,result[selected_target].file);
      *(char*)strrchr(tmp,'/')=0;
      
      Ftp_ftp("ftp",result[selected_target].host,tmp,"","","");
    }
  else
    {
      Ftp_ftp("ftp",result[selected_target].host,"","","","");
      Ftp_get("get",result[selected_target].file,"","","","");
    }
  
}

Ftp_aaget(ARGS)
{
  ARCHIE result[ARCHIE_MAX_TARGETS];
  int targets;
  int maxsize;
  register int i;
  FtpString pass,host,tmp;
  FTP save;
  
  if (*w2==0)
    {
      printf("%s <filename>\n",w1);
      return;
    }
  
  if ((targets=FtpArchie(w2,result,ARCHIE_MAX_TARGETS))<1) 
    {
      printf("Specified file not found by archie\n");
      return;
    }

  gethostname(host, sizeof host);
  sprintf(pass,"%s@%s",getpwuid(getuid())->pw_name,FtpGetHost(host)->h_name);
  
  for (i=0,maxsize=0;i<targets;i++)
    if (result[i].size>maxsize) maxsize=result[i].size;
  
  for (i=0; ; i++,(i==targets)?i=0:i)
    {
      FTP *ftp;
      
      if (result[i].size<maxsize) continue;

      save=FtpInit;

      FtpSetIOHandler(&FtpInit,NULL);
      FtpSetErrorHandler(&FtpInit,NULL);
      
      sprintf(tmp,"Retrive file %s from %s.....",result[i].file,result[i].host);
      FtpLog("aaget",tmp);

      
      if (FtpError(FtpLogin(&ftp,result[i].host,"anonymous",pass,NULL)) ||
	  FtpError(FtpGet(ftp,result[i].file,strrchr(result[i].file,'/')+1)))
	{
	  FtpQuickBye(ftp);
	  continue;
	}
      FtpQuickBye(ftp);
      break;
    }

  FtpInit=save;
  
}


INLINE char filetype(char *name)
{
  struct stat st;

  if (stat(name,&st)==-1) return '0';
  
  if ( S_IFREG == (st.st_mode & S_IFMT)) return '-';
  if ( S_IFDIR == (st.st_mode & S_IFMT)) return 'd';
  
  return '?';
}

Ftp_mput(ARGS)
{
  glob_t gl;
  char **args;
  int nargs;
  int chmod_status=0;
  FtpString local,pwd;
  FtpString tmp;
  
  glob((*w2==0)?"*":w2,GLOB_BRACE|GLOB_TILDE|GLOB_QUOTE,
       Ftp_mput_handler, &gl);

  nargs=gl.gl_matchc;
  args=gl.gl_pathv;
  
  while(nargs--)
    {
      if (filetype(*args)=='-')
	{
	  struct stat st;

	  getcwd(local,sizeof local);
	  
	  sprintf(tmp,"put %s/%s",local,*args);
	  log(tmp);
	  
	  Ftp_get("put",*args,"","","","");
	  stat(*args,&st);
	  if (chmod_status>=0)
	    chmod_status=FtpStatus(LINK,FtpChmod(LINK,*args,st.st_mode&0777));
	}
      args++;
    }

  nargs=gl.gl_matchc;
  args=gl.gl_pathv;
  
  while(nargs--)
    {
      if (filetype(*args)=='d')
	{
	  getcwd(local,sizeof local);

	  if (chdir(*args)==-1) continue;

	  strcpy(pwd,iftp[frame].pwd);

	  FtpStatus(LINK,FtpMkdir(LINK,*args));
	  
	  if (!FtpError(FtpStatus(LINK,FtpChdir(LINK,*args))))
	    {
	      Ftp_mput(NULLARGS);
	      FtpChdir(LINK,pwd);
	    }

	  chdir(local);
	}
      args++;
    }
}


Ftp_env(ARGS)
{

  if (!strcmp(w1,"setenv")) return setenv(w2,w3);
  if (!strcmp(w1,"unsetenv")) return unsetenv(w2);
  if (!strcmp(w1,"env")) return printenv();
}

Ftp_compctl(ARGS)
{
  if (*w2==0) return print_ctl("compctl",NULL);
  load_key(makestr(w2,w3,w4,w5,w6,NULL));
}


CMDS cmds[]={
  
  "compctl",		Ftp_compctl,      0,
  "compctl command arg1 arg2... - load complition map",

  "connect",		Ftp_connect,      0,
  "connect <hostname> - make new ftp connection",

  "preconnect",		Ftp_preconnect,      0,
  "preconnect <hostname> <user> <password> <path> - make fictivity  ftp connection",

  "open",		Ftp_open,         0,
  "open <hostname> <user> <pass> <directory> - login to server",
  
  "reopen",		Ftp_reopen,       0,
  "reopen - Open again connection with existing frame information",
  
  "ftp",		Ftp_ftp,          0,
  "ftp <hostname> - anonymously login to server",
  
  "close",		Ftp_close,        1,
  "close - Close connection",
  
  "quit",		Ftp_quit,         0,
  "quit - Exit from uftp",

  
  "exit",		Ftp_quit,         0,
  "exit - Exit from uftp",

  
  "set",		Ftp_set,          0,
"set - Set variables: (Without args print current settings)\n\
       frame <number>    - select another session(frame)\n\
       timeout <secs>    - Set network timeout\n\
       nooptimeout <secs>- Set network timeout with clearing timeout\n\
       noop <secs>       - Set time interval for sending NOOP operator\n\
                           to server for erased delay\n\
       sleep <secs>      - Set pause beetween transfer attempt\n\
       debug <y|n>       - Set debuging ftp's protocol (Default no)\n\
       try <y|n>         - Set retransfer mode with broken network (Default yes)\n\
       hash <y|n>        - Set hashing transfer (Default no)\n\
       restore <y|n>     - Set retransfer mode (reget/reput) (Default yes!!!!)\n\
       bin <y|n>         - Set automatic turn on binary mode (Default no) \n\
       prompt <string>   - Set prompt (See help prompt)\n\
       port <number>     - Set ftpd's port for next sessions\n\
       user <name>       - Set default user name (default you name)",

  "setenv",            Ftp_env,           0,
  "setenv - Set enviroment variable",
	 
  "unsetenv",          Ftp_env,           0,
  "unsetenv - Unset enviroment variable",
	 
  "env",               Ftp_env,           0,
  "env - Print enviroment variables",
	 
	 
  "prompt",               NULL,         0,
  "\
prompt is a string, which may be contains %<char> 
or ^<char> combitanion, which have next interprets:

%H, %h - full and short remote host names
%M, %m - full and short local host names
%u     - remote user's name
%d     - remote current directory
%D     - local current directory
%f     - number of current frame
%p     - the ftp's port number
%t     - timeout 
%T     - current time 
%P     - uftp process id
%%     - character %
^<char>- control character
%^     - character ^
",

  "list",		Ftp_list,         0,
"list - List session's information",

  "user",		Ftp_user,         1,
"user <user> - send user's name",

  "pass",		Ftp_pass,         1,
"pass <pass> - send user's password",

  "bin",		Ftp_bin,          1,
"bin - Set binary mode for current frame",

  "ascii",		Ftp_ascii,        1,
"ascii - Set ASCII mode for current frame",

  "cd",			Ftp_cd,           1,
"cd <directory> - change current remote directory ",

  "acd",		Ftp_acd,          0,
"acd <file_or_directory> - search pointed directory using archie, and setup connection to it",

  "lcd",		Ftp_lcd,          0,
"lcd <directory> - Change local directory",

  "abort",		Ftp_abort,        1,
"abort - abort last operation",

  "mkdir",		Ftp_mkdir,        1,
"mkdir <dirname> - create new directory",

  "rm",		        Ftp_rm,           1,
"rm <filename_spec> - remove file(s)",

  "mv",		        Ftp_move,         1,
"mv <old> <new> - rename file",

  "dir",		Ftp_get,          1,
"dir <argslist> ... - print list of files",

  "ls",		        Ftp_get,          1,
"ls <arglist> ... - print short list of files",

  "get",		Ftp_get,          1,
"get <remote_file> [<local_file>] - receive file from server",

  "pget",		Ftp_proxyget,          0,
"pget <http_spec> [<local_file>] - receive file from server thru HTTP proxy server",

  "mget",		Ftp_mget,         1,
"mget <remote_file(s)> - recursive receive file(s) from server",

  "aget",		Ftp_acd,          0,
"aget <file> - search pointed file using archie, and retrive it",

  "aaget",		Ftp_aaget,        0,
"aaget <file> - search pointed file using archie, and retrive it from anywhere",

  "put",		Ftp_get,          1,
"put <local_file> [<remote_file>] - send server to file",

  "mput",		Ftp_mput,         1,
"mput <local_file(s)>  - recursive send file(s) to server",

  "copy",               Ftp_copy,         1,
"copy [<frame>!]file [<frame>!]file - copy file via client cache",

  "ccopy",               Ftp_ccopy,       1,
"ccopy [<frame>!]file [<frame>!]file - copy file directly beetween servers",

  "cat",		Ftp_type,         1,
"cat <file> - print body of remote file to screen",
  
  "page",		Ftp_page,         1,
"page <file> - print body of remote file to screen via pager",

  "bg",			Ftp_bg,           0,
"bg <any_command> - run command(s) backgroundly (output redirect to file),\n\
You can also add &-char to back of line without \"bg\"",

  "archie",             Ftp_acd,       0,
"archie - Find file using archie service and display to screen",

  "dup",		Ftp_dup,          1,
"dup - Make new analogous frame",

  "quote",              Ftp_quote,        1,
"quote <some_string> - send command directly to server",

  "help",		Ftp_help,         0,
"help <command> - print list of commands or command description",

  "alias",              Ftp_alias,        0,
"\
alias aliasname <list> .... - make new alias, use $X for taking \n\
                  X's argument from command string, and $* for taking\n\
                  all arguments. If $<anything> on alias not present,\n\
                  the arguments appending to end of command string",

  "unalias",            Ftp_unalias,      0,
"unalias <aliasname> - remove alias",

  "mkalias",            Ftp_mkalias,      0,
"mkalias <Alias_name> - make alias for create this frame, use save for saving it to file",

  "savealiases",              Ftp_save_aliases,     0,
  "savealiases - Save aliases to file",

  "savesets",                 Ftp_save_sets,     0,
  "savesets - Save sets to file",

  "etc",               NULL,         0,
  "\
1. In any command you may use constructions <file and >file for\n\
   redirect input output.\n\
\n\
2. All local files files interprets as libftp file(s), \n\
   this support next specification:\n\
\n\
   |string - interprets string as shell command, which must be\n\
   execute and input or output take from/to it.\n\
   \n\
\n\
   *STDIN*, *STDOUT*, *STDERR* - opened streams.\n\
\n\
   anything - local file name.\n\
\n\
3. Command started with '!' passed to shell.\n
\n\
4. If string beetween two \" or \', its interprets as one word.\n\
\n\
5. Any string may be devide to few commands using ';'.",



  NULL

  };







