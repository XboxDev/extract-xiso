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
 * Revision 2.1  1995/02/04  09:02:53  orel
 * add rcsid
 *
 * Revision 2.1  1995/02/04  09:02:53  orel
 * add rcsid
 *

*/
/*		      Library for ftpd clients.(libftp)
			Copyright by Oleg Orel
			 All rights reserved.
			
This  library is desined  for  free,  non-commercial  software  creation. 
It is changeable and can be improved. The author would greatly appreciate 
any  advises, new  components  and  patches  of  the  exist
ing  programs.
Commercial  usage is  also  possible  with  participation of it's author.



*/

#include "FtpLibrary.h"


char * FtpPasv (FTP *ftp)
{
  char *msg;
  static FtpString PORT;
  char *p=PORT;
  
  if FtpError(FtpCommand(ftp,"PASV","",227,EOF)) 
    return "";
  
  msg = FtpMessage (227);

  msg+=3;
  
  while (!isdigit(*msg++));  
  msg--;

  while (isdigit(*msg)||*msg==',') *p++=*msg++;
  *p=0;
  
  return PORT;
}


STATUS FtpLink(FTP *ftp1, FTP *ftp2)
{
  
  FtpString PORT;

  strcpy(PORT,FtpPasv(ftp1));

  return FtpCommand(ftp2,"PORT %s",PORT,200,EOF);
}

STATUS FtpPassiveTransfer(FTP *ftp1, FTP *ftp2, char *f1, char *f2)
{
  FtpString tmp;
  fd_set fds;

  FtpAssert(ftp1,FtpLink(ftp1,ftp2));


  if (!*f2) f2=f1;

  FtpAssert(ftp2,FtpCommand(ftp2,"STOR %s",f2, 200, 120 , 150 , 125 , 250 , EOF ));
  FtpAssert(ftp1,FtpCommand(ftp1,"RETR %s",f1, 200, 120 , 150 , 125 , 250 , EOF ));
  
  FD_ZERO(&fds);
  
  FD_SET(fileno(FTPCMD(ftp1)),&fds);
  FD_SET(fileno(FTPCMD(ftp2)),&fds);
  
  if (select(getdtablesize(),&fds,0,0,0)<0)
    {
#if ! defined( __DARWIN__ ) && ! defined( __LINUX__ ) && ! defined( __FREEBSD__ )
      if (ftp1->error!=NULL)
	return (*(ftp1->error))(ftp1,QUIT,sys_errlist[errno]);
      if (ftp2->error!=NULL)
	return (*(ftp2->error))(ftp1,QUIT,sys_errlist[errno]);
#else
      if (ftp1->error!=NULL)
	return (*(ftp1->error))(ftp1,QUIT,strerror( errno ) );
      if (ftp2->error!=NULL)
	return (*(ftp2->error))(ftp1,QUIT,strerror( errno ) );
#endif
      return QUIT;
    }
  
  if (FD_ISSET(fileno(FTPCMD(ftp1)),&fds))
    {
      FtpGetMessage(ftp1,tmp);
      FtpLog(ftp1->title,tmp);
      FtpGetMessage(ftp2,tmp);
      FtpLog(ftp2->title,tmp);
    }
  else
    {
      FtpGetMessage(ftp2,tmp);
      FtpLog(ftp2->title,tmp);
      FtpGetMessage(ftp1,tmp);
      FtpLog(ftp1->title,tmp);
    }

	return 0;
}





