static char rcsid[] = "$Id";

/*
	$Log$
	Revision 1.1.1.1  2004/04/30 22:17:53  brian
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
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
 *
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
 *
 * Revision 1.2  1995/03/05  15:02:26  orel
 * /
 *
 * Revision 1.2  1995/03/05  15:02:26  orel
 * /
 *
 * Revision 1.1  1995/02/19  15:47:05  orel
 * Initial revision
 *
 * Revision 1.1  1995/02/19  15:47:05  orel
 * Initial revision
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

STATUS FtpChmod(FTP *ftp,char *file,int mode)
{
  FtpString msg;

  sprintf(msg,"SITE CHMOD %03o %s",mode,file); 
  return FtpCommand(ftp,msg,"",200,EOF);
  
}
