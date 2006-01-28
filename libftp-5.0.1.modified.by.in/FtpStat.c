/*
		      Library for ftpd clients.(libftp)
			Copyright by Oleg Orel
			 All rights reserved.
			
This  library is desined  for  free,  non-commercial  software  creation. 
It is changeable and can be improved. The author would greatly appreciate 
any  advises, new  components  and  patches  of  the  existing  programs.
Commercial  usage is  also  possible  with  participation of it's author.



*/

static char * months[] = { "Jan", "Feb" , "Mar", "Apr", "May", "Jun", "Jul",
			     "Aug", "Sep",  "Oct", "Nov", "Dec"};

#include "FtpLibrary.h"
#include <sys/stat.h>

#if ! defined( _WIN32 )
#include <pwd.h>
#else

#endif

static unix_parse(char *,FTP_STAT*); 
static vms_parse(char *,FTP_STAT*, FILE*); 
static vms_axp_parse(char *,FTP_STAT*); 
static msdos_parse(char *,FTP_STAT*);

STATUS FtpStat(FTP *ftp, char *patern, FTP_STAT **first)
{
	char *tmp2;
  FtpString tmp,syst;
  register FILE *in;
  register FTP_STAT *stat=NULL, *prev=NULL;
  register int sys_unix,sys_vms,sys_msdos;
  
  *first = NULL;
  strcpy(syst,FtpSyst(ftp));

  sys_unix = !strcmp(syst,"UNIX") || !strcmp(syst,"UNKNOWN");
  sys_vms  = !strcmp(syst,"VMS");
  sys_msdos  = !strcmp(syst,"MSDOS");

#if ! defined( _WIN32 )
  {
  	int fd;
  
	sprintf(tmp,"/tmp/%s.XXXXXX",getpwuid(getuid())->pw_name);
  	if ( ( fd = mkstemp( tmp ) ) == -1 ) return EXIT( ftp, LQUIT );
  	
  	close( fd );
  }
#else
	sprintf( tmp, "extract-xiso.tmp.XXXXXX" );
	if ( _mktemp( tmp ) == NULL ) return EXIT( ftp, LQUIT );
#endif

	if ( ( tmp2 = strdup( tmp ) ) == NULL ) {
		errno = ENOMEM;
#if ! defined( _WIN32 )
		unlink( tmp );
#else
		_unlink( tmp );
#endif

		return EXIT( ftp, LQUIT );
	}

  if (*patern==0)
    FtpRetr(ftp,"LIST","",tmp);
  else
    FtpRetr(ftp,sys_unix?"LIST -d %s":"LIST %s",patern,tmp);
  
  if ( (in=fopen(tmp,"r")) == NULL) {
#if ! defined( _WIN32 )
	unlink( tmp );
#else
	_unlink( tmp );
#endif
	return EXIT(ftp,LQUIT);
  }

  
  while(1)
    {

      if (fgets(tmp,sizeof tmp,in)==NULL)
	break;
      
      if ( stat == NULL )
	{
	  *first = stat = (FTP_STAT *) malloc(sizeof(FTP_STAT));
	  bzero(stat,sizeof(FTP_STAT));
	}
      
      if (  ( sys_unix && unix_parse(tmp,stat))
	  ||( sys_vms  && (vms_parse(tmp,stat,in)||vms_axp_parse(tmp,stat)))
	  ||( sys_msdos && msdos_parse(tmp,stat)))
	  {
	  stat -> next = (FTP_STAT *) malloc(sizeof(FTP_STAT));
	  prev = stat;
	  stat = stat -> next;
	}
    }

  if (prev!=NULL)
    free(prev->next),
    prev->next=NULL;
  
  fclose(in);
#if ! defined( _WIN32 )
	unlink( tmp2 );
#else
	_unlink( tmp2 );
#endif

	free( tmp2 );

  return 0;
}

