static char rcsid[] = "$Id$";

/*
	$Log$
	Revision 1.1.1.1  2004/04/30 22:17:53  brian
	initial import
	
 * Revision 5.0  1995/12/10  10:28:38  orel
 * LIBFTP Version 5.0 (Distributed revision)
 *
 * Revision 4.2  1995/09/09  10:10:01  orel
 * Clean problems with aborting already broken connection.
 *
 * Revision 4.2  1995/09/09  10:10:01  orel
 * Clean problems with aborting already broken connection.
 *
 * Revision 4.1  1995/09/09  09:49:42  orel
 *  Change type String to FtpString for clean conflicting with
 * X11's type string which is pointer to character only.
 * Thanks for MIT for this good name of type.
 *
 * Revision 4.0  1995/07/11  07:00:26  orel
 * Libftp Version 4.0
 *
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
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


FtpAbort(FTP *ftp)
{
  fd_set fds;
  char msgc=IAC;
  FtpString msg;
  

  if (FTPCMD(ftp)!=NULL) /* To avoid segmentation fault
			    for abort nonexisting connections */
    {
      FD_ZERO(&fds);
      FD_SET(fileno(FTPCMD(ftp)),&fds);
      
      FtpPutc(ftp, FTPCMD(ftp), IAC);
      FtpPutc(ftp, FTPCMD(ftp), IP);
      
      if ( send ( fileno(FTPCMD(ftp)), &msgc , 1 ,MSG_OOB) != 1 )
		return EXIT(ftp,QUIT);
      
      FtpPutc(ftp, FTPCMD(ftp), DM);
      
      FtpSendMessage(ftp,"ABOR");
      
      while (select ( getdtablesize(), &fds, 0,0, &(ftp->timeout) )>0)
	{
	  FtpGetMessage(ftp,msg);
	  if (FtpGood(FtpNumber(msg),225,226,EOF)) break;
	}
    }

	return 0;
}


