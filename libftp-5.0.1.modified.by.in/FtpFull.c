/*
		      Library for ftpd clients.(libftp)
			Copyright by Oleg Orel
			 All rights reserved.
			
This  library is desined  for  free,  non-commercial  software  creation. 
It is changeable and can be improved. The author would greatly appreciate 
any  advises, new  components  and  patches  of  the  existing  programs.
Commercial  usage is  also  possible  with  participation of it's author.



*/

#include "FtpLibrary.h"

#if ! defined( _WIN32 )
#include <pwd.h>
#endif

static FTP *ftp_table[256];
static STATUS syntax();

FILE * FtpFullOpen(char * file , char * mode )
{
  FTP *ftp;
  FILE *tmp;
  FtpString Host,User,Passwd,RemoteFile;

  if ( ! FtpFullSyntax (file,Host,User,Passwd,RemoteFile))
    {
      tmp=fopen(file,mode);
      if (tmp==NULL) return tmp;
      ftp_table[(int)fileno(tmp)] = NULL;
      return tmp;
    }
  if ( FtpLogin(&ftp,Host,User,Passwd,NULL) < 0 )
    return NULL;

  if (mode[1]=='b') FtpBinary(ftp);

  switch(mode[0])
    {
    case 'r': 
	if (FtpError(FtpOpenRead(ftp,RemoteFile))) 
	  return NULL;
	ftp_table[fileno(FTPDATA(ftp))] = ftp;
	return FTPDATA(ftp);
    case 'w':
	if (FtpError(FtpOpenWrite(ftp,RemoteFile)))
	  return NULL;
	ftp_table[fileno(FTPDATA(ftp))] = ftp;
	return FTPDATA(ftp);
    case 'a':
	if (FtpError(FtpOpenAppend(ftp,RemoteFile)))
	  return NULL;
	ftp_table[fileno(FTPDATA(ftp))] = ftp;
	return FTPDATA(ftp);
    }
  /* Error Mode */
  return NULL;
}

STATUS   FtpFullClose(FILE *f)
{
  FTP *ftp=ftp_table[(int)fileno(f)];
  if (ftp == NULL)
    return fclose(f);
  FtpClose(ftp);
  return FtpQuickBye(ftp);
}


/* Format of ftp's file [user[/passord]@]hostname:filename_with_path */

STATUS FtpFullSyntax ( FtpString source ,
		       FtpString host ,
		       FtpString user ,
		       FtpString passwd ,
		       FtpString file)

{
  char *in,*out;
  FtpString tmp;
  
  host[0] = user[0] = passwd[0] = file[0] = '\0';
 
  for ( in=source, out = user;
       *in !='\0' && *in != '/' && *in!='@' && *in!=':' ;
       *out++ = *in++);
  *out = '\0';
  
  if ( *in == '\0' ) return 0;
  
  if ( *in == ':' )
    {
      strcpy(host,user);
      strcpy(user,"anonymous");
      gethostname(tmp, sizeof tmp);
#if ! defined( _WIN32 )
      sprintf(passwd,"%s@%s",
	      getpwuid(getuid())->pw_name,gethostbyname(tmp)->h_name);
#else
		sprintf( passwd, "user@%s", gethostbyname(tmp)->h_name);
#endif
	  
	  goto file;
    } 

  if ( *in == '/' )
    {
      for ( in++, out = passwd;
	   *in !='\0' && *in!='@' ;
	   *out++ = *in++);
      *out = '\0';
      if ( *in == '\0' ) return 0;
    }
  else
    {
      gethostname(tmp, sizeof tmp);

#if ! defined( _WIN32 )	  
	  sprintf(passwd,"%s@%s",
	      getpwuid(getuid())->pw_name,gethostbyname(tmp)->h_name);
#else
		sprintf( passwd, "user@%s", gethostbyname(tmp)->h_name );
#endif	
	}
  
  
  for ( in++, out = host;
       *in !='\0' && *in!=':' ;
       *out++ = *in++);
  *out = '\0';
  
  if ( *in == '\0' ) return 0;

file:
  
  for ( in++, out = file;
       *in !='\0';
       *out++ = *in++);
  *out = '\0';
  
  return 1;
}
  