static unix_parse(char *str, FTP_STAT *stat )
{
  char				   *p;
  register int			i;
  FtpString				tmp;
  register int			column=1;
  
  if (*word(str,8)=='\0') return 0; /* Isn't file description */
  
  strcpy(tmp,word(str,column++));

  if (strlen(tmp)!=10) return 0;

  stat->type = tmp[0];

  stat->mode =
	(( tmp[1] == 'r' ) ? S_IRUSR : 0)  |
	(( tmp[2] == 'w' ) ? S_IWUSR : 0)  |
	(( tmp[3] == 'x' ) ? S_IXUSR : 0)  |
	(( tmp[4] == 'r' ) ? S_IRGRP : 0)  |
	(( tmp[5] == 'w' ) ? S_IWGRP : 0)  |
	(( tmp[6] == 'x' ) ? S_IXGRP : 0)  |
	(( tmp[7] == 'r' ) ? S_IROTH : 0)  |
	(( tmp[8] == 'w' ) ? S_IWOTH : 0)  |
	(( tmp[9] == 'x' ) ? S_IXOTH : 0);

  
  stat -> inodes = atoi ( word(str,column++));
  
  strcpy ( stat -> user , word ( str, column ++ ));
  
  if (*word(str,9)=='\0')
    stat -> group[0] = 0;
  else
    strcpy ( stat -> group , word ( str, column ++ ));
  
  stat -> size = atoi ( word(str,column++));


  strcpy(tmp,word(str,column++));
  
  stat->month = -1;
  for (i=0;i<12;i++)
    if (!strncasecmp(months[i],tmp,3))
      {
	stat->month=i;
	break;
      }

  
  stat -> day = atoi ( word(str,column++));

#if 0
  strcpy(stat->time,word(str,column++));
  strcpy(stat->name,word(str,column++));

  if ( stat->type == 'l' &&  !strcmp(word(str,column++),"->") )
    strcpy(stat->link,word(str,column++));
#else	// in 05.14.03
	strcpy( stat->time, word( str, column ) );

	for ( p = str, i = 0; *p && i < column; ++i ) {
		while ( *p && isspace( *p ) ) ++p;
		while ( *p && ! isspace( *p ) ) ++p;
	}
	
	if ( *p ) {
		++p;
	
		if ( stat->type != 'l' ) {
			for ( i = 0; p[ i ] != '\r' || p[ i + 1 ] != '\n'; ++i ) ;
			strncpy( stat->name, p, i );
			stat->name[ i ] = 0;
		} else {
			for ( i = 0; p[ i ] != ' ' || p[ i + 1 ] != '-' || p[ i + 2 ] != '>' || p[ i + 3 ] != ' '; ++i ) ;
			strncpy( stat->name, p, i );
			stat->name[ i ] = 0;
			
			for ( p = &p[ i + 4 ], i = 0; p[ i ] != '\r' || p[ i + 1 ] != '\n'; ++i ) ;
			strncpy( stat->link, p, i );
			stat->link[ i ] = 0;
		}
	}
#endif

  return 1;  
}

static vms_parse(char *str, FTP_STAT *stat, FILE *in)
{
  FtpString tmp;
  register char *p;
  register int i;

  strcpy(tmp,word(str,1));
  
  if (*word(str,2)==0 && ((char*)strchr(tmp,'.')!=NULL && 
     	((char*)strchr(tmp,';'))!=NULL)
      && fgets(tmp,sizeof tmp,in)!=NULL)
    *(char *)strchr(str,10)=0,
    strcat(str,tmp);
  
  
  if (*word(str,5) != '[' && *word(str,6)!='(' ) return 0;

  strcpy(stat -> name , word(str,1));
  stat -> size = atoi(word(str,2));

  strcpy(tmp,word(str,5));


  if (strchr(tmp+1,',')==NULL)
    {
      strcpy(stat->user,tmp+1);
      *(char *)strchr(stat->user,']')='\0';
      stat->group[0]=0;
    }
  else
    {
      strcpy(stat->group,tmp+1);
      *(char *)strchr(stat->group,',')='\0';
      strcpy(stat->user,(char *)strchr(tmp,',')+1);
      *(char *)strchr(stat->user,']')='\0';
    }
  
  for (p=stat->name;*p;p++) *p=tolower(*p);
  
  if ( (char*)strstr(stat->name,".dir;") != NULL)
    {
      *(char *)strchr(stat->name,'.')=0;
      stat->type='d';
    }
  else
    {
      *(char *)strchr(stat->name,';')=0;
      stat->type = '-';
    }

  stat -> day = atoi(word(str,3));

  strcpy(tmp,(char *)strchr(word(str,3),'-')+1);
  tmp[3]=0;

  stat->month = -1;

  for (i=0;i<12;i++)
    if (!strncasecmp(months[i],tmp,3))
      {
	stat->month=i;
	break;
      }

  strcpy(stat->time,word(str,4));

  strcpy(tmp,word(str,6));


  for(p=tmp;*p!=',';p++);
  p++;

  stat->mode=0;
  
  if (*p=='R') { stat -> mode |= S_IRUSR; p++;}
  if (*p=='W') { stat -> mode |= S_IWUSR; p++;}
  if (*p=='E') { stat -> mode |= S_IXUSR; p++;}
  if (*p=='D') { p++;}
  p++;
  
  if (*p=='R') { stat -> mode |= S_IRGRP; p++;}
  if (*p=='W') { stat -> mode |= S_IWGRP; p++;}
  if (*p=='E') { stat -> mode |= S_IXGRP; p++;}
  if (*p=='D') { p++;}
  p++;
  
  if (*p=='R') { stat -> mode |= S_IROTH; p++;}
  if (*p=='W') { stat -> mode |= S_IWOTH; p++;}
  if (*p=='E') { stat -> mode |= S_IXOTH; p++;}
  if (*p=='D') { p++;}

  return 1;
}


