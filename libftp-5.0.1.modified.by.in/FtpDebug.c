static char rcsid[] = "$Id$";

/*
	$Log$
	Revision 1.1.1.1  2004/04/30 22:17:54  brian
	initial import
	
 * Revision 5.0  1995/12/10  10:28:38  orel
 * LIBFTP Version 5.0 (Distributed revision)
 *
 * Revision 4.1  1995/09/09  09:49:42  orel
 *  Change type String to FtpString for clean conflicting with
 * X11's type string which is pointer to character only.
 * Thanks for MIT for this good name of type.
 *
 * Revision 4.1  1995/09/09  09:49:42  orel
 *  Change type String to FtpString for clean conflicting with
 * X11's type string which is pointer to character only.
 * Thanks for MIT for this good name of type.
 *
 * Revision 4.0  1995/07/11  07:00:26  orel
 * Libftp Version 4.0
 *
 * Revision 3.1  1995/06/20  15:53:50  orel
 * Porting to AIX
 *
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
 *
 * Revision 2.4  1995/03/20  05:18:13  orel
 * *** empty log message ***
 *
 * Revision 2.3  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 2.3  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 2.2  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 2.2  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 2.1  1995/02/04  09:01:40  orel
 * add rcsid
 *
 * Revision 2.1  1995/02/04  09:01:40  orel
 * add rcsid
 *

*/
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

void FtpDebug(FTP *ftp)
{
  STATUS FtpDebugDebug(),
         FtpDebugError(),
         FtpDebugIO();

  FtpSetDebugHandler(ftp,FtpDebugDebug);
  FtpSetErrorHandler(ftp,FtpDebugError);
  FtpSetIOHandler(ftp,FtpDebugIO);
}

STATUS FtpDebugDebug(FTP *ftp,int n, char * Message)
{
  FtpString tmp;

  
  strcpy(tmp,Message);
  
  if (strncmp(tmp,"PASS ",5)==0)
    {
      char *p=tmp+5;
      while ( *p != '\0') *p++='*';
    };
	
  FtpLog(ftp->title,tmp);
  return 1;
}

STATUS FtpDebugError(FTP *ftp,int n, char * Message)
{
  
  FtpLog("FtpDebugError","");
  FtpLog(ftp->title,Message);
  if ( ! ( FtpTestFlag(ftp,(FTP_NOEXIT))))
    exit(1);

  return 0;
}

STATUS FtpDebugIO(FTP *ftp,int n, char * Message)
{
  FtpLog("FtpDebugIO","");
  FtpLog(ftp->title,Message);
  if ( ! FtpTestFlag(ftp,FTP_NOEXIT))
    exit(1);

  return 0;
}

STATUS FtpLog(char *name,char *str)
{
  time_t t=time((time_t *)0);
  struct tm *lt=localtime(&t);
  fprintf(stderr,"%02d:%02d:%02d %s %s\n",lt->tm_hour,
	  lt->tm_min,lt->tm_sec,name,str);
  fflush(stderr);

  return 0;
}

FtpHash(FTP *ftp, unsigned long chars)
{
  
  if (chars==0) return ftp->counter=0;
  ftp->counter+=chars;
  fprintf(stderr,"%10u bytes transfered\r",ftp->counter);
  fflush(stderr);
  return ftp->counter;
}


STATUS FtpBadReply550(char *s)
{
  if(
     ((char *)strstr(s,"unreachable")!=NULL) ||
     ((char *)strstr(s,"Broken pipe")!=NULL)
     ) 
    return 0;
  return 1;
}











