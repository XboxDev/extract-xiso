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
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
 *
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
 *
 * Revision 1.5  1995/03/20  05:18:13  orel
 * *** empty log message ***
 *
 * Revision 1.4  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 1.4  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 1.3  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 1.3  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 1.2  1995/02/04  09:02:53  orel
 * add rcsid
 *
 * Revision 1.2  1995/02/04  09:02:53  orel
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

STATUS FtpPort(FTP *con,int a,int b,int c,int d,int e,int f)
{
  FtpString cmd;
  int i;

  sprintf(cmd,"PORT %d,%d,%d,%d,%d,%d",a,b,c,d,e,f);
  if ( FtpSendMessage(con,cmd) == QUIT)
    return QUIT;
  if ( (i=FtpGetMessage(con,cmd)) == QUIT)
    return QUIT;
  
  if ( ! FtpGood ( i , 200 , EOF ))
    return EXIT(con,-i);

  return EXIT(con,i);
}