static vms_axp_parse(char *str, FTP_STAT *stat)
{
  FtpString tmp;
  register char *p;
  register int i;

  if (*word(str,4) != '[' && *word(str,5)!='(' ) return 0;

  strcpy(stat -> name , word(str,1));
  stat -> size = atoi(word(str,2))*512;

  strcpy(tmp,word(str,4));

  strcpy(stat->group,tmp+1);
  *(char *)strchr(stat->group,',')='\0';
  strcpy(stat->user,(char *)strchr(tmp,',')+1);
  *(char *)strchr(stat->user,']')='\0';
  
  for (p=stat->name;*p;p++) *p=tolower(*p);
  
  if ( (char*)strstr(stat->name,".dir;") != NULL)
    {
      *(char *)strchr(stat->name,'.')=0;
      stat->type='d';
    }
  else
    {
      *(char *)strchr(stat->name,';')='.';
      stat->type = '-';
    }

  stat -> day = atoi(word(str,3));

  strcpy(tmp,(char *)strchr(word(str,3),'-')+1);
  tmp[3]=0;

  stat->month = -1;

  for (i=0;i<12;i++)
    if (!strncasecmp(months[i],tmp,3))
      {
	stat->month=i;
	break;
      }

  strcpy(tmp,word(str,5));
  for(p=tmp;*p!=',';p++);
  p++;

  stat->mode=0;
  
  if (*p=='R') { stat -> mode |= S_IRUSR; p++;}
  if (*p=='W') { stat -> mode |= S_IWUSR; p++;}
  if (*p=='E') { stat -> mode |= S_IXUSR; p++;}
  if (*p=='D') { p++;}
  p++;
  
  if (*p=='R') { stat -> mode |= S_IRGRP; p++;}
  if (*p=='W') { stat -> mode |= S_IWGRP; p++;}
  if (*p=='E') { stat -> mode |= S_IXGRP; p++;}
  if (*p=='D') { p++;}
  p++;
  
  if (*p=='R') { stat -> mode |= S_IROTH; p++;}
  if (*p=='W') { stat -> mode |= S_IWOTH; p++;}
  if (*p=='E') { stat -> mode |= S_IXOTH; p++;}
  if (*p=='D') { p++;}

  return 1;
}

static msdos_parse(char *str, FTP_STAT *stat)
{
  FtpString tmp;
  register char *p;
  
  if (*word(str,5) != 0) return 0;

  strcpy(stat -> name , word(str,1));

  for (p=stat->name;*p;*p++) *p=tolower(*p);

  stat ->mode = 0644;
  
  if (!strcmp(stat->name,".")||!strcmp(stat->name,"..")) return 0;
  
  strcpy(tmp,word(str,2));

  if (!strcasecmp(tmp,"<dir>"))
    {
      stat ->type = 'd';
      stat ->mode |= S_IXUSR;
    }
  else
    {
      stat -> size = atoi(tmp);
      stat -> type = '-';
    }

  strcpy(tmp,word(str,3));
  stat -> month = atoi(tmp);
  stat -> day = atoi((char*)strchr(tmp,'-')+1);

  strcpy(stat->time,word(str,4));
  return 1;
}


      
      
STATUS FtpStatFree(FTP_STAT *stat)
{
  FTP_STAT *next;
  
  while ( stat != NULL )
    {
      next = stat -> next;
      free(stat);
      stat = next;
    }

	return 0;
}

      
      
	
      
  
  
	 
